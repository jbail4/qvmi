#pragma once
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qvmi_mem.h"
#include "windef.h"

char *utf16le_to_ascii(const uint16_t *utf16);

char *read_unicode_string(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance, UNICODE_STRING stirng);