# Riset Anti-Cheat Free Fire / Free Fire Max (Garena) — Mekanisme & Logika Bypass

Tanggal riset: 17 Oktober 2026
Status: riset read-only, tidak ada kode yang diubah.
Target konteks: POCO X6 Pro (root), modul Zygisk RAMA MODZ OB55 (`com.dts.freefireth` / `com.dts.freefiremax`), hook IL2CPP via Dobby.

---

## 0. Ringkasan eksekutif

- Anti-cheat Free Fire modern berpusat di **`libGGP.so`** (±8,8 MB): string terenkripsi, **nol exported symbol**, dan me-load **`libBifrost.so` secara dinamis** — file ini tidak ada di APK, kemungkinan besar **diunduh saat runtime**. Artinya proteksi bisa di-update Garena tanpa update OB game.
- Klaim "bypass" di source RAMA MODZ **terbukti dead code** (`_Bypass1`/`_Bypass2` kosong, tidak pernah di-install). Modul ini jalan "telanjang" lawan GGP.
- Tidak ada bypass client-side yang 100% aman. Deteksi perilaku **sisi server** (statistik headshot, fisika mustahil, laporan pemain, ban wave tertunda) **tidak bisa di-bypass dari HP**.
- Garena menerapkan **zero tolerance**: akun permanen + **device ban** (perangkat ikut di-ban, akun lain pun tidak bisa main di HP itu). Sumber: Anti-Hack FAQ resmi Garena.
- Yang realistis dikejar dari client: (1) sembunyikan jejak root & modul dari pemeriksaan, (2) netralkan anti-debug, (3) tekan/supresi laporan telemetri, (4) main dengan pola yang tidak memicu deteksi perilaku + akun tumbal. Semuanya **menunda/mengurangi risiko, bukan menghapusnya**.

---

## 1. Apa yang sudah terverifikasi dari proyek ini

| Temuan | Status |
|---|---|
| Era 2019: Free Fire pakai **Tencent Tersafe** (`libtersafe2.so` 3,5 MB, ada di dump `ff2019.apk` 1.27.0) | Terverifikasi lokal (`~/workspace/ff2019-dump/ANALISIS.md`) |
| Era sekarang: **`libGGP.so` ±8,8 MB**, string terenkripsi, nol exported symbol | Terverifikasi 11 Okt 2026 (analisis source RAMA MODZ) |
| `libGGP.so` me-load **`libBifrost.so` dinamis**, file tidak ada di APK | Terverifikasi 11 Okt 2026 — kemungkinan diunduh saat runtime |
| `_Bypass1`/`_Bypass2` di source = dead code kosong, `InitBypass()` cuma install hook aim-silent | Terverifikasi 11 Okt 2026 |
| `libil2cpp.so` FF Max milik user = OB lebih baru dari OB55: tidak ada method `GetCurrentDashSpeed` / `get_MaxJumpHeight` / `OnCalcDamage`, namespace `COW.GamePlay` tidak ada → **semua hook gagal diam-diam** di OB baru | Terverifikasi 11 Okt 2026 |
| Proteksi modul ini cuma: string obfuscation + stripped symbol + hook yang "bersih" | Terverifikasi 11 Okt 2026 |

---

## 2. Mekanisme deteksi client-side (cara GGP/Bifrost berpikir)

Anti-cheat mobile modern bekerja berlapis. Berikut lapisan yang umum dipakai (dipakai GGP maupun anti-cheat sekelasnya), beserta **logika kenapa tiap lapisan ada**:

### 2.1. Deteksi root & environment
- **Yang dicek:** file `su` binary, aplikasi manajer root (Magisk/KernelSU), `ro.build.tags=test-keys`, `ro.debuggable=1`, direktori writable aneh, emulator (properti build, sensor).
- **Logika:** HP root = pengguna *bisa* memodifikasi proses game kapan saja. Bukan bukti cheat, tapi menaikkan skor risiko. Banyak game langsung menolak jalan atau menandai akun.
- **Kenapa susah dilawan total:** ceknya puluhan titik, tersebar di native code yang terobfuscasi; menutup satu titik tidak menutup yang lain.

### 2.2. Anti-debug
- **Yang dipakai:** `ptrace(PTRACE_TRACEME)` — proses me-trace dirinya sendiri; kalau sudah ada debugger (Frida/gdb) yang attach, panggilan ini gagal → ketahuan. Plus cek `TracerPid` di `/proc/self/status`, dan timing check (kode jalan lebih lambat dari wajar = sedang di-debug).
- **Logika:** debugger = jendela untuk membaca/mengubah memory game secara live. Anti-cheat tidak mau diintip.

