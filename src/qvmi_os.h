#include <stdbool.h>
#include <stdlib.h>

#include "qvmi_mem.h"
#include "qvmi_objects.h"
#include "qvmi_process.h"
#include "qvmi_utils.h"

void *get_ps_loaded_module_list(struct qvmi_os_instance *os_instance);

// credit: https://github.com/ufrisk/MemProcFS/blob/master/vmm/vmmwininit.c#L802

__attribute__((visibility("default"))) bool qvmi_get_os_instance(struct qvmi_os_instance *os_instance,
                                                                 struct qvmi_vm_instance *vm_instance);

__attribute__((visibility("default"))) bool qvmi_get_kernel_module_by_name(struct qvmi_os_instance *os_instance,
                                                                           struct qvmi_module_instance *module_instance,
                                                                           char *target_module_name);