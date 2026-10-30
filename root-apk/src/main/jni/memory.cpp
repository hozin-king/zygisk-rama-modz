#include "memory.h"
#include <sys/uio.h>
#include <fstream>
#include <sstream>
#include <string>
bool mem_read(pid_t pid, uintptr_t addr, void *buf, size_t len) {
    struct iovec local{buf, len}, remote{(void*)addr, len};
    return process_vm_readv(pid, &local, 1, &remote, 1, 0) == (ssize_t)len;
}
bool mem_write(pid_t pid, uintptr_t addr, const void *buf, size_t len) {
    struct iovec local{(void*)buf, len}, remote{(void*)addr, len};
    return process_vm_writev(pid, &local, 1, &remote, 1, 0) == (ssize_t)len;
}
uintptr_t get_module_base(pid_t pid, const char *name) {
    std::stringstream path; path << "/proc/" << pid << "/maps";
    std::ifstream maps(path.str());
    if (!maps.is_open()) return 0;
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find(name) != std::string::npos) {
            size_t d = line.find('-');
            if (d != std::string::npos) return std::stoull(line.substr(0, d), nullptr, 16);
        }
    }
    return 0;
}
