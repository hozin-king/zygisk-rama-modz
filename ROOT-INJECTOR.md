# RAMA MODZ — Root Injector APK

Arsitektur (sejak commit ini): APK root berisi **3 artefak native**:
1. `libramaroot.so` — JNI tools lama (find_pid, base, read/write) — tidak diubah.
2. `librama_inject.so` — **executable PIE** injector ptrace (arm64 + armv7), dijalankan via su.
   Resolve `__loader_dlopen`/`dlopen` di proses game (rebase simbol via maps), panggil remote
   dengan argumen caller = alamat dalam libil2cpp (namespace classloader game), regs dipulihkan penuh.
3. `libRAMA_MODZ.so` — **payload in-process penuh** dari sumber Zygisk yang sama
   (`app/src/main/jni`), di-compile dengan `-DRAMA_ROOTINJECT` → entry constructor, bukan Zygisk.
   Berisi semua fitur (aim/ESP/menu ImGui via eglSwapBuffers), resolver tahan-update, dan stealth.

Alur: MainActivity → RootHelper.injectSo → salin payload ke /data/local/tmp +
`chcon u:object_r:apk_data_file:s0` → su jalankan injector → constructor payload →
stealth::Install() → hack() → hook + menu. **Tidak butuh Zygisk/LSPosed — cukup root.**

## Provenance hook (dari repo hasil-dump, diverifikasi 2026-10-17, dicek ulang 2026-11-14)
- FF Max `com.dts.freefiremax` 2.132.1 (arm64-v8a, APK resmi signer 111dots studio,
  versionCode 2019118524, targetSdk 36). Unity 2018.4.x IL2CPP, libil2cpp 185 MB stripped.
- `global-metadata.dat` XOR single-byte **0xE8**, metadata **v31**. Dump OB55 == 2.132.1
  (blob identik). Tidak ada RVA statis → alamat di-resolve **by-name runtime** via export
  `il2cpp_*` (Il2CppAttach/xdl). Ini satu-satunya jalur yang benar untuk build ini.
- Nama hook terverifikasi 8/8 ada di metadata: FollowCamera.get_OffsetForNormal,
  GameConfig.get_ResetGuest, Player.GetCurrentDashSpeed/get_MaxJumpHeight/get_CustomGravity/
  get_RisingGravity/get_FallingGravity/get_HeadCollider; aim-silent
  `COW.GamePlay.HBIBDMMOOOK.LHNJLFOINDO` (1 param, TypeDefIndex 33090).
- Anti-cheat: `libGGP.so` 9,4 MB stripped tanpa dynamic symbol; `libBifrost.so` tidak ada di
  bundle (kemungkinan diunduh runtime) → aturan deteksi bisa berubah tanpa update OB.

## Tahan update
`Struct/HookTable.h`: kandidat nama per-OB → fallback AOB unik di .text libil2cpp → gagal aman (log).
Saat resolve sukses, modul **memanen signature sendiri**: log `[HARVEST] <key> @ <addr> : <pola>`
(branch/ADRP/LDR-literal di-wildcard). OB baru: tempel pola ke kolom `aob`, atau tambah kandidat
nama dari dump baru (pipeline `scripts/offset_pipeline` tetap berlaku untuk InitBypass).

## Stealth (coverage jujur)
Tertutup level libc+proses: filter /proc/self/maps+smaps (nama modul, anon-exec kecil),
TracerPid→0, root-path→ENOENT, spoof properti build, ptrace(TRACEME)→sukses.
Belum tertutup: scan prolog inline-hook Dobby oleh GGP, integrity hash memori vs file,
report/heartbeat GGP (butuh RE libGGP aktual), deteksi perilaku server (tidak bisa dari client),
baca maps via syscall langsung (bypass libc). Jangan klaim "tidak terdeteksi".

## Build & pakai
CI `build.yml` otomatis build `:root-apk:assembleRelease` + rilis APK tiap push ke main.
Lokal: `./gradlew :root-apk:assembleRelease` (JDK 17, SDK 34, NDK 25.2.9519653).
Pakai: install APK → buka FF sampai **lobby** → buka app → pilih package → INJECT → balik ke game.

Log harapan: `INJECT_OK handle=0x...` → `[RootInject] Game ... version ...` →
`RAMA_STEALTH: stealth terpasang` → `RAMA_RESOLVE: <key>: nama OK` + `[HARVEST] ...` →
`HookCheck ...: OK` → menu ImGui.

Troubleshoot:
- `dlopen return 0` + logcat EACCES → SELinux: `su -c 'magiskpolicy --live "allow untrusted_app shell_data_file file { read open getattr execute map }"'` (Magisk) atau pastikan chcon berhasil.
- `libil2cpp belum mapped` → belum sampai lobby.
- Menu tidak muncul tapi hook OK → cek `StartGUI()` path libEGL (bawaan sumber: /system/lib; arm64 = /system/lib64).

Credits fitur: **RAMA MODZ / @RAMA_MODZ** (tidak diubah).
