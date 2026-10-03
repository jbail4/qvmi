#pragma once
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>

#include "qvmi_objects.h"

// credit: https://bsdio.com/edk2/docs/master/_mde_module_pkg_2_core_2_dxe_ipl_peim_2_x64_2_virtual_memory_8h_source.html

// Page Table Entry 4KB
typedef union {
    struct
    {
        uint64_t Present : 1;               // 0 = Not present in memory, 1 = Present in memory
        uint64_t ReadWrite : 1;             // 0 = Read-Only, 1= Read/Write
        uint64_t UserSupervisor : 1;        // 0 = Supervisor, 1=User
        uint64_t WriteThrough : 1;          // 0 = Write-Back caching, 1=Write-Through caching
        uint64_t CacheDisabled : 1;         // 0 = Cached, 1=Non-Cached
        uint64_t Accessed : 1;              // 0 = Not accessed, 1 = Accessed (set by CPU)
        uint64_t Dirty : 1;                 // 0 = Not Dirty, 1 = written by processor on access to page
        uint64_t PAT : 1;                   //
        uint64_t Global : 1;                // 0 = Not global page, 1 = global page TLB not cleared on CR3 write
        uint64_t Available : 3;             // Available for use by system software
        uint64_t PageTableBaseAddress : 40; // Page Table Base Address
        uint64_t AvabilableHigh : 11;       // Available for use by system software
        uint64_t Nx : 1;                    // 0 = Execute Code, 1 = No Code Execution
    } bits;
    uint64_t value;
} PAGE_TABLE_4K_ENTRY;

__attribute__((visibility("default"))) bool qvmi_read_phys(struct qvmi_os_instance *os_instance, void *physical_address, void *buffer,
                                                           size_t size);

void *resolve_va_to_pa(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance, void *virtual_address);

__attribute__((visibility("default"))) bool qvmi_read_virt(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance,
                                                           void *virtual_address, void *buffer, size_t size);