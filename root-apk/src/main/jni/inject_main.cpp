// rama_inject — injector ptrace root (arm64-v8a + armeabi-v7a).
// Dijalankan sebagai root via su: librama_inject.so <pid> <path_lib_payload>
// Status stdout: INJECT_OK / ALREADY_LOADED / INJECT_FAIL <alasan>. Regs target dipulihkan penuh.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <string>
#include "memory.h"

#if defined(__aarch64__)
#include <asm/ptrace.h>
#include <linux/elf.h>
typedef struct user_pt_regs regs_t;
static bool get_regs(pid_t p, regs_t &r) {
    struct iovec io = { &r, sizeof r };
    return ptrace(PTRACE_GETREGSET, p, (void*)NT_PRSTATUS, &io) == 0;
}
static bool set_regs(pid_t p, const regs_t &r) {
    struct iovec io = { (void*)&r, sizeof r };
    return ptrace(PTRACE_SETREGSET, p, (void*)NT_PRSTATUS, &io) == 0;
}
static void setup_call(regs_t &r, uintptr_t fn, uintptr_t a0, uintptr_t a1, uintptr_t a2) {
    r.pc = fn; r.regs[30] = 0; r.regs[0] = a0; r.regs[1] = a1; r.regs[2] = a2;
    r.sp &= ~15ULL;
}
static uintptr_t retval(const regs_t &r) { return r.regs[0]; }
static uintptr_t curpc(const regs_t &r) { return r.pc; }
#elif defined(__arm__)
#include <sys/user.h>
typedef struct user_regs regs_t;
static bool get_regs(pid_t p, regs_t &r) { return ptrace(PTRACE_GETREGS, p, 0, &r) == 0; }
static bool set_regs(pid_t p, const regs_t &r) { return ptrace(PTRACE_SETREGS, p, 0, (void*)&r) == 0; }
static void setup_call(regs_t &r, uintptr_t fn, uintptr_t a0, uintptr_t a1, uintptr_t a2) {
    r.uregs[15] = fn & ~1u; r.uregs[14] = 0;
    r.uregs[0] = a0; r.uregs[1] = a1; r.uregs[2] = a2;
    if (fn & 1) r.uregs[16] |= 0x20u; else r.uregs[16] &= ~0x20u; // thumb
    r.uregs[13] &= ~7u;
}
static uintptr_t retval(const regs_t &r) { return r.uregs[0]; }
static uintptr_t curpc(const regs_t &r) { return r.uregs[15]; }
#else
#error "arsitektur tidak didukung"
#endif

static int wait_stop(pid_t p, int timeout_ms) {
    for (int t = 0; t < timeout_ms; t += 5) {
        int st = 0;
        pid_t r = waitpid(p, &st, WNOHANG);
        if (r == p && WIFSTOPPED(st)) return WSTOPSIG(st);
        if (r == p && WIFEXITED(st)) return -2;
        if (r < 0) return -1;
        usleep(5000);
    }
    return -1;
}

// Resolve simbol di proses target: alamat lokal (RTLD_DEFAULT, ikut forwarding libdl->linker)
// dipetakan ke modul pemiliknya, lalu di-rebase ke base modul yang sama di target.
static uintptr_t remote_func(pid_t pid, const char *sym) {
    void *s = dlsym(RTLD_DEFAULT, sym);
    if (!s) return 0;
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) return 0;
    char line[512], path[256] = {0};
    while (fgets(line, sizeof line, f)) {
        unsigned long lo, hi;
        if (sscanf(line, "%lx-%lx", &lo, &hi) != 2) continue;
        if ((uintptr_t)s >= lo && (uintptr_t)s < hi) {
            char *p = strchr(line, '/');
            if (p) { strncpy(path, p, sizeof path - 1); path[strcspn(path, "\n")] = 0; }
            break;
        }
    }
    fclose(f);
    if (!path[0]) return 0;
    uintptr_t lb = get_module_base(getpid(), path);
    uintptr_t rb = get_module_base(pid, path);
    if (!lb || !rb) return 0;
    return rb + ((uintptr_t)s - lb);
}

static bool do_inject(pid_t pid, const char *lib, uintptr_t caller) {
    if (ptrace(PTRACE_ATTACH, pid, 0, 0) < 0) {
        printf("INJECT_FAIL attach: %s\n", strerror(errno)); return false;
    }
    if (wait_stop(pid, 5000) <= 0) {
        printf("INJECT_FAIL wait attach\n"); ptrace(PTRACE_DETACH, pid, 0, 0); return false;
    }
    regs_t saved{}, work{};
    if (!get_regs(pid, saved)) {
        printf("INJECT_FAIL getregs\n"); ptrace(PTRACE_DETACH, pid, 0, 0); return false;
    }
    bool ok = false;
    uintptr_t fn = remote_func(pid, "__loader_dlopen");
    bool loader3 = fn != 0;
    if (!fn) fn = remote_func(pid, "dlopen");
    if (!fn) { printf("INJECT_FAIL resolve dlopen di target\n"); goto restore; }
    {
        work = saved;
        uintptr_t str = (saved.sp - 0x1000) & ~15ULL;
        char buf[512];
        snprintf(buf, sizeof buf, "%s", lib);
        if (!mem_write(pid, str, buf, strlen(buf) + 1)) {
            printf("INJECT_FAIL tulis path: %s\n", last_error().c_str()); goto restore;
        }
        // caller = alamat dalam libil2cpp -> linker pakai namespace classloader game
        setup_call(work, fn, str, 2 /*RTLD_NOW*/, loader3 ? caller : 0);
        if (!set_regs(pid, work)) { printf("INJECT_FAIL setregs\n"); goto restore; }
        if (ptrace(PTRACE_CONT, pid, 0, 0) < 0) { printf("INJECT_FAIL cont\n"); goto restore; }
        bool done = false;
        for (int i = 0; i < 12 && !done; i++) {
            int sig = wait_stop(pid, 5000);
            if (sig <= 0) { printf("INJECT_FAIL wait call (sig=%d)\n", sig); goto restore; }
            regs_t out{};
            if (!get_regs(pid, out)) goto restore;
            if ((sig == SIGSEGV || sig == SIGTRAP || sig == SIGILL) && curpc(out) < 0x10000) {
                uintptr_t handle = retval(out);
                if (handle) { printf("INJECT_OK handle=0x%llx loader3=%d\n",
                                     (unsigned long long)handle, (int)loader3); ok = true; }
                else printf("INJECT_FAIL dlopen return 0 (cek SELinux/path; lihat ROOT-INJECTOR.md)\n");
                done = true;
            } else {
                // sinyal lain milik game: teruskan, jangan ditelan
                ptrace(PTRACE_CONT, pid, 0, (void*)(long)sig);
            }
        }
        if (!done) printf("INJECT_FAIL timeout call\n");
    }
restore:
    set_regs(pid, saved);            // rollback penuh apa pun hasilnya
    ptrace(PTRACE_DETACH, pid, 0, 0);
    return ok;
}

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s <pid> <lib_path>\n", argv[0]); return 2; }
    if (getuid() != 0) { printf("INJECT_FAIL harus root\n"); return 4; }
    pid_t pid = (pid_t)atoi(argv[1]);
    if (get_module_base(pid, "libRAMA_MODZ.so")) { printf("ALREADY_LOADED\n"); return 0; }
    uintptr_t il2 = get_module_base(pid, "libil2cpp.so");
    if (!il2) { printf("INJECT_FAIL libil2cpp belum mapped — masuk lobby dulu\n"); return 3; }
    return do_inject(pid, argv[2], il2 + 0x1000) ? 0 : 1;
}
