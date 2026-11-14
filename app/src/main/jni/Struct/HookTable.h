#ifndef RAMA_HOOKTABLE_H
#define RAMA_HOOKTABLE_H
#include <Il2Cpp.h>
// CONSTRAINT: include SETELAH Il2Cpp.h + provider OBFUSCATE (rantai include Main.cpp).
//
// PROVENANCE (hasil-dump hozin-king, diverifikasi 2026-10-17, dicek ulang 2026-11-14):
//   com.dts.freefiremax OB55 == 2.132.1 (blob dump.cs identik, sha eb96b2c5bb34312a)
//   global-metadata.dat XOR 0xE8, metadata v31, libil2cpp stripped -> TIDAK ADA RVA statis;
//   satu-satunya jalur alamat = resolve by-name runtime via export il2cpp_* (Il2CppAttach).
//   verify_hooks 8/8 OK: FollowCamera.get_OffsetForNormal, GameConfig.get_ResetGuest,
//   Player.GetCurrentDashSpeed / get_MaxJumpHeight / get_CustomGravity / get_RisingGravity /
//   get_FallingGravity / get_HeadCollider; aim-silent HBIBDMMOOOK.LHNJLFOINDO (1 param,
//   TypeDefIndex 33090) MASIH ADA. Nama obfuscated IDENTIK OB55 -> OB baru (17 Okt).
//
// Urutan resolve: kandidat nama per-OB (baru->lama) -> AOB unik di .text libil2cpp -> gagal (log).
// Saat nama sukses: harvest 24 byte pertama jadi pola AOB (branch/ADRP di-wildcard) -> log [HARVEST].
// Update OB baru: tempel hasil [HARVEST] ke kolom aob, atau tambah kandidat nama dari dump baru.
#include <android/log.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string>
#include <vector>

#define HLOG(...) __android_log_print(ANDROID_LOG_DEBUG, "RAMA_RESOLVE", __VA_ARGS__)

