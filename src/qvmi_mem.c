#define _GNU_SOURCE
#include "qvmi_mem.h"

bool qvmi_read_phys(struct qvmi_os_instance *os_instance, void *physical_address, void *buffer, size_t size)
{
    struct iovec local = {buffer, size};

    if ((uint64_t)physical_address <= 0x7fffffff)
    {
        struct iovec remote;
        remote.iov_base = (void *)((uint64_t)os_instance->vm_instance.vm_mmap.start + (uint64_t)physical_address);
        remote.iov_len = size;

        ssize_t bytes_read = process_vm_readv(os_instance->vm_instance.qemu_pid, &local, 1, &remote, 1, 0);
        if (bytes_read == -1)
            printf("process_vm_readv failed: %s (errno = %d)\n", strerror(errno), errno);
    }

    if ((uint64_t)physical_address >= 0x100000000)
    {
        struct iovec remote;
        remote.iov_base =
            (void *)((uint64_t)os_instance->vm_instance.vm_mmap.start + 0x80000000 + ((uint64_t)physical_address - 0x100000000));
        remote.iov_len = size;

        ssize_t bytes_read = process_vm_readv(os_instance->vm_instance.qemu_pid, &local, 1, &remote, 1, 0);
        if (bytes_read == -1)
            printf("process_vm_readv failed: %s (errno = %d)\n", strerror(errno), errno);
    }

    return true;
}

void *resolve_va_to_pa(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance, void *virtual_address)
{
    uint64_t pml4_table = (uint64_t)proc_instance->dtb & 0x000ffffffffff000ULL;

    uint64_t pml4_index = ((uint64_t)virtual_address >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t)virtual_address >> 30) & 0x1FF;
    uint64_t pd_index = ((uint64_t)virtual_address >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t)virtual_address >> 12) & 0x1FF;

    PAGE_TABLE_4K_ENTRY pml4_entry;
    qvmi_read_phys(os_instance, (void *)(pml4_table + pml4_index * 8), &pml4_entry, sizeof(PAGE_TABLE_4K_ENTRY));
    if (!(pml4_entry.bits.Present))
        return 0;

    uint64_t pdpt_table = pml4_entry.value & 0x000ffffffffff000ULL;

    PAGE_TABLE_4K_ENTRY pdpt_entry;
    qvmi_read_phys(os_instance, (void *)(pdpt_table + pdpt_index * 8), &pdpt_entry, sizeof(PAGE_TABLE_4K_ENTRY));
    if (!pdpt_entry.bits.Present)
        return 0;

    uint64_t pd_table = pdpt_entry.value & 0x000ffffffffff000ULL;

    PAGE_TABLE_4K_ENTRY pd_entry;
    qvmi_read_phys(os_instance, (void *)(pd_table + pd_index * 8), &pd_entry, sizeof(PAGE_TABLE_4K_ENTRY));

    if (!pd_entry.bits.Present)
        return 0;

    if (pd_entry.bits.AvabilableHigh)
    {
        uint64_t page_frame = pd_entry.value & 0x000ffffffffff000ULL;
        return (void *)(page_frame + ((uint64_t)virtual_address & 0x1FFFFF)); // offset within 2MB page
    }

    uint64_t pt_table = pd_entry.value & 0x000ffffffffff000ULL;

    PAGE_TABLE_4K_ENTRY pt_entry;
    qvmi_read_phys(os_instance, (void *)(pt_table + pt_index * 8), &pt_entry, sizeof(PAGE_TABLE_4K_ENTRY));
    if (!pt_entry.bits.Present)
        return 0;

    uint64_t page_frame = pt_entry.value & 0x000ffffffffff000ULL;

    return (void *)(page_frame + ((uint64_t)virtual_address & 0xFFF));
}

bool qvmi_read_virt(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance, void *virtual_address, void *buffer,
                    size_t size)
{
    void *physical_address = resolve_va_to_pa(os_instance, proc_instance, virtual_address);
    return qvmi_read_phys(os_instance, physical_address, buffer, size);
}