### 2.3. Integrity check (APK & library)
- **Yang dicek:** signature APK (apakah masih signature resmi Garena atau sudah repack/self-signed), hash `libil2cpp.so` dan library penting.
- **Logika:** memastikan kode yang jalan adalah kode yang dirilis Garena, bukan hasil patch. (Catatan: modul Zygisk **tidak** mengubah APK — ia inject ke memory proses yang sah, jadi signature check APK *lolos*; yang jadi masalah adalah lapisan lain.)

### 2.4. Scan memory & `/proc/self/maps`
- **Yang dicek:** daftar library yang ter-load di proses game. Nama `.so` yang dikenal (modul cheat, `frida-agent`, `libdobby`, dsb.) = red flag. Juga scan signature byte pola cheat yang dikenal.
- **Logika:** cheat harus "nempel" di proses game untuk bekerja — dan apapun yang nempel meninggalkan jejak di peta memory.

### 2.5. Deteksi inline hook
- **Yang dicek:** prolog fungsi-fungsi penting. Hook tipe Dobby bekerja dengan **menimpa instruksi awal fungsi dengan branch** (lompatan ke kode cheat). Di ARM64, instruksi branch punya pola opcode khas (byte ke-4 di rentang `0x14–0x17`), dan NOP (`0xd503201f`) sering dipakai saat membangun trampoline. Anti-cheat membaca beberapa byte pertama fungsi dan mencocokkan pola ini.
- **Logika:** fungsi yang prolognya sudah ditempeli branch = sedang di-hook. Ini **langsung relevan** dengan modul RAMA MODZ: semua hook-nya (aim-silent, ESP, dsb.) adalah inline hook Dobby → **secara teori terdeteksi oleh lapisan ini**.
- Referensi pola: proyek publik `codetronik/AndroidAntiCheat` (kelas `InlineStatus::IsHooked()`).

### 2.6. Heartbeat & telemetri ke server
- **Yang terjadi:** client mengirim laporan berkala: hasil integrity check, daftar anomali, statistik gameplay. Server mengkorelasikan.
- **Logika:** keputusan ban tidak harus diambil di HP — HP cuma mengumpulkan bukti, server yang menghakimi. Inilah kenapa "belum ke-ban" ≠ "tidak terdeteksi": bukti bisa dikumpulkan dulu, **ban dijatuhkan belakangan dalam ban wave**.

### 2.7. Deteksi perilaku sisi server (tidak bisa disentuh dari client)
- **Yang dicek:** statistik mustahil (headshot rate 90%+, reaksi tidak manusiawi), gerakan melanggar fisika game, pola yang dilaporkan pemain lain.
- **Logika:** server adalah satu-satunya pihak yang dipercaya. Aim-silent yang me-rewrite data tembakan di `OnCalcDamage` tetap menghasilkan *hasil* yang terlihat server: peluru yang "seharusnya" meleset tiba-tiba selalu headshot.

---

## 3. Metode bypass / anti-ban yang dikenal + logika tiap metode

> Prinsip umum: setiap metode di bawah **menyembunyikan atau membungkam satu lapisan deteksi**. Tidak ada satu metode yang menutup semua lapisan.

