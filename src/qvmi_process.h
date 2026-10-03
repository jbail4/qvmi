#include <string.h>
#include <stdlib.h>

#include "qvmi_objects.h"
#include "windef.h"
#include "qvmi_utils.h"

#include "qvmi_mem.h"

#define PsActiveProcessHead_offset 0xF05790

__attribute__((visibility("default"))) qvmi_proc_instance_list *qvmi_get_proc_instance_list(struct qvmi_os_instance *os_instance);

__attribute__((visibility("default"))) void *qvmi_get_module_export_address(struct qvmi_os_instance *os_instance,
                                                                            struct qvmi_proc_instance *proc_instance,
                                                                            struct qvmi_module_instance *module_instance,
                                                                            char *target_export_name);