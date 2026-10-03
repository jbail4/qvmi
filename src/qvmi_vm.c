#include "qvmi_vm.h"

bool get_qemu_guest_name(pid_t qemu_pid, char *buffer)
{
    char cmdline_path[PATH_MAX];
    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%i/cmdline", qemu_pid);

    FILE *cmdline_file = fopen(cmdline_path, "r");
    if (!cmdline_file)
        return false;

    char cmdline[4096];
    fread(cmdline, 1, sizeof(cmdline), cmdline_file);
    fclose(cmdline_file);

    for (int i = 0; i < sizeof(cmdline); i++)
        if (cmdline[i] == '\0' || cmdline[i] == '\n')
            cmdline[i] = ' ';

    char *guest_name = strstr(cmdline, "guest=");
    if (!guest_name)
        return false;

    guest_name += strlen("guest=");

    char *comma = strchr(guest_name, ',');
    if (!comma)
        return false;
    *comma = '\0';

    strcpy(buffer, guest_name);

    return true;
}

bool get_process_larget_mmap_region(struct mmap_region *largest_map, pid_t qemu_pid)
{
    char maps_path[PATH_MAX];
    snprintf(maps_path, sizeof(maps_path), "/proc/%i/maps", qemu_pid);

    FILE *maps_file = fopen(maps_path, "r");
    if (!maps_file)
        return false;

    char map_desc[512];
    while (fgets(map_desc, sizeof(map_desc), maps_file))
    {
        struct mmap_region current_mmap;
        sscanf(map_desc, "%p-%p", &current_mmap.start, &current_mmap.end);

        current_mmap.size = current_mmap.end - current_mmap.start;

        if (largest_map->size < current_mmap.size)
            memcpy(largest_map, &current_mmap, sizeof(mmap_region));
    }

    return true;
}

bool qvmi_get_vm_instance_by_vm_name(qvmi_vm_instance *vm_instance, char *vm_name)
{
    DIR *proc_dir = opendir("/proc/");

    struct dirent *pid_ent;

    while ((pid_ent = readdir(proc_dir)))
    {
        pid_t current_pid = atoi(pid_ent->d_name);
        if (!current_pid)
            continue;

        char linkpath[PATH_MAX];
        sprintf(linkpath, "/proc/%s/exe", pid_ent->d_name);

        char exepath[PATH_MAX];
        ssize_t len = readlink(linkpath, exepath, sizeof(exepath) - 1);
        if (len == -1)
            continue;
        exepath[len] = '\0';

        if (!strstr(exepath, "qemu"))
            continue;

        char guest_name[32];
        if (!get_qemu_guest_name(current_pid, guest_name))
            continue;

        if (!strcmp(guest_name, vm_name))
        {
            closedir(proc_dir);
            vm_instance->qemu_pid = current_pid;
            get_process_larget_mmap_region(&vm_instance->vm_mmap, vm_instance->qemu_pid);

            return true;
        }
    }

    return false;
}