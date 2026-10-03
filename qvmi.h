#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct mmap_region
{
    void *start;
    void *end;
    size_t size;
} mmap_region;

typedef struct qvmi_vm_instance
{
    mmap_region vm_mmap;
    pid_t qemu_pid;
} qvmi_vm_instance;

typedef struct qvmi_proc_instance
{
    uint32_t process_id;
    void *dtb;
    void *entry;
    char *image_path_name;
} qvmi_proc_instance;

typedef struct qvmi_proc_instance_list qvmi_proc_instance_list;

typedef struct qvmi_proc_instance_list
{
    qvmi_proc_instance current;
    qvmi_proc_instance_list *next;
} qvmi_proc_instance_list;

typedef struct qvmi_module_instance
{
    void *base;
    size_t size;
} qvmi_module_instance;

typedef struct qvmi_ntoskrnl_instance
{
    qvmi_module_instance mod;
    void *ps_loaded_module_list;
    void *ps_active_process_head;
} qvmi_ntoskrnl_instance;

typedef struct qvmi_os_instance
{
    qvmi_vm_instance vm_instance;
    qvmi_proc_instance kernel_proc;
    qvmi_ntoskrnl_instance ntoskrnl;
} qvmi_os_instance;

__attribute__((visibility("default"))) bool qvmi_read_phys(struct qvmi_os_instance *os_instance, void *physical_address, void *buffer,
                                                           size_t size);

__attribute__((visibility("default"))) bool qvmi_read_virt(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance,
                                                           void *virtual_address, void *buffer, size_t size);

__attribute__((visibility("default"))) bool qvmi_get_os_instance(struct qvmi_os_instance *os_instance,
                                                                 struct qvmi_vm_instance *vm_instance);

__attribute__((visibility("default"))) bool qvmi_get_kernel_module_by_name(struct qvmi_os_instance *os_instance,
                                                                           struct qvmi_module_instance *module_instance,
                                                                           char *target_module_name);

__attribute__((visibility("default"))) qvmi_proc_instance_list *qvmi_get_proc_instance_list(struct qvmi_os_instance *os_instance);

__attribute__((visibility("default"))) void *qvmi_get_module_export_address(struct qvmi_os_instance *os_instance,
                                                                            struct qvmi_proc_instance *proc_instance,
                                                                            struct qvmi_module_instance *module_instance,
                                                                            char *export_name);

__attribute__((visibility("default"))) bool qvmi_get_vm_instance_by_vm_name(qvmi_vm_instance *vm_instance, char *vm_name);