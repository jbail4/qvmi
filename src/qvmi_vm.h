#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "qvmi_objects.h"

bool get_qemu_guest_name(pid_t qemu_pid, char *buffer);

bool get_process_larget_mmap_region(struct mmap_region *largest_map, pid_t qemu_pid);

__attribute__((visibility("default"))) bool qvmi_get_vm_instance_by_vm_name(qvmi_vm_instance *vm_instance, char *vm_name);