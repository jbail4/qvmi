#include "qvmi_process.h"

qvmi_proc_instance_list *qvmi_get_proc_instance_list(struct qvmi_os_instance *os_instance)
{
    qvmi_proc_instance_list *proc_instance_list = malloc(sizeof(qvmi_proc_instance_list));

    LIST_ENTRY list_start;
    qvmi_read_virt(os_instance, &os_instance->kernel_proc, os_instance->ntoskrnl.mod.base + PsActiveProcessHead_offset, &list_start,
                   sizeof(LIST_ENTRY));

    LIST_ENTRY current_entry = list_start;

    qvmi_proc_instance_list *current_process = proc_instance_list;

    do
    {
        EPROCESS eproc;
        qvmi_read_virt(os_instance, &os_instance->kernel_proc, (void *)((uint64_t)current_entry.Flink - 0x1d8), &eproc, sizeof(EPROCESS));

        current_process->current.dtb = (void *)eproc.Pcb.DirectoryTableBase;
        current_process->current.process_id = eproc.UniqueProcessId;

        PEB peb;
        qvmi_read_virt(os_instance, &current_process->current, eproc.Peb, &peb, sizeof(PEB));

        RTL_USER_PROCESS_PARAMETERS user_proc_params;

        qvmi_read_virt(os_instance, &current_process->current, peb.ProcessParameters, &user_proc_params,
                       sizeof(RTL_USER_PROCESS_PARAMETERS));

        current_process->current.image_path_name =
            read_unicode_string(os_instance, &current_process->current, user_proc_params.ImagePathName);

        qvmi_read_virt(os_instance, &os_instance->kernel_proc, (void *)current_entry.Flink, &current_entry, sizeof(LIST_ENTRY));
        current_process->next = malloc(sizeof(qvmi_proc_instance_list));
        current_process = current_process->next;
    } while (current_entry.Flink != list_start.Flink);

    current_process->next = NULL;

    return proc_instance_list;
}

void *qvmi_get_module_export_address(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance,
                                     struct qvmi_module_instance *module_instance, char *target_export_name)
{
    uint32_t e_lfanew = 0;
    qvmi_read_virt(os_instance, proc_instance, (void *)((uint64_t)module_instance->base + 0x3C), &e_lfanew, sizeof(e_lfanew));

    uint64_t image_nt_header = (uint64_t)module_instance->base + e_lfanew;
    uint64_t optional_header = image_nt_header + 0x18;

    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-image_data_directory
    IMAGE_DATA_DIRECTORY data_directory;
    qvmi_read_virt(os_instance, proc_instance, (void *)(optional_header + 112), &data_directory, sizeof(data_directory));

    IMAGE_EXPORT_DIRECTORY export_dir;
    qvmi_read_virt(os_instance, proc_instance, (void *)((uint64_t)module_instance->base + data_directory.VirtualAddress), &export_dir,
                   sizeof(export_dir));

    for (uint32_t i = 0; i < export_dir.NumberOfNames; i++)
    {
        uint32_t name_rva;
        qvmi_read_virt(os_instance, proc_instance, (void *)((uint64_t)module_instance->base + export_dir.AddressOfNames) + i * 4, &name_rva,
                       sizeof(name_rva));

        char export_name[256] = {0};
        qvmi_read_virt(os_instance, proc_instance, (void *)((uint64_t)module_instance->base + name_rva), export_name,
                       sizeof(export_name) - 1);

        if (strcmp(export_name, target_export_name) == 0)
        {
            uint16_t ordinal_index = 0;
            qvmi_read_virt(os_instance, proc_instance, (void *)((uint64_t)module_instance->base + export_dir.AddressOfNameOrdinals + i * 2),
                           &ordinal_index, sizeof(ordinal_index));

            uint32_t func_rva = 0;
            qvmi_read_virt(os_instance, proc_instance,
                           (void *)((uint64_t)module_instance->base + export_dir.AddressOfFunctions + ordinal_index * 4), &func_rva,
                           sizeof(func_rva));

            return (void *)((uint64_t)module_instance->base + func_rva);
        }
    }

    return 0;
}