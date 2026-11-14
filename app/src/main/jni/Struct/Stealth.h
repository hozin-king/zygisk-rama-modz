#ifndef RAMA_STEALTH_H
#define RAMA_STEALTH_H
#include <dlfcn.h>
// CONSTRAINT: include SETELAH header provider DobbyHook (Struct/main.h / RamaModzTeam.h).
// Semua hook independen: gagal satu = layer lain tetap jalan, status dilog per hook.
#include <android/log.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <cstdarg>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <sys/system_properties.h>
#include <string>
#include <vector>
#include <utility>

#define SLOG(...) __android_log_print(ANDROID_LOG_DEBUG, "RAMA_STEALTH", __VA_ARGS__)

namespace stealth {

static bool g_installed = false;
static int  g_selfPid = 0;

static bool ihas(const char *hay, const char *needle) {
    if (!hay || !needle) return false;
    std::string h(hay);
    for (auto &c : h) c = (char)tolower((unsigned char)c);
    return h.find(needle) != std::string::npos; // needle sudah lowercase
}

static const char *kBad[] = {
    "ramamodz", "rama_modz", "dobby", "lsposed", "xposed", "magisk",
    "riru", "zygisk", "substrate", "frida", "gum-js", "linjector", "/data/adb/"
};

static bool lineHidden(const char *line) {
    for (auto k : kBad) if (ihas(line, k)) return true;
    // Region exec anonim kecil (<1MB) = ciri trampoline Dobby.
    char addr[64], perms[8], off[32], dev[32], ino[32];
    int n = 0;
    if (sscanf(line, "%63s %7s %31s %31s %31s %n", addr, perms, off, dev, ino, &n) < 5) return false;
    if (!strchr(perms, 'x')) return false;
    unsigned long lo = 0, hi = 0;
    if (sscanf(addr, "%lx-%lx", &lo, &hi) != 2 || hi <= lo) return false;
    if (hi - lo > 0x100000) return false;
    const char *path = (n > 0) ? line + n : "";
    while (*path == ' ' || *path == '\t') path++;
    if (*path == '/') return false;                          // file-backed: cukup filter keyword
    if (!strncmp(path, "[vdso", 5) || !strncmp(path, "[vvar", 5) ||
        !strncmp(path, "[stack", 6)) return false;
    if (ihas(path, "dalvik")) return false;                  // ART JIT: biarkan
    return true;
}

static bool isProcWatch(const char *path, bool &isStatus) {
    isStatus = false;
    if (!path || strncmp(path, "/proc/", 6) != 0) return false;
    const char *p = path + 6, *rest = nullptr;
    char selfstr[16]; snprintf(selfstr, sizeof selfstr, "%d", g_selfPid);
    if (!strncmp(p, "self/", 5)) rest = p + 5;
    else {
        size_t l = strlen(selfstr);
        if (!strncmp(p, selfstr, l) && p[l] == '/') rest = p + l + 1;
    }
    if (!rest) return false;
    if (!strncmp(rest, "task/", 5)) {
        const char *t = strchr(rest + 5, '/');
        if (!t) return false;
        rest = t + 1;
    }
    if (!strcmp(rest, "maps") || !strcmp(rest, "smaps")) return true;
    if (!strcmp(rest, "status")) { isStatus = true; return true; }
    return false;
}

// Baca via syscall mentah (tidak lewat hook sendiri), filter, sajikan lewat memfd.
static int makeFiltered(const char *path, bool isStatus) {
    int fd = (int)syscall(SYS_openat, AT_FDCWD, path, O_RDONLY | O_CLOEXEC, 0);
    if (fd < 0) return -1;
    std::string data; char buf[8192]; ssize_t r;
    while ((r = read(fd, buf, sizeof buf)) > 0) data.append(buf, (size_t)r);
    close(fd);
    std::string out; out.reserve(data.size());
    size_t pos = 0; int hidden = 0;
    while (pos < data.size()) {
        size_t nl = data.find('\n', pos);
        size_t end = (nl == std::string::npos) ? data.size() : nl + 1;
        std::string line = data.substr(pos, end);
        pos = end;
        if (isStatus) {
            if (!line.compare(0, 10, "TracerPid:")) { out += "TracerPid:\t0\n"; hidden++; continue; }
        } else if (lineHidden(line.c_str())) { hidden++; continue; }
        out += line;
    }
    int mfd = (int)syscall(__NR_memfd_create, "jit-cache", 0);
    if (mfd < 0) return -1;
    size_t offw = 0;
    while (offw < out.size()) {
        ssize_t w = write(mfd, out.data() + offw, out.size() - offw);
        if (w <= 0) { close(mfd); return -1; }
        offw += (size_t)w;
    }
    lseek(mfd, 0, SEEK_SET);
    if (hidden) SLOG("%s: %d baris disembunyikan", path, hidden);
    return mfd;
}

static const char *kRootPaths[] = {
    "/system/bin/su", "/system/xbin/su", "/sbin/su", "/system/sbin/su",
    "/vendor/bin/su", "/su/bin/su", "/data/local/xbin/su", "/data/local/bin/su",
    "/data/local/su", "/system/bin/magisk", "/system/bin/ksud", "/data/adb",
    "/sbin/.magisk", "/system/app/Superuser.apk", "/system/etc/init.d",
    "/cache/magisk.log", "/system/bin/.ext"
};
static bool isRootPath(const char *p) {
    if (!p) return false;
    for (auto k : kRootPaths) {
        size_t l = strlen(k);
        if (!strcmp(p, k) || (!strncmp(p, k, l) && p[l] == '/')) return true;
    }
    return false;
}

static int   (*orig_open)(const char*, int, ...);
static int   (*orig_openat)(int, const char*, int, ...);
static int   (*orig_open2)(const char*, int);
static int   (*orig_openat2)(int, const char*, int);
static FILE* (*orig_fopen)(const char*, const char*);
static int   (*orig_access)(const char*, int);
static int   (*orig_faccessat)(int, const char*, int, int);
static int   (*orig_stat)(const char*, struct stat*);
static int   (*orig_lstat)(const char*, struct stat*);
static int   (*orig_prop_get)(const char*, char*);
static long  (*orig_ptrace)(int, pid_t, void*, void*);

static int my_open(const char *path, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT) { va_list ap; va_start(ap, flags); mode = (mode_t)va_arg(ap, int); va_end(ap); }
    if (isRootPath(path)) { errno = ENOENT; return -1; }
    bool st;
    if (isProcWatch(path, st)) { int fd = makeFiltered(path, st); if (fd >= 0) return fd; }
    if (!orig_open) return (int)syscall(SYS_openat, AT_FDCWD, path, flags, mode);
    return orig_open(path, flags, mode);
}
static int my_openat(int dirfd, const char *path, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT) { va_list ap; va_start(ap, flags); mode = (mode_t)va_arg(ap, int); va_end(ap); }
    if (isRootPath(path)) { errno = ENOENT; return -1; }
    bool st;
    if (dirfd == AT_FDCWD && isProcWatch(path, st)) { int fd = makeFiltered(path, st); if (fd >= 0) return fd; }
    if (!orig_openat) return (int)syscall(SYS_openat, dirfd, path, flags, mode);
    return orig_openat(dirfd, path, flags, mode);
}
static int my_open2(const char *path, int flags) { return my_open(path, flags); }
static int my_openat2(int dirfd, const char *path, int flags) { return my_openat(dirfd, path, flags); }