namespace hooktable {

struct NameCand { std::string ns, klass, method; int argc; };
struct Entry {
    const char *key;
    std::string image;
    std::vector<NameCand> names;   // kandidat OB terbaru di DEPAN
    const char *aob;               // "FD 7B BF A9 ?? ..." ; nullptr = UNSET (isi dari log [HARVEST])
};

// OBFUSCATE mengembalikan buffer sementara — salin ke std::string selagi hidup.
#define NC(ns, k, m, a) NameCand{std::string(OBFUSCATE(ns)), std::string(OBFUSCATE(k)), std::string(OBFUSCATE(m)), a}

static std::vector<Entry> &registry() {
    static std::vector<Entry> r = {
        {"Camera",         std::string(OBFUSCATE("Assembly-CSharp.dll")),
            { NC("COW.GamePlay", "FollowCamera", "get_OffsetForNormal", 0) }, nullptr},
        {"ResetGuest",     std::string(OBFUSCATE("Assembly-CSharp.dll")),
            { NC("COW",          "GameConfig",   "get_ResetGuest",      0) }, nullptr},
        {"DashSpeed",      std::string(OBFUSCATE("Assembly-CSharp.dll")),
            { NC("COW.GamePlay", "Player",       "GetCurrentDashSpeed", 0) }, nullptr},
        {"Jump",           std::string(OBFUSCATE("Assembly-CSharp.dll")),
            { NC("COW.GamePlay", "Player",       "get_MaxJumpHeight",   0) }, nullptr},
        {"GravityCustom",  std::string(OBFUSCATE("Assembly-CSharp.dll")),
            { NC("COW.GamePlay", "Player",       "get_CustomGravity",   0) }, nullptr},
        {"GravityRising",  std::string(OBFUSCATE("Assembly-CSharp.dll")),
            { NC("COW.GamePlay", "Player",       "get_RisingGravity",   0) }, nullptr},
        {"GravityFalling", std::string(OBFUSCATE("Assembly-CSharp.dll")),
            { NC("COW.GamePlay", "Player",       "get_FallingGravity",  0) }, nullptr},
        // OB baru: tambah NC(...) di depan daftar names entry terkait, dan/atau isi aob dari [HARVEST].
    };
    return r;
}

struct Seg { uintptr_t start, end; };
static std::vector<Seg> &il2cppText() {
    static std::vector<Seg> segs;
    if (!segs.empty()) return segs;
    int fd = (int)syscall(SYS_openat, AT_FDCWD, "/proc/self/maps", O_RDONLY | O_CLOEXEC, 0);
    if (fd < 0) return segs;
    std::string data; char b[8192]; ssize_t n;
    while ((n = read(fd, b, sizeof b)) > 0) data.append(b, (size_t)n);
    close(fd);
    size_t pos = 0;
    while (pos < data.size()) {
        size_t nl = data.find('\n', pos);
        size_t end = (nl == std::string::npos) ? data.size() : nl;
        std::string line = data.substr(pos, end);
        pos = end + 1;
        if (line.find("libil2cpp.so") == std::string::npos) continue;
        unsigned long lo, hi; char perms[8];
        if (sscanf(line.c_str(), "%lx-%lx %7s", &lo, &hi, perms) != 3) continue;
        if (!strchr(perms, 'x')) continue;
        segs.push_back({(uintptr_t)lo, (uintptr_t)hi});
    }
    HLOG("il2cpp .text: %zu segmen", segs.size());
    return segs;
}

static bool parsePat(const char *s, std::vector<int> &b) { // -1 = wildcard
    b.clear();
    for (const char *p = s; *p; ) {
        while (*p == ' ') p++;
        if (!*p) break;
        if (p[0] == '?' && p[1] == '?') { b.push_back(-1); p += 2; }
        else {
            char *e; long v = strtol(p, &e, 16);
            if (e == p || v < 0 || v > 255) return false;
            b.push_back((int)v); p = e;
        }
    }
    return !b.empty();
}

// Wajib unik: >1 match = tolak (salah target lebih bahaya daripada gagal).
static uintptr_t scan(const std::vector<int> &pat, int &matches) {
    matches = 0; uintptr_t found = 0;
    size_t first = 0;
    while (first < pat.size() && pat[first] < 0) first++;
    if (first == pat.size()) return 0;
    for (auto &s : il2cppText()) {
        size_t len = s.end - s.start;
        if (len < pat.size()) continue;
        const uint8_t *base = (const uint8_t*)s.start;
        size_t last = len - pat.size();
        for (size_t off = 0; off <= last; ) {
            const uint8_t *hit = (const uint8_t*)memchr(base + off + first, pat[first], last - off + 1);
            if (!hit) break;
            size_t start = (size_t)(hit - base) - first;
            bool ok = true;
            for (size_t i = 0; i < pat.size(); i++)
                if (pat[i] >= 0 && base[start + i] != (uint8_t)pat[i]) { ok = false; break; }
            if (ok) { matches++; if (!found) found = s.start + start; if (matches > 1) return found; }
            off = start + 1;
        }
    }
    return found;
}

static bool wildWord(uint32_t w) {
    if ((w & 0x7C000000u) == 0x14000000u) return true; // B / BL
    if ((w & 0x9F000000u) == 0x90000000u) return true; // ADRP
    if ((w & 0x3B000000u) == 0x18000000u) return true; // LDR literal
    if ((w & 0xFF000010u) == 0x54000000u) return true; // B.cond
    if ((w & 0x7E000000u) == 0x34000000u) return true; // CBZ / CBNZ
    if ((w & 0x7E000000u) == 0x36000000u) return true; // TBZ / TBNZ
    return false;
}

static void harvest(const char *key, uintptr_t addr) {
    char pat[160]; size_t o = 0;
#if defined(__aarch64__)
    const uint32_t *w = (const uint32_t*)addr;
    for (int i = 0; i < 6 && o < sizeof(pat) - 16; i++) {
        if (wildWord(w[i])) o += snprintf(pat + o, sizeof(pat) - o, "?? ?? ?? ?? ");
        else o += snprintf(pat + o, sizeof(pat) - o, "%02X %02X %02X %02X ",
                           w[i] & 255, (w[i] >> 8) & 255, (w[i] >> 16) & 255, (w[i] >> 24) & 255);
    }
#else
    const uint8_t *b = (const uint8_t*)addr; // armv7: dump mentah, wildcard manual saat dipakai
    for (int i = 0; i < 24 && o < sizeof(pat) - 8; i++)
        o += snprintf(pat + o, sizeof(pat) - o, "%02X ", b[i]);
#endif
    HLOG("[HARVEST] %s @ %p : %s", key, (void*)addr, pat);
}

static uintptr_t Resolve(const char *key) {
    for (auto &e : registry()) {
        if (strcmp(e.key, key) != 0) continue;
        for (auto &c : e.names) {
            uintptr_t a = (uintptr_t)Il2CppGetMethodOffset(e.image.c_str(), c.ns.c_str(),
                                                           c.klass.c_str(), c.method.c_str(), c.argc);
            if (a && a >= 0x100000) {
                HLOG("%s: nama OK (%s::%s)", key, c.klass.c_str(), c.method.c_str());
                harvest(key, a);
                return a;
            }
        }
        if (e.aob && e.aob[0]) {
            std::vector<int> pat;
            if (parsePat(e.aob, pat)) {
                int m = 0; uintptr_t a = scan(pat, m);
                if (m == 1 && a) { HLOG("%s: AOB OK (unik) @ %p", key, (void*)a); return a; }
                HLOG("%s: AOB %s (matches=%d)", key, m == 0 ? "tidak ketemu" : "ambigu, ditolak", m);
            }
        }
        HLOG("%s: GAGAL semua kandidat — tambah nama OB baru / pola [HARVEST] di HookTable.h", key);
        return 0;
    }
    HLOG("key tidak terdaftar: %s", key);
    return 0;
}

} // namespace hooktable
#endif