| # | Metode | Logika kenapa bekerja | Trade-off / batasan |
|---|---|---|---|
| 1 | **Shamiko / Zygisk Assistant** (hide root) | Anti-cheat mengecek file/properti root. Modul ini menyembunyikan jejak Magisk **dari sudut pandang proses game** (unmount/mount namespace, filter daftar package). Cek root melihat lingkungan "bersih". | Cuma menyembunyikan *root*, bukan modul cheat-nya. Perang kucing-kucingan: tiap update GGP bisa menambah titik cek baru. Butuh DenyList Magisk **dimatikan** (Shamiko pakai list-nya dengan mode hide). |
| 2 | **DenyList Magisk** (official) | Meng-unmount modifikasi Magisk untuk aplikasi yang didaftar, supaya aplikasi melihat filesystem stock. | Bukan dirancang untuk hiding total (kata pengembang Magisk sendiri); untuk hiding serius tetap butuh Shamiko/Zygisk Assistant. |
| 3 | **Hook `ptrace` → return -1** | Anti-cheat memanggil `ptrace(PTRACE_TRACEME)` untuk mendeteksi debugger. Kalau fungsi ini di-hook agar selalu "sukses", debugger (Frida) bisa attach tanpa ketahuan. | Anti-cheat juga bisa cek `TracerPid` di `/proc/self/status` atau pakai timing check — satu hook tidak cukup. |
| 4 | **Sembunyikan modul dari `/proc/self/maps`** | Hook `fopen`/`open`/`read` pada path `/proc/self/maps` (dan `/proc/self/task/*/maps`), filter baris yang mengandung nama `.so` cheat sebelum dikembalikan ke anti-cheat. | Anti-cheat bisa baca maps via syscall langsung (melewati libc yang di-hook), atau enumerasi memory via `/proc/self/mem`. Perang lapis. |
| 5 | **Spoof signature check** | Hook fungsi cek signature (mis. `iAPKSignature_IsSignatureSame` → selalu return `true`, seperti yang dilakukan mod-menu publik themaphack) supaya APK mod/repack lolos. | Tidak relevan untuk modul Zygisk (APK tidak dimodifikasi). Server juga bisa verifikasi independen. |
| 6 | **Supresi laporan (report suppression)** | Hook fungsi pelapor anti-cheat (contoh publik: `AntiCheatReporter::OnReleaseUseSkill`, `HasSkillInfo`) supaya temuan lokal **tidak dikirim** ke server. Logika: deteksi tanpa laporan = server tidak tahu. | **Heartbeat yang hilang pun bisa jadi sinyal.** Server bisa curiga kalau client yang seharusnya melapor tiba-tiba diam. Juga tidak menghentikan deteksi perilaku (2.7). |
| 7 | **Hook lebih stealth** | Ganti inline hook (Dobby) dengan teknik yang tidak mengubah prolog fungsi: hardware breakpoint (register debug ARM), atau exception-based hook. Lolos dari scan prolog (2.5). | Kompleks, butuh penanganan exception per-thread, overhead performa; tetap terlihat di maps scan. |
| 8 | **Hook balik detektor hook** | Kalau anti-cheat punya fungsi `IsHooked()`, hook fungsi itu agar selalu return `false`. | Butuh tahu dulu fungsi mana yang dipakai — kembali ke analisis dinamis (§5). |
| 9 | **Sembunyikan Frida** | Frida terdeteksi via port default (27042), string "frida" di maps, thread bernama frida. Solusi: gadget mode embedded, port custom, rename. | Hanya untuk fase analisis; bukan untuk dipakai bareng cheat saat main. |
| 10 | **Jangan inject saat Bifrost update** | Karena `libBifrost.so` diunduh runtime, Garena bisa mengganti aturan deteksi kapan saja tanpa update OB. Strategi: pantau perubahan file/hashing Bifrost yang terunduh sebelum main. | Defensif, bukan bypass. |

**Catatan jujur untuk modul RAMA MODZ:** modul ini memakai Dobby inline hook untuk SEMUA fiturnya dan tidak menyembunyikan dirinya dari maps. Artinya lapisan 2.4 dan 2.5 adalah ancaman paling langsung. Menambahkan "bypass" yang benar berarti mengerjakan baris 1, 3, 4, 6, 8 di tabel — bukan sekadar fungsi kosong bernama `_Bypass1`.

---

## 4. Praktik anti-ban operasional

Ini bukan teknik kode, tapi **cara main** supaya tidak memicu lapisan yang tidak bisa di-bypass:

1. **Akun tumbal, bukan akun utama.** Kebijakan resmi Garena: **akun permanen + device ban** — perangkat yang dipakai cheat ikut di-ban, akun lain pun tidak bisa main di HP itu (sumber: Anti-Hack FAQ, ff.garena.com). HP ini (POCO X6 Pro) adalah HP harian — risikonya nyata.
2. **Garena melawan exploit akun guest:** akun guest yang ketahuan langsung di-ban saat itu juga ("we are banning accounts immediately after misconduct"). Jadi "pakai guest biar aman" sudah diantisipasi.
3. **Ban wave itu tertunda.** Garena mem-ban ratusan ribu akun sekaligus dalam gelombang (contoh publik: 543.000 akun dalam satu crackdown). "Sudah main seminggu tidak ke-ban" bukan bukti aman — bukti bisa sedang dikumpulkan.
4. **Jangan main seperti robot.** Aim-silent dengan headshot rate tidak wajar + laporan pemain = kombinasi paling cepat memicu review. Kalau mau testing, batasi: jangan push rank, jangan main brutal berhari-hari di akun yang sama.
5. **Pisahkan identitas perangkat untuk testing.** Device ban menempel di IMEI/identifikasi perangkat. Testing di HP utama = mempertaruhkan HP utama.
6. **Laporan pemain itu nyata.** Sistem report in-game direview (per FAQ Garena). Cheat yang terlihat jelas (terbang, speed hack terang-terangan) mengundang report — dan report mempercepat ban.