static FILE *my_fopen(const char *path, const char *mode) {
    if (isRootPath(path)) { errno = ENOENT; return nullptr; }
    bool st;
    if (isProcWatch(path, st)) {
        int fd = makeFiltered(path, st);
        if (fd >= 0) { FILE *f = fdopen(fd, "r"); if (f) return f; close(fd); }
    }
    if (!orig_fopen) { int fd = my_open(path, O_RDONLY); return fd >= 0 ? fdopen(fd, "r") : nullptr; }
    return orig_fopen(path, mode);
}
static int my_access(const char *path, int mode) {
    if (isRootPath(path)) { errno = ENOENT; return -1; }
    if (!orig_access) return (int)syscall(SYS_faccessat, AT_FDCWD, path, mode, 0);
    return orig_access(path, mode);
}
static int my_faccessat(int dirfd, const char *path, int mode, int flags) {
    if (isRootPath(path)) { errno = ENOENT; return -1; }
    if (!orig_faccessat) return (int)syscall(SYS_faccessat, dirfd, path, mode, flags);
    return orig_faccessat(dirfd, path, mode, flags);
}
static int my_stat(const char *path, struct stat *st) {
    if (isRootPath(path)) { errno = ENOENT; return -1; }
    if (!orig_stat) return (int)syscall(SYS_fstatat, AT_FDCWD, path, st, 0);
    return orig_stat(path, st);
}
static int my_lstat(const char *path, struct stat *st) {
    if (isRootPath(path)) { errno = ENOENT; return -1; }
    if (!orig_lstat) return (int)syscall(SYS_fstatat, AT_FDCWD, path, st, AT_SYMLINK_NOFOLLOW);
    return orig_lstat(path, st);
}
static int my_prop_get(const char *name, char *value) {
    int r = orig_prop_get ? orig_prop_get(name, value) : 0;
    if (!name || !value) return r;
    static const struct { const char *n, *v; } fake[] = {
        {"ro.build.tags", "release-keys"}, {"ro.debuggable", "0"}, {"ro.secure", "1"},
        {"ro.build.type", "user"}, {"ro.boot.verifiedbootstate", "green"},
        {"ro.boot.flash.locked", "1"}, {"ro.boot.vbmeta.device_state", "locked"},
    };
    for (auto &f : fake)
        if (!strcmp(name, f.n)) { strcpy(value, f.v); return (int)strlen(f.v); }
    return r;
}
static long my_ptrace(int request, pid_t pid, void *addr, void *data) {
    if (request == PTRACE_TRACEME) return 0; // anti-debug GGP: pura-pura sukses
    if (!orig_ptrace) return (long)syscall(SYS_ptrace, request, pid, addr, data);
    return orig_ptrace(request, pid, addr, data);
}

