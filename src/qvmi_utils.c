#include "qvmi_utils.h"

char *utf16le_to_ascii(const uint16_t *utf16)
{
    if (!utf16)
        return "invalid string";

    size_t length = 0;
    while (utf16[length] != 0)
        length++;

    char *ascii = (char *)malloc(length + 1);
    if (!ascii)
        return "invalid string";

    for (size_t i = 0; i < length; i++)
    {
        uint16_t code = utf16[i];

        if (code <= 0x7F)
            ascii[i] = (char)code;
        else
            ascii[i] = '?';
    }

    ascii[length] = '\0';
    return ascii;
}

char *read_unicode_string(struct qvmi_os_instance *os_instance, struct qvmi_proc_instance *proc_instance, UNICODE_STRING stirng)
{
    uint16_t unicode_string_buffer[stirng.MaximumLength];
    qvmi_read_virt(os_instance, proc_instance, (void *)stirng.Buffer, &unicode_string_buffer, sizeof(unicode_string_buffer));

    return utf16le_to_ascii(unicode_string_buffer);
}