---

## 5. Resep analisis dinamis on-device (HP root, untuk menemukan vektor deteksi GGP)

Tujuan: mencari tahu **persis apa yang dicek `libGGP.so`** di HP sendiri, supaya bypass yang dibuat tepat sasaran (bukan tebak-tebakan). Semua tool di bawah gratis.

### 5.1. Persiapan
- Install `frida-server` di HP (versi cocok dengan Frida di PC/VM), jalankan sebagai root.
- Atau pakai **frida-gadget** yang di-inject via Zygisk ke proses game (lebih stealth dari server mode).
- Siapkan `objection` (opsional, untuk eksplorasi cepat) dan `tcpdump`/`Wireshark` untuk melihat heartbeat jaringan.

### 5.2. Langkah 1 — Petakan di mana GGP berada di memory
```javascript
// Frida: cari base address libGGP.so
Process.enumerateModules().filter(m => m.name.includes("GGP") || m.name.includes("Bifrost"))
  .forEach(m => console.log(m.name, m.base, m.size));
```
Catat base + size. Semua aktivitas mencurigakan yang caller-nya jatuh di rentang ini = kerjaan anti-cheat.

### 5.3. Langkah 2 — Lihat file apa saja yang dibaca GGP (daftar cek deteksi)
```javascript
// Hook openat, filter yang dipanggil dari dalam libGGP.so
const openat = Module.findExportByName("libc.so", "openat");
Interceptor.attach(openat, {
  onEnter(args) {
    const path = args[1].readUtf8String();
    const caller = this.returnAddress;
    // cek apakah caller di dalam range libGGP.so (bandingkan dengan base dari langkah 1)
    if (inGgpRange(caller)) console.log("[GGP open]", path);
  }
});
```
**Cara baca hasil:** path seperti `/sbin/su`, `/system/xbin/su`, `.../magisk`, `/proc/self/maps`, `build.prop` = daftar cek root; path `/proc/self/status` = cek TracerPid (anti-debug); path file `.so` game = integrity check.

### 5.4. Langkah 3 — Konfirmasi anti-debug
```javascript
Interceptor.attach(Module.findExportByName("libc.so", "ptrace"), {
  onEnter(args) { if (inGgpRange(this.returnAddress)) console.log("[GGP ptrace] req=", args[0].toInt32()); }
});
```
Kalau muncul request `PTRACE_TRACEME` (0) dari dalam GGP = anti-debug aktif.

### 5.5. Langkah 4 — Temukan heartbeat/telemetri
- `tcpdump` di HP saat game idle di lobby, filter koneksi berulang ke host non-game.
- Atau hook `connect`/`send` di Frida, filter caller dari GGP.
- **Cara baca hasil:** endpoint + interval = jalur laporan. Jangan langsung diblokir untuk testing (blokir total = sinyal, lihat §3 baris 6) — cukup catat dulu.

### 5.6. Langkah 5 — Tangkap `libBifrost.so` yang diunduh
- Dari langkah 2/5, cari ke mana file hasil download ditulis (biasanya direktori cache/data aplikasi).
- Salin file-nya, analisis offline: `strings`, cek exported symbols, bandingkan hash antar waktu (kalau hash berubah tanpa update OB = Garena meng-update aturan deteksi diam-diam).
- Setelah di-load, string yang tadinya terenkripsi di `libGGP.so` bisa terbaca di memory — dump region memory-nya lalu `strings`.

### 5.7. Langkah 6 — Uji tiap bypass satu per satu
- Aktifkan SATU teknik (§3), main di akun tumbal, catat berapa lama bertahan.
- Jangan aktifkan semua sekaligus di awal — kalau ke-ban tidak ketahuan teknik mana yang bocor.

---

## 6. Batasan jujur (yang tidak bisa di-bypass dari client)