static void hookSym(const char *sym, void *rep, void **orig) {
    static std::vector<std::pair<void*, void*>> done;
    void *h = dlopen("libc.so", RTLD_NOW);
    void *t = h ? dlsym(h, sym) : nullptr;
    if (!t) { SLOG("resolve gagal: %s", sym); return; }
    for (auto &d : done) if (d.first == t) { *orig = d.second; SLOG("%s: alias, skip", sym); return; }
    int r = DobbyHook(t, rep, orig);
    if (r == 0) done.push_back({t, *orig});
    SLOG("libc!%s -> %s", sym, r == 0 ? "OK" : "FAIL");
}

static void Install() {
    if (g_installed) return;
    g_installed = true;
    g_selfPid = getpid();
    hookSym("open",                 (void*)my_open,      (void**)&orig_open);
    hookSym("open64",               (void*)my_open,      (void**)&orig_open);
    hookSym("openat",               (void*)my_openat,    (void**)&orig_openat);
    hookSym("__open_2",             (void*)my_open2,     (void**)&orig_open2);
    hookSym("__openat_2",           (void*)my_openat2,   (void**)&orig_openat2);
    hookSym("fopen",                (void*)my_fopen,     (void**)&orig_fopen);
    hookSym("fopen64",              (void*)my_fopen,     (void**)&orig_fopen);
    hookSym("access",               (void*)my_access,    (void**)&orig_access);
    hookSym("faccessat",            (void*)my_faccessat, (void**)&orig_faccessat);
    hookSym("stat",                 (void*)my_stat,      (void**)&orig_stat);
    hookSym("stat64",               (void*)my_stat,      (void**)&orig_stat);
    hookSym("lstat",                (void*)my_lstat,     (void**)&orig_lstat);
    hookSym("lstat64",              (void*)my_lstat,     (void**)&orig_lstat);
    hookSym("__system_property_get",(void*)my_prop_get,  (void**)&orig_prop_get);
    hookSym("ptrace",               (void*)my_ptrace,    (void**)&orig_ptrace);
    SLOG("stealth terpasang (pid=%d)", g_selfPid);
}

} // namespace stealth
#endif
