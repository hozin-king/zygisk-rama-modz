#pragma once
#include <cstdint>
#include <sys/types.h>
#include <cstddef>
bool mem_read(pid_t pid, uintptr_t addr, void *buf, size_t len);
bool mem_write(pid_t pid, uintptr_t addr, const void *buf, size_t len);
uintptr_t get_module_base(pid_t pid, const char *name);