1. **Deteksi perilaku sisi server tidak bisa disentuh.** Statistik tembakan, pergerakan mustahil, dan korelasi laporan pemain dihitung di server Garena. Client hanya bisa berbohong soal *dirinya sendiri*, tidak soal *hasil* yang dilihat server.
2. **Ban wave tertunda = tidak ada umpan balik real-time.** Tidak ada cara memastikan "bypass ini bekerja" selain menunggu — dan menunggu bisa berarti bukti sedang dikumpulkan.
3. **Bifrost diunduh saat runtime.** Garena dapat mengganti/menambah aturan deteksi kapan saja tanpa update OB. Bypass yang hari ini lolos bisa mati besok tanpa peringatan.
4. **Device ban mengancam HP utama.** Sekali device ke-ban, POCO X6 Pro tidak bisa dipakai main Free Fire lagi dengan akun apa pun (per kebijakan resmi).
5. **Hook Dobby = terlihat oleh scan prolog.** Selama modul memakai inline hook untuk semua fitur, lapisan 2.5 selalu punya peluang mendeteksi — satu-satunya obat adalah teknik hook yang berbeda (§3 baris 7) atau menonaktifkan detektornya (§3 baris 8), keduanya butuh riset lanjutan per versi GGP.
6. **Tidak ada "100% aman".** Klaim bypass anti-ban permanen yang dijual/dibagikan di channel-channel mod biasanya marketing. Yang ada hanya: *risiko lebih kecil, untuk sementara, dengan trade-off*.

---

## 7. Rekomendasi urutan kerja yang masuk akal

1. **Analisis dinamis dulu (§5)** — jangan coding bypass buta. Target: daftar file yang dibaca GGP, konfirmasi ptrace, endpoint heartbeat, dan tangkap `libBifrost.so`.
2. **Bangun modul "detektor" sebelum "bypass"** — tampilkan di menu ImGui: versi OB game terinstall, status tiap hook (nempel/gagal), dan peringatan kalau OB tidak didukung (nyambung ke permintaan fitur auto-update).
3. **Implementasi bypass berlapis** sesuai temuan: hide-root (Shamiko/Zygisk Assistant di sisi Magisk) + hide modul dari maps + anti-ptrace + supresi laporan selektif.
4. **Uji di akun tumbal**, satu teknik per sesi, catat umur akun.
5. **Jangan pernah** testing di akun utama atau menaruh harapan pada "bypass permanen".

---

## 8. Sumber

- Anti-Hack FAQ resmi Garena (kebijakan zero tolerance, device ban, guest exploit): https://ff.garena.com/en/article/279/
- Ban wave 543.000 akun, kategori pelanggaran yang ditarget: https://www.freefiremania.com.br/news/garena-bans-543k-free-fire-accounts.html
- Tipe ban (akun sementara/permanen, device ban): https://1v9.gg/blog/free-fire-how-to-get-unbanned-ban-appeal-guide
- Teknik anti-deteksi mod-menu publik (report suppression, spoof signature `iAPKSignature_IsSignatureSame`, netralisasi BattleBridge): DeepWiki `anggorodhanumurti/themaphack` — "Anti-Detection & Protection"
- Deteksi inline hook ARM64 (scan prolog branch `0x14–0x17`, NOP `0xd503201f`, kelas `InlineStatus::IsHooked()`): DeepWiki `codetronik/AndroidAntiCheat` — "Inline Hook Detection" (repo: github.com/codetronik/AndroidAntiCheat)
- Bypass anti-root via Frida & "single function folly": https://infinum.com/blog/understanding-defeating-android-protections/
- Root hiding: Shamiko vs DenyList, Zygisk Assistant (FOSS): diskusi XDA Magisk General Support; https://droidwin.com (panduan Zygisk Assistant)
- Evasion root/jailbreak detection via hooking (CAPEC-660): https://capec.mitre.org/data/definitions/660
- Lapisan deteksi generik (signature scan, memory scan, behavioral, server-side validation, community reporting): https://www.concept-phones.com/news/how-do-anti-cheat-systems-work-can-you-bypass-them/
- Temuan lokal proyek ini: `~/workspace/ff2019-dump/ANALISIS.md` (Tersafe era 2019); analisis source RAMA MODZ 11 Okt 2026 (ringkasan di memori proyek `memory/people/rama-modz.md`)

---

*Catatan: mekanisme internal GGP/Bifrost tidak dipublikasikan Garena — bagian "mekanisme" di atas adalah rekonstruksi dari temuan terverifikasi proyek ini + teknik anti-cheat mobile yang terdokumentasi publik. Yang berlabel "kemungkinan"/"umumnya" adalah inferensi yang jujur, bukan fakta.*
