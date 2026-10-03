#include "qvmi_os.h"

bool qvmi_get_os_instance(struct qvmi_os_instance *os_instance, struct qvmi_vm_instance *vm_instance)
{
    os_instance->vm_instance = *(qvmi_vm_instance *)vm_instance;

    void *lowstub = malloc(0x100000);
    qvmi_read_phys(os_instance, 0x0, lowstub, 0x100000);

    for (uint32_t o = 0; o < 0x100000; o++)
    {
        if (0x00000001000600E9 != (0xffffffffffff00ff & *(uint64_t *)((uint64_t)lowstub + o + 0x000)))
        {
            continue;
        }
        if (0xfffff80000000000 != (0xfffff80000000003 & *(uint64_t *)((uint64_t)lowstub + o + 0x070)))
        {
            continue;
        }
        if (0xffffff0000000fff & *(uint64_t *)((uint64_t)lowstub + o + 0x0a0))
        {
            continue;
        }

        os_instance->kernel_proc.entry = (void *)*(uint64_t *)((uint64_t)lowstub + o + 0x070);
        os_instance->kernel_proc.entry = (void *)((uint64_t)os_instance->kernel_proc.entry & ~0x1FFFFF);

        os_instance->kernel_proc.dtb = *(void **)((uint64_t)lowstub + o + 0x0a0);
    }

    free(lowstub);

    for (uint16_t i = 0; i < 0x1000; i++)
    {
        qvmi_module_instance current_module = {(void *)((uint64_t)os_instance->kernel_proc.entry + i * 0x200000), 0};

        uint16_t mz_buffer;
        qvmi_read_virt(os_instance, &os_instance->kernel_proc, current_module.base, &mz_buffer, sizeof(mz_buffer));
        if (mz_buffer != 0x5a4D)
            continue;

        os_instance->ntoskrnl.ps_loaded_module_list =
            qvmi_get_module_export_address(os_instance, &os_instance->kernel_proc, &current_module, "PsLoadedModuleList");
        if (!os_instance->ntoskrnl.ps_loaded_module_list)
            continue;

        os_instance->ntoskrnl.mod = current_module;

        break;
    }

    return true;
}

bool qvmi_get_kernel_module_by_name(struct qvmi_os_instance *os_instance, struct qvmi_module_instance *module_instance,
                                    char *target_module_name)
{
    LIST_ENTRY first_flink;
    qvmi_read_virt(os_instance, &os_instance->kernel_proc, os_instance->ntoskrnl.ps_loaded_module_list, &first_flink, sizeof(first_flink));

    LDR_DATA_TABLE_ENTRY first_entry;
    qvmi_read_virt(os_instance, &os_instance->kernel_proc, (void *)first_flink.Flink, &first_entry, sizeof(LDR_DATA_TABLE_ENTRY));

    LDR_DATA_TABLE_ENTRY current_entry = first_entry;

    while (true)
    {
        char *module_name = read_unicode_string(os_instance, &os_instance->kernel_proc, current_entry.BaseDllName);

        if (strstr(module_name, target_module_name))
        {
            module_instance->base = (void *)current_entry.DllBase;
            module_instance->size = current_entry.SizeOfImage;

            break;
        }

        qvmi_read_virt(os_instance, &os_instance->kernel_proc, (void *)current_entry.InLoadOrderLinks.Flink, &current_entry,
                       sizeof(LDR_DATA_TABLE_ENTRY));

        if (first_entry.InLoadOrderLinks.Flink == current_entry.InLoadOrderLinks.Flink)
            break;
    }

    return true;
}