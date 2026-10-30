#include "memory.h"
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <errno.h>

static std::string g_err;

std::string last_error() { return g_err; }

static bool read_proc_mem(pid_t pid, uintptr_t addr, void *buf, size_t len) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/mem", (int)pid);
    int fd = open(path, O_RDONLY);
    if (fd < 0) { g_err = "open mem r failed"; return false; }
    ssize_t n = pread(fd, buf, len, (off_t)addr);
    close(fd);
    if (n != (ssize_t)len) { g_err = "pread short"; return false; }
    return true;
}

static bool write_proc_mem(pid_t pid, uintptr_t addr, const void *buf, size_t len) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/mem", (int)pid);
    int fd = open(path, O_RDWR);
    if (fd < 0) { g_err = "open mem w failed"; return false; }
    ssize_t n = pwrite(fd, buf, len, (off_t)addr);
    close(fd);
    if (n != (ssize_t)len) { g_err = "pwrite short"; return false; }
    return true;
}

bool mem_read(pid_t pid, uintptr_t addr, void *buf, size_t len) {
    struct iovec local{buf, len}, remote{(void *)addr, len};
    ssize_t n = process_vm_readv(pid, &local, 1, &remote, 1, 0);
    if (n == (ssize_t)len) return true;
    return read_proc_mem(pid, addr, buf, len);
}

bool mem_write(pid_t pid, uintptr_t addr, const void *buf, size_t len) {
    struct iovec local{(void *)buf, len}, remote{(void *)addr, len};
    ssize_t n = process_vm_writev(pid, &local, 1, &remote, 1, 0);
    if (n == (ssize_t)len) return true;
    return write_proc_mem(pid, addr, buf, len);
}

uintptr_t get_module_base(pid_t pid, const char *name) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/maps", (int)pid);
    FILE *f = fopen(path, "r");
    if (!f) { g_err = "maps open fail"; return 0; }
    char line[512];
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), f)) {
        if (!strstr(line, name)) continue;
        // prefer executable mapping (r-xp)
        if (!strstr(line, "r-xp") && !strstr(line, "r--p")) continue;
        unsigned long long start = 0;
        if (sscanf(line, "%llx-", &start) == 1) {
            base = (uintptr_t)start;
            break;
        }
    }
    fclose(f);
    if (!base) g_err = "module not in maps";
    return base;
}

// Scan /proc/*/cmdline for package (more reliable than pidof on some ROMs)
pid_t find_pid(const char *pkg) {
    DIR *d = opendir("/proc");
    if (!d) return -1;
    pid_t found = -1;
    struct dirent *e;
    while ((e = readdir(d)) != nullptr) {
        if (e->d_name[0] < '1' || e->d_name[0] > '9') continue;
        char cpath[64];
        snprintf(cpath, sizeof(cpath), "/proc/%s/cmdline", e->d_name);
        int fd = open(cpath, O_RDONLY);
        if (fd < 0) continue;
        char cmd[256];
        ssize_t n = read(fd, cmd, sizeof(cmd) - 1);
        close(fd);
        if (n <= 0) continue;
        cmd[n] = 0;
        // cmdline is NUL-separated; first token is process name
        if (strcmp(cmd, pkg) == 0) {
            found = (pid_t)atoi(e->d_name);
            break;
        }
    }
    closedir(d);
    return found;
}
