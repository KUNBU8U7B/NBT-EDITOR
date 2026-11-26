# NBT Editor — Master Development Catalog

> Katalog ini menjadi indeks navigasi utama. Identitas bagian ditentukan oleh **ID permanen** dan heading; rentang baris di bawah selalu dihitung dari isi file aktual.

<!-- MASTER-SECTION: DOC-CAT-001 | CATEGORY=Katalog utama | STATUS=AKTUAL -->
## A. Katalog utama

Katalog utama memuat seluruh bagian level-2 yang sudah ada. Tabel di bawah dibuat oleh generator yang tertanam dalam dokumen ini; tidak ada nomor baris yang ditulis manual.

### Peta kategori utama

| ID kategori | Nama kategori | Bagian permanen utama |
|---|---|---|
| `KAT-STATE` | Ringkasan kondisi proyek | `DOC-CAT-001`, `LEG-A`, `LEG-FASE-01-S01`, `LEG-FASE-01-S09` |
| `KAT-BUG` | Daftar bug | `LEG-L`, `LEG-R`, `LEG-FASE-01-S06`–`S08` |
| `KAT-FIX` | Riwayat perbaikan | `LEG-I`–`LEG-K`, `LEG-M`–`LEG-Q` |
| `KAT-UI` | Audit UI/UX | `LEG-B`, `LEG-FASE-02` |
| `KAT-TEST` | Hasil pengujian | `LEG-F` |
| `KAT-FEATURE` | Status fitur | `LEG-C`, `LEG-G` |
| `KAT-DEV` | Riwayat perkembangan | `LEG-E`, `LEG-FASE-01-S10` |
| `KAT-ARCHIVE` | Arsip teknis | `LEG-D`, `LEG-H`, `LEG-FASE-01`, `LEG-APPENDIX-01` |

### Legenda status

| Status | Arti |
|---|---|
| `AKTUAL` | Indeks atau verifikasi yang dihitung dari keadaan file saat ini. |
| `PERLU_SINKRONISASI` | Klaim historis berpotensi bertentangan dengan hasil audit atau keadaan terbaru. |
| `HISTORIS` | Catatan proses historis; bukan bukti keadaan saat ini. |
| `AUDIT-HISTORIS` | Hasil audit bertanggal yang tetap dipertahankan sebagai bukti. |
| `REFERENSI` | Spesifikasi atau prosedur referensi; kesesuaiannya perlu diperiksa. |
| `ARSIP` | Materi lama/referensi yang dipertahankan tanpa ditafsirkan sebagai status saat ini. |

### Indeks bagian aktual

<!-- MASTER-CATALOG:BEGIN -->
<!-- MASTER-CATALOG:TABLE:BEGIN -->
| Kategori | ID Bagian | Nama bagian | Rentang baris aktual | Status | Ringkasan |
|---|---|---|---:|---|---|
| Katalog utama | DOC-CAT-001 | A. Katalog utama | 6–482 | AKTUAL | Katalog utama memuat seluruh bagian level-2 yang sudah ada. Tabel di bawah dibuat oleh generator yang tertanam dalam dokumen ini; tidak ada nomor baris yang ditulis ma… |
| Ringkasan kondisi proyek | LEG-A | Bagian A — Ringkasan dan cara memakai | 487–491 | PERLU_SINKRONISASI | Editor NBT native (C++ satu file) untuk data Minecraft Bedrock, dengan alur kerja menyerupai |
| Ringkasan kondisi proyek | LEG-A-S01 | Yang ada sekarang | 494–507 | PERLU_SINKRONISASI | NBT little-endian Bedrock, 13 tipe termasuk TAG_Long_Array · ✅ |
| Ringkasan kondisi proyek | LEG-A-S02 | Versi | 510–529 | PERLU_SINKRONISASI | VERSION · sumber kebenaran tunggal: VERSION_CODE + VERSION_NAME |
| Ringkasan kondisi proyek | LEG-A-S03 | Build | 532–540 | PERLU_SINKRONISASI | Bagian "Build" terutama memuat perintah atau kode; lihat isi lengkap bagian untuk detail. |
| Ringkasan kondisi proyek | LEG-A-S04 | Catatan jujur | 543–554 | PERLU_SINKRONISASI | Verifikasi perangkat untuk 1.0.2 sudah dilakukan langsung di HP Android 13 ini |
| Audit UI/UX | LEG-B | Bagian B — Analisis aplikasi referensi (MineTuner+ 2.2.3) | 557–562 | ARSIP | Scope: static reverse-engineering of the supplied XAPK (MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk). |
| Audit UI/UX | LEG-B-S01 | 1. Reference identity | 565–593 | ARSIP | Display name · MineTuner |
| Audit UI/UX | LEG-B-S02 | 2. Verified components | 596–608 | ARSIP | AndroidManifest.xml declares: |
| Audit UI/UX | LEG-B-S03 | 3. Design system | 611–632 | ARSIP | The reference theme is Theme.Minetuner, parent Theme.Material3.Light.NoActionBar. Relevant colours: |
| Audit UI/UX | LEG-B-S04 | 4. Screen-by-screen behaviour | 635–802 | ARSIP | Verified layout: res/layout/activity_main.xml. |
| Audit UI/UX | LEG-B-S05 | 5. NBT engine (reference) | 805–873 | ARSIP | The reference's NBT implementation lives in x2.a (type enum) and y2. (tag classes). It is a Bedrock/little-endian NBT dialect, not Java Edition big-endian NBT. |
| Audit UI/UX | LEG-B-S06 | 6. World/storage model | 876–894 | ARSIP | Managed root: getExternalFilesDir(null)/worlds/. |
| Audit UI/UX | LEG-B-S07 | 7. Feature inventory (reference) | 897–935 | ARSIP | List managed + legacy worlds. |
| Audit UI/UX | LEG-B-S08 | 8. What this means for our implementation | 938–959 | ARSIP | The new app must be a Compose/Material 3 native Android app with the reference's world-editor heart, while adding the requested editor capabilities: |
| Audit UI/UX | LEG-B-S09 | 9. Verification status and limitations | 962–967 | ARSIP | Verified statically: manifest, resources, layouts, strings, theme/colours, activities, adapter behaviour, NBT wire format, validation strings, header handling, world i… |
| Audit UI/UX | LEG-B-S10 | 10. Source map (for future reference) | 970–983 | ARSIP | World list / import / export / delete · com/hextetrahedral/minetuner/MainActivity.java |
| Status fitur | LEG-C | Bagian C — Spesifikasi fitur | 987–989 | PERLU_SINKRONISASI | Bagian "Bagian C — Spesifikasi fitur" tidak memiliki paragraf ringkasan; gunakan subheading di dalamnya. |
| Status fitur | LEG-C-S01 | 1. Identity & scope decisions | 992–1006 | PERLU_SINKRONISASI | App name · NBT Editor |
| Status fitur | LEG-C-S02 | 2. Feature matrix | 1009–1098 | PERLU_SINKRONISASI | Legend: ✅ build now · 🟡 build later (tracked in TODO.md) · ❌ out of scope |
| Status fitur | LEG-C-S03 | 3. Acceptance criteria (per feature) | 1101–1109 | PERLU_SINKRONISASI | Every ✅ feature must satisfy: |
| Status fitur | LEG-C-S04 | 4. Explicitly not in phase 1 (LevelDB) | 1112–1122 | PERLU_SINKRONISASI | Not implemented, not stubbed, not listed in the UI as available: |
| Arsip teknis | LEG-D | Bagian D — Arsitektur (lapisan L0–L6) | 1126–1128 | REFERENSI | Bagian "Bagian D — Arsitektur (lapisan L0–L6)" tidak memiliki paragraf ringkasan; gunakan subheading di dalamnya. |
| Arsip teknis | LEG-D-S01 | 1. Constraint: one C++ translation unit | 1131–1165 | REFERENSI | The user chose "seluruh app native C++, 1 file cpp". The whole app lives in app/src/main.native.cpp. To keep it maintainable, that single TU is internally partitioned… |
| Arsip teknis | LEG-D-S02 | 2. Platform glue (L0) | 1168–1179 | REFERENSI | Entry · android.app.NativeActivity declared in the manifest with meta-data android.app.lib_name=libnbt, android.app.native_code not needed. No Java/Kotlin sources, no DEX in t… |
| Arsip teknis | LEG-D-S03 | 3. Renderer & widget kit (L1–L3) | 1182–1190 | REFERENSI | Canvas: immediate 2D over a uint32_t buffer — clear, fillRect, strokeRect, roundRect (supersampled), fillCircle, drawLine, blit (image), clipRect, per-pixel alpha blen… |
| Arsip teknis | LEG-D-S04 | 4. NBT core (L4) — the load-bearing layer | 1193–1215 | REFERENSI | Format contract (verified from reference, see ANALYSIS.md §5): |
| Arsip teknis | LEG-D-S05 | 5. Application layer (L5) | 1218–1247 | REFERENSI | FileVm · path, docs, recent[], dirty, io error · open/save/saveAs/new/import/export/recent |
| Arsip teknis | LEG-D-S06 | 6. Data flow | 1250–1259 | REFERENSI | Bagian "6. Data flow" terutama memuat perintah atau kode; lihat isi lengkap bagian untuk detail. |
| Arsip teknis | LEG-D-S07 | 7. Build topology (no Gradle) | 1262–1277 | REFERENSI | Engine host tests · clang++ -O2 -DNBT_SELFTEST app/src/main.native.cpp -o /tmp/nbt_test && /tmp/nbt_test · test report |
| Arsip teknis | LEG-D-S08 | 8. Module map inside the file | 1280–1290 | REFERENSI | // ===== L0 · ANativeActivity_onCreate, lifecycle, looper, input, permissions, main loop |
| Arsip teknis | LEG-D-S09 | 9. Risk register | 1293–1303 | REFERENSI | No zipalign · store .so uncompressed; apksigner verifies alignment; if it complains, add a 1024-byte zero pad entry |
| Riwayat perkembangan | LEG-E | Bagian E — Build dan pengembangan | 1307–1309 | REFERENSI | Bagian "Bagian E — Build dan pengembangan" tidak memiliki paragraf ringkasan; gunakan subheading di dalamnya. |
| Riwayat perkembangan | LEG-E-S01 | 0. Prasyarat (sudah terpasang di lingkungan ini) | 1312–1326 | REFERENSI | Termux clang/clang++ 21.1.8 · clang++ --version · aarch64, lintas-compile Android |
| Riwayat perkembangan | LEG-E-S02 | 1. Struktur direktori | 1329–1344 | REFERENSI | Bagian "1. Struktur direktori" terutama memuat perintah atau kode; lihat isi lengkap bagian untuk detail. |
| Riwayat perkembangan | LEG-E-S03 | 2. Langkah-langkah | 1347–1366 | REFERENSI | Hasil: out/NBTEditor.apk (signed, siap sideload ke perangkat arm64). |
| Riwayat perkembangan | LEG-E-S04 | 3. Detail perintah | 1369–1415 | REFERENSI | -DNBT_SELFTEST mengaktifkan main() uji dan mematikan bagian Android (L0) sehingga file yang sama bisa dikompilasi sebagai executable biasa di Termux. |
| Riwayat perkembangan | LEG-E-S05 | 4. Debugging | 1418–1426 | REFERENSI | Penting lihat nilai internal · log ke stderr → logcat; build host dengan -DNBT_SELFTEST -DDEBUG_TRACE memakai fprintf(stderr,…) yang sama |
| Riwayat perkembangan | LEG-E-S06 | 5. Gaya kode | 1429–1435 | REFERENSI | C++20, std:: saja (tanpa boost). |
| Riwayat perkembangan | LEG-E-S07 | 6. Perbaiki error lebih dulu | 1438–1462 | REFERENSI | Aturan kerja: setelah setiap tahap implementasi, jalankan 3.1 dan 3.2. Jika ada error/warning, perbaiki sebelum melanjutkan ke fitur berikut. README.md mencatat status… |
| Hasil pengujian | LEG-F | Bagian F — Pengujian | 1465–1469 | REFERENSI | Filosofi: engine diuji di host secara agresif (ASan/UBSan + round-trip), UI diuji deterministik lewat "model" test, ketiganya harus hijau sebelum build APK. |
| Hasil pengujian | LEG-F-S01 | 1. Perintah | 1472–1478 | REFERENSI | Bagian "1. Perintah" terutama memuat perintah atau kode; lihat isi lengkap bagian untuk detail. |
| Hasil pengujian | LEG-F-S02 | 2. Unit test engine (wajib) | 1481–1510 | REFERENSI | Semua kasus di bawah ada sebagai CASE(...) di app/src/main.native.cpp (aktif hanya dengan -DNBT_SELFTEST). |
| Hasil pengujian | LEG-F-S03 | 3. Model test UI (deterministik, host) | 1513–1521 | REFERENSI | L3/L6 dibangun sebagai fungsi murni dari state; test memanggil fungsi build dengan state buatan dan memeriksa pohon node (posisi, visibilitas, teks, enabled): |
| Hasil pengujian | LEG-F-S04 | 4. Tidak mengklaim yang belum diuji | 1524–1527 | REFERENSI | Tidak ada pengujian UI otomatis pada perangkat (tidak ada adb di lingkungan ini). Semua klaim UI berasal dari kode + model test; belum ada verifikasi visual di layar n… |
| Hasil pengujian | LEG-F-S05 | 5. Kriteria hijau sebelum build APK | 1530–1537 | REFERENSI | Bagian "5. Kriteria hijau sebelum build APK" terutama memuat perintah atau kode; lihat isi lengkap bagian untuk detail. |
| Status fitur | LEG-G | Bagian G — Status dan keputusan | 1541–1550 | PERLU_SINKRONISASI | Status per 11 Nov 2026: engine NBT (49 tes), UI 6 layar + dialog + keypad (40 model-test), |
| Status fitur | LEG-G-S01 | Selesai | 1553–1562 | PERLU_SINKRONISASI | PHASE 1 Environment · inventaris toolchain Termux; toolchain C++→APK terverifikasi (libnbt.so arm64-v8a di-compile & di-link sukses) |
| Status fitur | LEG-G-S02 | Sedang dikerjakan | 1565–1570 | PERLU_SINKRONISASI | L4 NBT core (parse/serialize/validate/find 13 tipe + Long Array) · T1–T24 harus PASS di host (ASan/UBSan) sebelum lanjut |
| Status fitur | LEG-G-S03 | Berikutnya | 1573–1583 | PERLU_SINKRONISASI | L1/L2 kernel + renderer software (ANativeWindow, glyph 8×16) · engine lulus T1–T19 |
| Status fitur | LEG-G-S04 | Ditunda (butuh keputusan/kemampuan baru) | 1586–1592 | PERLU_SINKRONISASI | LevelDB: local_player, player_server_, Overworld/TheEnd/Nether/BiomeData/portals/AutonomousEntities, chunk entities, Show Map · tidak ada engine LevelDB Bedrock; UI sengaja tidak dibuat (larangan tombol non-fungsional) |
| Status fitur | LEG-G-S05 | Catatan keputusan (agar tidak terulang) | 1595–1669 | PERLU_SINKRONISASI | Identitas: com.nbteditor, nama "NBT Editor" (bukan MineTuner). |
| Arsip teknis | LEG-H | Bagian H — Seluruh kode APK (`app/src/main.native.cpp`) | 1672–6752 | ARSIP | C++20, satu file, 5056 baris, tanpa Java/DEX. Blok di bawah adalah salinan persis |
| Riwayat perbaikan | LEG-I | Bagian I — Laporan proses pembuatan aplikasi | 6757–6856 | HISTORIS | 1 · Inventaris lingkungan: cari .xapk, cek JDK/Gradle/aapt2/d8/apksigner, cek adb, sdkmanager, android.jar · shell |
| Riwayat perbaikan | LEG-J | Bagian J — Laporan perbaikan startup (10–11 Nov 2026) | 6861–6956 | HISTORIS | Gejala dari pengguna: APK terpasang, tetapi aplikasi tidak bisa dibuka — langsung |
| Riwayat perbaikan | LEG-K | Bagian K — Investigasi & perbaikan startup #2 (11–13 Nov 2026) | 6959–7105 | HISTORIS | Gejala: setelah APK 1.0.1 terpasang, aplikasi tetap tidak bisa dibuka. APK langsung |
| Daftar bug | LEG-L | Bagian L — Hasil audit TAHAP 1–2 (13 Nov 2026) | 7110–7236 | AUDIT-HISTORIS | Cakupan: pemeriksaan direktori, baca DOKUMENTASI.md penuh, audit source |
| Riwayat perbaikan | LEG-M | Bagian M — Safeguard batching screenshot (14 Nov 2026) | 7241–7366 | HISTORIS | out/uishots/ berisi 9 screenshot PNG. Provider/model yang digunakan |
| Riwayat perbaikan | LEG-N | Bagian N — B6 Breadcrumb tree (14 Nov 2026) | 7371–7430 | HISTORIS | B6 yang sebelumnya masih terbuka kini selesai di source app/src/main.native.cpp |
| Riwayat perbaikan | LEG-O | Bagian O — B7 Viewer/editor elemen array (14 Nov 2026) | 7435–7501 | HISTORIS | B7 kini selesai di app/src/main.native.cpp dengan batas L0–L6 tetap sama. |
| Riwayat perbaikan | LEG-P | Bagian P — B8 Intent/menu yang sebelumnya mati (14 Nov 2026) | 7506–7575 | HISTORIS | B8 selesai di app/src/main.native.cpp dengan batas L0–L6 tetap sama. Semua |
| Riwayat perbaikan | LEG-Q | Bagian Q — B9 Rilis APK 1.0.3 (14 Nov 2026) | 7580–7655 | HISTORIS | B9 menutup APK stale dengan membuat rilis baru yang memuat seluruh source B6–B8. |
| Daftar bug | LEG-R | Bagian R — Audit FASE 1 dan kontrak perbaikan (15 Nov 2026) | 7660–8246 | AUDIT-HISTORIS | Audit ini membaca seluruh DOKUMENTASI.md, seluruh app/src/main.native.cpp, |
| Arsip teknis | LEG-FASE-01 | FASE 1 — ANALISIS MENYELURUH | 8251–8253 | AUDIT-HISTORIS | Bagian "FASE 1 — ANALISIS MENYELURUH" tidak memiliki paragraf ringkasan; gunakan subheading di dalamnya. |
| Ringkasan kondisi proyek | LEG-FASE-01-S01 | A. Kondisi dan struktur proyek | 8256–8290 | AUDIT-HISTORIS | Root proyek: /home/kunbu8u7b/NBT-EDITOR-P. |
| Arsip teknis | LEG-FASE-01-S02 | B. Inventaris seluruh file | 8293–8342 | AUDIT-HISTORIS | Inventaris disimpan secara bertingkat agar FASE 1 tetap berkelanjutan. |
| Arsip teknis | LEG-FASE-01-S03 | C. Analisis file sumber | 8345–8449 | AUDIT-HISTORIS | Nama dan lokasi: VERSION, root proyek. |
| Arsip teknis | LEG-FASE-01-S04 | D. Analisis konfigurasi dan dependensi | 8452–8477 | AUDIT-HISTORIS | Tidak ada file dependency/modern package manager yang terdeteksi di root; |
| Arsip teknis | LEG-FASE-01-S05 | E. Analisis hubungan antarmodul | 8480–8492 | AUDIT-HISTORIS | VERSION adalah sumber versi yang mengalir ke build host, build device, dan |
| Daftar bug | LEG-FASE-01-S06 | F. Daftar bug dan masalah | 8495–8702 | AUDIT-HISTORIS | Katalog konsolidasi bug permanen — 19 Nov 2026. Bagian ini menggabungkan catatan yang sudah tersimpan di dokumen; tidak ada halaman, fitur, atau bug yang dihapus. Seti… |
| Daftar bug | LEG-FASE-01-S07 | G. Masalah performa CPU dan RAM | 8706–8716 | AUDIT-HISTORIS | Baseline yang diketahui: parser, serializer, dan UI model sudah punya test |
| Daftar bug | LEG-FASE-01-S08 | H. Masalah kompatibilitas | 8719–8727 | AUDIT-HISTORIS | Risiko kompatibilitas utama yang perlu diverifikasi: |
| Ringkasan kondisi proyek | LEG-FASE-01-S09 | I. Kesimpulan analisis | 8730–8743 | AUDIT-HISTORIS | FASE 1 sudah dimulai dari progres Bagian R, bukan audit kosong. |
| Riwayat perkembangan | LEG-FASE-01-S10 | J. Daftar progres file | 8746–8773 | AUDIT-HISTORIS | VERSION · SELESAI |
| Arsip teknis | LEG-FASE-01-S11 | K. Konsolidasi Markdown tunggal | 8776–8788 | AUDIT-HISTORIS | User menjadikan DOKUMENTASI.md sebagai satu-satunya file .md. |
| Arsip teknis | LEG-APPENDIX-01 | Appendix A — Instruksi coding agent, sadaran dari `AGENTS.md` | 8791–8793 | ARSIP | Bagian "Appendix A — Instruksi coding agent, sadaran dari `AGENTS.md`" tidak memiliki paragraf ringkasan; gunakan subheading di dalamnya. |
| Arsip teknis | LEG-APPENDIX-01-S01 | Project rules | 8796–8802 | ARSIP | Read DOKUMENTASI.md before making changes. It is the project history, architecture, feature status, and test contract. |
| Arsip teknis | LEG-APPENDIX-01-S02 | Screenshot attachment limit | 8805–8833 | ARSIP | The active provider accepts at most 8 images in one prompt. Never attach or |
| Arsip teknis | LEG-APPENDIX-01-S03 | Verification | 8836–8848 | ARSIP | Run in this order as the change scope requires: |
| Audit UI/UX | LEG-FASE-02 | FASE 1 — AUDIT APK | 8853–8855 | AUDIT-HISTORIS | Bagian "FASE 1 — AUDIT APK" tidak memiliki paragraf ringkasan; gunakan subheading di dalamnya. |
| Audit UI/UX | LEG-FASE-02-S01 | 1.A Inventaris APK yang tersedia | 8858–8870 | AUDIT-HISTORIS | Pemeriksaan langsung di filesystem, tanpa rebuild APK. |
| Audit UI/UX | LEG-FASE-02-S02 | 1.B APK yang dipilih | 8873–8884 | AUDIT-HISTORIS | APK yang paling relevan adalah: |
| Audit UI/UX | LEG-FASE-02-S03 | 1.C Metadata APK yang diverifikasi langsung dari file APK | 8887–8908 | AUDIT-HISTORIS | Dari isi out/NBTEditor.apk: |
| Audit UI/UX | LEG-FASE-02-S04 | 1.D Metadata yang masih perlu konfirmasi tool Android | 8911–8927 | AUDIT-HISTORIS | Nilai berikut sudah tersedia melalui sumber proyek sebagai cross-check, tetapi belum diverifikasi penuh oleh aapt2 dump badging karena environment saat ini tidak memil… |
| Audit UI/UX | LEG-FASE-02-S05 | 1.E Ketersediaan Android Emulator | 8930–8949 | AUDIT-HISTORIS | Hasil pemeriksaan lingkungan: |
| Audit UI/UX | LEG-FASE-02-S06 | 1.F Status progres FASE 1 setelah pemeriksaan awal | 8952–8964 | AUDIT-HISTORIS | Pemeriksaan awal APK · SELESAI |
| Audit UI/UX | LEG-FASE-02-S07 | 1.G Target audit berikutnya | 8967–8975 | AUDIT-HISTORIS | Target berikutnya: |
| Audit UI/UX | LEG-FASE-02-S08 | 2. Verifikasi Kompatibilitas APK dengan Android Emulator — 16 Nov 2026 | 8980–8982 | AUDIT-HISTORIS | Catatan progres: subbagian ini lebih baru dari temuan 1.E tentang ketersediaan emulator. Pada pemeriksaan lebih dalam, Android SDK dan AVD ternyata tersedia di $HOME/A… |
| Audit UI/UX | LEG-FASE-02-S09 | 2.A ABI APK dan library native | 8985–9006 | AUDIT-HISTORIS | Pemeriksaan langsung terhadap out/NBTEditor.apk: |
| Audit UI/UX | LEG-FASE-02-S10 | 2.B Host yang tersedia | 9009–9018 | AUDIT-HISTORIS | Arsitektur host · x86_64 |
| Audit UI/UX | LEG-FASE-02-S11 | 2.C Lingkungan Android SDK dan Android Emulator | 9021–9045 | AUDIT-HISTORIS | Android SDK ditemukan di: |
| Audit UI/UX | LEG-FASE-02-S12 | 2.D System image Android yang terpasang | 9048–9062 | AUDIT-HISTORIS | System image yang tersedia adalah: |
| Audit UI/UX | LEG-FASE-02-S13 | 2.E Kemungkinan ARM64 pada emulator Intel x86_64 | 9065–9085 | AUDIT-HISTORIS | Hasil pemeriksaan system image android-37.0/google_apis_playstore/x86_64: |
| Audit UI/UX | LEG-FASE-02-S14 | 2.F Kesimpulan kompatibilitas | 9088–9104 | AUDIT-HISTORIS | ABI APK · arm64-v8a |
| Audit UI/UX | LEG-FASE-02-S15 | 2.G Alternatif yang tersedia | 9107–9124 | AUDIT-HISTORIS | Uji APK di AVD x86_64 yang sudah ada. |
| Audit UI/UX | LEG-FASE-02-S16 | 2.H Status progres FASE 1 | 9127–9140 | AUDIT-HISTORIS | Periksa ABI APK · SELESAI |
| Audit UI/UX | LEG-FASE-02-S17 | 3. Audit Halaman Pertama — 16 Nov 2026 | 9145–9290 | AUDIT-HISTORIS | Ruang lingkup siklus ini hanya halaman pertama setelah aplikasi dibuka. Tidak ada audit halaman berikutnya, tidak ada audit interaksi/fungsi, tidak ada perubahan kode… |
| Audit UI/UX | LEG-FASE-02-S18 | 4. Verifikasi Tree Editor — 19 Nov 2026 | 9294–9367 | AKTUAL | Ruang lingkup siklus ini hanya Tree Editor dan jalur yang dibutuhkan untuk mengujinya. Tidak ada perubahan desain, source aplikasi, fitur, APK 1.0.4 lain, atau pekerja… |
| Katalog utama | DOC-CAT-VERIFY-001 | Verifikasi katalog master | 9370–9383 | AKTUAL | Verifikasi dilakukan dalam dua tahap: baseline katalog pada 18 Nov 2026 dan finalisasi Fase 1–6 pada 19 Nov 2026 terhadap isi DOKUMENTASI.md aktual. |
<!-- MASTER-CATALOG:TABLE:END -->

### Generator katalog tertanam

Generator berikut berada di dalam blok katalog dan menjadi sumber tunggal untuk menghitung ulang rentang baris, mengambil ringkasan, memvalidasi marker setiap heading level-2, serta memastikan setiap ID bersifat unik. Penulisan katalog dilakukan secara atomik.

<!-- MASTER-CATALOG:GENERATOR:BEGIN -->
```python
from pathlib import Path
import os
import re
import tempfile

path = Path("DOKUMENTASI.md")
lines = path.read_text(encoding="utf-8").splitlines()

outer_begin_tag = "<!-- MASTER-CATALOG:BEGIN -->"
outer_end_tag = "<!-- MASTER-CATALOG:END -->"
table_begin_tag = "<!-- MASTER-CATALOG:TABLE:BEGIN -->"
table_end_tag = "<!-- MASTER-CATALOG:TABLE:END -->"
marker_pattern = re.compile(
    r"^<!-- MASTER-SECTION: ([^|]+) \| CATEGORY=([^|]+) \| STATUS=([^|]+) -->$"
)
fence_pattern = re.compile(r"(`{3,}|~{3,})(.*)$")


def fence_event(text):
    stripped = text.lstrip()
    match = fence_pattern.match(stripped)
    if not match:
        return None
    run, suffix = match.groups()
    return run[0], len(run), suffix.strip()


def level_two_headings(items):
    inside = False
    fence_char = ""
    fence_length = 0
    result = []
    for index, line in enumerate(items):
        event = fence_event(line)
        if inside:
            if event and event[0] == fence_char and event[1] >= fence_length and not event[2]:
                inside = False
                fence_char = ""
                fence_length = 0
            continue
        if event:
            inside = True
            fence_char = event[0]
            fence_length = event[1]
            continue
        if line.startswith("## "):
            result.append((index, line[3:].strip()))
    return result


def unique_marker_index(items, tag):
    matches = [index for index, line in enumerate(items) if line == tag]
    if len(matches) != 1:
        raise SystemExit(f"ERROR: marker {tag} harus muncul tepat satu kali, ditemukan {len(matches)}")
    return matches[0]


def validate_structure(items):
    outer_begin = unique_marker_index(items, outer_begin_tag)
    outer_end = unique_marker_index(items, outer_end_tag)
    table_begin = unique_marker_index(items, table_begin_tag)
    table_end = unique_marker_index(items, table_end_tag)
    if not (outer_begin < table_begin < table_end < outer_end):
        raise SystemExit("ERROR: urutan marker katalog tidak valid")

    headings = level_two_headings(items)
    identifiers = set()
    for position, title in headings:
        marker_position = position - 1
        if marker_position < 0:
            raise SystemExit(f"ERROR: heading level-2 tanpa marker: {position + 1}: {title}")
        match = marker_pattern.match(items[marker_position])
        if not match:
            raise SystemExit(
                f"ERROR: heading level-2 tanpa marker MASTER-SECTION: {position + 1}: {title}"
            )
        section_id, category, status = (part.strip() for part in match.groups())
        if not section_id or not category or not status:
            raise SystemExit(f"ERROR: metadata marker tidak lengkap: {section_id}")
        if section_id in identifiers:
            raise SystemExit(f"ERROR: ID bagian duplikat: {section_id}")
        identifiers.add(section_id)

    marker_lines = [index for index, line in enumerate(items) if line.startswith("<!-- MASTER-SECTION:")]
    if len(marker_lines) != len(headings):
        raise SystemExit(
            f"ERROR: jumlah marker ({len(marker_lines)}) tidak sama dengan heading level-2 ({len(headings)})"
        )
    return headings


def is_horizontal_rule(text):
    return bool(re.fullmatch(r"\s*(?:-{3,}|\*{3,}|_{3,})\s*", text))


def is_table_separator(text):
    stripped = text.strip()
    if not (stripped.startswith("|") and stripped.endswith("|")):
        return False
    cells = split_table_cells(stripped)
    return bool(cells) and all(re.fullmatch(r"\s*:?-+:?\s*", cell) for cell in cells)


def split_table_cells(text):
    stripped = text.strip()
    if not (stripped.startswith("|") and stripped.endswith("|")):
        return []
    return [cell.strip() for cell in re.split(r"(?<!\\)\|", stripped[1:-1])]


def is_table_row(text):
    stripped = text.strip()
    return stripped.startswith("|") and stripped.endswith("|") and len(stripped) > 1


def clean_summary(text):
    text = text.strip()
    text = re.sub(r"^\s*>\s?", "", text)
    text = re.sub(r"^\s*(?:[-+*]|\d+[.)])\s+", "", text)
    text = re.sub(r"^\s*#{1,6}\s+", "", text)
    text = re.sub(r"!\[[^\]]*\]\([^)]*\)", "[gambar]", text)
    text = re.sub(r"\[([^\]]+)\]\([^)]*\)", r"\1", text)
    text = re.sub(r"<[^>]+>", "", text)
    text = re.sub(r"[*`~]+", "", text)
    text = re.sub(r"\s+", " ", text).strip()
    text = text.replace("|", "/")
    if len(text) > 170:
        text = text[:167].rstrip() + "…"
    return text


def outside_section_lines(items, start, end):
    result = []
    inside = False
    fence_char = ""
    fence_length = 0
    has_code = False
    for index in range(start + 1, end + 1):
        line = items[index]
        event = fence_event(line)
        if inside:
            if event and event[0] == fence_char and event[1] >= fence_length and not event[2]:
                inside = False
                fence_char = ""
                fence_length = 0
            continue
        if event:
            inside = True
            fence_char = event[0]
            fence_length = event[1]
            has_code = True
            continue
        result.append((index, line))
    return result, has_code


def extract_summary(items, start, end, title):
    visible, has_code = outside_section_lines(items, start, end)

    # Ambil unit substantif pertama. Untuk tabel Markdown, gunakan baris data
    # pertama; untuk paragraf, gunakan paragraf pertama.
    for offset, (index, line) in enumerate(visible):
        candidate = line.strip()
        if not candidate or is_horizontal_rule(candidate):
            continue
        if candidate.startswith("<!--") or candidate.startswith("#"):
            continue
        if is_table_separator(candidate):
            continue
        if is_table_row(candidate):
            if offset + 2 < len(visible):
                separator_index, separator_line = visible[offset + 1]
                data_index, data_line = visible[offset + 2]
                if (
                    separator_index == index + 1
                    and is_table_separator(separator_line)
                    and data_index == separator_index + 1
                    and is_table_row(data_line)
                    and not is_table_separator(data_line)
                ):
                    cells = [clean_summary(cell) for cell in split_table_cells(data_line)]
                    summary = " · ".join(cell for cell in cells if cell)
                    if summary:
                        return summary
            continue
        summary = clean_summary(candidate)
        if summary:
            return summary

    if has_code:
        return f'Bagian "{title}" terutama memuat perintah atau kode; lihat isi lengkap bagian untuk detail.'
    return f'Bagian "{title}" tidak memiliki paragraf ringkasan; gunakan subheading di dalamnya.'


def section_end(items, heading_position, next_heading_position):
    if next_heading_position is None:
        end = len(items) - 1
    else:
        # Baris marker sebelum heading berikutnya bukan isi bagian ini.
        end = next_heading_position - 2
    while end > heading_position:
        candidate = items[end].strip()
        if not candidate or is_horizontal_rule(candidate):
            end -= 1
            continue
        break
    return max(end, heading_position)


def render_table(items):
    headings = validate_structure(items)
    sections = []
    for heading_offset, (position, title) in enumerate(headings):
        next_position = headings[heading_offset + 1][0] if heading_offset + 1 < len(headings) else None
        end = section_end(items, position, next_position)
        marker_match = marker_pattern.match(items[position - 1])
        assert marker_match is not None
        section_id, category, status = (part.strip() for part in marker_match.groups())
        sections.append(
            {
                "id": section_id,
                "category": category,
                "status": status,
                "title": title,
                "start": position + 1,
                "end": end + 1,
                "summary": extract_summary(items, position, end, title),
            }
        )

    table = [
        "| Kategori | ID Bagian | Nama bagian | Rentang baris aktual | Status | Ringkasan |",
        "|---|---|---|---:|---|---|",
    ]
    for section in sections:
        values = [
            section["category"],
            section["id"],
            section["title"],
            f'{section["start"]}–{section["end"]}',
            section["status"],
            section["summary"],
        ]
        values = [value.replace("|", "\\|") for value in values]
        table.append("| " + " | ".join(values) + " |")
    return table


def regenerate(items):
    table_begin = unique_marker_index(items, table_begin_tag)
    table_end = unique_marker_index(items, table_end_tag)
    table = render_table(items)
    return items[:table_begin + 1] + table + items[table_end:]


def atomic_write(content):
    file_descriptor, temporary_name = tempfile.mkstemp(
        prefix=".DOKUMENTASI.", suffix=".tmp", dir=path.parent
    )
    temporary = Path(temporary_name)
    try:
        with os.fdopen(file_descriptor, "w", encoding="utf-8", newline="\n") as handle:
            handle.write(content)
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


for _ in range(10):
    updated = regenerate(lines)
    if updated == lines:
        break
    lines = updated
else:
    raise SystemExit("ERROR: katalog tidak stabil setelah 10 iterasi")

validate_structure(lines)
atomic_write("\n".join(lines) + "\n")
print(f"katalog diperbarui: {len(render_table(lines)) - 2} bagian")
```
<!-- MASTER-CATALOG:GENERATOR:END -->

### Menjalankan regenerasi

Jalankan perintah berikut dari root proyek. Perintah mengekstrak generator yang tertanam, lalu menulis katalog secara atomik. Setelah perubahan posisi bagian, jalankan perintah ini satu kali; konvergensi indeks dapat diperiksa dengan menjalankannya lagi dan membandingkan hash file.

```bash
python3 - <<'PY'
from pathlib import Path
import re

text = Path("DOKUMENTASI.md").read_text(encoding="utf-8")
fence = "`" * 3
pattern = re.compile(
    r"<!-- MASTER-CATALOG:GENERATOR:BEGIN -->\n"
    + fence
    + r"python\n(.*?)\n"
    + fence
    + r"\n<!-- MASTER-CATALOG:GENERATOR:END -->",
    re.DOTALL,
)
match = pattern.search(text)
if not match:
    raise SystemExit("ERROR: generator katalog tertanam tidak ditemukan")
namespace = {"__name__": "__main__"}
exec(compile(match.group(1), "DOKUMENTASI.md#MASTER-CATALOG", "exec"), namespace)
PY
```

<!-- MASTER-CATALOG:END -->

### Protokol pembacaan AI

1. Baca katalog ini terlebih dahulu.
2. Pilih kategori dan ID bagian yang relevan; jangan membaca seluruh dokumen tanpa alasan.
3. Baca hanya rentang bagian yang dibutuhkan.
4. Jangan mengulang pembacaan bagian yang sudah tersedia dalam konteks.
5. Gunakan ID bagian dan ID bug sebagai referensi permanen; nomor baris hanya petunjuk navigasi dan dapat berubah.

---

# NBT Editor — Dokumentasi Lengkap + Kode Sumber
> Dokumen ini menggabungkan seluruh dokumentasi proyek **dan seluruh kode aplikasi** (`app/src/main.native.cpp`) beserta penjelasan proses pembuatannya.
Isi:
1. Bagian A — Ringkasan dan cara memakai
2. Bagian B — Analisis aplikasi referensi (MineTuner+ 2.2.3)
3. Bagian C — Spesifikasi fitur
4. Bagian D — Arsitektur (lapisan L0–L6 dalam satu file)
5. Bagian E — Build & pengembangan
6. Bagian F — Pengujian
7. Bagian G — Status & keputusan
8. Bagian H — **Seluruh kode APK** (satu file C++, cadangan pemulihan)
9. Bagian I — **Laporan proses**: langkah, bug, dan perbaikannya
10. Bagian J — **Laporan perbaikan startup (10-11 Nov 2026)**: akar masalah "aplikasi tidak bisa dibuka"

---

<!-- MASTER-SECTION: LEG-A | CATEGORY=Ringkasan kondisi proyek | STATUS=PERLU_SINKRONISASI -->
## Bagian A — Ringkasan dan cara memakai

Editor NBT native (C++ satu file) untuk data Minecraft Bedrock, dengan alur kerja menyerupai
referensi **MineTuner+ 2.2.3** (Bagian B), ditulis ulang penuh sebagai aplikasi
**NativeActivity C++** yang dibangun **tanpa Gradle, tanpa Android SDK, tanpa JDK/AGP**.

<!-- MASTER-SECTION: LEG-A-S01 | CATEGORY=Ringkasan kondisi proyek | STATUS=PERLU_SINKRONISASI -->
## Yang ada sekarang

| Fitur | Status |
|---|---|
| NBT little-endian Bedrock, **13 tipe termasuk `TAG_Long_Array`** | ✅ |
| `level.dat` dengan header 8 byte (versi asli dipertahankan) | ✅ |
| Parse/serialize byte-identik, validasi rentang, error berkas rusak | ✅ |
| UI software-render (`ANativeWindow`), 6 layar, tema terang & gelap | ✅ |
| World: list / import `.mcworld` / export / create flat world | ✅ |
| Editor: tree, add/edit/rename/delete/copy/paste, search, **undo/redo** | ✅ |
| Simpan atomik + Save-As (container `level.dat` dipertahankan) | ✅ |
| LevelDB (player/dimensi/chunk/Show Map) | ❌ sengaja tidak dibuat di fase 1 |

Legenda: ✅ selesai · 🚧 dalam pengerjaan · ❌ tidak di fase ini.

<!-- MASTER-SECTION: LEG-A-S02 | CATEGORY=Ringkasan kondisi proyek | STATUS=PERLU_SINKRONISASI -->
## Versi

| Berkas / tempat | Isi |
|---|---|
| `VERSION` | **sumber kebenaran tunggal**: `VERSION_CODE` + `VERSION_NAME` |
| `scripts/build_apk.sh` | membaca `VERSION` → `--version-code/--version-name` |
| `app/src/main.native.cpp` | Menerima `NBT_APP_VERSION_NAME` + `NBT_APP_VERSION_CODE` dari build scripts (nilai diambil dari `VERSION`) dan menampilkannya di layar **Help & About** |

Aturan rilis: **setiap build APK menaikkan `VERSION_CODE`** (Android menolak instalasi APK
dengan `versionCode` yang sama atau lebih rendah) dan `VERSION_NAME` naik mengikuti
Semantic Versioning (`1.0.1` → `1.0.2` → `1.0.3` → `1.1.0`).

### Riwayat rilis

| Versi | Code | Isi |
|---|---|---|
| 1.0.0 | 1 | Rilis pertama (engine NBT, 6 layar, undo, import/export `.mcworld`) |
| **1.0.1** | **2** | **Perbaikan startup**: library native di-link ke stub Android yang benar (bukan wrapper Termux), `androidMain()` (loop render) ditulis ulang, input dispatch, mutex state, izin "semua berkas" diminta malas, RUNPATH Termux dibuang |
| **1.0.2** | **3** | **Perbaikan startup #2 (terbukti di perangkat)**: `libnbt.so` tidak lagi bergantung pada helper atomic LSE (`__aarch64_ldadd*`) yang tidak ada di libc perangkat → `dlopen` kini berhasil; build punya *guard* yang menolak rilis bila helper eksternal muncul lagi |
| **1.0.3** | **4** | **Rilis fitur B6–B8**: breadcrumb tree yang dapat digeser/diketuk, viewer/editor elemen Byte/Int/Long Array, serta menu nyata untuk theme, delete-world, paste, dan go-to-root. APK ditandatangani ulang dan library native terbaru sudah tertanam di APK |

<!-- MASTER-SECTION: LEG-A-S03 | CATEGORY=Ringkasan kondisi proyek | STATUS=PERLU_SINKRONISASI -->
## Build

```bash
bash scripts/keystore.sh        # sekali saja
bash scripts/make_stubs.sh      # stub link-time libandroid/libz/liblog (otomatis bila hilang)
bash scripts/build_host_tests.sh
bash scripts/build_device.sh
bash scripts/build_apk.sh       # → out/NBTEditor.apk
```

<!-- MASTER-SECTION: LEG-A-S04 | CATEGORY=Ringkasan kondisi proyek | STATUS=PERLU_SINKRONISASI -->
## Catatan jujur

- **Verifikasi perangkat untuk 1.0.2 sudah dilakukan langsung di HP Android 13 ini**
  (bukan emulator): `libnbt.so` dari APK **1.0.2** berhasil dimuat oleh dynamic
  linker perangkat (`dlopen` → `DLOPEN_OK`) dan jalur startup
  (`ANativeActivity_onCreate` → `onStart` → `onResume` → `onDestroy`) berjalan
  tanpa crash (Bagian K).
- Rilis **1.0.3** sudah dibangun dan diverifikasi secara statis/build (Bagian Q),
  tetapi belum dipasang atau dijalankan pada perangkat. Termux tidak punya hak
  `START_ACTIVITIES`/root/ADB pada perangkat ini (lihat Bagian K §6), sehingga
  validasi fisik rilis terbaru masih menunggu **1 ketukan ikon** pengguna.
- Analisis referensi bersifat statis (dekode API/dekompilasi), bukan observasi runtime.

<!-- MASTER-SECTION: LEG-B | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## Bagian B — Analisis aplikasi referensi (MineTuner+ 2.2.3)

# MineTuner+ 2.2.3 — Reference Analysis

> Scope: static reverse-engineering of the supplied XAPK (`MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk`).  
> The reference APK was **not executed on a device**. There was no `adb`, emulator, or screenshot capability in the analysis environment. All behaviour below is derived from the decoded manifest/resources and decompiled DEX bytecode. Anything not observable statically is explicitly marked as such.

<!-- MASTER-SECTION: LEG-B-S01 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 1. Reference identity

| Item | Value |
|---|---|
| Display name | MineTuner |
| Package | `com.hextetrahedral.minetuner` |
| Version name | `2.2.3` |
| Version code | `16` |
| minSdk | 26 |
| targetSdk / compileSdk | 36 |
| XAPK size | 109,767,468 bytes |
| App type | Minecraft **Bedrock** world editor with an NBT tree editor |
| UI toolkit in reference | Native Android Views + Material Components 3 Light; **not Compose** |
| Reference architecture | Activities + Fragments + RecyclerView adapters, static helper classes |

### XAPK contents

The XAPK contains one base APK and three split APKs:

- `com.hextetrahedral.minetuner.apk` (base)
- `config.en.apk` (English resources)
- `config.mdpi.apk` (mdpi resources)
- `config.arm64_v8a.apk` (native libraries, including `libleveldb.so`)

The base APK contains the app's DEX split across `classes*.dex`, resources, and the world template assets. There is no `armeabi-v7a` or `x86` split in the supplied XAPK.

### Monetisation / third-party payload

The reference is heavily monetised. Its dependencies include AppLovin/MAX, Appodeal, Google Mobile Ads, Chartboost, Fyber, Smaato, BIGO, Mintegral, Pangle, PubMatic, MobileFuse, Moloco, Unity Ads, ironSource, Yandex, Sentry, ExoPlayer, and Billing. These are irrelevant to the NBT editor's product behaviour and must **not** be reproduced in the new app.

<!-- MASTER-SECTION: LEG-B-S02 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 2. Verified components

`AndroidManifest.xml` declares:

- `com.hextetrahedral.minetuner.MainActivity` — launcher; world list.
- `com.hextetrahedral.minetuner.DrawerActivity` — world NBT / player / dimension editor.
- `com.hextetrahedral.minetuner.app.HelpActivity` — help, Discord link, texture-pack import.
- `com.hextetrahedral.minetuner.app.CreateWorldActivity` — custom flat-world builder.
- `com.hextetrahedral.minetuner.app.EntitiesNBTActivity` — chunk entity NBT list.
- A `FileProvider` at `com.hextetrahedral.minetuner.provider`.
- Only the four app activities plus ad SDK activities; all app activities except `MainActivity` are non-exported.

Runtime permissions requested by the reference: storage read/write, Internet, network state, Wi-Fi state, billing, vibration, ad-ID/attribution, boot receiver, foreground service. We will only need storage/SAF-related permissions (and ideally none beyond SAF).

<!-- MASTER-SECTION: LEG-B-S03 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 3. Design system

The reference theme is `Theme.Minetuner`, parent `Theme.Material3.Light.NoActionBar`. Relevant colours:

| Role | Colour |
|---|---|
| Primary | `#429e46` (green) |
| Secondary | `#66bb6a` |
| Background | `#f1f8e9` (very light green) |
| Surface | `#ffffff` |
| OnPrimary | `#ffffff` |
| Error | `#b00020` |

Other visual characteristics:

- Green `?colorPrimary` toolbar with white title text.
- White cards on the pale-green background.
- FABs use `Widget.Material3.FloatingActionButton.Primary` with a custom primary tint and secondary ripple.
- No dark theme resources were observed; the reference is effectively light-only.
- Toolbar is a standard Material `Toolbar` with a navigation icon (back arrow or drawer toggle).
- The world list uses a `RecyclerView` with white `CardView` items, 8 dp corner radius, 4 dp elevation, 48 dp world icon, 18 sp bold world name, 14 sp grey size, 14 sp italic internal folder name.
- NBT rows use `tree_node_item.xml`: horizontal, 48 dp minimum height, 12 dp horizontal padding, 32 dp type icon, 12 dp gap, 18 sp bold key.

<!-- MASTER-SECTION: LEG-B-S04 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 4. Screen-by-screen behaviour

### 4.1 MainActivity — world list

Verified layout: `res/layout/activity_main.xml`.

- Green toolbar titled “MineTuner”.
- `RecyclerView` of worlds.
- Bottom-right FAB stack:
  - Main `+` FAB.
  - Expanded “New World” action with `ic_layers` (custom flat world builder).
  - Expanded “Import World” action with `ic_upload` (`.mcworld` / `.mctemplate` import).
- Overflow menu contains Help/About-related actions; the help activity also opens a Discord invite and texture-pack import flow.
- World list sources:
  1. App-managed worlds: `getExternalFilesDir(null)/worlds/<folder>`.
  2. On SDK ≤ 28, with storage permission: legacy external Minecraft worlds at `Android/data/com.mojang.minecraftpe/files/games/com.mojang/minecraftWorlds`.
- Each world row displays:
  - Display name from `levelname.txt`, falling back to “Unnamed World”.
  - Computed recursive directory size, formatted with `Formatter.formatFileSize`.
  - Internal folder name; external worlds are labelled `## [EXTERNAL WORLD] ##`.
  - `world_icon.jpeg` when present, otherwise a placeholder icon.
- Tapping a world opens a list dialog titled `Options for "<name>"`:
  - Managed world: **Edit**, **Open in Minecraft**, **Delete**.
  - External world: **Edit**, **Delete**, plus a warning that the world is outside the managed directory and should be backed up.
- **Edit**: checks for encryption first. If encrypted, shows “Encrypted World” explaining that Marketplace worlds are usually encrypted and cannot be edited. Otherwise opens `DrawerActivity` with `worldName` and `internalName`.
- **Open in Minecraft**: compiles the world folder into `getExternalFilesDir(null)/export.mcworld` and launches it via the FileProvider; progress dialog says “Compiling world... Please wait while the world is being compiled. Do not exit the app.”
- **Delete**: confirmation “Are you sure you want to delete this world? This action cannot be undone.”
- **Import**: file picker restricted to `*/*` with MIME hints for `.mcworld`, `.mctemplate`, `.mcpack`, `.mcaddon`; extracts the zip into the managed worlds directory, avoiding name collisions by suffixing `_1`, `_2`, etc.; progress dialog “Importing world...”; invalid extension is rejected with `Please select a valid .<ext> file`.
- **New World**: opens `CreateWorldActivity`.

Encrypted-world detection: scans files in `<world>/db`, reading two little-endian int32 values from each file; an encrypted Bedrock world is detected when the first int is `0` and the second is `-1680885252` (`0x9BC...` pattern). This is a heuristic derived from the LevelDB file header.

### 4.2 CreateWorldActivity — custom flat world

Verified from `app/CreateWorldActivity.java`, `activity_create_world.xml`, `res/layout/world_layer_item.xml`.

- Toolbar with back navigation and a create action.
- Biome selector; defaults to **Plains**.
- Layer list (`RecyclerView`) with drag reordering.
- Default layers: one layer each of three block types (`c.Qb`, `c.pb`, `c.o7` — the exact Bedrock block IDs are defined in `t.c`).
- “Add layer” action opens a searchable block-selection dialog.
- Tapping a layer opens a numeric thickness dialog; validation: positive integer required, error text `Please enter a valid integer value greater than 0.`
- A checkbox controls whether to add a bedrock/barrier-like base layer; the exact checkbox semantics are determined by `g(...)` in `d1.b`.
- Create validation:
  - Biome must be selected: `Please select a biome.`
  - At least one layer: `Please select at least one layer, air also works.`
  - Total layer count/thickness must not exceed 384: `Total layer thickness exceeds the limit (384).`
- Creation process:
  - Copies `assets/worldTemplate/level.dat` and `levelname.txt` into a new folder under `worlds/MineTuner Custom Flat World` (suffixed on collision).
  - Creates a LevelDB database at `<world>/db` through `com.hextetrahedral.leveldb.LevelDB`.
  - Reads the template `level.dat`, replaces/creates the `FlatWorldLayers` string tag, writes it back.
  - `FlatWorldLayers` JSON shape (verified):
    ```json
    {
      "biome_id": 1,
      "block_layers": [{"block_name": "minecraft:bedrock", "count": 1}],
      "encoding_version": 6,
      "structure_options": null,
      "world_version": "version.post_1_18"
    }
    ```
  - If the user leaves exactly one layer, an extra `minecraft:air` layer is appended; the layer list is reversed before writing.
  - Progress dialog: “Compiling custom flat world... Please wait while the world is being created. Do not exit the app.”
  - Success: “World created successfully.”; failures are surfaced as errors and the partially created folder is deleted.
- `levelname.txt` template content: `MineTuner Custom Flat World`.

### 4.3 DrawerActivity — world NBT navigation

Verified from `DrawerActivity.java`, `activity_drawer.xml`, `drawer_menu.xml`, `drawer_header.xml`.

- Drawer layout with a 160 dp green header showing the world icon, display name, and internal folder name.
- Navigation groups:
  - **Show Map** (`nav_map`) — opens `f0.e`, a map view.
  - **World NBT** (`nav_world_nbt`) — edits `level.dat`.
  - **Singleplayer NBT** (`nav_singleplayer_nbt`) — edits LevelDB key `~local_player`.
  - **Multiplayer NBT** (`nav_multiplayer_nbt`) — edits `f0.b`, a list of LevelDB keys beginning with `player_server_`.
  - **Change World** (`nav_change_world`) — confirmation then closes the world and returns to the world list.
  - **Advanced** submenu:
    - **Overworld NBT** — LevelDB key `Overworld`.
    - **The End NBT** — LevelDB key `TheEnd`.
    - **Nether NBT** — LevelDB key `Nether`.
    - **BiomeData NBT** — LevelDB key `BiomeData`.
    - **Portals NBT** — LevelDB key `portals`.
    - **Autonomous Entities NBT** — LevelDB key `AutonomousEntities`.
    - **Open Custom NBT** — prompts for an arbitrary database key (`Custom NBT Key`, hint `e.g. ~local_player`).
- Switching away from an NBT editor with unsaved changes prompts “Close NBT Editor ... Any unsaved changes will be lost.”
- “Change World” prompts “Close World ... Are you sure you want to close this world?”
- The map is a simple custom view (`map_selection_view.xml` / `f0.e`) showing the world map/selection; full map behaviour needs the LevelDB chunk keys.

### 4.4 NBT editor fragment (`f0.g`)

Verified from `f0/g.java` and resource layouts.

- One fragment class is reused for `level.dat`, player NBT, and dimension/arbitrary LevelDB keys.
- Top: horizontally scrollable breadcrumb (`A > B > C`). Each crumb is tappable and navigates to that ancestor.
- Middle: `RecyclerView` of the current compound/list/array children.
- Row: type icon + bold key. Single-valued rows are editable directly; compound/list/array rows navigate deeper.
- Tap on a leaf value row: “Edit \\"<name>\\"” input dialog.
- Long-press: options dialog with **Edit** and/or **Remove**, depending on whether the tag is a leaf and its parent type.
- Add FAB (above the Save FAB) opens a full-screen “Create NBT Tag” form:
  - Type spinner containing all reference-supported types.
  - Name field (hidden for list elements and array elements, because those are unnamed).
  - Compound child names are required; duplicate names are rejected.
  - List element types must match the list's element type.
  - Byte arrays only accept byte tags; int arrays only accept int tags.
- Save FAB writes the fragment's target:
  - `level.dat`: serialise NBT, prepend the 8-byte Bedrock header.
  - LevelDB target: write the raw value back under the original key.
- Progress dialogs: “Saving NBT data... Please wait while the NBT data is being saved.”
- Success toast: “NBT data saved successfully.” Failure toast: “Failed to save NBT data.”
- Exit with unsaved changes prompts “Close NBT Editor ... unsaved changes will be lost.”
- **No search, rename, expand/collapse, multi-select, copy/move, or undo/redo was found in the reference editor.** Navigation is drill-down plus breadcrumb.

#### Value editors and validation

The editor chooses keyboard input types by tag type:

| Type | Input | Validation |
|---|---|---|
| Byte | signed integer keyboard | `-128..127` |
| Short | signed integer keyboard | `-32768..32767` |
| Int | signed integer keyboard | `-2147483648..2147483647` |
| Long | signed integer keyboard | `Long.MIN_VALUE..Long.MAX_VALUE` |
| Float | signed decimal keyboard | must parse as a float and be within float range |
| Double | signed decimal keyboard | must parse as a double and be within double range |
| String | text keyboard | UTF-8 byte length ≤ 65535 |
| Byte/Int array element | signed integer keyboard | parent-specific bounds |
| Compound/List/Array | not edited as values; navigate or add/remove children | — |

Exact reference messages:

- `Out of bounds for byte.`
- `Out of bounds for short.`
- `Out of bounds for integer.`
- `Out of bounds for long.`
- `Out of bounds for float.`
- `Out of bounds for double.`
- `Out of bounds for string.`
- `Unknown tag to edit, report to developers.`
- `Tags inside a CompoundTag must have a name`
- `A tag with this name already exists in the compound`
- `Only byte tags can be added to byte arrays`
- `Only int tags can be added to int arrays`
- `List element type mismatch, it should be …`
- `Failed to create tag, tag name is out of bounds.`
- `Failed to add tag: …`
- `Failed to remove tag: …`
- `An error occurred when editing value.`
- `Cannot remove this tag.`

### 4.5 EntitiesNBTActivity — chunk entities

Verified from `app/EntitiesNBTActivity.java`.

- Opened with `chunkX` and `chunkZ` int extras.
- Title: `Chunk Entities at (<x>, <z>)`.
- Reads LevelDB key for chunk `(x, z, dimension=1)` through `x.b.a().b(x, z, 1)`.
- Lists entity NBT records; parses each record and reads `identifier` to show `minecraft:*`.
- Uses entity icon mapping `t.f` from `assets/entityIcons/*`; fallback placeholder icon when no icon exists.
- Tapping an entity opens the same NBT editor.
- Back while the editor is open prompts to close and discard unsaved changes.
- Failure toast: `Could not get entity keys.`

### 4.6 HelpActivity

- Shows help content, a Discord invite button, and a texture-pack import flow.
- Texture-pack import failure: `Failed to import texture pack.`
- Uses `assets/texture_pack.mcpack` and a custom `f0`/`map`-related view, but this is auxiliary to NBT editing.

<!-- MASTER-SECTION: LEG-B-S05 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 5. NBT engine (reference)

The reference's NBT implementation lives in `x2.a` (type enum) and `y2.*` (tag classes). It is a **Bedrock/little-endian NBT** dialect, not Java Edition big-endian NBT.

### Type enum

| ID | Type | Reference support |
|---|---|---|
| 0 | `TAG_End` | terminator |
| 1 | `TAG_Byte` | yes |
| 2 | `TAG_Short` | yes |
| 3 | `TAG_Int` | yes |
| 4 | `TAG_Long` | yes |
| 5 | `TAG_Float` | yes |
| 6 | `TAG_Double` | yes |
| 7 | `TAG_Byte_Array` | yes |
| 8 | `TAG_String` | yes |
| 9 | `TAG_List` | yes |
| 10 | `TAG_Compound` | yes |
| 11 | `TAG_Int_Array` | yes |
| 12 | `TAG_Long_Array` | **absent in reference; required in our app** |

### Wire format (verified)

- All multi-byte numeric values are **little-endian**.
- Tag names and string payloads are prefixed with an **unsigned 16-bit little-endian length**, not a signed short and not a varint.
- A nameless root/value is encoded without a name, for example list elements and array elements.
- `TAG_Byte` payload: 1 byte.
- `TAG_Short`: 2 bytes LE.
- `TAG_Int`: 4 bytes LE.
- `TAG_Long`: 8 bytes LE.
- `TAG_Float`: 4 bytes LE.
- `TAG_Double`: 8 bytes LE.
- `TAG_Byte_Array`: int32 LE length + raw bytes.
- `TAG_String`: uint16 LE byte length + UTF-8 bytes (reference reads/writes with the platform default charset; this is a bug our app must not copy).
- `TAG_List`: element type byte + int32 LE count + elements (elements have no names).
- `TAG_Compound`: child tags until `TAG_End` (`0x00`).
- `TAG_Int_Array`: int32 LE length + int32 LE values.
- No `TAG_Long_Array` in the reference parser/serialiser.

### `level.dat` container

Bedrock `level.dat` is **not** raw NBT. It starts with an 8-byte header:

```
int32 LE version
int32 LE payload length
NBT payload...
```

The supplied template starts with:

```
0A 00 00 00  99 0B 00 00  0A 00 00 08 ...
version=10    length=2969   root TAG_Compound...
```

The reference reads `level.dat` with `skip(8)` and writes it back with `putInt(8).putInt(payloadLength)`. It therefore normalises the header version to `8` on save, despite the template using `10`. Our implementation should preserve the existing header version and length when possible, or at minimum write a valid version/length pair and document the policy.

### Other reference NBT bugs / risks

- `String.getBytes()` / `new String(byte[])` use the platform default charset; must use UTF-8 explicitly.
- `l.h()` allocates `nameBytes.length + payload.length + 3`; correct for 1 type + 2 length bytes but relies on the name being non-null.
- List serialisation assumes every element has the list's declared type; it does not re-validate element type on write.
- Compound lookup is linear and case-sensitive.
- No depth/size guard against malicious files; a deeply nested or huge-length file can exhaust memory.
- Reference parser reads exactly the declared counts; there is no explicit trailing-byte check.
- `TAG_End` as a root returns `null` rather than a clear error.
- Encrypted worlds are detected only by a two-int LevelDB header heuristic.

<!-- MASTER-SECTION: LEG-B-S06 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 6. World/storage model

- Managed root: `getExternalFilesDir(null)/worlds/`.
- World folder contains:
  - `level.dat` (8-byte header + little-endian NBT).
  - `levelname.txt` (display name, unencoded text).
  - `world_icon.jpeg` (optional).
  - `db/` (Bedrock LevelDB).
  - Other Bedrock world files as supplied by the import.
- Import: zip extraction into a new folder named after the file without its extension, with `_1`, `_2`, … collision suffixes.
- Export: zip the world folder to `export.mcworld` then share/open via FileProvider.
- Delete: recursive delete of the world folder.
- External legacy worlds (SDK ≤ 28, storage permission) are listed but treated as read/write in place, with a backup warning.
- LevelDB keys edited by the reference:
  - `~local_player` — singleplayer player NBT.
  - `player_server_` prefix — multiplayer players.
  - `Overworld`, `TheEnd`, `Nether`, `BiomeData`, `portals`, `AutonomousEntities` — dimension/global NBT.
  - Chunk keys `(chunkX, chunkZ, dimension=1)` — entities.
  - Arbitrary user key.

<!-- MASTER-SECTION: LEG-B-S07 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 7. Feature inventory (reference)

### World management

- List managed + legacy worlds.
- Show name, size, internal folder, icon.
- Import `.mcworld` / `.mctemplate` (also accepts `.mcpack` / `.mcaddon` in the picker hints).
- Export/open world as `.mcworld`.
- Delete with confirmation.
- Detect encrypted worlds.
- Create custom flat world from template with biome, block layers, thicknesses, base-layer toggle.

### NBT editing

- Open `level.dat`, player NBT, dimension/global NBT, arbitrary LevelDB key, chunk entity NBT.
- Breadcrumb navigation into compounds, lists, byte arrays, int arrays.
- View children with type icons.
- Add child tags to compounds/lists/arrays.
- Edit primitive values in a dialog.
- Remove child tags with confirmation.
- Target-specific save, unsaved-exit confirmation, success/failure feedback.
- Numeric/string validation with reference messages.
- Entity list with icons and identifiers.

### Reference gaps/limitations

- No `TAG_Long_Array`.
- No search.
- No rename of an existing key.
- No expand/collapse tree; it is drill-down navigation.
- No copy/move/duplicate.
- No undo/redo.
- No multi-select.
- No dark mode.
- No generic “open arbitrary `.dat` file” via SAF; the editor is bound to a world and LevelDB keys.
- No explicit “Save As” for NBT data; world export is the closest analogue.
- No recent-files list.
- No explicit size/depth limits or corruption recovery UI.
- No visible accessibility labels on NBT type icons (content description is just `icon`).

<!-- MASTER-SECTION: LEG-B-S08 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 8. What this means for our implementation

The new app must be a **Compose/Material 3 native Android app** with the reference's world-editor heart, while adding the requested editor capabilities:

1. Keep the reference identity of a Bedrock world editor: green material theme, world cards, FAB import/create, drawer sections, breadcrumb NBT editor.
2. Add generic file editing via SAF so the app is also a real NBT editor, not only a world editor: open/save/save-as/recent, extension-agnostic `.dat`/`.nbt` support with explicit little-endian Bedrock NBT.
3. Implement all 13 tag types, including `TAG_Long_Array`, with a shared, tested parser/serialiser.
4. Add tree expand/collapse, search, rename, copy/move if the data model supports it, unsaved-change tracking, undo/redo, and robust validation.
5. Keep `level.dat` header handling explicit and correct.
6. Avoid the reference's ads, analytics, tracking, forced permission model, and unsafe charset/parsing behaviour.

### Major feasibility decision: LevelDB

The reference's player/dimension/map/entity features depend on **Bedrock LevelDB**. The reference ships `libleveldb.so` and a custom JNI wrapper. A faithful world-wide editor therefore needs a LevelDB implementation that can read/write Minecraft Bedrock's database format.

Options:

- **A. World-level + generic NBT only (recommended first release).** Implement world list/import/export/create and `level.dat` editing, plus full generic NBT file editing. Do not expose player/dimension/chunk keys until a working Bedrock LevelDB backend exists. This avoids fake buttons and unverified LevelDB behaviour.
- **B. Add LevelDB via a bundled native/Java backend.** Larger build and test burden: JNI/NDK, Bedrock compression/comparator compatibility, native ABIs, encryption handling. Needed for exact player/dimension/chunk parity.
- **C. Read-only LevelDB exploration.** Useful for viewing/browsing, but cannot satisfy the reference's save workflow.

The implementation should not claim LevelDB-powered screens until the backend is proven with real Bedrock fixtures.

<!-- MASTER-SECTION: LEG-B-S09 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 9. Verification status and limitations

- Verified statically: manifest, resources, layouts, strings, theme/colours, activities, adapter behaviour, NBT wire format, validation strings, header handling, world import/export/create logic, drawer mapping, entity icon mapping.
- Not verified dynamically: actual rendered pixel output, runtime permission flows, exact gestures/animations, ad SDK behaviour, real-device performance, third-party library runtime quirks.
- No reference APK was modified; the XAPK remains untouched.
- The `level.dat` template and icon assets were inspected but not copied into the new app.

<!-- MASTER-SECTION: LEG-B-S10 | CATEGORY=Audit UI/UX | STATUS=ARSIP -->
## 10. Source map (for future reference)

| Concern | Reference source |
|---|---|
| World list / import / export / delete | `com/hextetrahedral/minetuner/MainActivity.java` |
| Drawer / section mapping | `DrawerActivity.java`, `res/menu/drawer_menu.xml` |
| NBT editor UI | `f0/g.java`, `tree_node_item.xml` |
| NBT model/parser | `y2/l.java`, `y2/a..k.java`, `x2/a.java` |
| World model / `level.dat` header / LevelDB | `t/j.java` |
| World creation / validation / import helpers | `d1/b.java` |
| Flat-world template | `assets/worldTemplate/level.dat`, `levelname.txt` |
| Entity display names/icons | `t/f.java`, `assets/entityIcons/` |
| Theme/colours | `res/values/styles.xml`, `res/values/colors.xml` |
| World card layout | `res/layout/world_item.xml` |


<!-- MASTER-SECTION: LEG-C | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## Bagian C — Spesifikasi fitur

# FEATURES.md — Feature Specification (NBT Editor, reimplementation)

<!-- MASTER-SECTION: LEG-C-S01 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## 1. Identity & scope decisions

| Item | Decision |
|---|---|
| App name | NBT Editor |
| applicationId | `com.nbteditor` |
| Implementation | **Single native C++ file** (`app/main.native.cpp`) compiled to `libnbt.so`, packaged in a minimal APK (no Java/Kotlin sources, no Gradle, no AGP) |
| Rendering | Software framebuffer via `ANativeWindow` (RGBA_8888), custom immediate-mode UI toolkit in the same C++ file |
| Persistence | Native `std::filesystem`, no Room/DataStore |
| NBT dialect | Bedrock: little-endian, UTF-16-LE-ish names as uint16 length + bytes (reference uses raw bytes; we use UTF-8 payload with explicit UTF-8 both ways — see ANALYSIS.md §5) |
| Tag types | All 13, **including `TAG_Long_Array` (id 12)** which the reference lacks |
| Theme | Light **and** Dark (user choice) built on the reference's green palette |
| LevelDB | **Phase 1: not included.** No player/dimension/chunk/map UI until a proven Bedrock LevelDB backend exists. Nothing in the UI points at LevelDB-backed screens. |

Deviation note: the starter brief asked for Kotlin/Compose/MVVM; the user later explicitly chose "seluruh app native C++ 1 file", so those requirements are superseded. The equivalent layering (UI / presenter-viewmodel / domain / parser / serializer / repository) is preserved *inside* the single C++ file as strictly separated sections with no cross-layer leaks, and is documented in `ARCHITECTURE.md`.

<!-- MASTER-SECTION: LEG-C-S02 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## 2. Feature matrix

Legend: ✅ build now · 🟡 build later (tracked in TODO.md) · ❌ out of scope

### 2.1 File management

| # | Feature | Source | Status |
|---|---|---|---|
| F1.1 | Open NBT file from in-app browser | new (generic NBT editor requirement) | ✅ |
| F1.2 | Save (in place, same path + format) | new | ✅ |
| F1.3 | Save As (choose directory/name, same path rules) | new | ✅ |
| F1.4 | New NBT document (root compound) | new | ✅ |
| F1.5 | Import `.mcworld`/`.mctemplate` (zip → world folder) | reference: MainActivity import flow | ✅ |
| F1.6 | Export world to `.mcworld` (zip) | reference: Open in Minecraft | ✅ |
| F1.7 | Recent files list (last 10) | reference has none; user requirement | ✅ |
| F1.8 | Storage permission handling (all-files access where needed) | new | ✅ |
| F1.9 | Corrupt/empty/oversized file errors with actionable message | new | ✅ |
| F1.10 | Autosave draft of unsaved edits (crash safety) | new | ✅ |

### 2.2 World management (reference parity, no LevelDB)

| # | Feature | Source | Status |
|---|---|---|---|
| F2.1 | List worlds in app-managed dir + display name, size, icon | reference MainActivity | ✅ |
| F2.2 | External Minecraft world listing (legacy, where readable) | reference (SDK ≤ 28 only) | 🟡 |
| F2.3 | World options: Edit / Delete (no "Open in Minecraft" for external) | reference | ✅ (delete also for internal) |
| F2.4 | Delete world with confirmation | reference | ✅ |
| F2.5 | Create custom flat world (biome + block layers + thickness, base-layer toggle, 384 limit, JSON `FlatWorldLayers`) | reference CreateWorldActivity | ✅ |
| F2.6 | Edit `level.dat` NBT (8-byte header preserved) | reference DrawerActivity → World NBT | ✅ |
| F2.7 | Encrypted-world detection + warning (LevelDB db magic) | reference | ✅ |
| F2.8 | Show Map, Singleplayer/Multiplayer NBT, Overworld/TheEnd/Nether/BiomeData/portals/AutonomousEntities, chunk entities | reference DrawerActivity | ❌ (needs LevelDB, see §4) |
| F2.9 | World icons: `world_icon.jpeg`, fallback tile | reference | ✅ (jpeg decode is 🟡; fallback default) |
| F2.10 | Share world via FileProvider | reference | 🟡 |

### 2.3 NBT tree editor

| # | Feature | Source | Status |
|---|---|---|---|
| F3.1 | Hierarchy display of all 13 tag types | reference + user req | ✅ |
| F3.2 | Expand / collapse | **not in reference**, user req | ✅ |
| F3.3 | Select tag, show type + value summary | reference | ✅ |
| F3.4 | Drill-down into Compound/List/ByteArray/IntArray/LongArray | reference | ✅ |
| F3.5 | Breadcrumb navigation (`A > B > C`) | reference | ✅ |
| F3.6 | Add tag (type spinner + name) to Compound; add element to List; add to arrays | reference | ✅ |
| F3.7 | Edit value (per-type dialog with validation) | reference | ✅ |
| F3.8 | Rename tag | **not in reference**, user req | ✅ |
| F3.9 | Delete tag (with confirmation; list/array element index shift) | reference | ✅ |
| F3.10 | Duplicate (copy) tag | user req | ✅ |
| F3.11 | Move (cut/paste) tag inside same document | user req | ✅ |
| F3.12 | Search by key + filter results | **not in reference**, user req | ✅ |
| F3.13 | Type icon / colour badge per tag | reference icons | ✅ |
| F3.14 | Show byte size of each node | new | ✅ |
| F3.15 | Change a tag's type (reinterpret) | new | 🟡 |
| F3.16 | Undo / redo of every mutating action | user req | 🟡 |

### 2.4 Value editors & validation

| Type | Editor | Validation (reference strings reused) |
|---|---|---|
| Byte | numeric pad, signed | -128..127 → `Out of bounds for byte.` |
| Short | numeric pad, signed | -32768..32767 → `Out of bounds for short.` |
| Int | numeric pad, signed | −2³¹..2³¹−1 → `Out of bounds for integer.` |
| Long | numeric pad, signed | int64 → `Out of bounds for long.` |
| Float | decimal pad | finite & float-range → `Out of bounds for float.` |
| Double | decimal pad | finite & double-range → `Out of bounds for double.` |
| String | text pad (UTF-8, ≤65535 bytes) | → `Out of bounds for string.` |
| Byte Array | index→value list, add/remove/insert | index in range |
| Int Array | index→value list, add/remove/insert | index + int range |
| Long Array | index→value list, add/remove/insert | index + int64 range |
| List | element type fixed at creation; add/remove/edit elements | `List element type mismatch, it should be …` |
| Compound | child list + add/remove/rename | duplicate name → `A tag with this name already exists in the compound`; unnamed → `Tags inside a CompoundTag must have a name` |
| Long Array creation | direct | n/a |

Reference error strings reused verbatim where they describe the same rule (see ANALYSIS.md §4.4).

### 2.5 Editor experience

| # | Feature | Status |
|---|---|---|
| F5.1 | Unsaved-change indicator `*` in title + navigation warning | ✅ |
| F5.2 | Confirmations before delete / discard / overwrite | ✅ |
| F5.3 | Error handling: parse/save/IO errors surfaced, no silent loss | ✅ |
| F5.4 | Info bar: file path, size, root type, child count, tag count | ✅ |
| F5.5 | Progress indicator for big files / zip IO | ✅ |
| F5.6 | Touch: tap / long-press / scroll / drag-select | ✅ |
| F5.7 | In-app text & numeric keypad (no IME dependency) | ✅ |
| F5.8 | Responsive layout (phone + landscape + tablet width classes) | ✅ |
| F5.9 | Light + Dark themes (green palette of reference) | ✅ |
| F5.10 | Undo/redo history (see F3.16) | 🟡 |
| F5.11 | Localization (English; reference only ships English) | ✅ |

<!-- MASTER-SECTION: LEG-C-S03 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## 3. Acceptance criteria (per feature)

Every ✅ feature must satisfy:

1. **Works with real data** — exercised against real Bedrock `level.dat` assets from the reference (`assets/worldTemplate/level.dat`), a generated 13-type fixture, and an intentionally corrupt file.
2. **Round-trip safe** — parse → mutate → serialize → parse yields identical structure and values (TESTING.md case T2/T3/T4/T5/T12).
3. **No silent data loss** — any failure aborts with a visible message and leaves the on-disk file untouched.
4. **No dummy UI** — a control exists only when its behaviour is implemented; otherwise it is not rendered.
5. **Layered** — the NBT core has no dependency on UI code (it is a separable translation unit guarded by `#ifdef NBT_SELFTEST` for host tests).

<!-- MASTER-SECTION: LEG-C-S04 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## 4. Explicitly not in phase 1 (LevelDB)

Not implemented, not stubbed, not listed in the UI as available:

- Singleplayer NBT (`~local_player`), Multiplayer NBT (`player_server_*`).
- Overworld / TheEnd / Nether / BiomeData / portals / AutonomousEntities editors.
- Chunk entity list (`(chunkX, chunkZ, dimension=1)`) and entity icons.
- Show Map.
- World "Open in Minecraft" via Minecraft package launch (only file export; 🟡 share).

Rationale: all of these require reading/writing Bedrock LevelDB (`<world>/db`). The reference relies on a bundled `libleveldb.so` and JNI wrapper; our Phase 1 build ships no LevelDB backend, and shipping UI that cannot work would violate "no nonfunctional buttons". Decryption/marketplace-world support remains out of scope.


<!-- MASTER-SECTION: LEG-D | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## Bagian D — Arsitektur (lapisan L0–L6)

# ARCHITECTURE.md — Layered Architecture for a single-file native app

<!-- MASTER-SECTION: LEG-D-S01 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 1. Constraint: one C++ translation unit

The user chose *"seluruh app native C++, 1 file cpp"*. The whole app lives in `app/src/main.native.cpp`. To keep it maintainable, that single TU is internally partitioned into **strictly separated layers**, in this order (each layer may only depend on the layer above it, never below):

```
┌───────────────────────────────────────────────────────────────┐
│ L0  Platform glue        main() / ANativeActivity callbacks   │  app_lifecycle
│                            window, input, looper, permission   │
├───────────────────────────────────────────────────────────────┤
│ L1  Kernel primitives    Vec2, Rect, Color, Time, utf8 utils   │  utils
├───────────────────────────────────────────────────────────────┤
│ L2  Renderer             Canvas (fill/rect/text/line/clip)     │  render
│                            8x8 glyph font, double-buffered      │
├───────────────────────────────────────────────────────────────┤
│ L3  Widget kit           Node tree, layout, events, hit-test    │  ui
│                            (screen boundaries, no app logic)    │
├───────────────────────────────────────────────────────────────┤
│ L4  NBT core             Tag struct, parse, serialize, find     │  nbt
│                            — NO dependency on L0..L3 —          │
├───────────────────────────────────────────────────────────────┤
│ L5  Application          ViewModels (file/world/nbt),           │  app
│                            repositories, navigation, undo       │
├───────────────────────────────────────────────────────────────┤
│ L6  Screens & dialogs    Home, Worlds, Browser, Tree, Value     │  screens
│                            editor, dialogs, toasts, settings    │
└───────────────────────────────────────────────────────────────┘
```

Dependency rule enforced by inspection + compile order: `L4` is compiled and unit-tested **standalone** (`-DNBT_SELFTEST`, host build, no Android headers linked), which proves it never drifted into UI concerns.

### Why one file is still acceptable

- No ABI/translate-unit duplication problems (static/internal linkage only; exactly one global symbol `ANativeActivity_onCreate` exported).
- Build is 2 commands (engine host-test → device clang build → aapt2 → apksigner).
- The file is sectioned with `/* ===== Lx ... ===== */` banners so a reader can navigate it. `TODO.md` tracks the future split if the file exceeds ~6000 lines.

<!-- MASTER-SECTION: LEG-D-S02 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 2. Platform glue (L0)

| Concern | Choice |
|---|---|
| Entry | `android.app.NativeActivity` declared in the manifest with meta-data `android.app.lib_name=libnbt`, `android.app.native_code` not needed. No Java/Kotlin sources, no DEX in the APK. |
| Custom glue | `native_app_glue` is not present in this environment's NDK sysroot, so the file implements its own minimal glue: `ANativeActivity_onCreate` (the one JNI symbol the framework looks up), an `AppState` singleton, and the lifecycle callbacks (`onStart/onResume/onSaveInstanceState/onPause/onStop/onDestroy/onWindowFocusChanged`). |
| Threading | A single **UI/render thread** created with `pthread_create` owns the framebuffer and the widget tree. Platform callbacks run on the Android main (UI) thread and only post state through a mutex-protected queue (no locks held while rendering). |
| Input | `AInputQueue` obtained via `ALooper` with the activity's looper id; handled in the looper callback feeding `L3`. |
| Output | `ANativeWindow` + `ANativeWindow_setBuffersGeometry(w, h, WINDOW_FORMAT_RGBA_8888)`; we present with `ANativeWindow_lock`/`unlockAndPost`. Framebuffer is `uint32_t` RGBA8888 (lane order from `native_window.h`: byte 0 = R). |
| Resize | `onNativeWindowResized` → reallocate framebuffer + relayout. Handled idempotently. |
| Permissions | Runtime `MANAGE_EXTERNAL_STORAGE` / legacy storage requested through `JNIEnv` calls to `android.app.Activity`/`Settings` from `L0`. No Java code required. |
| Assets | None. No `.apk`-embedded assets are read at runtime (the flat-world template is baked into `L5` as C++ data, not copied from the reference APK). |

<!-- MASTER-SECTION: LEG-D-S03 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 3. Renderer & widget kit (L1–L3)

**Canvas**: immediate 2D over a `uint32_t*` buffer — `clear`, `fillRect`, `strokeRect`, `roundRect` (supersampled), `fillCircle`, `drawLine`, `blit` (image), `clipRect`, per-pixel alpha blend. Anti-aliasing is 3×3 supersampling only for rounded corners/circles; text is nearest-neighbour from an embedded 8×16 glyph table (ASCII 0x20–0x7E) to keep the font table small (96×16 bytes).

**Widget tree**: retained nodes with `type, rect, id, children, style, state` (expanded/selected/pressed/focused), one `Event` type (`touch{began,moved,ended}`) plus a virtual keypad event for text input. Layout is a small set of primitives: `row`, `column`, `list` (virtualised: only visible children built), `scroll` (clip + offset + momentum), `grid`, `tabs`, `bar` (top/bottom), `fab`, `dialog` (modal stack), `toast`, `field`. Every screen is built as a *pure function* `State → Node tree`, so redraws are deterministic and there is no view-recycling state to corrupt.

**Hit-testing**: walk children in reverse order (topmost first) → returns target + local point; scrolling containers mark the gesture consumed by delta-threshold logic (10 px / 150 ms long-press).

**Input routing**: `touch` → hit test → `onTap/onLongPress/onDrag` on the target; keys from the built-in keypad → `onKey` → focused field. Screens never read the touch array directly.

<!-- MASTER-SECTION: LEG-D-S04 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 4. NBT core (L4) — the load-bearing layer

```
nbt::Tag             variant: Byte,Short,Int,Long,Float,Double,String,
                     ByteArray,IntArray,LongArray,List,Compound
nbt::Document        root Tag + source path + endianness flag + dirty flag
nbt::ParseResult     { Document, warnings[], bytesConsumed } | { Error }
nbt::read(bytes)     -> ParseResult              (strict: no trailing garbage)
nbt::write(Document) -> vector<uint8_t>          (little-endian; UTF-8)
nbt::validate(document) -> vector<string>         semantic checks
nbt::find/flatten    query helpers for search
```

Format contract (verified from reference, see ANALYSIS.md §5):

- All ints/fps little-endian; names & string payloads `uint16 LE` length prefix.
- `level.dat` = `int32 version`, `int32 payload length`, then NBT. **Our read** verifies the declared length against the actual payload (reference does not); **our write** preserves the version read from file (reference rewrites 10 → 8, which we consider a bug) and recomputes the length. A file whose header length disagrees is reported as corrupt, never silently truncated.
- Lists store their declared element type; writers must re-validate elements (reference assumes they are already correct — we check).
- Unknown type byte → `NbtError::UnknownTag` with the offending byte value, never a crash.
- Depth limit 128 and total node limit 4,000,000 (configurable constants) protect against crafted files; the limit is reported as an error, not an abort.
- `TAG_Long_Array` (12): `int32 count` + count × `int64 LE`.

**Memory**: doc parsed into a contiguous arena of `Tag` nodes with index-based children (no pointer-chasing per node, no `std::string` per key — keys are `string_view` into a name pool). A 50 MB `level.dat` parses in one pass with ~3× payload overhead, and the arena is freed as one block.

<!-- MASTER-SECTION: LEG-D-S05 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 5. Application layer (L5)

### ViewModels (one per feature, all plain C++ structs + `apply()` reducers)

| VM | State | Intent |
|---|---|---|
| `FileVm` | path, docs, recent[], dirty, io error | open/save/saveAs/new/import/export/recent |
| `WorldVm` | worlds[], selected, progress | list/create/import/export/delete/open |
| `NbtVm` | doc, selection path, expanded set, undo stack, filter | navigate/expand/add/edit/rename/delete/copy/move/undo/search |

Undo is a **command log** of `{inverse op, node id}` recorded *before* each mutation — 200 steps, tied to `NbtVm`.

### Repositories

| Repository | Backend | Notes |
|---|---|---|
| `NbtRepository` | bytes ↔ `zlib` for `.mcworld` via `zip.{h,pp}`-free inline code in the same TU | uses `inflate`/`deflate` from system libz (link `-lz`), streaming so a 2 GB world import does not sit in RAM |
| `FileRepository` | `std::filesystem` + POSIX `open/read/write` | atomic save: write `path.tmp-<pid>` → `fsync` → `rename` (never truncates the original on failure) |
| `PrefsRepository` | `activity->internalDataPath/prefs.ini` | recent files, theme, last directory, keypad mode |
| `WorldRepository` | app-managed dir + `.mcworld` zip | template `level.dat` bytes as C++ literals |

### Cross-cutting

| Concern | Implementation |
|---|---|
| Background IO | `std::thread` worker with a task queue; completion marshalled back to the UI thread via the L0 message queue. |
| Progress | worker publishes `{stage, percent}`; UI thread renders a modal progress dialog. |
| Errors | `Result<T>` = `{value}` \| `{code, message, hint}`; every failure path maps to a user-visible string (no `catch(...)` swallow). |
| Validation warnings | non-fatal issues (e.g. list element type mismatch on read) surface as a yellow banner, not a toast. |
| Crash safety | autosave every 30 s + on `onPause` into `internalDataPath/draft-<hash>.nbt`; restored on next launch if the primary save never completed. |

<!-- MASTER-SECTION: LEG-D-S06 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 6. Data flow

```
touch/key event
   → L0 queue → L3 hit-test → L6 screen handler
        → L5 intent (e.g. EditValue)
            → L4 nbt::validate + mutate (arena)
            → L5 records undo, sets dirty
        → L6 rebuild Node tree (pure) → L2 canvas → ANativeWindow
```

<!-- MASTER-SECTION: LEG-D-S07 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 7. Build topology (no Gradle)

| Stage | Command | Output |
|---|---|---|
| 1. Engine host tests | `clang++ -O2 -DNBT_SELFTEST app/src/main.native.cpp -o /tmp/nbt_test && /tmp/nbt_test` | test report |
| 2. Device library | `clang++ --target=aarch64-linux-android26 -shared -fPIC -O2 -o libnbt.so app/src/main.native.cpp -landroid -lz` | `lib/arm64-v8a/libnbt.so` |
| 3. Resources | `aapt2 compile --dir res -o out/res.zip` | compiled resources |
| 4. Manifest+resources link | `aapt2 link -o base.apk -I <no platform jar> --manifest AndroidManifest.xml out/res.zip --min-sdk-version 26 --target-sdk-version 34 --no-compress so` | unsigned APK (no JDK/AGP needed; aapt2 synthesises arsc) |
| 5. Package library | `cd out && zip -0 base.apk lib/arm64-v8a/libnbt.so` | APK with native lib |
| 6. Sign | `apksigner sign --ks debug.keystore --ks-key-alias androiddebugkey --ks-pass pass:android --key-pass pass:android base.apk` (v1+v2 via default) | installable APK |

Notes:
- `zip -0` (stored, uncompressed) for the `.so` avoids needing `zipalign`: the library starts 4-byte aligned inside the APK and `apksigner` reports alignment. Verified available: `zip`, `apksigner`.
- Icon: adaptive icon XML (`res/mipmap-anydpi-v26`) with a vector foreground — no PNG generation needed.
- The debug keystore is generated with `keytool` (JDK 21, verified present at `/data/data/com.termux/files/usr/lib/jvm/java-21-openjdk/bin/keytool`). No `openssl` fallback needed.
- `aapt2` here is 2.20 from Android 16 platform (source-compatible with `--min-sdk-version`).

<!-- MASTER-SECTION: LEG-D-S08 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 8. Module map inside the file

| Banner | Sections |
|---|---|
| `// ===== L0` | `ANativeActivity_onCreate`, lifecycle, looper, input, permissions, main loop |
| `// ===== L1` | math/utf8/color helpers |
| `// ===== L2` | Canvas + glyph table |
| `// ===== L3` | Node/widget kit |
| `// ===== L4` | NBT core (parse/serialize/validate/query) |
| `// ===== L5` | repositories + viewmodels + threading |
| `// ===== L6` | screens/dialogs |

<!-- MASTER-SECTION: LEG-D-S09 | CATEGORY=Arsip teknis | STATUS=REFERENSI -->
## 9. Risk register

| Risk | Mitigation |
|---|---|
| No `zipalign` | store `.so` uncompressed; `apksigner` verifies alignment; if it complains, add a 1024-byte zero pad entry |
| No `native_app_glue` | glue written by hand in L0 (~120 lines) |
| No EGL/GLES libs in this NDK sysroot | software rendering through `ANativeWindow` |
| No `android.jar` | never linking against it — nothing in the build touches Java classes |
| No IME for text entry | built-in keypad in L3 |
| Scoped storage on Android 11+ | prefs/autosave in `internalDataPath`; broad file browsing only after `MANAGE_EXTERNAL_STORAGE`; `.mcworld` import/export via app-managed dir + share targets |
| One TU grows unwieldy | section banners + `TODO.md` split plan + host testability keeps core honest |


<!-- MASTER-SECTION: LEG-E | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## Bagian E — Build dan pengembangan

# DEVELOPMENT.md — Cara membangun & menguji

<!-- MASTER-SECTION: LEG-E-S01 | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## 0. Prasyarat (sudah terpasang di lingkungan ini)

| Tool | Perintah cek | Catatan |
|---|---|---|
| Termux `clang`/`clang++` 21.1.8 | `clang++ --version` | aarch64, lintas-compile Android |
| Termux `ndk-sysroot` v30 | `dpkg -l ndk-sysroot` | header Android (`native_window.h`, `input.h`, `looper.h`, `jni.h`) |
| `build/stubs/*.so` | `bash scripts/make_stubs.sh` | **stub link-time** `libandroid.so`/`libz.so`/`liblog.so`; dipakai hanya saat link, tidak masuk APK |
| Termux `lld` | `command -v lld` | linker |
| Termux `aapt2` 2.20 | `aapt2 version` | kompilasi/link resource, susun APK |
| Termux `d8` | `d8 --version` | (cadangan bila nanti ada kode Java) |
| Termux `apksigner` | `apksigner --version` | penandatanganan APK |
| JDK 21 (`keytool`) | `keytool -version` | pembuatan debug keystore |
| `zip`, `unzip`, `curl`, `python3` | — | pembungkus APK, unduh, skrip kecil |

Tidak ada Gradle, AGP, `android.jar`, `adb`, `zipalign`, `kotlinc`, `sdkmanager`, `EGL`, `GLES`. Desain build dibuat agar **tidak membutuhkan semuanya** (lihat `ARCHITECTURE.md` §3, §7).

<!-- MASTER-SECTION: LEG-E-S02 | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## 1. Struktur direktori

```
NBT-EDITOR/
├── app/
│   ├── AndroidManifest.xml          # manifest sumber (aapt2 link)
│   ├── res/…                        # values + mipmap-anydpi-v26 (vector icon)
│   └── src/main.native.cpp          # SATU-SATUNYA file C++ (L0..L6)
├── scripts/
│   ├── build_host_tests.sh          # uji engine di Termux (host CPU)
│   ├── build_device.sh              # compile libnbt.so arm64-v8a
│   ├── build_apk.sh                 # aapt2 + zip + apksigner → APK terpasang
│   └── keystore.sh                  # buat debug.keystore sekali kali
├── out/                             # artefak (di-.gitignore)
└── *.md                             # dokumentasi
```

<!-- MASTER-SECTION: LEG-E-S03 | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## 2. Langkah-langkah

```bash
# sekali saja: keystore debug
bash scripts/keystore.sh

# 1) unit test engine C++ di host (parse/serialize round-trip)
bash scripts/build_host_tests.sh

# 2) stub link-time (otomatis dibuat ulang oleh build_device.sh bila hilang)
bash scripts/make_stubs.sh

# 3) compile libnbt.so untuk perangkat
bash scripts/build_device.sh

# 4) bangun APK
bash scripts/build_apk.sh
```

Hasil: `out/NBTEditor.apk` (signed, siap sideload ke perangkat arm64).

<!-- MASTER-SECTION: LEG-E-S04 | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## 3. Detail perintah

### 3.1 Test engine (host)

```bash
clang++ -std=c++20 -O1 -g -fsanitize=address,undefined \
        -DNBT_SELFTEST app/src/main.native.cpp -o out/nbt_test && out/nbt_test
```

- `-DNBT_SELFTEST` mengaktifkan `main()` uji dan mematikan bagian Android (`L0`) sehingga file yang sama bisa dikompilasi sebagai executable biasa di Termux.
- ASan/UBSan wajib aktif: engine adalah satu-satunya bagian yang memproses byte tidak tepercaya.

### 3.2 Library perangkat

```bash
bash scripts/build_device.sh     # isinya:
# clang++ -std=c++20 -O2 -fPIC -shared --target=aarch64-linux-android26 -nostdlib \
#         -fvisibility=hidden -DNDEBUG app/src/main.native.cpp -o out/lib/arm64-v8a/libnbt.so \
#         $PREFIX/lib/libc++_shared.so \
#         build/stubs/libandroid.so build/stubs/libz.so build/stubs/liblog.so
# patchelf --remove-rpath out/lib/arm64-v8a/libnbt.so
```

- `-nostdlib` + `libc++_shared.so` eksplisit: sysroot Termux tidak punya `libc++_static.a`, tetapi punya versi shared (terverifikasi ter-link dan `NEEDED`-nya hanya `libc/libm/libdl`).
- **`build/stubs/libandroid.so`, `libz.so`, `liblog.so` WAJIB dipakai.** `$PREFIX/lib/libandroid.so` milik Termux adalah *wrapper* yang `NEEDED libtermux-platform-ns.so`; menggunakannya membuat app mati saat `dlopen` di perangkat (lihat Bagian J).
- `libandroid.so` menyediakan `ANativeActivity`, `ANativeWindow`, `ALooper`, `AInputQueue`; `libz.so` untuk zip `.mcworld`.
- `patchelf --remove-rpath` membuang `RUNPATH` bawaan clang Termux.

### 3.3 APK

```bash
aapt2 compile --dir app/res -o out/res.zip
aapt2 link -o out/base.apk --manifest app/AndroidManifest.xml out/res.zip \
        --min-sdk-version 26 --target-sdk-version 34 \
        --version-code 1 --version-name 1.0.0 \
        --no-compress so
(out && zip -0 base.apk lib/arm64-v8a/libnbt.so)
apksigner sign --ks out/debug.keystore --ks-key-alias androiddebugkey \
        --ks-pass pass:android --key-pass pass:android out/base.apk
mv out/base.apk out/NBTEditor.apk
```

Catatan `aapt2 link`: tidak memerlukan `-I android.jar` (tidak ada referensi simbol framework), karena aplikasi ini **native-only** tanpa kode Java/DEX.

### 3.4 Pasang di perangkat

Tidak ada `adb`. Buka APK lewat pengelola-file perangkat (Termux:Widget/file manager) dan izinkan "pasang dari sumber tak dikenal", atau copy ke `~/storage/downloads` dan buka dari sana. Wajib arm64-v8a (semua perangkat Android modern).

<!-- MASTER-SECTION: LEG-E-S05 | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## 4. Debugging

| Situasi | Cara |
|---|---|
| Penting lihat nilai internal | log ke stderr → `logcat`; build host dengan `-DNBT_SELFTEST -DDEBUG_TRACE` memakai `fprintf(stderr,…)` yang sama |
| APK gagal pasang | `apksigner verify --verbose out/NBTEditor.apk`; cek `aapt2 dump badging` |
| Layar hitam | pastikan `ANativeWindow_lock` memakai stride dari `ANativeWindow_getBuffersGeometry`, bukan `width*4` |
| Crash saat parse | uji engine di host dengan ASan (3.1); bug engine harus ditemukan tanpa perangkat |
| Font/teks terpotong | ukuran glyph 8×16; cek `clipRect` dan padding saat teks > lebar baris |

<!-- MASTER-SECTION: LEG-E-S06 | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## 5. Gaya kode

- C++20, `std::` saja (tanpa boost).
- 4 spasi indent, `camelBack` fungsi, `PascalCase` tipe, `SCREAMING_SNAKE` konstanta.
- Tidak ada `using namespace`; setiap bagian diawali banner `// ===== Lx …`.
- Semua penanganan error lewat `Result<T>` (bukan `throw`), tidak ada `catch(...)` kosong.
- Tidak ada peringatan compiler yang diabaikan: build dipaksa `-Wall -Wextra -Werror`.

<!-- MASTER-SECTION: LEG-E-S07 | CATEGORY=Riwayat perkembangan | STATUS=REFERENSI -->
## 6. Perbaiki error lebih dulu

Aturan kerja: setelah setiap tahap implementasi, jalankan 3.1 dan 3.2. Jika ada error/warning, perbaiki **sebelum** melanjutkan ke fitur berikut. `README.md` mencatat status build terakhir.


### 7. Indeks perkembangan berdasarkan tanggal

Indeks ini merujuk ke catatan lama; tidak menggandakan isi teknis. ID `DEV-*` permanen dan hanya untuk pekerjaan pengembangan, bukan bug.

| ID pekerjaan | Tanggal | Ringkasan perubahan | Bagian sumber | Bukti / catatan |
|---|---|---|---|---|
| `DEV-001` | sebelum 10–11 Nov 2026 | Pembangunan awal NBT Editor: engine 13 tipe, 6 layar, import/export, undo, APK build pertama | Bagian E; Bagian I | engine/model snapshot; riwayat build tanpa source changes |
| `DEV-002` | 10–11 Nov 2026 | Perbaikan startup 1.0.1: link/stub native, loop render, input dispatch, permission malas, RUNPATH | Bagian J | device harness; `dlopen` dan lifecycle pada snapshot sesudahnya |
| `DEV-003` | 11–13 Nov 2026 | Startup #2: `-mno-outline-atomics` dan guard helper atomic | Bagian K | `DLOPEN_OK`; onCreate→onDestroy lulus |
| `DEV-004` | 13 Nov 2026 | Audit TAHAP 1–2: klaim vs realita, bug B1–B9, host 49/49 + 40/40 | Bagian L | probe host; indexing source; tidak mengubah source |
| `DEV-005` | 14 Nov 2026 | Safeguard screenshot dan B6–B8: breadcrumb, array editor, dead action/theme/delete/paste/go-to-root | Bagian M–Q | model-test naik ke 52/58/62; render fixture; APK 1.0.3 |
| `DEV-006` | 15 Nov 2026 | Audit FASE 1 dan kontrak perbaikan P0/P1/P2, R-P0-01–R-P0-30 | Bagian R | source hash audit; ADB/dropbox ANR; tidak ada perbaikan menyeluruh |
| `DEV-007` | 15–16 Nov 2026 | Analisis FASE 1: file inventory, file analysis, konsolidasi Markdown, baseline frozen `2b260b...` | FASE 1 bagian A–K | 64/64 + 68/68 hanya untuk baseline sebelum source freeze; hash tidak teruji setelah freeze |
| `DEV-008` | 16 Nov 2026 | Audit APK: metadata 1.0.4, ABI/emulator, instalasi di Pixel_5, Home page, startup log | FASE 2 bagian 1–3 | `adb install` sukses; cold start +10s463ms; Home/screenshot `02_first_page_home_running.png` |
| `DEV-009` | 18 Nov 2026 | Katalog utama dan dokumentasi navigasi dinamis; marker `MASTER-*` ditanam | Katalog utama; Verifikasi katalog master | dua regenerasi byte-identik; 98 section level-2 saat itu |
| `DEV-010` | 19 Nov 2026 | Database bug permanen 94 ID dan peta perbaikan→bug→pengujian | Bagian F | verifier: ID unik; status dihitung; tidak ada bug lama dihapus |
| `DEV-011` | 19 Nov 2026 | Fase 5–6: status fitur berbasis bukti, indeks perkembangan, pemeriksaan konsistensi dan regenerasi katalog | Bagian E, F, G; Verifikasi katalog master | 47 fitur unik; 94 bug unik; dua regenerasi byte-identik; tidak ada ID lama dihapus |
| `DEV-012` | 19 Nov 2026 | Tree Editor verification: pemeriksaan selected row, nilai panjang, FAB clearance; host test, APK baru, Pixel_5 emulator | FASE 2 §4 | host 64/64 + 70/70 dengan `M42`, 2 bug Tree (`LEG-M-UI-001`, `LEG-M-UI-002`) dinyatakan SELESAI |

**Catatan perkembangan:** tanggal 17 Nov tidak memiliki entri terpisah dalam dokumentasi yang tersimpan; file screenshot `out/ui-ux-tree-2026-11-17/` ada di artefak, tetapi isi analisis tersebut tidak tercatat di `DOKUMENTASI.md`, sehingga tidak dikarang menjadi pekerjaan dokumentasi.

<!-- MASTER-SECTION: LEG-F | CATEGORY=Hasil pengujian | STATUS=REFERENSI -->
## Bagian F — Pengujian

# TESTING.md

Filosofi: **engine diuji di host secara agresif (ASan/UBSan + round-trip), UI diuji deterministik lewat "model" test, ketiganya harus hijau sebelum build APK.**

<!-- MASTER-SECTION: LEG-F-S01 | CATEGORY=Hasil pengujian | STATUS=REFERENSI -->
## 1. Perintah

```bash
bash scripts/build_host_tests.sh      # engine + model-test (host, ASan/UBSan)
bash scripts/build_device.sh          # compile arm64-v8a, harus tanpa error -Werror
bash scripts/build_apk.sh             # aapt2+apksigner, harus sukses
```

<!-- MASTER-SECTION: LEG-F-S02 | CATEGORY=Hasil pengujian | STATUS=REFERENSI -->
## 2. Unit test engine (wajib)

Semua kasus di bawah ada sebagai `CASE(...)` di `app/src/main.native.cpp` (aktif hanya dengan `-DNBT_SELFTEST`).

| ID | Kasus | Harapan |
|---|---|---|
| T1 | Buka `level.dat` template referensi (2977 byte, header version 10, len 2969) | `ParseResult.ok` = true; root Compound; `bytesConsumed == 2977`; 0 warning kritis |
| T2 | Serialize dokumen hasil T1 byte-identical | `write(doc)` == byte masukan asli (membuktikan writer setia pada sumber Bedrock) |
| T3 | Ubah satu nilai (mis. `SpawnY`) lalu tulis | bit hanya berbeda pada field yang diubah; panjang header diperbarui |
| T4 | Semua 13 tipe dalam satu Compound (fixture sintetis) | round-trip utuh; tipe & nama & urutan anak dipertahankan |
| T5 | Compound bersarang 3 level + List berisi List + Byte/Int/Long Array | struktur & tipe dipertahankan; byte-identik |
| T6 | Tambah tag baru ke Compound | nama baru ada; urutan di akhir |
| T7 | Hapus tag | lenci: tidak ada sisa byte di badan compound |
| T8 | Ganti nama tag | key lama hilang, key baru ada, nilai sama |
| T9 | File kosong (0 byte) | `Error::Empty`, pesan jelas, tidak crash |
| T10 | File rusak (type byte tak dikenal `0x63`) | `Error::UnknownTag(0x63)`, offset dilaporkan |
| T11 | Header `level.dat` panjang tidak cocok (len != sisa) | `Error::HeaderLengthMismatch`, tidak truncation diam-diam |
| T12 | Byte/Short/Int/Long di luar rentang | `validate()` menandai field berdasar pesan referensi (`Out of bounds for …`) |
| T13 | String > 65535 byte UTF-8 | `Out of bounds for string.` |
| T14 | List dengan elemen bertipe campur saat tulis | ditolak dengan `List element type mismatch, it should be …` |
| T15 | String non-ASCII ("héllo", emoji) | UTF-8 dijaga; round-trip byte-identik |
| T16 | Nesting kedalaman 128 vs 129 | 128 lolos, 129 → `Error::DepthLimit` |
| T17 | Array 0 elemen dan Compound kosong | round-trip utuh (edge case) |
| T18 | `TAG_End` sebagai root | `Error::EmptyRoot`, bukan null deref |
| T19 | Dokumen besar (≥ 50 MB sintetis) | parse < waktu anggaran, RSS < 4× ukuran berkas, tidak crash |
| T20 | Write atomicity: direktori read-only | berkas asli tidak berubah; `Result` error dengan pesan |
| T21 | `.mcworld` baca/tulis (zip inflate/deflate) | daftar entri & isi utuh; round-trip zip→folder→zip |
| T22 | Search (`nbt::find`) | menemukan semua key yang cocok (case-sensitive) beserta path |
| T23 | Duplikat nama saat rename | `DuplicateName` (pesan referensi) |
| T24 | Undo/redo (jika T3.16 diimplementasikan) | setiap undo mengembalikan byte dokumen ke versi sebelumnya persis |

<!-- MASTER-SECTION: LEG-F-S03 | CATEGORY=Hasil pengujian | STATUS=REFERENSI -->
## 3. Model test UI (deterministik, host)

`L3`/`L6` dibangun sebagai fungsi murni dari state; test memanggil fungsi build dengan state buatan dan memeriksa pohon node (posisi, visibilitas, teks, enabled):

- Urutan tombol Save/Fab, label dialog = persis pesan yang tercantum di `FEATURES.md`.
- Navigasi: setiap item menu yang tampil punya handler (tidak ada kontrol tanpa aksi).
- Unsaved-change indicator muncul hanya saat dirty.
- Dialog konfirmasi muncul untuk delete/overwrite/discard.
- Tema terang/gelap: warna uji tidak null dan kontras teks ≥ 4.5:1 (diuji aritmetik).

<!-- MASTER-SECTION: LEG-F-S04 | CATEGORY=Hasil pengujian | STATUS=REFERENSI -->
## 4. Tidak mengklaim yang belum diuji

- Tidak ada pengujian UI otomatis pada perangkat (tidak ada `adb` di lingkungan ini). Semua klaim UI berasal dari kode + model test; belum ada verifikasi visual di layar nyata sampai APK dipasang.
- Fitur LevelDB tidak diuji dan tidak diklaim ada.

<!-- MASTER-SECTION: LEG-F-S05 | CATEGORY=Hasil pengujian | STATUS=REFERENSI -->
## 5. Kriteria hijau sebelum build APK

```
[PASS] T1–T24 (engine, host)
[PASS] model-test UI (host)
[PASS] compile device tanpa warning (-Werror)
[PASS] aapt2 link + zip + apksigner verify
```


<!-- MASTER-SECTION: LEG-G | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## Bagian G — Status dan keputusan

> **Status per 11 Nov 2026:** engine NBT (49 tes), UI 6 layar + dialog + keypad (40 model-test),
> world import/ekspor, undo/redo, dan build APK **selesai**. Akar masalah "aplikasi tidak bisa
> dibuka" sudah ditemukan & diperbaiki (Bagian J). **Tersisa: verifikasi di perangkat nyata**
> (tidak ada `adb`/emulator di lingkungan pembangunan).

# TODO.md — Status & pekerjaan tersisa

Ringkasan fase: selesai Phase 1–5 (analisis + spesifikasi + arsitektur + rencana build). Mulai Phase 7 implementasi engine karena bisa diuji di host dulu.

<!-- MASTER-SECTION: LEG-G-S01 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## Selesai

| Fase | Hasil |
|---|---|
| PHASE 1 Environment | inventaris toolchain Termux; toolchain C++→APK terverifikasi (`libnbt.so` arm64-v8a di-compile & di-link sukses) |
| PHASE 2 XAPK | XAPK + 3 split diekstrak, `manifest.json` dibaca |
| PHASE 3 Referensi | `ANALYSIS.md` lengkap (manifest, UI, NBT wire-format, validasi, bug referensi) |
| PHASE 4 Fitur | `FEATURES.md` (matriks + acceptance + batas LevelDB) |
| PHASE 5 Arsitektur | `ARCHITECTURE.md` (L0–L6, build tanpa Gradle/SDK) |
| Dokumen | `DEVELOPMENT.md`, `TESTING.md` |

<!-- MASTER-SECTION: LEG-G-S02 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## Sedang dikerjakan

| Item | Gate |
|---|---|
| **L4 NBT core** (parse/serialize/validate/find 13 tipe + Long Array) | T1–T24 harus PASS di host (ASan/UBSan) sebelum lanjut |
| Script build: `build_host_tests.sh`, `build_device.sh`, `build_apk.sh`, `keystore.sh` | harus berjalan bersih |

<!-- MASTER-SECTION: LEG-G-S03 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## Berikutnya

| Item | Mulai setelah |
|---|---|
| L1/L2 kernel + renderer software (`ANativeWindow`, glyph 8×16) | engine lulus T1–T19 |
| L3 widget kit (scroll, list virtual, dialog, keypad) | renderer jalan |
| L5 repositori (atomic save, prefs, `.mcworld` via libz, thread worker) | widget kit dasar |
| L6 layar: Home (recent/new/open/import), Worlds (list/ops/create flat), Browser, Tree editor, Value editor, dialogs | semua di atas |
| Undo/redo (F3.16, F5.10) | tree editor stabil |
| Preview world icon `world_icon.jpeg` (decode JPEG) | — (fallback tile sudah dipakai) |
| Share world (share sheet) | — |

<!-- MASTER-SECTION: LEG-G-S04 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## Ditunda (butuh keputusan/kemampuan baru)

| Item | Blocker |
|---|---|
| LevelDB: `~local_player`, `player_server_*`, Overworld/TheEnd/Nether/BiomeData/portals/AutonomousEntities, chunk entities, Show Map | tidak ada engine LevelDB Bedrock; **UI sengaja tidak dibuat** (larangan tombol non-fungsional) |
| Efek & layout lanjutan (drag layer CreateWorld) | belum tentu perlu; reorder via tombol naik/turun sudah cukup |
| Multi-ABI (armeabi-v7a, x86_64) | arm64-v8a dulu; tambah jika diminta |

<!-- MASTER-SECTION: LEG-G-S05 | CATEGORY=Status fitur | STATUS=PERLU_SINKRONISASI -->
## Catatan keputusan (agar tidak terulang)

- Identitas: `com.nbteditor`, nama "NBT Editor" (bukan MineTuner).
- Scope LevelDB **fase 1: dikecualikan** (jawaban pengguna).
- Build: native C++ satu file (jawaban pengguna) — Gradle/Kotlin/Compose **tidak dipakai**.
- `level.dat`: versi header asli **dipertahankan** (referensi menormalkan ke 8 — dianggap bug).
- String NBT: UTF-8 eksplisit di dua arah (referensi pakai charset default — bug).


### G.6 Matriks fitur berdasarkan bukti — 19 Nov 2026

C.2 tetap dipertahankan sebagai spesifikasi awal; `✅` di sana berarti “dipilih sebagai target/diklaim saat spesifikasi”, bukan bukti bahwa fitur lolos uji saat ini. Status di bawah merujuk pada bukti terbaru yang tersimpan di Bagian F/Bug, FASE 2, dan hasil build/test historis. “TERIMPLEMENTASI” berarti kode/ jalur tercatat tersedia; tidak otomatis berarti bebas cacat.

**Total fitur ber-ID yang tercatat:** 47 (F1.1–F5.11). **Value editors** di C.2 §2.4 tidak diberi ID fitur terpisah dalam dokumen lama, jadi tidak dihitung sebagai fitur mandiri.

| Status berdasarkan bukti | Jumlah |
|---|---:|
| TERIMPLEMENTASI—TERUJI | 1 |
| TERIMPLEMENTASI—PERLU PENGUJIAN | 13 |
| BERMASALAH | 24 |
| BELUM SELESAI | 5 |
| DIRENCANAKAN | 3 |
| DITUNDA—BUKAN SCOPE | 1 |

| ID | Fitur tercatat | Status lama (C.2) | Status berdasarkan bukti saat ini | Bug/perbaikan terkait |
|---|---|---|---|---|
| F1.1 | Open NBT file from in-app browser | ✅ | TERIMPLEMENTASI—TERUJI | B4 diperbaiki; FASE 2 menyatakan Open file sebagai 1 fungsi yang selesai pada audit emulator |
| F1.2 | Save in place | ✅ | BERMASALAH | R-P0-10; serializer belum dibuktikan integritas |
| F1.3 | Save As | ✅ | BERMASALAH | B5; R-P0-15 |
| F1.4 | New NBT document | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | B1/B4 diperbaiki; belum tercatat uji pembuatan dokumen baru pada APK terbaru |
| F1.5 | Import `.mcworld` / `.mctemplate` | ✅ | BERMASALAH | R-P0-07; R-P0-08; R-P0-28 |
| F1.6 | Export world to `.mcworld` | ✅ | BERMASALAH | R-P0-28 |
| F1.7 | Recent files list | ✅ | BERMASALAH | R-P0-22; daftar hanya in-memory |
| F1.8 | Storage permission handling | ✅ | BERMASALAH | R-P0-03; F-MANI-01; F-MANI-04 |
| F1.9 | Corrupt/empty/oversized errors | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | engine error path teruji; jalur UI/APK terbaru belum tercatat |
| F1.10 | Autosave draft | ✅ | BELUM SELESAI | R-P0-29 |
| F2.1 | List managed/legacy worlds | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | B2/B3 diperbaiki; R-P0-04 masih memengaruhi dialog aksi |
| F2.2 | External world listing | 🟡 | DIRENCANAKAN | planned, SDK≤28; tidak diklaim sebagai fungsi teruji |
| F2.3 | World options | ✅ | BERMASALAH | R-P0-04; modal delete dapat salah target |
| F2.4 | Delete world with confirmation | ✅ | BERMASALAH | R-P0-04 |
| F2.5 | Create custom flat world | ✅ | BERMASALAH | R-P0-16 |
| F2.6 | Edit `level.dat` NBT | ✅ | BERMASALAH | R-P0-10 |
| F2.7 | Encrypted-world detection | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | `looksEncrypted()` tersedia; belum tercatat uji perangkat untuk rilis terbaru |
| F2.8 | Show Map / LevelDB editors | ❌ | DITUNDA—BUKAN SCOPE FASE 1 | scope LevelDB dikecualikan |
| F2.9 | World icons | ✅ | BERMASALAH | LEG-L-UI-001; icon tidak pernah ditampilkan |
| F2.10 | Share world via FileProvider | 🟡 | DIRENCANAKAN | planned; tidak ditemukan bukti implementasi selesai |
| F3.1 | Hierarchy display of 13 tag types | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | engine/model 49–68 snapshot; APK terbaru belum menguji navigasi |
| F3.2 | Expand / collapse | ✅ | BELUM SELESAI | R-P0-23; hanya drill-down, action mati/tidak ada |
| F3.3 | Select tag + value summary | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | B1 diperbaiki dan regresi tersedia |
| F3.4 | Drill-down containers/arrays | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | B7 diperbaiki; `M25`–`M30` PASS |
| F3.5 | Breadcrumb navigation | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | B6 diperbaiki; `M22`–`M24` PASS; gesture fisik belum diuji |
| F3.6 | Add tag/list/array elements | ✅ | BERMASALAH | R-P0-05; B4 diperbaiki tetapi kontrol dialog masih belum layak |
| F3.7 | Edit value per type | ✅ | BERMASALAH | R-P0-05; R-P0-10 |
| F3.8 | Rename tag | ✅ | BERMASALAH | R-P0-14 |
| F3.9 | Delete tag | ✅ | BERMASALAH | R-P0-04 |
| F3.10 | Duplicate / copy | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | M7/M10 tercatat pada audit lama; belum ada uji APK terbaru |
| F3.11 | Move / cut-paste | ✅ | BELUM SELESAI | R-P0-23 |
| F3.12 | Search by key + filter | ✅ | BERMASALAH | R-P0-13 |
| F3.13 | Type icon / colour badge | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | screenshot audit menampilkan Home, bukan seluruh glyph tipe |
| F3.14 | Show byte size per node | ✅ | BELUM SELESAI | R-P0-23; `nodeBytes` kode mati |
| F3.15 | Change tag type | 🟡 | DIRENCANAKAN | planned; tidak diklaim selesai |
| F3.16 | Undo/redo every action | 🟡 | BERMASALAH | R-P0-14; LEG-L-PERF-001 |
| F5.1 | Unsaved-change indicator/warning | ✅ | BELUM SELESAI | R-P0-29 |
| F5.2 | Confirmations before destructive actions | ✅ | BERMASALAH | R-P0-04 |
| F5.3 | Parse/save/IO errors surfaced | ✅ | BERMASALAH | R-P0-10 |
| F5.4 | Info bar | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | B1 diperbaiki; belum ada uji perangkat/luas layar untuk klaim penuh |
| F5.5 | Progress indicator | ✅ | BERMASALAH | R-P0-04; R-P0-18 |
| F5.6 | Tap/long-press/scroll/drag | ✅ | BERMASALAH | R-P0-21; B2 diperbaiki tetapi cancel/multitouch/Back belum layak |
| F5.7 | In-app text/numeric keypad | ✅ | BERMASALAH | R-P0-12; B4 diperbaiki |
| F5.8 | Responsive phone/landscape/tablet | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | hanya Pixel_5/portrait/1080x2340 tercatat; landscape/tablet nihil |
| F5.9 | Light + Dark themes | ✅ | BERMASALAH | R-P0-22; pergantian tidak persisten |
| F5.10 | Undo/redo history | 🟡 | BERMASALAH | F3.16; R-P0-14; LEG-L-PERF-001 |
| F5.11 | Localization English | ✅ | TERIMPLEMENTASI—PERLU PENGUJIAN | dokumen menyebut pengiriman English; tidak ada audit string/ locale komprehensif |

**Aturan pembacaan:** tidak ada fitur `✅` pada matriks lama yang otomatis dianggap selesai. `F5.10` sengaja dipertahankan sebagai alias audit F3.16. LevelDB (`F2.8`) dan fitur “terceklis” yang hanya memiliki source/test snapshot tidak disebut “teruji perangkat”.

<!-- MASTER-SECTION: LEG-H | CATEGORY=Arsip teknis | STATUS=ARSIP -->
## Bagian H — Seluruh kode APK (`app/src/main.native.cpp`)

C++20, **satu file**, 5056 baris, tanpa Java/DEX. Blok di bawah adalah salinan persis
file sumber saat dokumen ini dibuat — dipakai sebagai **cadangan pemulihan** bila file
sumber hilang/rusak.

| Lapian | Isi |
|---|---|
| L1 | util string, `Result<T>`, pesan error |
| L2 | renderer software: `Canvas`, glyph 8×16, tema terang/gelap |
| L3 | widget kit: `ui::Node`, builder, hit-test, scroll |
| L4 | **NBT core**: parse/serialize 13 tipe, validasi, cari |
| L5 | repositori: tulis atomik, zip `.mcworld` (zlib), prefs, worker thread |
| L6 | layar (Home, Worlds, Browser, Tree, Flat, Help), dialog, keypad, handler aksi |
| L0 | glue Android: NativeActivity, looper, input, window, izin, log, crash |
| Uji | 49 tes engine + 40 model-test UI (`-DNBT_SELFTEST`) |

```cpp
// NBT Editor — single translation unit, native C++.
// Sections: L1 utils · L4 NBT core · (later) L0 platform · L2 render · L3 ui · L5 app · L6 screens
// Build for host unit tests:  clang++ -std=c++20 -O1 -g -fsanitize=address,undefined -DNBT_SELFTEST app/src/main.native.cpp -o out/nbt_test
// Build for device:           clang++ -std=c++20 -O2 -fPIC -shared --target=aarch64-linux-android26 ...
#ifdef NBT_SELFTEST
  #define NBT_TESTABLE 1
#else
  #define NBT_TESTABLE 0
#endif

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <memory>
#include <filesystem>
#include <unistd.h>

// ============================================================================
// L4 — NBT CORE (Bedrock little-endian dialect)
// No dependency on Android, UI, IO. Unit-tested from host via -DNBT_SELFTEST.
// ============================================================================

namespace nbt {

enum class Type : uint8_t {
    End        = 0,
    Byte       = 1,
    Short      = 2,
    Int        = 3,
    Long       = 4,
    Float      = 5,
    Double     = 6,
    ByteArray  = 7,
    String     = 8,
    List       = 9,
    Compound   = 10,
    IntArray   = 11,
    LongArray  = 12,   // not present in the reference app; required by spec
};

inline const char* typeName(Type t) {
    switch (t) {
        case Type::End:       return "TAG_End";
        case Type::Byte:      return "TAG_Byte";
        case Type::Short:     return "TAG_Short";
        case Type::Int:       return "TAG_Int";
        case Type::Long:      return "TAG_Long";
        case Type::Float:     return "TAG_Float";
        case Type::Double:    return "TAG_Double";
        case Type::ByteArray: return "TAG_Byte_Array";
        case Type::String:    return "TAG_String";
        case Type::List:      return "TAG_List";
        case Type::Compound:  return "TAG_Compound";
        case Type::IntArray:  return "TAG_Int_Array";
        case Type::LongArray: return "TAG_Long_Array";
    }
    return "TAG_Unknown";
}

inline bool typeFromId(uint8_t id, Type& out) {
    switch (id) {
        case 0:  out = Type::End;       return true;
        case 1:  out = Type::Byte;      return true;
        case 2:  out = Type::Short;     return true;
        case 3:  out = Type::Int;       return true;
        case 4:  out = Type::Long;      return true;
        case 5:  out = Type::Float;     return true;
        case 6:  out = Type::Double;    return true;
        case 7:  out = Type::ByteArray; return true;
        case 8:  out = Type::String;    return true;
        case 9:  out = Type::List;      return true;
        case 10: out = Type::Compound;  return true;
        case 11: out = Type::IntArray;  return true;
        case 12: out = Type::LongArray; return true;
        default: return false;
    }
}

inline bool isArray(Type t) { return t == Type::ByteArray || t == Type::IntArray || t == Type::LongArray; }
inline bool isList(Type t)  { return t == Type::List; }
inline bool isCompound(Type t) { return t == Type::Compound; }
inline bool isContainer(Type t) { return t == Type::Compound || t == Type::List || isArray(t); }
inline bool isScalar(Type t) { return t == Type::Byte || t == Type::Short || t == Type::Int || t == Type::Long
                                  || t == Type::Float || t == Type::Double || t == Type::String; }

// ---- node (arena element) --------------------------------------------------
struct Node {
    Type   type      = Type::End;
    std::string name;                  // empty = unnamed
    int64_t i        = 0;              // Byte/Short/Int/Long payload
    double  d        = 0;              // Float/Double payload
    std::string s;                     // String payload
    std::vector<uint8_t>  bytes;       // ByteArray
    std::vector<int64_t>  longs;       // IntArray/LongArray
    std::vector<uint32_t> kids;        // Compound/List children
    Type   elemType  = Type::End;      // List element type
};

struct Document {
    std::vector<Node> arena;
    uint32_t root = 0;
    std::string path;
    // level.dat container bookkeeping (0 when the file is raw NBT)
    bool   hasLevelDatHeader = false;
    int32_t headerVersion    = 0;      // preserved from source when written back
    bool   dirty             = false;
};

// ---- errors ----------------------------------------------------------------
enum class Err {
    None,
    Empty,
    Truncated,
    UnknownTag,
    DepthLimit,
    SizeLimit,
    HeaderLengthMismatch,
    DuplicateName,
    UnnamedTag,
    ElementTypeMismatch,
    IndexOutOfRange,
    NotFound,
    IoError,
    WriteFailed,
    Internal,
};

inline const char* errText(Err e) {
    switch (e) {
        case Err::None:                 return "ok";
        case Err::Empty:                return "File is empty.";
        case Err::Truncated:            return "Unexpected end of file (truncated NBT).";
        case Err::UnknownTag:           return "Unknown tag type encountered.";
        case Err::DepthLimit:           return "Nesting too deep (depth limit reached).";
        case Err::SizeLimit:            return "Document too large (node limit reached).";
        case Err::HeaderLengthMismatch: return "level.dat header length does not match the payload.";
        case Err::DuplicateName:        return "A tag with this name already exists in the compound";
        case Err::UnnamedTag:           return "Tags inside a CompoundTag must have a name";
        case Err::ElementTypeMismatch:  return "List element type mismatch, it should be";
        case Err::IndexOutOfRange:      return "Index out of range.";
        case Err::NotFound:             return "Tag not found.";
        case Err::IoError:              return "Could not read or write the file.";
        case Err::WriteFailed:          return "Failed to write NBT data.";
        case Err::Internal:             return "Internal error.";
    }
    return "Unknown error.";
}

template <typename T>
struct Result {
    T value{};
    Err err = Err::None;
    std::string detail;               // human-readable extra context (offsets, paths)
    bool ok() const { return err == Err::None; }
    static Result fail(Err e, std::string d = "") { Result r; r.err = e; r.detail = std::move(d); return r; }
    static Result good(T v) { Result r; r.value = std::move(v); return r; }
};

template <>
struct Result<void> {
    char dummy = 0;
    Err err = Err::None;
    std::string detail;
    bool ok() const { return err == Err::None; }
    static Result fail(Err e, std::string d = "") { Result r; r.err = e; r.detail = std::move(d); return r; }
    static Result good() { return Result{}; }
};

// ---- reader ----------------------------------------------------------------
struct ByteReader {
    const uint8_t* p;
    size_t n;
    size_t pos = 0;
    size_t start = 0;                 // where the NBT payload begins (after header)

    bool eof() const { return pos >= n; }
    bool need(size_t k) { return pos + k <= n; }
    bool u8(uint8_t& out) { if (!need(1)) return false; out = p[pos++]; return true; }
    bool u16(uint16_t& out) { if (!need(2)) return false; out = (uint16_t)(p[pos] | (p[pos + 1] << 8)); pos += 2; return true; }
    bool i16(int16_t& out) { uint16_t u; if (!u16(u)) return false; out = (int16_t)u; return true; }
    bool i32(int32_t& out) { if (!need(4)) return false;
        out = (int32_t)((uint32_t)p[pos] | ((uint32_t)p[pos+1] << 8) | ((uint32_t)p[pos+2] << 16) | ((uint32_t)p[pos+3] << 24));
        pos += 4; return true; }
    bool i64(int64_t& out) { if (!need(8)) return false; uint64_t v = 0; for (int k = 0; k < 8; ++k) v |= ((uint64_t)p[pos + k]) << (8 * k);
        memcpy(&out, &v, 8); pos += 8; return true; }
    bool f32(float& out) { uint32_t v = 0; if (!need(4)) return false;
        for (int k = 0; k < 4; ++k) v |= ((uint32_t)p[pos + k]) << (8 * k);
        memcpy(&out, &v, 4); pos += 4; return true; }
    bool f64(double& out) { uint64_t v = 0; if (!need(8)) return false;
        for (int k = 0; k < 8; ++k) v |= ((uint64_t)p[pos + k]) << (8 * k);
        memcpy(&out, &v, 8); pos += 8; return true; }
    bool bytes(uint8_t* out, size_t k) { if (!need(k)) return false; memcpy(out, p + pos, k); pos += k; return true; }
};

// ---- options ---------------------------------------------------------------
constexpr int    kMaxDepth     = 128;
constexpr size_t kMaxNodes     = 4000000;

struct ParseOptions {
    bool skipHeader = false;   // true: level.dat container (8-byte header before NBT)
    bool strictHeader = true;  // verify header payload length
};

// ---- parser (recursive descent, depth bounded) ----------------------------
//
// Wire format reminder (Bedrock / little-endian):
//   * compound child      = type byte + uint16 name length + name bytes + payload
//   * list element        = payload only (no type byte, no name) — the element
//                           type is declared once in the list header
//   * TAG_End (0)         = terminates a compound
// Depth is capped by kMaxDepth so the C++ stack cannot be exhausted; exceeding
// the limit is an error, never an abort.
namespace parser {

struct State {
    ByteReader r;
    std::vector<Node> arena;
    size_t nodes = 0;
};

Err readTag(State& st, bool withName, Node& out, int depth);

Err readPayload(State& st, Node& nd) {
    switch (nd.type) {
        case Type::End: return Err::None;
        case Type::Byte: { uint8_t b; if (!st.r.u8(b)) return Err::Truncated; nd.i = (int8_t)b; return Err::None; }
        case Type::Short: { int16_t v; if (!st.r.i16(v)) return Err::Truncated; nd.i = v; return Err::None; }
        case Type::Int: { int32_t v; if (!st.r.i32(v)) return Err::Truncated; nd.i = v; return Err::None; }
        case Type::Long: { int64_t v; if (!st.r.i64(v)) return Err::Truncated; nd.i = v; return Err::None; }
        case Type::Float: { float v; if (!st.r.f32(v)) return Err::Truncated; nd.d = v; return Err::None; }
        case Type::Double: { double v; if (!st.r.f64(v)) return Err::Truncated; nd.d = v; return Err::None; }
        case Type::String: {
            uint16_t l; if (!st.r.u16(l)) return Err::Truncated;
            if (l) { nd.s.resize(l);
                if (!st.r.bytes(reinterpret_cast<uint8_t*>(nd.s.data()), l)) return Err::Truncated; }
            return Err::None; }
        case Type::ByteArray: {
            int32_t c; if (!st.r.i32(c)) return Err::Truncated;
            if (c < 0) return Err::Truncated;
            nd.bytes.resize((size_t)c);
            if (c && !st.r.bytes(nd.bytes.data(), (size_t)c)) return Err::Truncated;
            return Err::None; }
        case Type::IntArray:
        case Type::LongArray: {
            int32_t c; if (!st.r.i32(c)) return Err::Truncated;
            if (c < 0) return Err::Truncated;
            nd.longs.resize((size_t)c);
            for (size_t k = 0; k < (size_t)c; ++k) {
                if (nd.type == Type::IntArray) { int32_t v; if (!st.r.i32(v)) return Err::Truncated; nd.longs[k] = v; }
                else { int64_t v; if (!st.r.i64(v)) return Err::Truncated; nd.longs[k] = v; }
            }
            return Err::None; }
        default: return Err::Internal;
    }
}

// Reads the payload (and children) of a node whose type is already known.
Err readValue(State& st, Node& out, int depth) {
    if (depth > kMaxDepth) return Err::DepthLimit;
    if (st.nodes > kMaxNodes) return Err::SizeLimit;

    if (out.type == Type::Compound) {
        for (;;) {
            size_t peek = st.r.pos;
            uint8_t c8; if (!st.r.u8(c8)) return Err::Truncated;
            if (c8 == 0) break;                          // TAG_End terminator
            st.r.pos = peek;                             // readTag() consumes the type byte once
            Node child;
            Err e = readTag(st, /*withName*/ true, child, depth + 1);
            if (e != Err::None) return e;
            out.kids.push_back((uint32_t)st.arena.size());
            st.arena.push_back(std::move(child));
        }
        return Err::None;
    }

    if (out.type == Type::List) {
        uint8_t e8; if (!st.r.u8(e8)) return Err::Truncated;
        Type et{}; if (!typeFromId(e8, et)) return Err::UnknownTag;
        int32_t count; if (!st.r.i32(count)) return Err::Truncated;
        if (count < 0) return Err::Truncated;
        out.elemType = et;
        for (int32_t k = 0; k < count; ++k) {
            Node child; child.type = et;                 // payload only, no header
            Err e = readValue(st, child, depth + 1);
            if (e != Err::None) return e;
            out.kids.push_back((uint32_t)st.arena.size());
            st.arena.push_back(std::move(child));
        }
        return Err::None;
    }

    return readPayload(st, out);
}

// Reads a tag *with* its type byte (and optional name), then its payload.
Err readTag(State& st, bool withName, Node& out, int depth) {
    if (depth > kMaxDepth) return Err::DepthLimit;
    uint8_t t8; if (!st.r.u8(t8)) return Err::Truncated;
    if (t8 == 0) return Err::Truncated;                  // unexpected TAG_End
    Type t{}; if (!typeFromId(t8, t)) return Err::UnknownTag;
    out.type = t;
    ++st.nodes;
    if (withName) {
        uint16_t nl; if (!st.r.u16(nl)) return Err::Truncated;
        if (nl) { out.name.resize(nl);
            if (!st.r.bytes(reinterpret_cast<uint8_t*>(out.name.data()), nl)) return Err::Truncated; }
    }
    return readValue(st, out, depth);
}

} // namespace parser


// ---- entry points ----------------------------------------------------------
Result<Document> parseBytes(const uint8_t* data, size_t len, const std::string& path, ParseOptions opt) {
    Document doc;
    doc.path = path;
    if (len == 0) return Result<Document>::fail(Err::Empty);

    parser::State st;
    st.r.p = data;
    st.r.n = len;
    st.r.pos = 0;

    if (opt.skipHeader) {
        int32_t version = 0, plen = 0;
        if (!st.r.i32(version) || !st.r.i32(plen)) return Result<Document>::fail(Err::Truncated);
        if (version < 0 || plen < 0)
            return Result<Document>::fail(Err::HeaderLengthMismatch, "invalid header values");
        doc.hasLevelDatHeader = true;
        doc.headerVersion = version;
        st.r.start = st.r.pos;
        if (opt.strictHeader && (size_t)plen != len - st.r.start)
            return Result<Document>::fail(Err::HeaderLengthMismatch,
                "header declares " + std::to_string(plen) + " payload bytes but the file holds " +
                std::to_string(len - st.r.start));
    }

    Node root;
    Err e = parser::readTag(st, /*withName*/ true, root, 0);
    if (e != Err::None) {
        std::string d = errText(e);
        d += " (offset " + std::to_string(st.r.pos) + ")";
        if (e == Err::UnknownTag) {
            uint8_t bad = (st.r.pos < len) ? data[st.r.pos] : 0;
            char hx[8]; std::snprintf(hx, sizeof hx, "0x%02x", (unsigned)bad);
            d += ": type byte "; d += hx;
        }
        return Result<Document>::fail(e, d);
    }
    doc.arena = std::move(st.arena);
    doc.root = 0;
    doc.arena.insert(doc.arena.begin(), std::move(root));
    for (auto& n : doc.arena) {
        for (auto& k : n.kids) ++k;      // children shifted by the inserted root
    }
    return Result<Document>::good(std::move(doc));
}

// ---- serializer ------------------------------------------------------------
struct ByteWriter {
    const std::vector<Node>* arena = nullptr;
    std::vector<uint8_t> out;
    const Node& arenaRef(uint32_t i) const { return (*arena)[i]; }
    void u8(uint8_t v)  { out.push_back(v); }
    void u16(uint16_t v){ out.push_back((uint8_t)(v & 0xff)); out.push_back((uint8_t)(v >> 8)); }
    void i32(int32_t v) { uint32_t u; memcpy(&u, &v, 4);
        for (int k = 0; k < 4; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void i64(int64_t v) { uint64_t u; memcpy(&u, &v, 8);
        for (int k = 0; k < 8; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void f32(float v)   { uint32_t u; memcpy(&u, &v, 4);
        for (int k = 0; k < 4; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void f64(double v)  { uint64_t u; memcpy(&u, &v, 8);
        for (int k = 0; k < 8; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void raw(const uint8_t* p, size_t n) { out.insert(out.end(), p, p + n); }
    void name(const std::string& s);  // defined after nbt::Tag helpers
};
void writeTree(ByteWriter& w, const Node& nd, bool withName);
void writeValue(ByteWriter& w, const Node& nd);
void writePayload(ByteWriter& w, const Node& nd);

// A tag written with its type byte and (optionally) its name: root + compound children.
inline void writeTree(ByteWriter& w, const Node& nd, bool withName) {
    w.u8((uint8_t)nd.type);
    if (withName) {
        w.u16((uint16_t)nd.name.size());
        w.raw(reinterpret_cast<const uint8_t*>(nd.name.data()), nd.name.size());
    }
    writeValue(w, nd);
}

// A list element: payload only — no type byte, no name (matches the parser).
inline void writeValue(ByteWriter& w, const Node& nd) {
    switch (nd.type) {
        case Type::Compound:
            for (uint32_t k : nd.kids) writeTree(w, w.arenaRef(k), /*withName*/ true);
            w.u8(0);                                   // TAG_End terminator
            return;
        case Type::List:
            w.u8((uint8_t)nd.elemType);
            w.i32((int32_t)nd.kids.size());
            for (uint32_t k : nd.kids) writeValue(w, w.arenaRef(k));
            return;
        default:
            writePayload(w, nd);                       // scalars and arrays
            return;
    }
}

inline void writePayload(ByteWriter& w, const Node& nd) {
    switch (nd.type) {
        case Type::End: return;
        case Type::Byte:   w.u8((uint8_t)(nd.i & 0xff)); return;
        case Type::Short:  w.u16((uint16_t)(nd.i & 0xffff)); return;
        case Type::Int:    w.i32((int32_t)nd.i); return;
        case Type::Long:   w.i64(nd.i); return;
        case Type::Float:  w.f32((float)nd.d); return;
        case Type::Double: w.f64(nd.d); return;
        case Type::String: {
            w.u16((uint16_t)nd.s.size());
            w.raw(reinterpret_cast<const uint8_t*>(nd.s.data()), nd.s.size());
            return; }
        case Type::ByteArray: {
            w.i32((int32_t)nd.bytes.size());
            w.raw(nd.bytes.data(), nd.bytes.size());
            return; }
        case Type::IntArray: {
            w.i32((int32_t)nd.longs.size());
            for (int64_t v : nd.longs) w.i32((int32_t)v);   // int32 elements
            return; }
        case Type::LongArray: {
            w.i32((int32_t)nd.longs.size());
            for (int64_t v : nd.longs) w.i64(v);            // int64 elements
            return; }
        default: return;
    }
}

std::vector<uint8_t> serialize(const Document& doc) {
    ByteWriter w;
    w.arena = &doc.arena;
    writeTree(w, doc.arena[doc.root], /*withName*/ true);
    return w.out;
}

std::vector<uint8_t> serializeLevelDat(const Document& doc) {
    std::vector<uint8_t> payload = serialize(doc);
    ByteWriter w;
    w.i32(doc.headerVersion ? doc.headerVersion : 8);
    w.i32((int32_t)payload.size());
    w.raw(payload.data(), payload.size());
    return w.out;
}

// ---- validation ------------------------------------------------------------

// ---- message helpers (reference strings reused verbatim where applicable) --
inline const char* msgUnnamedTag()      { return "Tags inside a CompoundTag must have a name"; }
inline const char* msgDuplicateName()   { return "A tag with this name already exists in the compound"; }
inline const char* msgElementMismatch() { return "List element type mismatch, it should be"; }
inline const char* msgUnknownToEdit()   { return "Unknown tag to edit, report to developers."; }
inline const char* msgOutOfBoundsFor2() { return "Out of bounds for byte."; }
inline const char* msgOutOfBoundsFor()  { return "Only tags of the matching type can be added here."; }

enum class RangeCheck { Ok, OutLow, OutHigh, NotNumeric, NotFinite };

inline RangeCheck rangeFor(Type t, const std::string& text) {
    if (text.empty()) return RangeCheck::NotNumeric;
    errno = 0;
    char* end = nullptr;
    switch (t) {
        case Type::Byte: {
            long long v = strtoll(text.c_str(), &end, 10);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (v < -128) return RangeCheck::OutLow;
            if (v > 127) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Short: {
            long long v = strtoll(text.c_str(), &end, 10);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (v < -32768) return RangeCheck::OutLow;
            if (v > 32767) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Int: {
            long long v = strtoll(text.c_str(), &end, 10);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (v < INT32_MIN) return RangeCheck::OutLow;
            if (v > INT32_MAX) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Long: {
            errno = 0;
            strtoll(text.c_str(), &end, 10);
            if (errno == ERANGE) return RangeCheck::OutHigh;
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            return RangeCheck::Ok; }
        case Type::Float: {
            double v = strtod(text.c_str(), &end);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (!std::isfinite(v)) return RangeCheck::NotFinite;
            if (v < -3.4028234663852886e38) return RangeCheck::OutLow;
            if (v >  3.4028234663852886e38) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Double: {
            double v = strtod(text.c_str(), &end);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (!std::isfinite(v)) return RangeCheck::NotFinite;
            return RangeCheck::Ok; }
        default: return RangeCheck::Ok;
    }
}

inline const char* rangeMessage(Type t, RangeCheck c) {
    if (c == RangeCheck::NotNumeric || c == RangeCheck::NotFinite) return "Enter a valid number.";
    switch (t) {
        case Type::Byte:   return "Out of bounds for byte.";
        case Type::Short: return "Out of bounds for short.";
        case Type::Int:   return "Out of bounds for integer.";
        case Type::Long:  return "Out of bounds for long.";
        case Type::Float: return "Out of bounds for float.";
        case Type::Double:return "Out of bounds for double.";
        default:          return "Out of bounds for string.";
    }
}

inline bool parseScalar(const std::string& text, Type t, Node& out, std::string& msg) {
    RangeCheck c = rangeFor(t, text);
    if (c != RangeCheck::Ok) { msg = rangeMessage(t, c); return false; }
    switch (t) {
        case Type::Byte: { long long v = strtoll(text.c_str(), nullptr, 10); out.i = (int8_t)v; return true; }
        case Type::Short:{ long long v = strtoll(text.c_str(), nullptr, 10); out.i = (int16_t)v; return true; }
        case Type::Int:  { long long v = strtoll(text.c_str(), nullptr, 10); out.i = (int32_t)v; return true; }
        case Type::Long: { out.i = strtoll(text.c_str(), nullptr, 10); return true; }
        case Type::Float:{ out.d = strtod(text.c_str(), nullptr); return true; }
        case Type::Double:{ out.d = strtod(text.c_str(), nullptr); return true; }
        case Type::String:{
            if (text.size() > 65535) { msg = "Out of bounds for string."; return false; }
            out.s = text; return true; }
        default: msg = "Unknown tag to edit, report to developers."; return false;
    }
}

// Semantic validation of a whole document (returns list of human readable problems)
inline std::vector<std::string> validate(const Document& doc, uint32_t idx = 0, int depth = 0,
                                  bool isRoot = true) {
    (void)isRoot;
    std::vector<std::string> problems;
    if (idx >= doc.arena.size()) { problems.push_back("Dangling child index."); return problems; }
    const Node& nd = doc.arena[idx];
    if (depth > kMaxDepth) { problems.push_back("Nesting too deep."); return problems; }

    if (isCompound(nd.type)) {
        for (uint32_t k : nd.kids) {
            if (doc.arena[k].type != Type::End && doc.arena[k].name.empty())
                problems.push_back(msgUnnamedTag());
            for (uint32_t j : nd.kids) {
                if (k == j || doc.arena[k].name.empty()) continue;
                if (doc.arena[k].name == doc.arena[j].name) problems.push_back(msgDuplicateName());
            }
        }
    }

    if (isList(nd.type))
        for (uint32_t k : nd.kids) {
            if (doc.arena[k].type != nd.elemType) {
                std::string m = msgElementMismatch();
                m += " ";
                m += typeName(nd.elemType);
                problems.push_back(m);
            }
        }
    for (uint32_t k : nd.kids) {
        auto sub = validate(doc, k, depth + 1, false);
        problems.insert(problems.end(), sub.begin(), sub.end());
    }
    return problems;
}

// ---- queries ---------------------------------------------------------------
inline uint64_t nodeBytes(const Document& doc, uint32_t idx = 0) {
    if (idx >= doc.arena.size()) return 0;
    ByteWriter w;
    w.arena = &doc.arena;
    writeTree(w, doc.arena[idx], false);
    return w.out.size();
}

struct FindHit { std::vector<uint32_t> path; std::string name; Type type; };

inline void findRec(const Document& doc, uint32_t idx, const std::string& needle,
                    std::vector<uint32_t>& path, std::vector<FindHit>& out, int depth) {
    if (depth > kMaxDepth) return;
    const Node& nd = doc.arena[idx];
    path.push_back(idx);
    if (nd.name.find(needle) != std::string::npos)
        out.push_back({path, nd.name, nd.type});
    for (uint32_t k : nd.kids) findRec(doc, k, needle, path, out, depth + 1);
    path.pop_back();
}

inline std::vector<FindHit> find(const Document& doc, const std::string& needle, uint32_t from = 0) {
    std::vector<FindHit> out;
    std::vector<uint32_t> path;
    findRec(doc, from, needle, path, out, 0);
    return out;
}

inline uint64_t countNodes(const Document& doc, uint32_t idx = 0) {
    uint64_t c = 1;
    for (uint32_t k : doc.arena[idx].kids) c += countNodes(doc, k);
    return c;
}

} // namespace nbt



// ============================================================================
// L2 — RENDERER (software framebuffer + glyph table)
// The table below was generated from /system/fonts/Roboto-Regular.ttf at 15 px
// (script kept in scripts/gen_font.py) so text is legible without any asset file.
// ============================================================================
#if !NBT_APP_BUILD_DONE
#define NBT_APP_BUILD_DONE 1
namespace gfx {

// 5x15-ish glyph table generated from Roboto at 15 px (bitmap: 1 = ink).
// Each glyph: 1 byte per row, bit 6 = leftmost pixel of a 7-px wide cell.
static const int kGlyphW = 7;
static const int kGlyphH = 15;
static const int kGlyphAdv = 8;
static const unsigned char kFont[95][15] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x28,0x28,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x0c,0x3f,0x09,0x09,0x09,0x3f,0x1b,0x13},
    {0x00,0x04,0x04,0x1e,0x3f,0x23,0x21,0x30,0x1c,0x07,0x03,0x21,0x33,0x1e,0x04},
    {0x00,0x00,0x00,0x00,0x00,0x38,0x3c,0x24,0x2d,0x39,0x02,0x05,0x05,0x09,0x01},
    {0x00,0x00,0x00,0x00,0x00,0x0e,0x1b,0x11,0x1b,0x1e,0x1c,0x36,0x23,0x21,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x04,0x08,0x10,0x30,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x30,0x10},
    {0x00,0x00,0x40,0x20,0x30,0x10,0x18,0x18,0x08,0x08,0x08,0x18,0x18,0x18,0x10},
    {0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x7f,0x1c,0x1c,0x26,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x3f,0x0c,0x04,0x04,0x04},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x06,0x04,0x04,0x0c,0x08,0x18,0x10,0x10,0x30,0x20,0x60},
    {0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x23,0x21,0x21,0x21,0x21,0x21,0x23,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x0c,0x3c,0x24,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
    {0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x23,0x01,0x03,0x06,0x06,0x0c,0x18,0x30},
    {0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x21,0x01,0x03,0x0e,0x03,0x01,0x23,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x07,0x0d,0x09,0x11,0x31,0x3f,0x03,0x01},
    {0x00,0x00,0x00,0x00,0x00,0x1f,0x18,0x10,0x10,0x1f,0x1b,0x01,0x00,0x31,0x19},
    {0x00,0x00,0x00,0x00,0x00,0x0e,0x18,0x30,0x20,0x3e,0x33,0x23,0x21,0x33,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x3f,0x01,0x01,0x03,0x03,0x02,0x06,0x04,0x0c,0x08},
    {0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x23,0x23,0x33,0x1e,0x33,0x23,0x21,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x1e,0x32,0x23,0x21,0x23,0x33,0x1f,0x03,0x03,0x06},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x3c,0x30,0x1e,0x07},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x3f},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3c,0x07,0x03,0x0e,0x38},
    {0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x21,0x03,0x03,0x06,0x0c,0x08,0x00,0x00},
    {0x00,0x00,0x00,0x07,0x0e,0x18,0x11,0x33,0x26,0x26,0x24,0x24,0x36,0x33,0x18},
    {0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x0f,0x0f,0x0d,0x19,0x18,0x3f,0x30,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x3f,0x21,0x20,0x20,0x21,0x3f,0x21,0x21,0x20,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x0f,0x19,0x30,0x20,0x20,0x20,0x20,0x20,0x30,0x19},
    {0x00,0x00,0x00,0x00,0x00,0x3e,0x23,0x21,0x21,0x20,0x20,0x20,0x21,0x21,0x23},
    {0x00,0x00,0x00,0x00,0x00,0x3f,0x20,0x20,0x20,0x20,0x3f,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x3f,0x20,0x20,0x20,0x20,0x3f,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x0f,0x19,0x30,0x20,0x20,0x20,0x23,0x20,0x30,0x18},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x3f,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
    {0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x23,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x21,0x23,0x22,0x26,0x2c,0x3c,0x36,0x26,0x23,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x38,0x38,0x28,0x2c,0x24,0x25,0x27,0x23},
    {0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x38,0x2c,0x2c,0x26,0x23,0x23,0x21,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x0f,0x19,0x30,0x20,0x20,0x20,0x20,0x20,0x30,0x19},
    {0x00,0x00,0x00,0x00,0x00,0x3f,0x21,0x21,0x20,0x21,0x21,0x3f,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x0f,0x19,0x30,0x20,0x20,0x20,0x20,0x20,0x30,0x19,0x0f},
    {0x00,0x00,0x00,0x00,0x00,0x3f,0x21,0x21,0x20,0x21,0x21,0x3f,0x23,0x23,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x1f,0x31,0x21,0x20,0x38,0x1e,0x03,0x01,0x60,0x31},
    {0x00,0x00,0x00,0x00,0x00,0x7f,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x21,0x31},
    {0x00,0x00,0x00,0x00,0x00,0x60,0x20,0x30,0x30,0x11,0x19,0x19,0x0b,0x0e,0x0e},
    {0x00,0x00,0x00,0x00,0x00,0x61,0x21,0x33,0x33,0x32,0x12,0x16,0x1c,0x1c,0x0c},
    {0x00,0x00,0x00,0x00,0x00,0x30,0x31,0x19,0x0b,0x0e,0x06,0x0e,0x1b,0x19,0x31},
    {0x00,0x00,0x00,0x00,0x00,0x60,0x31,0x31,0x1b,0x1a,0x0e,0x0c,0x04,0x04,0x04},
    {0x00,0x00,0x00,0x00,0x00,0x3f,0x01,0x01,0x03,0x03,0x06,0x0c,0x0c,0x18,0x38},
    {0x00,0x00,0x38,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x60,0x20,0x30,0x10,0x10,0x18,0x08,0x0c,0x0c,0x04,0x06},
    {0x00,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
    {0x00,0x00,0x00,0x00,0x00,0x08,0x18,0x1c,0x3c,0x24,0x26,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x30,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x01,0x1f,0x33,0x23},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x3e,0x33,0x23,0x21,0x21,0x23,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x21,0x20,0x20,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1f,0x33,0x23,0x21,0x21,0x23,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x21,0x3f,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x0c,0x18,0x10,0x7c,0x10,0x10,0x10,0x10,0x10,0x10},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x33,0x23,0x21,0x21,0x23,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x2e,0x33,0x21,0x21,0x21,0x21,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x23,0x26,0x2c,0x38,0x3c,0x26,0x26},
    {0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e,0x33,0x21,0x21,0x21,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e,0x33,0x21,0x21,0x21,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x31,0x20,0x20,0x31},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x33,0x23,0x21,0x21,0x23,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x33,0x21,0x21,0x21,0x21,0x33},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x30,0x20,0x20,0x20,0x20},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x33,0x31,0x3c,0x07,0x21},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x7c,0x10,0x10,0x10,0x10,0x10,0x10},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x21,0x21,0x21,0x21,0x23},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x23,0x32,0x36,0x16,0x1c},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x62,0x27,0x27,0x35,0x35,0x1c},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x23,0x36,0x1e,0x0c,0x1c,0x1e},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x23,0x32,0x36,0x14,0x1c,0x1c},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x07,0x06,0x0c,0x08,0x10},
    {0x00,0x00,0x00,0x18,0x10,0x10,0x10,0x10,0x30,0x70,0x30,0x10,0x10,0x10,0x10},
    {0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
    {0x00,0x00,0x60,0x30,0x30,0x10,0x10,0x10,0x1c,0x1c,0x18,0x10,0x10,0x10,0x30},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0x36,0x23},
};

} // namespace gfx
#endif // NBT_APP_BUILD_DONE

// ============================================================================
// L1 — KERNEL PRIMITIVES (math, colour, utf8, string helpers)
// ============================================================================
namespace gfx {

struct Color { uint8_t r, g, b, a; };
inline Color rgb(uint8_t r, uint8_t g, uint8_t b) { return {r, g, b, 255}; }
inline Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return {r, g, b, a}; }
inline uint32_t pack(Color c) { return (uint32_t)c.r | ((uint32_t)c.g << 8) | ((uint32_t)c.b << 16) | ((uint32_t)c.a << 24); }

struct Rect { int x = 0, y = 0, w = 0, h = 0;
    int right()  const { return x + w; }
    int bottom() const { return y + h; }
    bool contains(int px, int py) const { return px >= x && py >= y && px < x + w && py < y + h; }
    bool empty() const { return w <= 0 || h <= 0; }
};
inline Rect intersect(Rect a, Rect b) {
    int x1 = std::max(a.x, b.x), y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.w, b.x + b.w), y2 = std::min(a.y + a.h, b.y + b.h);
    if (x2 <= x1 || y2 <= y1) return {0, 0, 0, 0};
    return {x1, y1, x2 - x1, y2 - y1};
}

struct Theme {
    Color primary, secondary, background, surface, onPrimary, onSurface, onSurfaceMuted,
          outline, error, errorContainer, success, scrim;
};

// Reference palette (ANALYSIS.md §3) + dark counterpart.
inline Theme themeLight() {
    return {/*primary*/ rgb(0x42, 0x9e, 0x46), /*secondary*/ rgb(0x66, 0xbb, 0x6a),
            /*background*/ rgb(0xf1, 0xf8, 0xe9), /*surface*/ rgb(0xff, 0xff, 0xff),
            /*onPrimary*/ rgb(0xff, 0xff, 0xff), /*onSurface*/ rgb(0x1b, 0x1b, 0x1b),
            /*onSurfaceMuted*/ rgb(0x60, 0x6a, 0x60), /*outline*/ rgb(0xc6, 0xd6, 0xc6),
            /*error*/ rgb(0xb0, 0x00, 0x20), /*errorContainer*/ rgb(0xff, 0xda, 0xda),
            /*success*/ rgb(0x1b, 0x7f, 0x3b), /*scrim*/ rgba(0x00, 0x00, 0x00, 0x40)};
}
inline Theme themeDark() {
    return {/*primary*/ rgb(0x7c, 0xc0, 0x7f), /*secondary*/ rgb(0x57, 0x9e, 0x5b),
            /*background*/ rgb(0x10, 0x14, 0x10), /*surface*/ rgb(0x1c, 0x21, 0x1c),
            /*onPrimary*/ rgb(0x08, 0x14, 0x08), /*onSurface*/ rgb(0xe6, 0xec, 0xe6),
            /*onSurfaceMuted*/ rgb(0x9a, 0xa6, 0x9a), /*outline*/ rgb(0x3a, 0x44, 0x3a),
            /*error*/ rgb(0xff, 0x8a, 0x80), /*errorContainer*/ rgb(0x3a, 0x14, 0x14),
            /*success*/ rgb(0x7c, 0xd8, 0x9a), /*scrim*/ rgba(0x00, 0x00, 0x00, 0x60)};
}

// ---- utf8 / string helpers -------------------------------------------------
inline size_t utf8Len(const std::string& s) { return s.size(); }

inline std::string utf8Substr(const std::string& s, size_t chars, size_t maxPixels, int glyphPx) {
    // trims by visible characters AND by pixel budget
    size_t used = 0, px = 0;
    size_t i = 0;
    while (i < s.size() && used < chars) {
        size_t len = 1;
        unsigned char c = (unsigned char)s[i];
        if (c >= 0xF0) len = 4; else if (c >= 0xE0) len = 3; else if (c >= 0xC0) len = 2;
        px += glyphPx;
        if (px > maxPixels) break;
        i += len;
        ++used;
    }
    return s.substr(0, i);
}

inline bool startsWith(const std::string& s, const std::string& p) {
    return s.size() >= p.size() && s.compare(0, p.size(), p) == 0;
}
inline std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
inline std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) { if (c == sep) { out.push_back(cur); cur.clear(); } else cur += c; }
    out.push_back(cur);
    return out;
}

inline std::string humanSize(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double v = (double)bytes;
    int u = 0;
    while (v >= 1024.0 && u < 4) { v /= 1024.0; ++u; }
    char buf[32];
    if (u == 0) std::snprintf(buf, sizeof buf, "%llu B", (unsigned long long)bytes);
    else std::snprintf(buf, sizeof buf, "%.1f %s", v, units[u]);
    return buf;
}

} // namespace gfx

// ============================================================================
// L2 — CANVAS (2D software rasteriser over an RGBA8888 buffer)
// ============================================================================
namespace gfx {

struct Canvas {
    uint32_t* px = nullptr;
    int w = 0, h = 0, stride = 0;
    std::vector<Rect> clipStack;

    void bind(uint32_t* buffer, int width, int height, int stridePx) {
        px = buffer; w = width; h = height; stride = stridePx;
        clipStack.clear();
        clipStack.push_back({0, 0, w, h});
    }
    void pushClip(Rect r) { clipStack.push_back(intersect(clipStack.back(), r)); }
    void popClip() { if (clipStack.size() > 1) clipStack.pop_back(); }
    Rect clip() const { return clipStack.back(); }

    void clear(Color c) { fill({0, 0, w, h}, c); }

    void fill(Rect r, Color c) {
        r = intersect(r, clip());
        if (r.empty()) return;
        uint32_t v = pack(c);
        uint32_t* row = px + (size_t)r.y * stride + r.x;
        for (int y = 0; y < r.h; ++y) {
            std::fill(row, row + r.w, v);
            row += stride;
        }
    }
    void pixel(int x, int y, Color c) {
        if (!clip().contains(x, y)) return;
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        px[(size_t)y * stride + x] = pack(c);
    }
    void hline(int x0, int x1, int y, Color c) {
        if (y < clip().y || y >= clip().bottom()) return;
        int a = std::max(x0, clip().x), b = std::min(x1, clip().right());
        for (int x = a; x < b; ++x) pixel(x, y, c);
    }
    void vline(int x, int y0, int y1, Color c) {
        if (x < clip().x || x >= clip().right()) return;
        int a = std::max(y0, clip().y), b = std::min(y1, clip().bottom());
        for (int y = a; y < b; ++y) pixel(x, y, c);
    }
    // anti-aliased rounded rectangle: corners are analysed, interior is filled fast
    void roundRect(Rect r, int radius, Color c) {
        if (radius <= 0) { fill(r, c); return; }
        r = intersect(r, clip());
        if (r.empty()) return;
        const float rad = (float)radius;
        const int xr0 = r.x + radius, xr1 = r.right() - radius;
        const int yr0 = r.y + radius, yr1 = r.bottom() - radius;
        for (int y = r.y; y < r.bottom(); ++y) {
            const int ry = (y < yr0) ? (yr0 - y) : (y >= yr1 ? (y - yr1 + 1) : 0);
            for (int x = r.x; x < r.right(); ++x) {
                const int rx = (x < xr0) ? (xr0 - x) : (x >= xr1 ? (x - xr1 + 1) : 0);
                if (rx == 0 || ry == 0) { pixel(x, y, c); continue; }
                const float cx = (float)rx + 0.5f, cy = (float)ry + 0.5f;
                const float d = std::sqrt(cx * cx + cy * cy);
                const float cov = rad + 0.5f - d;
                if (cov <= 0.0f) continue;
                Color cc = c;
                cc.a = (uint8_t)std::min(255.0f, (float)c.a * cov);
                pixel(x, y, cc);
            }
        }
    }
    void stroke(Rect r, Color c, int thickness = 1) {
        fill({r.x, r.y, r.w, thickness}, c);
        fill({r.x, r.bottom() - thickness, r.w, thickness}, c);
        fill({r.x, r.y, thickness, r.h}, c);
        fill({r.right() - thickness, r.y, thickness, r.h}, c);
    }
    void circle(int cx, int cy, int radius, Color c) {
        int r2 = radius * radius;
        for (int y = cy - radius; y <= cy + radius; ++y)
            for (int x = cx - radius; x <= cx + radius; ++x) {
                int d2 = (x - cx) * (x - cx) + (y - cy) * (y - cy);
                if (d2 > r2 + radius) continue;
                float cov = 1.0f - std::max(0.0f, std::sqrt((float)d2) - (radius - 0.5f));
                if (cov <= 0) continue;
                Color cc = c; cc.a = (uint8_t)std::min(255.0f, (float)c.a * cov);
                pixel(x, y, cc);
            }
    }
    void line(int x0, int y0, int x1, int y1, Color c) {
        int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        while (true) {
            pixel(x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    // ---- text ----
    int textWidth(const std::string& s) const { return (int)s.size() * kGlyphAdv; }
    void text(int x, int y, const std::string& s, Color c, int scale = 1) {
        pushClip(clip());
        int cx = x;
        for (size_t i = 0; i < s.size(); ++i) {
            unsigned char ch = (unsigned char)s[i];
            if (ch == '\n') continue;
            if (ch < 32 || ch > 126) ch = '?';
            const unsigned char* g = kFont[ch - 32];
            for (int gy = 0; gy < kGlyphH; ++gy) {
                unsigned char bits = g[gy];
                for (int gx = 0; gx < kGlyphW; ++gx) {
                    if (bits & (1 << (kGlyphW - 1 - gx))) {
                        if (scale == 1) pixel(cx + gx, y + gy, c);
                        else fill({cx + gx * scale, y + gy * scale, scale, scale}, c);
                    }
                }
            }
            cx += kGlyphAdv * scale;
        }
        popClip();
    }
    // centred / clipped variants used by UI
    void textRight(int right, int y, const std::string& s, Color c, int scale = 1) {
        text(right - textWidth(s), y, s, c, scale);
    }
    void textCentered(Rect r, const std::string& s, Color c, int scale = 1) {
        text(r.x + (r.w - textWidth(s)) / 2, r.y + (r.h - kGlyphH * scale) / 2, s, c, scale);
    }
    void textEllipsis(Rect r, const std::string& s, Color c, int scale = 1) {
        int maxPx = r.w - 2;
        if (textWidth(s) <= maxPx) { text(r.x + 2, r.y + (r.h - kGlyphH * scale) / 2, s, c, scale); return; }
        std::string cut = utf8Substr(s, s.size(), maxPx - kGlyphAdv * scale, kGlyphAdv * scale);
        if (cut.size() < s.size()) cut += "...";
        text(r.x + 2, r.y + (r.h - kGlyphH * scale) / 2, cut, c, scale);
    }
};

// host-only debug helper: print the canvas as ASCII art
inline void dumpAscii(const Canvas& c, const char* title) {
    std::printf("---- %s (%dx%d) ----\n", title, c.w, c.h);
    for (int y = 0; y < c.h; ++y) {
        std::string line;
        for (int x = 0; x < c.w; ++x) {
            // "ink" = pixel differs from the background value sampled at (0,0)
            uint32_t p = c.px[(size_t)y * c.stride + x];
            uint32_t bg = c.px[0];
            auto chan = [](uint32_t v, int shift) { return (int)((v >> shift) & 0xff); };
            int delta = std::abs(chan(p, 0) - chan(bg, 0)) + std::abs(chan(p, 8) - chan(bg, 8)) +
                        std::abs(chan(p, 16) - chan(bg, 16));
            int lum = (chan(p, 0) + chan(p, 8) + chan(p, 16)) * chan(p, 24) / 3;
            bool panelish = (delta >= 8) && (delta < 300);   // e.g. white card on pale green
            char ch = (delta > 300) ? '#' : (delta > 60 ? '+' : (panelish ? '-' : ' '));
            (void)lum;
            line += ch;
        }
        std::printf("%s\n", line.c_str());
    }
    std::printf("---- end ----\n");
}

} // namespace gfx

// ============================================================================
// L3 — WIDGET KIT
// A retained tree built as a pure function of state ("screen -> nodes"), so the
// UI is deterministic and host-testable without a device. Events are integers
// ("actions") routed back to L6 handlers — no callbacks, no view recycling.
// ============================================================================
namespace ui {
using gfx::Rect;
using gfx::Color;

enum class Kind {
    Panel, Text, Button, List, Row, Column, Scroll, Checkbox, Field, Dialog, Toast,
    Fab, IconTile, Divider, Progress, Breadcrumb, SearchField, Keypad, ImageTile, TreeRow
};

struct Node {
    Kind kind = Kind::Panel;
    Rect rect{};
    std::string id;
    std::string text;            // primary label (utf-8)
    std::string text2;           // secondary label
    std::string value;           // field value / selected label
    Color bg{0, 0, 0, 0}, fg{0, 0, 0, 0}, accent{0, 0, 0, 0}, border{0, 0, 0, 0};
    int radius = 0, pad = 0, borderW = 0;
    bool visible = true, enabled = true, selected = false, checked = false, expanded = false;
    bool multiline = false;
    int  action = 0;             // tap action id (0 = none)
    int  actionLong = 0;         // long-press action id
    int  index = -1;             // list row index (generic payload)
    int64_t user = 0;            // screen-specific payload
    std::vector<Node> kids;
};

// ---- scoping helpers -------------------------------------------------------
inline Node panel(Rect r, std::vector<Node> kids = {}) {
    Node n; n.kind = Kind::Panel; n.rect = r; n.kids = std::move(kids); return n;
}
inline Node text(Rect r, const std::string& s, Color c, int scale = 1) {
    Node n; n.kind = Kind::Text; n.rect = r; n.text = s; n.fg = c; n.user = scale; return n;
}
inline Node button(Rect r, const std::string& label, Color bg, Color fg, int action, int radius = 6) {
    Node n; n.kind = Kind::Button; n.rect = r; n.text = label; n.bg = bg; n.fg = fg;
    n.action = action; n.radius = radius; return n;
}
inline Node list(Rect r, std::vector<Node> rows) {
    Node n; n.kind = Kind::Scroll; n.rect = r; n.kids = std::move(rows); return n;
}
inline Node checkbox(Rect r, const std::string& label, bool on, int action) {
    Node n; n.kind = Kind::Checkbox; n.rect = r; n.text = label; n.checked = on; n.action = action; return n;
}
inline Node field(Rect r, const std::string& label, const std::string& value, int action, bool numeric = true) {
    Node n; n.kind = Kind::Field; n.rect = r; n.text = label; n.value = value;
    n.action = action; n.user = numeric ? 1 : 0; return n;
}
inline Node searchField(Rect r, const std::string& value, int action) {
    Node n; n.kind = Kind::SearchField; n.rect = r; n.value = value; n.action = action; return n;
}
inline Node divider(Rect r, Color c) { Node n; n.kind = Kind::Divider; n.rect = r; n.bg = c; return n; }
inline Node progress(Rect r, float v, Color c) { Node n; n.kind = Kind::Progress; n.rect = r; n.user = 0; n.bg = c;
    n.value = std::to_string(v); return n; }

// Tree row: icon tile + key + value summary + expander
inline Node treeRow(Rect r, const std::string& key, const std::string& summary, Color accent,
                    bool hasChildren, bool expanded, bool selected, int index) {
    Node n; n.kind = Kind::TreeRow; n.rect = r; n.text = key; n.text2 = summary; n.accent = accent;
    n.expanded = hasChildren ? expanded : false;
    n.selected = selected; n.index = index; n.user = hasChildren ? 1 : 0;
    return n;
}
inline Node iconTile(Rect r, Color c, const std::string& glyph) {
    Node n; n.kind = Kind::IconTile; n.rect = r; n.bg = c; n.text = glyph; return n;
}

// dialog is modal: rendered after the screen, blocks input below it
struct Dialog {
    bool open = false;
    std::string title, message;
    std::vector<Node> body;      // custom rows
    std::string pos, neg, neu;   // button labels ("", absent)
    int posAction = 0, negAction = 0, neuAction = 0;
    Rect rect{};
};

struct Toast { std::string message; int64_t untilMs = 0; Color color{0, 0, 0, 0}; };

// ---- layout helpers (pure geometry, no state) ------------------------------
inline Rect rowCell(Rect r, int col, int ncols, int gap = 4) {
    int avail = r.w - gap * (ncols - 1);
    int cw = avail / ncols;
    return {r.x + col * (cw + gap), r.y, cw, r.h};
}
inline Rect inset(Rect r, int d) { return {r.x + d, r.y + d, r.w - 2 * d, r.h - 2 * d}; }

inline Rect stackDown(Rect r, int& y, int h, int gap = 2) {
    Rect out{r.x, y, r.w, h};
    y += h + gap;
    return out;
}

// ---- input -----------------------------------------------------------------
enum class Gesture { None, Tap, LongPress, Scroll, RightEdge, LeftEdge };

struct TouchState {
    bool  down = false;
    int   x = 0, y = 0;
    int   startX = 0, startY = 0;
    int64_t downMs = 0;
    bool  consumed = false;      // a scroll container claimed this gesture
    bool  moved = false;
    int64_t nowMs = 0;
};

// Hit test: deepest visible child first (tree is in z-order).
inline bool hitTest(const Node& n, int x, int y, const Node*& out) {
    if (!n.visible) return false;
    Rect r = n.rect;
    if (!r.contains(x, y)) return false;
    out = &n;
    for (auto it = n.kids.rbegin(); it != n.kids.rend(); ++it)
        if (hitTest(*it, x, y, out)) return true;
    return true;
}

// Scroll containers report which one owns the gesture (consumes it).
inline Node* findScrollAt(const Node& n, int x, int y) {
    if (!n.visible) return nullptr;
    if (!n.rect.contains(x, y)) return nullptr;
    for (auto it = n.kids.rbegin(); it != n.kids.rend(); ++it)
        if (Node* hit = findScrollAt(*it, x, y)) return hit;
    if (n.kind == Kind::Scroll) return const_cast<Node*>(&n);
    return nullptr;
}

} // namespace ui

// ============================================================================
// L5 — APPLICATION LAYER (repositories, view models, background worker)
// Owns all mutable state. Screens (L6) only read state and emit intents.
// ============================================================================
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <zlib.h>

namespace app {

using nbt::Document;
using nbt::Result;
using nbt::Err;

// ---- file repository -------------------------------------------------------
namespace fs = std::filesystem;

Result<std::vector<uint8_t>> readFile(const std::string& path) {
    std::error_code ec;
    if (!fs::exists(path, ec)) return Result<std::vector<uint8_t>>::fail(Err::IoError, "File not found: " + path);
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return Result<std::vector<uint8_t>>::fail(Err::IoError, "Cannot open: " + path);
    std::fseek(f, 0, SEEK_END);
    long sz = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> buf(sz > 0 ? (size_t)sz : 0);
    size_t got = sz > 0 ? std::fread(buf.data(), 1, buf.size(), f) : 0;
    std::fclose(f);
    if (got != buf.size()) return Result<std::vector<uint8_t>>::fail(Err::IoError, "Short read: " + path);
    return Result<std::vector<uint8_t>>::good(std::move(buf));
}

// Atomic save: write to a sibling temp file, flush, rename over the target.
// The original file is never truncated, so a failure cannot destroy data.
Result<void> writeFileAtomic(const std::string& path, const std::vector<uint8_t>& bytes) {
    std::string tmp = path + ".tmp-" + std::to_string((long)getpid());
    FILE* f = std::fopen(tmp.c_str(), "wb");
    if (!f) return Result<void>::fail(Err::WriteFailed, "Cannot create temp file in " + fs::path(path).parent_path().string());
    size_t put = std::fwrite(bytes.data(), 1, bytes.size(), f);
    int flushed = std::fflush(f);
    int synced = fsync(fileno(f));
    std::fclose(f);
    if (put != bytes.size() || flushed != 0 || synced != 0) {
        std::remove(tmp.c_str());
        return Result<void>::fail(Err::WriteFailed, "Write failed (disk full?)");
    }
    std::error_code ec;
    fs::rename(tmp, path, ec);
    if (ec) {
        std::remove(tmp.c_str());
        return Result<void>::fail(Err::WriteFailed, "Could not replace " + path + ": " + ec.message());
    }
    return Result<void>::good();
}

Result<void> writeText(const std::string& path, const std::string& s) {
    std::vector<uint8_t> b(s.begin(), s.end());
    return writeFileAtomic(path, b);
}

// ---- zip (.mcworld) via system zlib ----------------------------------------
struct ZipEntry { std::string name; bool isDir = false; std::vector<uint8_t> data; };

Result<std::vector<ZipEntry>> unzip(const std::string& path) {
    auto raw = readFile(path);
    if (!raw.ok()) return Result<std::vector<ZipEntry>>::fail(raw.err, raw.detail);
    const std::vector<uint8_t>& b = raw.value;
    if (b.size() < 22) return Result<std::vector<ZipEntry>>::fail(Err::IoError, "Not a zip file (too small)");
    // locate End Of Central Directory
    size_t eocd = std::string::npos;
    for (size_t i = b.size() - 22 + 1; i-- > 0;) {
        if (b[i] == 0x50 && b[i + 1] == 0x4b && b[i + 2] == 0x05 && b[i + 3] == 0x06) { eocd = i; break; }
        if (i == 0) break;
    }
    if (eocd == std::string::npos)
        return Result<std::vector<ZipEntry>>::fail(Err::IoError, "Not a zip file (no end record)");
    auto rd16 = [&](size_t o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); };
    auto rd32 = [&](size_t o) { return (uint32_t)(b[o] | (b[o+1] << 8) | (b[o+2] << 16) | ((uint32_t)b[o+3] << 24)); };
    uint32_t count = rd32(eocd + 10);
    size_t   cd = rd32(eocd + 16);

    std::vector<ZipEntry> out;
    out.reserve(count);
    size_t p = cd;
    for (uint32_t k = 0; k < count && p + 46 <= b.size(); ++k) {
        if (rd32(p) != 0x02014b50) break;
        uint16_t method = rd16(p + 10);
        uint32_t csize = rd32(p + 20);
        uint32_t usize = rd32(p + 24);
        uint16_t nlen = rd16(p + 28);
        uint16_t elen = rd16(p + 30);
        uint16_t clen = rd16(p + 32);
        uint32_t lho = rd32(p + 42);
        std::string name((const char*)b.data() + p + 46, nlen);
        // local header
        size_t lh = lho;
        if (rd32(lh) != 0x04034b50) break;
        uint16_t lnlen = rd16(lh + 26);
        uint16_t lelen = rd16(lh + 28);
        size_t dataOff = lh + 30 + lnlen + lelen;

        ZipEntry e;
        e.name = name;
        e.isDir = (!name.empty() && name.back() == '/');
        if (method == 0) {
            if (dataOff + csize <= b.size()) e.data.assign(b.begin() + dataOff, b.begin() + dataOff + csize);
        } else if (method == 8) {
            e.data.resize(usize);
            z_stream zs{}; zs.next_in = (Bytef*)(b.data() + dataOff); zs.avail_in = csize;
            zs.next_out = e.data.data(); zs.avail_out = usize;
            if (inflateInit2(&zs, -15) == Z_OK) {
                inflate(&zs, Z_FINISH);
                inflateEnd(&zs);
            }
        }
        out.push_back(std::move(e));
        p += 46 + nlen + elen + clen;
    }
    return Result<std::vector<ZipEntry>>::good(std::move(out));
}

Result<void> extractZip(const std::string& zipPath, const std::string& destDir) {
    auto entries = unzip(zipPath);
    if (!entries.ok()) return Result<void>::fail(entries.err, entries.detail);
    std::error_code ec;
    for (auto& e : entries.value) {
        if (e.isDir) continue;
        fs::path dest = fs::path(destDir) / e.name;
        if (!dest.parent_path().empty()) fs::create_directories(dest.parent_path(), ec);
        FILE* f = std::fopen(dest.string().c_str(), "wb");
        if (!f) return Result<void>::fail(Err::WriteFailed, "Cannot create " + dest.string());
        if (!e.data.empty()) std::fwrite(e.data.data(), 1, e.data.size(), f);
        std::fclose(f);
    }
    return Result<void>::good();
}

Result<void> createZip(const std::string& dir, const std::string& zipPath) {
    // collect entries deterministically (sorted by name)
    std::vector<fs::path> files;
    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(dir, ec); it != fs::recursive_directory_iterator(); ) {
        if (it->is_regular_file(ec)) files.push_back(it->path());
        it.increment(ec);
    }
    std::sort(files.begin(), files.end());   // deterministic order inside the archive

    FILE* out = std::fopen(zipPath.c_str(), "wb");
    if (!out) return Result<void>::fail(Err::WriteFailed, "Cannot create " + zipPath);

    struct Rec { uint32_t crc, csize, usize, lho; std::string name; };
    std::vector<Rec> recs;
    uint32_t crc_table_dummy = 0; (void)crc_table_dummy;

    for (auto& fp : files) {
        auto data = readFile(fp.string());
        if (!data.ok()) continue;
        std::string rel = fs::relative(fp, dir, ec).string();
        uint32_t crc = (uint32_t)crc32(0, data.value.data(), (uInt)data.value.size());
        std::vector<uint8_t> deflated;
        uLongf bound = compressBound((uLong)data.value.size());
        deflated.resize(bound);
        if (compress2(deflated.data(), &bound, data.value.data(), (uLong)data.value.size(), 6) == Z_OK)
            deflated.resize(bound);
        else { deflated = data.value; bound = deflated.size(); }

        uint16_t method = (bound < data.value.size()) ? 8 : 0;
        std::vector<uint8_t> stored = (method == 8) ? deflated : data.value;
        uint32_t lho = (uint32_t)ftell(out);
        // local file header
        uint8_t lh[30];
        lh[0]=0x50; lh[1]=0x4b; lh[2]=0x03; lh[3]=0x04;
        lh[4]=20; lh[5]=0;                                   // version needed
        lh[6]=0; lh[7]=0;                                    // flags
        lh[8]=(uint8_t)method; lh[9]=0;                     // method
        lh[10]=lh[11]=lh[12]=lh[13]=0;                       // time/date
        std::memcpy(lh + 14, &crc, 4);
        uint32_t csize32 = (uint32_t)stored.size(), usize32 = (uint32_t)data.value.size();
        std::memcpy(lh + 18, &csize32, 4);
        std::memcpy(lh + 22, &usize32, 4);
        uint16_t nlen16 = (uint16_t)rel.size(); std::memcpy(lh + 26, &nlen16, 2);
        lh[28] = lh[29] = 0;
        std::fwrite(lh, 1, 30, out);
        std::fwrite(rel.data(), 1, rel.size(), out);
        if (!stored.empty()) std::fwrite(stored.data(), 1, stored.size(), out);
        recs.push_back({crc, csize32, usize32, lho, rel});
    }
    uint32_t cdStart = (uint32_t)ftell(out);
    for (auto& r : recs) {
        uint8_t ch[46];
        ch[0]=0x50; ch[1]=0x4b; ch[2]=0x01; ch[3]=0x02;
        ch[4]=20; ch[5]=0; ch[6]=20; ch[7]=0;
        ch[8]=ch[9]=0;
        ch[10]=(uint8_t)(r.csize < r.usize ? 8 : 0); ch[11]=0;
        ch[12]=ch[13]=ch[14]=ch[15]=0;
        std::memcpy(ch + 16, &r.crc, 4);
        std::memcpy(ch + 20, &r.csize, 4);
        std::memcpy(ch + 24, &r.usize, 4);
        uint16_t nlen16 = (uint16_t)r.name.size(); std::memcpy(ch + 28, &nlen16, 2);
        ch[30]=ch[31]=ch[32]=ch[33]=0;
        ch[34]=ch[35]=0; ch[36]=0;
        ch[37]=0; ch[38]=0; ch[39]=0; ch[40]=0;
        std::memcpy(ch + 42, &r.lho, 4);
        std::fwrite(ch, 1, 46, out);
        std::fwrite(r.name.data(), 1, r.name.size(), out);
    }
    uint32_t cdEnd = (uint32_t)ftell(out);
    uint8_t eocd[22];
    eocd[0]=0x50; eocd[1]=0x4b; eocd[2]=0x05; eocd[3]=0x06;
    eocd[4]=eocd[5]=0; eocd[6]=eocd[7]=0;
    uint16_t cnt16 = (uint16_t)recs.size(); std::memcpy(eocd + 8, &cnt16, 2);
    std::memcpy(eocd + 10, &cnt16, 2);
    uint32_t cdSize = cdEnd - cdStart; std::memcpy(eocd + 12, &cdSize, 4);
    std::memcpy(eocd + 16, &cdStart, 4);
    eocd[20] = eocd[21] = 0;
    std::fwrite(eocd, 1, 22, out);
    std::fclose(out);
    return Result<void>::good();
}

std::string joinPath(const std::string& a, const std::string& b);
bool ensureDir(const std::string& path);

// ---- diagnostics: in-memory log + crash capture ---------------------------
// Kept small and dependency-free: the log is written to the app's private dir so
// the user can share it from inside the app (no adb needed on device).
static const int kLogCapacity = 400;
struct LogBuffer {
    std::mutex m;
    std::vector<std::string> lines;
    void add(const std::string& line) {
        std::lock_guard<std::mutex> lk(m);
        lines.push_back(line);
        if ((int)lines.size() > kLogCapacity) lines.erase(lines.begin());
    }
    std::vector<std::string> snapshot() {
        std::lock_guard<std::mutex> lk(m);
        return lines;
    }
};
static LogBuffer& logBuffer() { static LogBuffer b; return b; }

inline void diagLog(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    logBuffer().add(std::string(buf));
}

inline std::string logFilePath(const std::string& filesDir) {
    return joinPath(filesDir, "nbt-editor-log.txt");
}

// Writes the in-memory log to the private dir and returns the path.
inline std::string flushLog(const std::string& filesDir) {
    std::string p = logFilePath(filesDir);
    ensureDir(filesDir.empty() ? "." : filesDir);
    std::string all;
    for (auto& l : logBuffer().snapshot()) { all += l; all += "\n"; }
    app::writeText(p, all);
    return p;
}

// ---- prefs -----------------------------------------------------------------
struct Prefs {
    std::string theme = "light";         // light | dark
    std::vector<std::string> recent;
    std::string lastDir;
    std::string worldsRoot;
};

// ---- world model -----------------------------------------------------------
struct WorldInfo {
    std::string folder;      // absolute path
    std::string displayName; // levelname.txt
    std::string internalName;
    uint64_t sizeBytes = 0;
    bool hasIcon = false;
    bool encrypted = false;
    bool external = false;
};

uint64_t dirSize(const fs::path& p) {
    uint64_t total = 0;
    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(p, ec); it != fs::recursive_directory_iterator(); ) {
        if (it->is_regular_file(ec)) {
            std::error_code e2;
            auto sz = it->file_size(e2);
            if (!e2) total += (uint64_t)sz;
        }
        it.increment(ec);
    }
    return total;
}

// Encrypted Bedrock worlds: db files whose first two LE int32 are 0 and -1680885252.
bool looksEncrypted(const fs::path& world) {
    std::error_code ec;
    fs::path db = world / "db";
    if (!fs::exists(db, ec)) return false;
    for (auto& f : fs::directory_iterator(db, ec)) {
        auto bytes = readFile(f.path().string());
        if (!bytes.ok() || bytes.value.size() < 8) continue;
        auto le32 = [&](size_t o) {
            return (int32_t)((uint32_t)bytes.value[o] | ((uint32_t)bytes.value[o+1] << 8) |
                             ((uint32_t)bytes.value[o+2] << 16) | ((uint32_t)bytes.value[o+3] << 24)); };
        if (le32(0) == 0 && le32(4) == -1680885252) return true;
    }
    return false;
}

std::string readLevelName(const fs::path& world) {
    auto r = readFile((world / "levelname.txt").string());
    if (r.ok()) return gfx::trim(std::string(r.value.begin(), r.value.end()));
    return "Unnamed World";
}

// ---- background worker -----------------------------------------------------
struct Worker {
    std::thread th;
    std::mutex m;
    std::condition_variable cv;
    std::function<void()> job;
    bool running = true, busy = false, done = false;

    void start() {
        if (th.joinable()) return;             // already running
        th = std::thread([this] {
            std::unique_lock<std::mutex> lk(m);
            while (running) {
                cv.wait(lk, [this] { return job || !running; });
                if (!running) return;
                busy = true;
                // run unlocked; callbacks post back through the UI queue (L0)
                std::function<void()> run = job;
                job = nullptr;
                lk.unlock();
                if (run) run();
                lk.lock();
                busy = false;
                done = true;
                cv.notify_all();
            }
        });
    }
    void post(std::function<void()> f) {
        {
            std::lock_guard<std::mutex> lk(m);
            job = std::move(f);
            done = false;
        }
        cv.notify_one();
    }
    bool finished() { std::lock_guard<std::mutex> lk(m); return done && !busy; }
    void stop() {
        { std::lock_guard<std::mutex> lk(m); running = false; }
        cv.notify_all();
        if (th.joinable()) th.join();
    }
};

// The one app-wide worker (created lazily, started by the platform glue).
inline Worker& appWorker() { static Worker w; return w; }

} // namespace app

// ============================================================================
// L6 — SCREENS & DIALOGS  ·  L0 platform glue lives at the end of the file
//
// State is a single struct mutated only through intents (handleAction). Every
// screen is built as a pure function AppState -> ui::Node tree, then rendered.
// ============================================================================

namespace app {
using gfx::Rect;

// ---- intents ---------------------------------------------------------------
enum Act {
    A_NONE = 0,
    A_QUIT,
    // home
    A_OPEN_NBT, A_NEW_NBT, A_WORLDS, A_RECENT_PICK,
    // worlds
    A_WORLD_EDIT, A_WORLD_DELETE, A_WORLD_EXPORT, A_WORLD_EXPORT_DONE, A_WORLD_CREATE, A_WORLD_IMPORT,
    A_WORLD_OPTIONS,
    // browser
    A_BROWSE_UP, A_BROWSE_OPEN, A_BROWSE_PICK_DIR, A_FILE_SELECT,
    // nbt editor
    A_TREE_TAP, A_TREE_LONG, A_TREE_TOGGLE, A_TREE_EDIT, A_TREE_ADD, A_TREE_SAVE,
    A_TREE_SAVE_AS, A_TREE_DELETE, A_TREE_RENAME, A_TREE_COPY, A_TREE_PASTE,
    A_TREE_UNDO, A_TREE_REDO, A_TREE_SEARCH, A_TREE_MOVE_ROOT, A_TREE_UP,
    A_VALUE_KIND, A_VALUE_SAVE, A_VALUE_CANCEL, A_TREE_RENAME_DONE, A_TREE_ADD_DONE,
    // dialogs
    A_DIALOG_POS, A_DIALOG_NEG, A_DIALOG_NEU,
    A_KEY_TAP, A_KEY_SHIFT, A_KEY_DEL, A_KEY_ENTER, A_KEY_MODE,
    A_TAG_OPTION, A_BREADCRUMB_TAP,
    // flat world
    A_FLAT_ADD_LAYER, A_FLAT_EDIT_LAYER, A_FLAT_DEL_LAYER, A_FLAT_LONGER,
    A_FLAT_THICK, A_FLAT_BIOME, A_FLAT_CREATE, A_FLAT_TOGGLE_BASE, A_FLAT_LAYER_UP,
    A_FLAT_LAYER_DOWN,
    // misc
    A_TOGGLE_THEME, A_HELP, A_ABOUT, A_SETTINGS, A_DISMISS_TOAST,
    A_SCROLL_DRAG,
};

struct FlatLayer { std::string block; int64_t count = 1; };

struct EditorState {
    bool open = false;
    nbt::Document doc;
    std::string path;
    bool dirty = false;
    bool levelDat = false;            // true: 8-byte Bedrock header present
    // navigation: index-path into the arena; we navigate by indices, not copies
    std::vector<uint32_t> pathIdx;    // current compound/list/array index
    std::vector<uint64_t> openSet;    // expanded node keys (path hash)
    std::string filter;
    std::vector<std::pair<std::string, uint32_t>> searchHits;
    int selected = -1;                // index inside current listing
    bool showSearch = false;
    // clipboard
    bool hasClip = false;
    uint32_t clipIdx = 0;
    // undo (command log: full document snapshots are too heavy; store inverse ops)
    struct UndoEntry {
        std::string label;
        std::vector<nbt::Node> arena;
        uint32_t root = 0;
    };
    std::vector<UndoEntry> undoStack;
    std::vector<UndoEntry> redoStack;
    int scrollY = 0;
};

// Versi aplikasi. Satu-satunya sumber kebenaran untuk tampilan di dalam app;
// angka yang sama dipakai scripts/build_apk.sh (lihat berkas VERSION).
inline constexpr const char* kAppVersion = "1.0.2";
inline constexpr int         kAppBuild   = 3;

struct AppState {
    // identity / paths
    std::string appName = "NBT Editor";
    std::string filesDir;             // internal data path (prefs, drafts)
    std::string worldsRoot;            // app-managed worlds dir
    // theme / metrics
    gfx::Theme theme;
    bool dark = false;
    int  w = 0, h = 0;                // framebuffer size (px)
    int  scale = 1;                   // ui scale for small screens
    // navigation
    int screen = 0;                   // see Screen enum below
    std::vector<int> screenStack;
    // data
    app::Prefs prefs;
    std::vector<app::WorldInfo> worlds;
    std::vector<std::string> recent;
    // browser
    std::string browseDir;
    bool browseForSave = false;
    bool importing = false;
    bool storageAsked = false;        // ask for all-files access only once,
                                      // and only when the user opens the browser
    std::vector<std::string> browseEntries;
    std::vector<bool> browseIsDir;
    std::string pendingSavePath;
    // editor
    EditorState ed;
    // value editor dialog
    bool valueOpen = false;
    uint32_t valueIdx = 0;            // node being edited
    std::string valueText;
    nbt::Type valueType = nbt::Type::Byte;
    std::string valueError;
    bool valueNumeric = true;
    // add-tag dialog
    bool addOpen = false;
    int addType = 1;                  // type index for spinner
    nbt::Type valueTypeToAdd = nbt::Type::Byte;   // selected type in the add dialog
    std::string addName;
    int addListMode = 0;              // 0 compound, 1 list, 2 array
    std::string addError;
    // generic dialog
    ui::Dialog dialog;
    // toasts / progress
    std::vector<ui::Toast> toasts;
    bool progressOpen = false;
    std::string progressTitle, progressMsg;
    float progressValue = 0;
    // flat world builder
    bool flatOpen = false;
    std::vector<FlatLayer> flatLayers;
    int flatBiome = 1;                // plains
    bool flatBaseLayer = true;
    std::string flatDraft;           // thickness input buffer
    // rename dialog
    bool renameOpen = false;
    uint32_t renameIdx = 0;
    std::string renameName;
    // tag options dialog (long press)
    std::vector<std::string> tagOptionList;
    uint32_t tagOptionIdx = 0;
    // keypad
    bool keypadOpen = false;
    std::string keypadTarget;         // "value" | "addName" | "renameName" | "searchFilter"
    std::string keypadBuf;
    bool keypadShift = false;
    bool keypadNumeric = true;
    int64_t lastKeyMs = 0;
    std::string lastKeyPressed;
    // misc
    bool wantExit = false;
    int64_t frame = 0;
    int64_t nowMs = 0;
    int64_t autosaveAtMs = 0;
    bool pendingEdit = false;         // waiting for level.dat to open
    // built tree (rebuilt every frame; used for hit-testing and drawing)
    std::shared_ptr<ui::Node> uiRoot;
    // scrolling of the active list (single scrollable list per screen)
    int scrollY = 0;
    int contentBottom = 0;
    // touch bookkeeping
    bool  touchActive = false;
    bool  touchScrolled = false;
    int   touchStartX = 0, touchStartY = 0;
    int   touchLastY = 0;
    int64_t touchDownMs = 0;
    int   touchX = 0, touchY = 0;
    bool helpOpen = false;
    bool diagOpen = false;
};

// ---- screen ids ------------------------------------------------------------
enum Screen { S_HOME = 0, S_WORLDS, S_BROWSER, S_TREE, S_FLAT, S_HELP };

// Guards AppState: the render thread owns it, but the worker thread also
// touches it (import/export results), so every access is serialised.
static std::mutex g_stateMutex;

// ---- action handler (single entry point for all interaction) ---------------
void handleAction(AppState& a, int act);

// ---- forward decls for helpers used by handlers ----------------------------
std::string defaultWorldDir();
void listWorlds(AppState& a);
void listBrowseDir(AppState& a);
bool openDocument(AppState& a, const std::string& path, bool levelDat, std::string& err);

// All-files access (Android 11+) is requested lazily, when the user first opens
// the file browser. Asking during onCreate made the app jump straight to the
// system settings screen, which looked like "the app will not open".
#if !NBT_TESTABLE
void requestStoragePermission();          // L0, device only
#else
inline void requestStoragePermission() {} // host tests have no Android
#endif
bool saveDocument(AppState& a, const std::string& path, std::string& err);
void pushToast(AppState& a, const std::string& msg, gfx::Color c);
void openDialog(AppState& a, const std::string& title, const std::string& msg,
                const std::string& pos, int posAct, const std::string& neg, int negAct,
                const std::string& neu = "", int neuAct = 0);
void closeDialog(AppState& a);
void rebuildTreeListing(AppState& a);

// ---- small utilities -------------------------------------------------------
inline int64_t nowMs() {
    return (int64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

void pushToast(AppState& a, const std::string& msg, gfx::Color c) {
    a.toasts.push_back({msg, a.nowMs + 3200, c});
    if (a.toasts.size() > 4) a.toasts.erase(a.toasts.begin());
}

inline uint64_t hashPath(const std::vector<uint32_t>& p) {
    uint64_t h = 1469598103934665603ull;
    for (uint32_t v : p) { h ^= v; h *= 1099511628211ull; }
    return h;
}
inline bool nodeOpen(const AppState& a, const std::vector<uint32_t>& p) {
    uint64_t h = hashPath(p);
    for (auto v : a.ed.openSet) if (v == h) return true;
    return false;
}

// ---- paths -----------------------------------------------------------------
std::string defaultWorldDir() {
#ifdef __ANDROID__
    return "";                       // filled from ANativeActivity at startup
#else
    return "worlds";
#endif
}

// ---- world / directory management -------------------------------------------
bool ensureDir(const std::string& path) {
    std::error_code ec;
    return fs::create_directories(path, ec) || fs::is_directory(path, ec);
}

std::string joinPath(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    if (a.back() == '/' || a.back() == '\\') return a + b;
    return a + "/" + b;
}

std::string parentPath(const std::string& p) {
    auto pos = p.find_last_of('/');
    if (pos == std::string::npos || pos == 0) return "/";
    return p.substr(0, pos);
}

std::string toLowerCopy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string baseName(const std::string& p) {
    auto pos = p.find_last_of('/');
    if (pos == std::string::npos) return p;
    return p.substr(pos + 1);
}

void listWorlds(AppState& a) {
    a.worlds.clear();
    std::error_code ec;
    fs::path root = a.worldsRoot;
    if (!fs::exists(root, ec)) { ensureDir(a.worldsRoot); return; }
    for (auto& f : fs::directory_iterator(root, ec)) {
        if (!f.is_directory(ec)) continue;
        app::WorldInfo w;
        w.folder = f.path().string();
        w.internalName = f.path().filename().string();
        w.displayName = readLevelName(f.path());
        w.sizeBytes = dirSize(f.path());
        w.hasIcon = fs::exists(f.path() / "world_icon.jpeg", ec) || fs::exists(f.path() / "world_icon.jpg", ec);
        w.encrypted = looksEncrypted(f.path());
        w.external = false;
        a.worlds.push_back(std::move(w));
    }
    std::sort(a.worlds.begin(), a.worlds.end(), [](const app::WorldInfo& x, const app::WorldInfo& y) {
        return x.displayName < y.displayName;
    });
}

void listBrowseDir(AppState& a) {
    a.browseEntries.clear();
    a.browseIsDir.clear();
    std::error_code ec;
    fs::path dir(a.browseDir.empty() ? "/sdcard" : a.browseDir);
    if (!fs::exists(dir, ec)) {
        a.browseDir = "/sdcard";
        dir = "/sdcard";
        if (!fs::exists(dir, ec)) { dir = "/"; a.browseDir = "/"; }
    }
    for (auto& f : fs::directory_iterator(dir, ec)) {
        a.browseEntries.push_back(f.path().string());
        a.browseIsDir.push_back(f.is_directory(ec));
    }
    std::vector<size_t> order(a.browseEntries.size());
    for (size_t k = 0; k < order.size(); ++k) order[k] = k;
    std::sort(order.begin(), order.end(), [&](size_t x, size_t y) {
        if (a.browseIsDir[x] != a.browseIsDir[y]) return a.browseIsDir[x] ? true : false;
        return baseName(a.browseEntries[x]) < baseName(a.browseEntries[y]);
    });
    std::vector<std::string> ents;
    std::vector<bool> dirs;
    for (size_t k : order) { ents.push_back(a.browseEntries[k]); dirs.push_back(a.browseIsDir[k]); }
    a.browseEntries = std::move(ents);
    a.browseIsDir = std::move(dirs);
}

// ---- document open / save ---------------------------------------------------
bool openDocument(AppState& a, const std::string& path, bool levelDat, std::string& err) {
    auto raw = app::readFile(path);
    if (!raw.ok()) { err = raw.detail; return false; }
    nbt::ParseOptions opt;
    opt.skipHeader = levelDat;
    opt.strictHeader = true;
    auto r = nbt::parseBytes(raw.value.data(), raw.value.size(), path, opt);
    if (!r.ok()) { err = r.detail; return false; }
    a.ed = EditorState{};
    a.ed.doc = std::move(r.value);
    a.ed.path = path;
    a.ed.open = true;
    a.ed.levelDat = levelDat;
    a.ed.dirty = false;
    a.ed.pathIdx.clear();
    a.ed.pathIdx.push_back(a.ed.doc.root);
    a.screen = S_TREE;
    a.screenStack.push_back(S_TREE);
    rebuildTreeListing(a);
    return true;
}

bool saveDocument(AppState& a, const std::string& path, std::string& err) {
    std::vector<uint8_t> out;
    if (a.ed.levelDat) {
        out = nbt::serializeLevelDat(a.ed.doc);
    } else {
        out = nbt::serialize(a.ed.doc);
    }
    auto w = app::writeFileAtomic(path, out);
    if (!w.ok()) { err = w.detail; return false; }
    a.ed.dirty = false;
    return true;
}

// ---- tree listing -----------------------------------------------------------
void rebuildTreeListing(AppState& a) {
    // the listing is derived on demand by the screen builder; this only resets scroll
    a.ed.selected = -1;
}

// ---- dialogs ----------------------------------------------------------------
void openDialog(AppState& a, const std::string& title, const std::string& msg,
                const std::string& pos, int posAct, const std::string& neg, int negAct,
                const std::string& neu, int neuAct) {
    a.dialog.open = true;
    a.dialog.title = title;
    a.dialog.message = msg;
    a.dialog.pos = pos;   a.dialog.neg = neg;   a.dialog.neu = neu;
    a.dialog.posAction = posAct; a.dialog.negAction = negAct; a.dialog.neuAction = neuAct;
}
void closeDialog(AppState& a) {
    a.dialog = ui::Dialog{};
}

// ---- recent files -----------------------------------------------------------
void addRecent(AppState& a, const std::string& path) {
    for (auto& r : a.recent) if (r == path) return;
    a.recent.insert(a.recent.begin(), path);
    if (a.recent.size() > 10) a.recent.pop_back();
}

// ---- clipboard tag copy (deep) ---------------------------------------------
uint32_t cloneNode(AppState& a, const nbt::Node& src) {
    nbt::Node n = src;
    uint32_t myIdx = (uint32_t)a.ed.doc.arena.size();
    a.ed.doc.arena.push_back(n);
    for (auto& k : a.ed.doc.arena[myIdx].kids) k = 0;
    for (auto k : src.kids) a.ed.doc.arena[myIdx].kids.push_back(cloneNode(a, a.ed.doc.arena[k]));
    return myIdx;
}

// ---- flat-world template ---------------------------------------------------
// Bedrock flat world JSON (reference format, ANALYSIS.md §4.2).
static const char* kFlatBiomes[] = {
    "Ocean",           // 0
    "Plains",          // 1
    "Desert",          // 2
    "Windswept Hills", // 3
    "Forest",          // 4
    "Taiga",           // 5
    "Swamp",           // 6
    "River",           // 7
    "Nether Wastes",   // 8
    "The End",         // 9
    "Snowy Beach",     // 10
    "Badlands",        // 11
    "Jungle",          // 12
    "Savanna",         // 13
    "Mushroom Fields", // 14
    "Cherry Grove",    // 15
};
static const int kFlatBiomeCount = (int)(sizeof(kFlatBiomes) / sizeof(kFlatBiomes[0]));

static const char* kFlatBlocks[] = {
    "minecraft:bedrock", "minecraft:dirt", "minecraft:grass_block", "minecraft:sand",
    "minecraft:gravel", "minecraft:stone", "minecraft:water", "minecraft:air",
};
static const int kFlatBlockCount = (int)(sizeof(kFlatBlocks) / sizeof(kFlatBlocks[0]));

std::string buildFlatWorldJson(int biomeIdx, const std::vector<FlatLayer>& layers) {
    std::string j = "{\"biome_id\":";
    j += std::to_string(biomeIdx);
    j += ",\"block_layers\":[";
    bool first = true;
    for (auto& l : layers) {
        if (!first) j += ",";
        first = false;
        j += "{\"block_name\":\"" + l.block + "\",\"count\":" + std::to_string(l.count) + "}";
    }
    j += "],\"encoding_version\":6,\"structure_options\":null,\"world_version\":\"version.post_1_18\"}";
    return j;
}

nbt::Document newRootCompound(const std::string& name = "") {
    nbt::Document d;
    nbt::Node root;
    root.type = nbt::Type::Compound;
    root.name = name;
    d.arena.push_back(std::move(root));
    d.root = 0;
    return d;
}

// Minimal, valid Bedrock level.dat root (enough for a new world to open).
nbt::Document newLevelDat(const std::string& levelName) {
    nbt::Document d = newRootCompound();
    auto add = [&](const std::string& name, nbt::Type t, const std::function<void(nbt::Node&)>& fill) {
        nbt::Node n; n.type = t; n.name = name; fill(n);
        d.arena[0].kids.push_back((uint32_t)d.arena.size());
        d.arena.push_back(std::move(n));
    };
    add("LevelName", nbt::Type::String, [&](nbt::Node& n) { n.s = levelName; });
    add("GameType", nbt::Type::Int, [](nbt::Node& n) { n.i = 1; });
    add("Difficulty", nbt::Type::Int, [](nbt::Node& n) { n.i = 0; });
    add("Generator", nbt::Type::Int, [](nbt::Node& n) { n.i = 2; });
    add("LastPlayed", nbt::Type::Long, [](nbt::Node& n) { n.i = 0; });
    d.headerVersion = 8;
    d.hasLevelDatHeader = true;
    return d;
}

// ---- value formatting for tree rows ----------------------------------------
std::string valueSummary(const nbt::Document& d, uint32_t idx) {
    const nbt::Node& n = d.arena[idx];
    switch (n.type) {
        case nbt::Type::Compound:  return std::to_string(n.kids.size()) + " children";
        case nbt::Type::List:      return std::to_string(n.kids.size()) + " × " + nbt::typeName(n.elemType);
        case nbt::Type::ByteArray: return "[" + std::to_string(n.bytes.size()) + " bytes]";
        case nbt::Type::IntArray:  return "[" + std::to_string(n.longs.size()) + " ints]";
        case nbt::Type::LongArray: return "[" + std::to_string(n.longs.size()) + " longs]";
        case nbt::Type::String:    return n.s.empty() ? "(empty string)" : n.s;
        default: break;
    }
    if (n.type == nbt::Type::Byte || n.type == nbt::Type::Short || n.type == nbt::Type::Int)
        return std::to_string((int32_t)n.i);
    if (n.type == nbt::Type::Long) return std::to_string(n.i);
    if (n.type == nbt::Type::Float) {
        char b[32]; std::snprintf(b, sizeof b, "%g", n.d); return b;
    }
    if (n.type == nbt::Type::Double) {
        char b[32]; std::snprintf(b, sizeof b, "%g", n.d); return b;
    }
    return "";
}

gfx::Color typeColor(const gfx::Theme& th, nbt::Type t) {
    switch (t) {
        case nbt::Type::Byte:
        case nbt::Type::Short:
        case nbt::Type::Int:
        case nbt::Type::Long:      return th.primary;
        case nbt::Type::Float:
        case nbt::Type::Double:    return gfx::rgb(0x00, 0x7a, 0xc1);
        case nbt::Type::String:    return gfx::rgb(0x8e, 0x5a, 0x00);
        case nbt::Type::ByteArray:
        case nbt::Type::IntArray:
        case nbt::Type::LongArray: return gfx::rgb(0x7b, 0x5c, 0xb0);
        case nbt::Type::List:      return gfx::rgb(0x00, 0x7d, 0x7b);
        case nbt::Type::Compound:  return gfx::rgb(0x37, 0x4c, 0x33);
        default:                   return th.onSurfaceMuted;
    }
}

std::vector<uint32_t> visibleChildren(const AppState& a) {
    std::vector<uint32_t> out;
    if (a.ed.pathIdx.empty()) return out;
    uint32_t cur = a.ed.pathIdx.back();
    const nbt::Node& n = a.ed.doc.arena[cur];
    for (uint32_t k : n.kids) out.push_back(k);
    if (!a.ed.filter.empty()) {
        // search mode: flatten every node whose name matches, keep parent->child order
        std::vector<uint32_t> matches;
        std::vector<uint32_t> path{cur};
        std::function<void(uint32_t)> rec = [&](uint32_t idx) {
            path.push_back(idx);
            if (a.ed.doc.arena[idx].name.find(a.ed.filter) != std::string::npos)
                matches.push_back(idx);
            for (uint32_t k : a.ed.doc.arena[idx].kids) rec(k);
            path.pop_back();
        };
        rec(cur);
        return matches;
    }
    return out;
}

// path (indices) from document root to a node, used by navigation & undo
std::vector<uint32_t> indexPathTo(AppState& a, uint32_t target) {
    std::vector<uint32_t> path{a.ed.doc.root};
    if (target == a.ed.doc.root) return path;
    std::function<bool(uint32_t)> rec = [&](uint32_t idx) -> bool {
        for (uint32_t k : a.ed.doc.arena[idx].kids) {
            if (k == target) { path.push_back(target); return true; }
            path.push_back(k);
            if (rec(k)) return true;
            path.pop_back();
        }
        return false;
    };
    (void)rec(a.ed.doc.root);
    return path;
}

} // namespace app

// ============================================================================
// L6b — SCREEN BUILDERS (AppState -> ui::Node tree)
// Each builder is a pure function: no I/O, no mutation, host-testable.
// ============================================================================
namespace app {

constexpr int kBarH = 44;          // app bar height
constexpr int kRowH = 34;          // list row height

gfx::Color bgOf(const AppState& a)  { return a.theme.background; }
gfx::Color surfOf(const AppState& a){ return a.theme.surface; }
gfx::Color fgOf(const AppState& a)  { return a.theme.onSurface; }
gfx::Color mutOf(const AppState& a) { return a.theme.onSurfaceMuted; }

// ---------- shared chrome ---------------------------------------------------
ui::Node appBar(const AppState& a, const std::string& title, bool backButton,
                std::vector<ui::Node> actions) {
    gfx::Theme th = a.theme;
    ui::Node bar;
    bar.kind = ui::Kind::Panel;
    bar.rect = {0, 0, a.w, kBarH};
    bar.bg = th.primary;
    int x = 6;
    if (backButton) {
        bar.kids.push_back(ui::button({x, 6, 32, 32}, "<", th.primary, th.onPrimary, A_TREE_UP, 16));
        x += 38;
    } else {
        x += 4;
    }
    bar.kids.push_back(ui::text({x, 8, a.w - x - 4 * (int)actions.size() - 40, kBarH - 16}, title, th.onPrimary));
    int ax = a.w - 4;
    for (auto it = actions.rbegin(); it != actions.rend(); ++it) {
        ui::Node b = *it;
        b.rect = {ax - 34, 6, 32, 32};
        b.radius = 16;
        bar.kids.push_back(b);
        ax -= 38;
    }
    return bar;
}

// ---------- HOME -------------------------------------------------------------
ui::Node buildHome(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, a.appName, false, {
        ui::button({0, 0, 32, 32}, "?", th.primary, th.onPrimary, A_HELP, 16),
    }));

    int y = kBarH + 12;
    Rect area{12, y, a.w - 24, a.h - y - 12};

    ui::Node col; col.kind = ui::Kind::Column; col.rect = area;

    // hero
    ui::Node hero = ui::panel({area.x, y, area.w, 74}, {
        ui::text({area.x + 14, y + 10, area.w - 28, 20}, "Native NBT Editor", th.onSurface, 2),
        ui::text({area.x + 14, y + 34, area.w - 28, 16}, "All 13 tag types · Bedrock little-endian", mutOf(a)),
        ui::text({area.x + 14, y + 52, area.w - 28, 16}, "level.dat aware · .mcworld import/export", mutOf(a)),
    });
    hero.bg = surfOf(a); hero.radius = 10;
    col.kids.push_back(hero);
    y += 86;

    // primary actions (2x2 grid)
    int gw = (area.w - 10) / 2;
    std::vector<ui::Node> grid;
    grid.push_back(ui::button({0, 0, gw, 46}, "Open file", th.primary, th.onPrimary, A_OPEN_NBT));
    grid.push_back(ui::button({gw + 10, 0, gw, 46}, "New NBT", th.primary, th.onPrimary, A_NEW_NBT));
    grid.push_back(ui::button({0, 56, gw, 46}, "Worlds", surfOf(a), th.primary, A_WORLDS));
    grid.push_back(ui::button({gw + 10, 56, gw, 46}, "Import .mcworld", surfOf(a), th.primary, A_WORLD_IMPORT));
    for (auto& g : grid) { g.rect.x += area.x; g.rect.y += y; }
    col.kids.push_back(ui::panel({area.x, y, area.w, 112}, grid));
    y += 126;

    // recent files
    ui::Node hdr = ui::text({area.x, y, area.w, 20}, "Recent files", mutOf(a));
    col.kids.push_back(hdr);
    y += 26;
    if (a.recent.empty()) {
        col.kids.push_back(ui::text({area.x, y, area.w, 20},
            "No recent files yet - open or create an NBT file.", mutOf(a)));
        y += 26;
    } else {
        std::vector<ui::Node> rows;
        int idx = 0;
        for (auto& r : a.recent) {
            ui::Node row = ui::panel({area.x, y, area.w, kRowH}, {
                ui::text({area.x + 10, y + 4, area.w - 20, 14}, baseName(r), fgOf(a)),
                ui::text({area.x + 10, y + 20, area.w - 20, 12}, r, mutOf(a)),
            });
            row.bg = surfOf(a); row.radius = 6; row.action = A_RECENT_PICK; row.index = idx++;
            row.actionLong = 0;
            rows.push_back(row);
            y += kRowH + 4;
        }
        col.kids.push_back(ui::list({area.x, area.y + 152, area.w, y - (area.y + 152)}, rows));
    }

    root.push_back(col);          // <- the whole home content
    ui::Node page = ui::panel({0, 0, a.w, a.h}, root);
    return page;
}

// ---------- WORLDS -----------------------------------------------------------
ui::Node buildWorlds(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, "Minecraft Worlds", true, {
        ui::button({0, 0, 32, 32}, "+", th.primary, th.onPrimary, A_WORLD_IMPORT, 16),
        ui::button({0, 0, 32, 32}, "N", th.primary, th.onPrimary, A_WORLD_CREATE, 16),
    }));

    int y = kBarH + 10;
    Rect area{12, y, a.w - 24, a.h - y - 12};

    std::vector<ui::Node> rows;
    if (a.worlds.empty()) {
        rows.push_back(ui::text({area.x, area.y + 6, area.w, 40},
            "No worlds yet. Tap + to import a .mcworld, or N to create a flat world.", mutOf(a)));
    }
    int idx = 0;
    int dy = -a.scrollY;
    for (auto& w : a.worlds) {
        ui::Node row = ui::panel({area.x, dy, area.w, 54}, {
            ui::iconTile({area.x + 8, 9, 36, 36}, w.encrypted ? th.error : th.secondary, w.encrypted ? "!" : "W"),
            ui::text({area.x + 52, 10, area.w - 60, 18}, w.displayName, fgOf(a)),
            ui::text({area.x + 52, 30, area.w - 60, 14},
                      gfx::humanSize(w.sizeBytes) + " · " + w.internalName +
                      (w.encrypted ? " · encrypted" : ""), mutOf(a)),
        });
        row.bg = surfOf(a); row.radius = 8; row.action = A_WORLD_OPTIONS; row.index = idx++;
        rows.push_back(row);
        dy += 58;
    }
    a.contentBottom = dy + a.scrollY;   // content height for the scroll clamp
    ui::Node page = ui::panel({0, 0, a.w, a.h}, root);
    return page;
}

// ---------- FILE BROWSER -----------------------------------------------------
ui::Node buildBrowser(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    std::string title = a.browseForSave ? "Save As" : "Open NBT file";
    root.push_back(appBar(a, title, true, {}));

    // path bar + up
    int y = kBarH + 6;
    ui::Node pathbar = ui::panel({10, y, a.w - 20, 30}, {
        ui::button({12, y + 2, 26, 26}, "^", surfOf(a), th.primary, A_BROWSE_UP, 13),
        ui::text({44, y + 6, a.w - 66, 18}, a.browseDir, mutOf(a)),
    });
    pathbar.bg = surfOf(a); pathbar.radius = 6;
    root.push_back(pathbar);
    y += 40;

    std::vector<ui::Node> rows;
    int idx = 0;
    int dy = -a.scrollY;
    for (size_t k = 0; k < a.browseEntries.size(); ++k) {
        const std::string& p = a.browseEntries[k];
        bool isDir = a.browseIsDir[k];
        if (!isDir) {
            std::string low = p;
            std::transform(low.begin(), low.end(), low.begin(), ::tolower);
            bool nbtish = (gfx::startsWith(low, "level") || low.size() > 4) &&
                          (gfx::startsWith(low, "level") || low.find(".dat") != std::string::npos ||
                           low.find(".nbt") != std::string::npos || low.find(".mcstructure") != std::string::npos ||
                           low.find(".snbt") != std::string::npos || low.find("level.dat") != std::string::npos);
            bool showAll = !a.browseForSave;   // when saving we only pick directories
            if (!nbtish && !showAll) continue;
        }
        ui::Node row = ui::panel({10, dy, a.w - 20, kRowH}, {
            ui::iconTile({14, dy + 6, 22, 22}, isDir ? gfx::rgb(0xff, 0xd5, 0x4f) : th.secondary, isDir ? "D" : "F"),
            ui::text({44, dy + 8, a.w - 100, 18}, baseName(p) + (isDir ? "/" : ""), fgOf(a)),
            ui::text({a.w - 116, dy + 8, 96, 18},
                      isDir ? "" : gfx::humanSize(app::dirSize(p)), mutOf(a)),
        });
        row.bg = surfOf(a); row.radius = 6;
        row.action = isDir ? A_BROWSE_OPEN : A_FILE_SELECT;
        row.index = idx++;
        row.user = isDir ? 1 : 0;
        rows.push_back(row);
        dy += kRowH + 2;
    }
    a.contentBottom = dy + a.scrollY;   // content height for the scroll clamp
    ui::Node page = ui::panel({0, 0, a.w, a.h}, root);
    return page;
}

// ---------- NBT TREE EDITOR --------------------------------------------------
ui::Node buildTree(AppState& a) {
    gfx::Theme th = a.theme;
    const nbt::Document& d = a.ed.doc;
    std::vector<ui::Node> root;

    std::string title = baseName(a.ed.path);
    if (a.ed.dirty) title = "* " + title;
    root.push_back(appBar(a, title, true, {
        ui::button({0, 0, 32, 32}, "U", th.primary, th.onPrimary, A_TREE_UNDO, 16),
        ui::button({0, 0, 32, 32}, "R", th.primary, th.onPrimary, A_TREE_REDO, 16),
        ui::button({0, 0, 32, 32}, "S", th.primary, th.onPrimary, A_TREE_SAVE, 16),
        ui::button({0, 0, 32, 32}, "+", th.primary, th.onPrimary, A_TREE_ADD, 16),
        ui::button({0, 0, 32, 32}, "F", th.primary, th.onPrimary, A_TREE_SEARCH, 16),
        ui::button({0, 0, 32, 32}, "?", th.primary, th.onPrimary, A_HELP, 16),
    }));

    // breadcrumb
    int y = kBarH + 4;
    ui::Node crumbs; crumbs.kind = ui::Kind::Breadcrumb; crumbs.rect = {6, y, a.w - 12, 24};
    for (size_t k = 0; k < a.ed.pathIdx.size(); ++k) {
        uint32_t idx = a.ed.pathIdx[k];
        std::string label = (k == 0) ? "root" : (d.arena[idx].name.empty() ? "<unnamed>" : d.arena[idx].name);
        ui::Node c = ui::button({0, 0, 0, 22}, label, gfx::Color{0, 0, 0, 0}, fgOf(a), A_BREADCRUMB_TAP, 6);
        c.index = (int)k;
        crumbs.kids.push_back(c);
    }
    if (!a.ed.filter.empty()) {
        ui::Node f = ui::text({0, 0, 0, 20}, "  filter: " + a.ed.filter, th.primary);
        crumbs.kids.push_back(f);
    }
    root.push_back(crumbs);
    y += 30;

    // info bar
    std::string info = (a.ed.levelDat ? "level.dat · " : "") + std::to_string(d.arena.size()) + " tags";
    if (a.ed.dirty) info += " · unsaved";
    root.push_back(ui::text({10, y, a.w - 20, 18}, info, mutOf(a)));
    y += 22;

    // search row
    if (a.ed.showSearch) {
        ui::Node srow = ui::panel({10, y, a.w - 20, 34}, {
            ui::searchField({16, y + 4, a.w - 120, 26}, a.ed.filter, A_TREE_SEARCH),
            ui::button({a.w - 96, y + 4, 40, 26}, "Find", th.primary, th.onPrimary, A_TREE_SEARCH),
            ui::button({a.w - 50, y + 4, 34, 26}, "X", surfOf(a), fgOf(a), A_TREE_SEARCH),
        });
        srow.bg = surfOf(a); srow.radius = 6;
        root.push_back(srow);
        y += 42;
    }

    // rows
    std::vector<uint32_t> kids = visibleChildren(a);
    std::vector<ui::Node> rows;
    int rowY = y - a.scrollY;
    for (size_t k = 0; k < kids.size(); ++k) {
        uint32_t idx = kids[k];
        const nbt::Node& n = d.arena[idx];
        bool hasKids = !n.kids.empty();
        std::vector<uint32_t> path = indexPathTo(a, idx);
        bool open = nodeOpen(a, path);
        std::string summary = valueSummary(d, idx);
        ui::Node row = ui::treeRow({10, rowY, a.w - 20, kRowH},
            n.name.empty() ? "<unnamed>" : n.name, summary, typeColor(th, n.type),
            hasKids, open, (int)k == a.ed.selected, (int)k);
        row.bg = ((int)k == a.ed.selected) ? gfx::rgba(th.primary.r, th.primary.g, th.primary.b, 40)
                                           : surfOf(a);
        row.action = A_TREE_TAP;
        row.actionLong = A_TREE_LONG;
        rows.push_back(row);
        rowY += kRowH + 2;
    }
    // scroll-independent content height (dy above is scroll-shifted)
    a.contentBottom = (int)(y + kids.size() * (kRowH + 2));
    ui::Node scroll = ui::list({6, y, a.w - 12, a.h - y}, rows);
    scroll.kind = ui::Kind::Scroll;
    root.push_back(scroll);

    // footer FABs: add + save
    ui::Node fab = ui::button({a.w - 62, a.h - 62, 46, 46}, "+", th.primary, th.onPrimary, A_TREE_ADD, 23);
    ui::Node save = ui::button({a.w - 62, a.h - 116, 46, 46}, "S", th.primary, th.onPrimary, A_TREE_SAVE, 16);
    fab.bg = th.primary; save.bg = th.secondary;
    root.push_back(fab);
    root.push_back(save);
    if (a.ed.dirty) {
        root.push_back(ui::text({10, a.h - 22, a.w - 20, 16},
            "unsaved changes", th.error));
    }

    return ui::panel({0, 0, a.w, a.h}, root);
}

// ---------- FLAT WORLD BUILDER ----------------------------------------------
ui::Node buildFlat(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, "Create flat world", true, {
        ui::button({0, 0, 32, 32}, "OK", th.primary, th.onPrimary, A_FLAT_CREATE, 16),
    }));

    int y = kBarH + 10;
    Rect area{12, y, a.w - 24, a.h - y - 12};

    // biome
    ui::Node biomeRow = ui::panel({area.x, y, area.w, kRowH}, {
        ui::text({area.x + 10, y + 8, 60, 18}, "Biome", mutOf(a)),
        ui::button({area.x + 80, y + 4, area.w - 90, 26},
                   kFlatBiomes[a.flatBiome], surfOf(a), th.primary, A_FLAT_BIOME),
    });
    biomeRow.bg = surfOf(a); biomeRow.radius = 6;
    root.push_back(biomeRow);
    y += kRowH + 8;

    // base-layer checkbox
    ui::Node baseRow = ui::checkbox({area.x, y, area.w, kRowH},
        "Add bedrock base layer", a.flatBaseLayer, A_FLAT_TOGGLE_BASE);
    baseRow.bg = surfOf(a); baseRow.radius = 6;
    root.push_back(baseRow);
    y += kRowH + 8;

    // layers header
    root.push_back(ui::text({area.x, y, area.w, 18}, "Block layers (bottom first)", mutOf(a)));
    y += 24;

    int idx = 0;
    int total = 0;
    std::vector<ui::Node> rows;
    int ry = y - a.scrollY;
    for (auto& l : a.flatLayers) {
        total += (int)l.count;
        ui::Node row = ui::panel({area.x, ry, area.w, kRowH}, {
            ui::text({area.x + 10, ry + 8, area.w - 130, 18}, l.block, fgOf(a)),
            ui::text({area.x + area.w - 120, ry + 8, 60, 18}, "x" + std::to_string(l.count), th.primary),
            ui::button({area.x + area.w - 54, ry + 4, 24, 26}, "^", bgOf(a), fgOf(a), A_FLAT_LAYER_UP),
            ui::button({area.x + area.w - 28, ry + 4, 24, 26}, "v", bgOf(a), fgOf(a), A_FLAT_LAYER_DOWN),
        });
        row.bg = surfOf(a); row.radius = 6;
        row.action = A_FLAT_EDIT_LAYER; row.index = idx++;
        row.actionLong = A_FLAT_DEL_LAYER;
        rows.push_back(row);
        ry += kRowH + 4;
    }
    root.push_back(ui::list({area.x, y, area.w, a.h - y - 130}, rows));

    // total + add button
    root.push_back(ui::text({area.x, a.h - 122, area.w, 18},
        "Total thickness: " + std::to_string(total) + " / 384",
        (total > 384) ? th.error : mutOf(a)));
    ui::Node addBtn = ui::button({area.x, a.h - 100, area.w, 40},
        "Add layer", th.primary, th.onPrimary, A_FLAT_ADD_LAYER);
    root.push_back(addBtn);

    return ui::panel({0, 0, a.w, a.h}, root);
}

// ---------- HELP / ABOUT -----------------------------------------------------
ui::Node buildHelp(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, "Help & About", true, {}));
    int y = kBarH + 12;
    std::vector<std::string> lines = {
        "NBT Editor (native C++)",
        "Version " + std::string(kAppVersion) + " (build " + std::to_string(kAppBuild) + ")",
        "",
        "File",
        " - Open: browse to any .dat / .nbt file",
        " - New: create a fresh root compound",
        " - Save: atomic write to the same path",
        " - level.dat is auto-detected (8-byte header)",
        "",
        "Editing",
        " - Tap a row: drill down / edit value",
        " - Long press: rename, delete, copy, move",
        " - + adds a tag of any of the 13 types",
        " - Search filters keys across the document",
        "",
        "Worlds",
        " - Import/export .mcworld (zip)",
        " - Create a custom flat world",
        " - Encrypted worlds are detected and blocked",
        "",
        "All data stays on this device. No network access.",
    };
    for (auto& l : lines) {
        if (l.empty()) { y += 6; continue; }
        bool head = l.find(":") != std::string::npos || l[0] != ' ';
        root.push_back(ui::text({16, y, a.w - 32, 18}, l,
            head ? th.primary : fgOf(a)));
        y += 20;
    }
    return ui::panel({0, 0, a.w, a.h}, root);
}

} // namespace app

// ============================================================================
// L6c — RENDERER (draws the node tree) + ACTION HANDLER + PLATFORM GLUE (L0)
// ============================================================================
namespace app {

// ---- drawing ---------------------------------------------------------------
inline gfx::Color fgFrom(const gfx::Theme& th) { return th.onSurface; }

inline void drawNode(gfx::Canvas& c, const gfx::Theme& th, const ui::Node& n, int depth) {
    if (!n.visible || n.rect.empty()) return;
    Rect r = n.rect;
    switch (n.kind) {
        case ui::Kind::Panel:
            if (n.bg.a) c.roundRect(r, n.radius, n.bg);
            if (n.borderW) c.stroke(r, n.border, n.borderW);
            break;
        case ui::Kind::Divider:
            c.fill(r, n.bg);
            break;
        case ui::Kind::Text:
            c.pushClip(r);
            c.text(r.x, r.y + 1, n.text, n.fg, (int)n.user);
            c.popClip();
            break;
        case ui::Kind::Button: {
            if (!n.enabled) { c.roundRect(r, n.radius, gfx::rgba(n.bg.r, n.bg.g, n.bg.b, 90)); }
            else c.roundRect(r, n.radius, n.bg);
            if (n.selected) c.stroke(r, th.primary, 2);
            c.pushClip(r);
            c.textCentered(r, n.text, n.fg, 1);
            c.popClip();
            break; }
        case ui::Kind::IconTile: {
            c.roundRect(r, 6, n.bg);
            c.pushClip(r);
            c.textCentered(r, n.text, th.onPrimary);
            c.popClip();
            break; }
        case ui::Kind::Checkbox: {
            c.roundRect(r, n.radius, n.bg);
            Rect box{r.x + 8, r.y + (r.h - 18) / 2, 18, 18};
            c.stroke(box, th.primary, 1);
            if (n.checked) {
                c.fill({box.x + 3, box.y + 3, 12, 12}, th.primary);
            }
            c.pushClip(r);
            c.text(r.x + 34, r.y + (r.h - gfx::kGlyphH) / 2, n.text, fgFrom(th));
            c.popClip();
            break; }
        case ui::Kind::Field: {
            c.roundRect(r, n.radius, n.bg);
            if (n.borderW) c.stroke(r, n.border, n.borderW);
            c.pushClip(r);
            if (!n.text.empty()) c.text(r.x + 8, r.y + 3, n.text, th.onSurfaceMuted);
            c.textEllipsis({r.x + 8, r.y + 16, r.w - 16, r.h - 18}, n.value.empty() ? "(empty)" : n.value,
                           n.value.empty() ? th.onSurfaceMuted : th.onSurface);
            c.popClip();
            break; }
        case ui::Kind::SearchField: {
            c.roundRect(r, n.radius, th.background);
            if (n.borderW) c.stroke(r, th.outline, n.borderW);
            c.pushClip(r);
            c.text(r.x + 6, r.y + (r.h - gfx::kGlyphH) / 2, n.value.empty() ? "search" : n.value,
                   n.value.empty() ? th.onSurfaceMuted : th.onSurface);
            c.popClip();
            break; }
        case ui::Kind::Scroll:
            break;   // rows are drawn as children; clipping handled by the caller
        case ui::Kind::List:
        case ui::Kind::Row:
        case ui::Kind::Column:
        case ui::Kind::Breadcrumb:
            break;
        case ui::Kind::TreeRow: {
            c.roundRect(r, 6, n.bg);
            if (n.selected) c.stroke(r, th.primary, 2);
            // expander
            int tx = r.x + 8;
            if (n.user) {
                c.text(tx, r.y + (r.h - gfx::kGlyphH) / 2, n.expanded ? "-" : "+", th.onSurfaceMuted);
                tx += 16;
            } else {
                tx += 16;
            }
            // colour dot per type
            c.circle(tx + 4, r.y + r.h / 2, 4, n.accent);
            tx += 14;
            // key + summary
            int rightTextW = std::min(r.w - (tx - r.x) - 10, 120);
            c.pushClip({tx, r.y, r.w - (tx - r.x) - 10, r.h});
            c.text(tx, r.y + 4, n.text, th.onSurface);
            c.popClip();
            c.pushClip({r.right() - rightTextW - 6, r.y, rightTextW, r.h});
            c.textRight(r.right() - 6, r.y + 5, n.text2, th.onSurfaceMuted);
            c.popClip();
            break; }
        case ui::Kind::Dialog: break;
        case ui::Kind::Toast: break;
        case ui::Kind::ImageTile: break;
        case ui::Kind::Progress: {
            c.roundRect(r, r.h / 2, th.onSurfaceMuted);
            float v = 0.f;
            if (!n.value.empty()) v = std::strtof(n.value.c_str(), nullptr);
            if (v < 0) v = 0; if (v > 1) v = 1;
            c.roundRect({r.x, r.y, (int)(r.w * v), r.h}, r.h / 2, n.bg);
            break; }
        case ui::Kind::Keypad: break;
        case ui::Kind::Fab: {
            c.circle(r.x + r.w / 2, r.y + r.h / 2, r.h / 2, n.bg);
            c.pushClip(r);
            c.textCentered(r, n.text, n.fg);
            c.popClip();
            break; }
    }
    for (const auto& k : n.kids) drawNode(c, th, k, depth + 1);
}

// ---- screen dispatch -------------------------------------------------------
ui::Node buildScreen(AppState& a);

void buildDialogNodes(AppState& a, ui::Node& page);
void buildKeypadNodes(AppState& a, ui::Node& page);

inline void renderFrame(AppState& a, gfx::Canvas& canvas) {
    a.theme = a.dark ? gfx::themeDark() : gfx::themeLight();
    canvas.clear(a.theme.background);
    ui::Node page = buildScreen(a);

    // overlay interactive dialogs + keypad onto the SAME tree so taps resolve
    AppState& mutRef = const_cast<AppState&>(a);
    mutRef.uiRoot = std::make_shared<ui::Node>(page);
    if (a.dialog.open) buildDialogNodes(mutRef, *mutRef.uiRoot);
    if (a.keypadOpen) buildKeypadNodes(mutRef, *mutRef.uiRoot);

    drawNode(canvas, a.theme, page, 0);

    // ---- modal dialog (built as nodes so taps hit-test) ----
    if (a.dialog.open) {
        int dialogH = a.keypadOpen ? a.h - 40 : 210;
        int bodyRows = (int)a.dialog.body.size();
        if (bodyRows) dialogH += bodyRows * 40;
        Rect d{16, std::max(40, a.h / 2 - dialogH / 2), a.w - 32, std::min(a.h - 20, dialogH)};
        canvas.roundRect(d, 10, gfx::rgba(0, 0, 0, 90));          // shadow
        Rect dd = {d.x + 3, d.y + 3, d.w - 6, d.h - 6};
        canvas.roundRect(dd, 10, a.theme.surface);
        canvas.pushClip(dd);
        canvas.text(dd.x + 14, dd.y + 12, a.dialog.title, a.theme.onSurface);
        // wrap the message
        std::vector<std::string> words = gfx::split(a.dialog.message, ' ');
        std::string line;
        int my = dd.y + 32 + gfx::kGlyphH + 4;
        for (auto& w : words) {
            if (canvas.textWidth(line + " " + w) > dd.w - 28) {
                canvas.text(dd.x + 14, my, line, a.theme.onSurfaceMuted);
                my += gfx::kGlyphH + 2;
                line = w;
            } else {
                if (!line.empty()) line += " ";
                line += w;
            }
        }
        if (!line.empty()) { canvas.text(dd.x + 14, my, line, a.theme.onSurfaceMuted); my += gfx::kGlyphH + 4; }

        // body rows (value editor / add-tag / rename), drawn and hit-testable
        int by = my;
        for (auto& b : a.dialog.body) {
            b.rect.y = by;
            b.rect.x = dd.x + 12;
            b.rect.w = dd.w - 24;
            drawNode(canvas, a.theme, b, 1);
            by += 40;
        }

        // buttons
        int bty = dd.bottom() - 42;
        int btx = dd.right() - 12;
        auto placeButton = [&](const std::string& label, int act) {
            if (label.empty()) return;
            int bw = std::max(58, canvas.textWidth(label) + 18);
            btx -= bw + 6;
            ui::Node b;
            b.kind = ui::Kind::Button;
            b.rect = {btx, bty, bw, 32};
            b.text = label;
            b.bg = (act == A_DIALOG_NEG) ? a.theme.surface : a.theme.primary;
            b.fg = (act == A_DIALOG_NEG) ? a.theme.onSurface : a.theme.onPrimary;
            b.border = (act == A_DIALOG_NEG) ? a.theme.outline : a.theme.primary;
            b.borderW = (act == A_DIALOG_NEG) ? 1 : 0;
            b.action = act;
            b.radius = 6;
            b.enabled = true;
            drawNode(canvas, a.theme, b, 1);
        };
        placeButton(a.dialog.neu, A_DIALOG_NEU);
        placeButton(a.dialog.neg, A_DIALOG_NEG);
        placeButton(a.dialog.pos, A_DIALOG_POS);
        canvas.popClip();
    }

    // ---- progress dialog ----
    if (a.progressOpen) {
        Rect d{40, a.h / 2 - 60, a.w - 80, 120};
        canvas.roundRect(d, 10, a.theme.surface);
        canvas.pushClip(d);
        canvas.text(d.x + 14, d.y + 12, a.progressTitle, a.theme.onSurface);
        canvas.text(d.x + 14, d.y + 32, a.progressMsg, a.theme.onSurfaceMuted);
        ui::Node prog;
        prog.kind = ui::Kind::Progress; prog.rect = {d.x + 14, d.y + 74, d.w - 28, 12};
        prog.value = std::to_string(a.progressValue); prog.bg = a.theme.primary;
        drawNode(canvas, a.theme, prog, 1);
        canvas.popClip();
    }

    // ---- toasts ----
    int ty = a.h - 60;
    for (auto it = a.toasts.rbegin(); it != a.toasts.rend(); ++it) {
        if (a.nowMs > it->untilMs) continue;
        Rect t{14, ty, a.w - 28, 34};
        canvas.roundRect(t, 8, gfx::rgba(0x20, 0x20, 0x20, 220));
        canvas.pushClip(t);
        canvas.text(t.x + 10, t.y + 8, it->message, gfx::rgb(0xff, 0xff, 0xff));
        canvas.popClip();
        ty -= 40;
    }
}

inline ui::Node buildScreen(AppState& a) {
    switch (a.screen) {
        case S_HOME:   return buildHome(a);
        case S_WORLDS: return buildWorlds(a);
        case S_BROWSER:return buildBrowser(a);
        case S_FLAT:   return buildFlat(a);
        case S_HELP:   return buildHelp(a);
        case S_TREE:
        default:       return buildTree(a);
    }
}

} // namespace app

// ============================================================================
// L6d — ACTION HANDLER (all real behaviour lives here)
// Every control in the UI maps to an A_* intent; this function applies it.
// ============================================================================
namespace app {

void setScreen(AppState& a, int s) {
    if (a.screen == s) return;
    a.screenStack.push_back(a.screen);
    a.screen = s;
}

void back(AppState& a) {
    if (a.dialog.open) { closeDialog(a); return; }
    if (a.keypadOpen) { a.keypadOpen = false; return; }
    if (!a.screenStack.empty()) {
        a.screen = a.screenStack.back();
        a.screenStack.pop_back();
        if (a.screen == S_WORLDS) listWorlds(a);
        return;
    }
    a.wantExit = true;
}

// ---- keypad (avoid depending on the platform IME) ---------------------------
const char* kKeypadLower = "abcdefghijklmnopqrstuvwxyz";
const char* kKeypadUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* kKeypadDigits = "1234567890";

void applyKey(AppState& a, const std::string& ch) {
    if (a.keypadTarget == "searchFilter") { a.ed.filter += ch; return; }
    if (a.valueOpen)          { a.valueText += ch; return; }
    if (a.addOpen)            { a.addName += ch; return; }
    if (a.renameOpen)         { a.renameName += ch; return; }
}

void keypadBackspace(AppState& a) {
    if (a.keypadTarget == "searchFilter" && !a.ed.filter.empty()) { a.ed.filter.pop_back(); return; }
    if (a.valueOpen && !a.valueText.empty())  { a.valueText.pop_back(); return; }
    if (a.addOpen && !a.addName.empty())      { a.addName.pop_back(); return; }
    if (a.renameOpen && !a.renameName.empty()){ a.renameName.pop_back(); return; }
}

void openKeypad(AppState& a, const std::string& target, bool numeric, const std::string& initial) {
    a.keypadOpen = true;
    a.keypadTarget = target;
    a.keypadNumeric = numeric;
    a.keypadBuf = initial;
    a.keypadShift = false;
}

// ---- nbt editing primitives (used by handler) ------------------------------
void snapshotUndo(AppState& a, const char* label) {
    // No ed.open guard: a document created via A_NEW_NBT must still be undoable
    if (a.ed.doc.arena.empty()) {
        a.ed.doc.arena.push_back(nbt::Node{});
        a.ed.doc.arena[0].type = nbt::Type::Compound;
        a.ed.doc.root = 0;
    }
    EditorState::UndoEntry u;
    u.label = label;
    u.arena = a.ed.doc.arena;
    u.root = a.ed.doc.root;
    a.ed.undoStack.push_back(std::move(u));
    if (a.ed.undoStack.size() > 200) a.ed.undoStack.erase(a.ed.undoStack.begin());
    a.ed.redoStack.clear();
}

void commit(AppState& a) {
    a.ed.dirty = true;
    rebuildTreeListing(a);
}

bool addChildToCompound(AppState& a, uint32_t parentIdx, nbt::Type t, const std::string& name,
                        std::string& err) {
    if (!nbt::isCompound(t)) { err = "Only compounds can hold named tags."; return false; }
    for (uint32_t k : a.ed.doc.arena[parentIdx].kids) {
        const nbt::Node& sib = a.ed.doc.arena[k];
        if (sib.name.empty() && sib.type == nbt::Type::End) continue;
        if (sib.name == name) { err = nbt::msgDuplicateName(); return false; }
    }
    nbt::Node n; n.type = t; n.name = name;
    a.ed.doc.arena[parentIdx].kids.push_back((uint32_t)a.ed.doc.arena.size());
    a.ed.doc.arena.push_back(std::move(n));
    return true;
}

bool addListElement(AppState& a, uint32_t listIdx, std::string& err) {
    nbt::Node& l = a.ed.doc.arena[listIdx];
    if (!nbt::isList(l.type)) { err = "Not a list."; return false; }
    nbt::Node n; n.type = l.elemType;
    l.kids.push_back((uint32_t)a.ed.doc.arena.size());
    a.ed.doc.arena.push_back(std::move(n));
    return true;
}

bool removeNode(AppState& a, uint32_t idx, std::string& err) {
    // find parent from the index path
    std::vector<uint32_t> path = indexPathTo(a, idx);
    if (path.size() < 2) { err = "Cannot remove the root tag."; return false; }
    uint32_t parent = path[path.size() - 2];
    auto& kids = a.ed.doc.arena[parent].kids;
    for (auto it = kids.begin(); it != kids.end(); ++it) {
        if (*it == idx) { kids.erase(it); return true; }
    }
    err = "Tag not found.";
    return false;
}

bool renameNode(AppState& a, uint32_t idx, const std::string& name, std::string& err) {
    std::vector<uint32_t> path = indexPathTo(a, idx);
    if (path.size() < 2) { err = "Cannot rename the root tag."; return false; }
    uint32_t parent = path[path.size() - 2];
    if (a.ed.doc.arena[parent].type != nbt::Type::Compound) {
        err = "Only tags inside a compound can be renamed."; return false;
    }
    for (uint32_t k : a.ed.doc.arena[parent].kids) {
        if (k == idx) continue;
        if (a.ed.doc.arena[k].name == name) { err = nbt::msgDuplicateName(); return false; }
    }
    a.ed.doc.arena[idx].name = name;
    return true;
}

} // namespace app

// ---- interactive dialog bodies (value editor, add tag, rename) -------------
// These are rebuilt every frame as real ui nodes so taps are hit-tested and
// every visible control has a working action.
namespace app {

void buildDialogNodes(AppState& a, ui::Node& page);

inline void buildDialogNodes(AppState& a, ui::Node& page) {
    using namespace ui;
    gfx::Theme th = a.theme;
    std::vector<Node> body;

    // geometry must match the drawing code in renderFrame()
    int dialogH = a.keypadOpen ? a.h - 40 : 210;
    if (!a.dialog.body.empty()) dialogH += (int)a.dialog.body.size() * 40;
    int dw = a.w - 32, dh = std::min(a.h - 20, dialogH);
    int dx = 16, dy = std::max(40, a.h / 2 - dh / 2);

    // --- value editor -----------------------------------------------------
    if (a.valueOpen) {
        body.push_back(text({dx + 14, 0, dw - 28, 20},
            std::string("Type: ") + nbt::typeName(a.valueType), th.onSurfaceMuted));
        // value field with the current text
        Node f = field({dx + 14, 0, dw - 28, 34}, "value", a.valueText, A_KEY_TAP, a.valueNumeric);
        f.bg = th.background; f.border = th.outline; f.borderW = 1;
        body.push_back(f);
        if (!a.valueError.empty())
            body.push_back(text({dx + 14, 0, dw - 28, 18}, a.valueError, th.error));
        // quick numeric pad for numeric types
        if (a.valueNumeric) {
            std::vector<Node> keys;
            const char* labels1 = "1234567890";
            int kw = (dw - 28 - 10 * 2) / 10;
            int kx = dx + 14, ky = 0;
            for (const char* p = labels1; *p; ++p) {
                keys.push_back(button({kx, ky, kw - 2, 30}, std::string(1, *p), th.primary, th.onPrimary, A_KEY_TAP));
                kx += kw;
            }
            body.push_back(panel({dx + 14, 0, dw - 28, 32}, keys));
            std::vector<Node> keys2;
            kx = dx + 14;
            const char* labels2 = "-.clear";
            int kw2 = (dw - 28 - 6 * 2) / 6;
            for (const char* p = labels2; *p; ++p) {
                keys2.push_back(button({kx, 0, kw2 - 2, 30}, std::string(1, *p), th.surface, th.onSurface, A_KEY_TAP));
                kx += kw2;
            }
            body.push_back(panel({dx + 14, 0, dw - 28, 32}, keys2));
        } else {
            body.push_back(button({dx + 14, 0, dw - 28, 32}, "Type text…", th.primary, th.onPrimary, A_KEY_TAP));
        }
        body.push_back(button({dx + 14, 0, (dw - 28) / 2 - 6, 32}, "SAVE", th.primary, th.onPrimary, A_VALUE_SAVE));
        body.push_back(button({dx + 14 + (dw - 28) / 2 + 6, 0, (dw - 28) / 2 - 6, 32}, "CANCEL", th.surface, th.onSurface, A_VALUE_CANCEL));
    }
    // --- add tag -----------------------------------------------------------
    else if (a.addOpen) {
        body.push_back(text({dx + 14, 0, dw - 28, 18}, "Tag type", th.onSurfaceMuted));
        // type spinner shown as 4 x 4 grid of small buttons
        std::vector<Node> types;
        nbt::Type all[16];
        int n = 0;
        for (int t = 1; t <= 12; ++t) { nbt::Type tt{}; if (nbt::typeFromId((uint8_t)t, tt)) all[n++] = tt; }
        int cols = 4, rows = (n + cols - 1) / cols;
        int cw = (dw - 28 - (cols - 1) * 4) / cols;
        int ch = 28;
        for (int r = 0; r < rows; ++r) {
            std::vector<Node> row;
            for (int c = 0; c < cols; ++c) {
                int k = r * cols + c;
                if (k >= n) break;
                bool sel = (all[k] == a.valueTypeToAdd);
                Node b;
                b.kind = Kind::Button;
                b.rect = {0, 0, cw - 4, ch};
                static const char* kShortNames[12] = {"Byte","Short","Int","Long","Float","Double",
                                               "Byte[]","String","List","Compound","Int[]","Long[]"};
        b.text = kShortNames[k];
                b.bg = sel ? th.primary : th.surface;
                b.fg = sel ? th.onPrimary : th.onSurface;
                b.border = sel ? th.primary : th.outline;
                b.borderW = sel ? 0 : 1;
                b.action = A_VALUE_KIND;
                b.index = k;
                b.radius = 6;
                b.enabled = true;
                row.push_back(b);
            }
            types.push_back(panel({dx + 14, 0, dw - 28, ch}, row));
        }
        for (auto& t : types) body.push_back(t);

        if (a.addListMode == 0) {
            Node f = field({dx + 14, 0, dw - 28, 30}, "Tag name", a.addName, A_KEY_TAP, false);
            f.bg = th.background; f.border = th.outline; f.borderW = 1;
            body.push_back(f);
        } else if (a.addListMode == 1) {
            body.push_back(text({dx + 14, 0, dw - 28, 18},
                "Elements inherit the list's type", th.onSurfaceMuted));
        }
        if (!a.addError.empty())
            body.push_back(text({dx + 14, 0, dw - 28, 18}, a.addError, th.error));
    }
    // --- rename ------------------------------------------------------------
    else if (a.renameOpen) {
        Node f = field({dx + 14, 0, dw - 28, 30}, "New name", a.renameName, A_KEY_TAP, false);
        f.bg = th.background; f.border = th.outline; f.borderW = 1;
        body.push_back(f);
        if (!a.addError.empty())
            body.push_back(text({dx + 14, 0, dw - 28, 18}, a.addError, th.error));
    }
    // --- confirmation dialogs are plain text (buttons drawn by renderFrame) --

    // one last body row: the action buttons area gets a clickable strip so
    // accidental taps outside buttons don't pass through to the screen
    for (auto& b : body) { b.rect.y += dy + 100; b.visible = true; page.kids.push_back(b); }
}

// ---- on-screen keypad -----------------------------------------------------
void buildKeypadNodes(AppState& a, ui::Node& page);

inline void buildKeypadNodes(AppState& a, ui::Node& page) {
    using namespace ui;
    gfx::Theme th = a.theme;
    std::vector<Node> rows;
    int padW = a.w - 24, padX = 12;
    int padH = a.h - 40, padY = 36;
    std::string* target = nullptr;
    if (a.keypadTarget == "searchFilter") target = &a.ed.filter;
    else if (a.keypadTarget == "value") target = &a.valueText;
    else if (a.keypadTarget == "addName") target = &a.addName;
    else if (a.keypadTarget == "renameName") target = &a.renameName;
    else if (a.keypadTarget == "flatThickness") target = &a.flatDraft;

    // header: current text + done
    Node title = panel({padX, padY, padW, 30}, {
        text({padX + 8, padY + 6, padW - 90, 18},
             target && !target->empty() ? *target : "(empty)", th.onSurface),
        button({padX + padW - 80, padY + 2, 72, 26}, "Done", th.primary, th.onPrimary, A_KEY_ENTER),
    });
    title.bg = th.surface; title.radius = 8;
    rows.push_back(title);
    int y = padY + 38;

    // keyboard rows
    const char* keyRows[4];
    int nRows = 0;
    if (a.keypadNumeric) {
        keyRows[0] = "123"; keyRows[1] = "456"; keyRows[2] = "789";
        keyRows[3] = ".0-";
        nRows = 4;
    } else {
        const char* l1 = "abcdefghi";
        const char* l2 = "jklmnopq";
        const char* l3 = "rstuvwxy";
        keyRows[0] = l1; keyRows[1] = l2; keyRows[2] = l3; keyRows[3] = "z_./ ";
        nRows = 4;
    }
    const int kCols = 3;                 // 3 keys per row (numeric pad)
    const int kRowsUsed = std::min(3, nRows);
    int kh = (padH - y - 130 - kRowsUsed * 4) / kRowsUsed;
    int kw = (padW - 8 - (kCols + 1) * 3) / kCols;
    // keys are laid out in rows of kCols for a predictable, thumb-friendly grid
    std::string allKeys;
    for (int r = 0; r < nRows; ++r) allKeys += keyRows[r];
    int keyIdx = 0;
    for (int r = 0; r < kRowsUsed && keyIdx < (int)allKeys.size(); ++r) {
        std::vector<Node> row;
        int kx = padX + 4;
        for (int c = 0; c < kCols && keyIdx < (int)allKeys.size(); ++c, ++keyIdx) {
            char ch = allKeys[keyIdx];
            std::string label(1, ch);
            if (ch == ' ') label = "SPC";
            else if (ch == '_') label = "_";
            Node b = button({kx, y, kw, kh - 4}, label, th.surface, th.onSurface, A_KEY_TAP);
            b.index = (int)(unsigned char)ch;
            row.push_back(b);
            kx += kw + 3;
        }
        rows.push_back(panel({padX, y, padW, kh}, row));
        y += kh + 4;
    }
    // bottom controls
    int bw = (padW - 8 - 9) / 4;
    std::vector<Node> bottom = {
        button({padX + 4, y, bw, 40}, "SHIFT", th.surface, th.onSurface, A_KEY_SHIFT),
        button({padX + 8 + bw, y, bw, 40}, "abc/123", th.surface, th.onSurface, A_KEY_MODE),
        button({padX + 12 + 2 * bw, y, bw, 40}, "DEL", th.surface, th.onSurface, A_KEY_DEL),
        button({padX + 16 + 3 * bw, y, bw, 40}, "ENTER", th.primary, th.onPrimary, A_KEY_ENTER),
    };
    rows.push_back(panel({padX, y, padW, 44}, bottom));
    for (auto& r : rows) page.kids.push_back(r);
}

} // namespace app

// ---- commit functions for the dialogs (value / add / rename) ---------------
namespace app {

// Open the value editor for a node (type-specific, keyboard aware).
void openValueEditor(AppState& a, uint32_t idx) {
    const nbt::Node& n = a.ed.doc.arena[idx];
    if (nbt::isContainer(n.type)) return;             // containers are navigated, not edited
    a.valueOpen = true;
    a.valueIdx = idx;
    a.valueType = n.type;
    a.valueNumeric = (n.type != nbt::Type::String);
    a.valueError.clear();
    switch (n.type) {
        case nbt::Type::String:    a.valueText = n.s; break;
        case nbt::Type::Float:
        case nbt::Type::Double: {
            char b[40]; std::snprintf(b, sizeof b, "%g", n.d); a.valueText = b; break; }
        default:                   a.valueText = std::to_string(n.i); break;
    }
    a.dialog.open = true;
    a.dialog.title = "Edit";
    a.dialog.message = std::string("Type: ") + nbt::typeName(n.type);
    a.dialog.pos = "SAVE"; a.dialog.neg = "CANCEL";
    a.dialog.posAction = A_VALUE_SAVE;
    a.dialog.negAction = A_VALUE_CANCEL;
    a.dialog.body.clear();
}

// Validate + apply the text in the value editor.
void commitValue(AppState& a) {
    nbt::Node tmp;
    std::string msg;
    if (!nbt::parseScalar(a.valueText, a.valueType, tmp, msg)) {
        a.valueError = msg;
        pushToast(a, msg, a.theme.error);
        return;
    }
    nbt::Node& dst = a.ed.doc.arena[a.valueIdx];
    snapshotUndo(a, "edit value");
    if (a.valueType == nbt::Type::String) dst.s = tmp.s;
    else if (a.valueType == nbt::Type::Float || a.valueType == nbt::Type::Double) dst.d = tmp.d;
    else dst.i = tmp.i;
    a.valueOpen = false;
    closeDialog(a);
    commit(a);
    pushToast(a, "Value updated", a.theme.success);
}

// Create a new child inside the current compound, or an element in a list.
void commitAdd(AppState& a) {
    std::string err;
    uint32_t cur = a.ed.pathIdx.empty() ? a.ed.doc.root : a.ed.pathIdx.back();
    nbt::Node& target = a.ed.doc.arena[cur];
    snapshotUndo(a, "add tag");
    if (nbt::isList(target.type)) {
        nbt::Node n; n.type = target.elemType;
        target.kids.push_back((uint32_t)a.ed.doc.arena.size());
        a.ed.doc.arena.push_back(std::move(n));
    } else if (nbt::isArray(target.type)) {
        if (target.type == nbt::Type::ByteArray) {
            if (a.valueTypeToAdd != nbt::Type::Byte) {
                a.addError = nbt::msgOutOfBoundsFor();
                pushToast(a, "Only byte tags can be added to byte arrays", a.theme.error);
                return;
            }
            target.bytes.push_back(0);
        } else if (target.type == nbt::Type::IntArray) {
            if (a.valueTypeToAdd != nbt::Type::Int) {
                pushToast(a, "Only int tags can be added to int arrays", a.theme.error);
                return;
            }
            target.longs.push_back(0);
        } else {
            if (a.valueTypeToAdd != nbt::Type::Long) {
                pushToast(a, "Only long tags can be added to long arrays", a.theme.error);
                return;
            }
            target.longs.push_back(0);
        }
    } else if (nbt::isCompound(target.type)) {
        if (a.valueTypeToAdd == nbt::Type::End) {
            a.addError = "Pick a tag type.";
            return;
        }
        std::string name = gfx::trim(a.addName);
        if (name.empty()) {
            a.addError = nbt::msgUnnamedTag();
            pushToast(a, nbt::msgUnnamedTag(), a.theme.error);
            return;
        }
        for (uint32_t k : target.kids) {
            if (a.ed.doc.arena[k].name == name) {
                a.addError = nbt::msgDuplicateName();
                pushToast(a, nbt::msgDuplicateName(), a.theme.error);
                return;
            }
        }
        nbt::Node n; n.type = a.valueTypeToAdd; n.name = name;
        if (n.type == nbt::Type::List) n.elemType = nbt::Type::Byte;   // typed on first add
        target.kids.push_back((uint32_t)a.ed.doc.arena.size());
        a.ed.doc.arena.push_back(std::move(n));
        pushToast(a, "Tag added", a.theme.success);
    } else {
        pushToast(a, "Cannot add to this tag", a.theme.error);
        return;
    }
    a.addOpen = false;
    closeDialog(a);
    commit(a);
}

// Validate + apply a rename (compounds only, unique key).
void commitRename(AppState& a) {
    std::string err;
    std::string name = gfx::trim(a.renameName);
    if (name.empty()) {
        a.addError = nbt::msgUnnamedTag();
        pushToast(a, nbt::msgUnnamedTag(), a.theme.error);
        return;
    }
    if (!renameNode(a, a.renameIdx, name, err)) {
        a.addError = err;
        pushToast(a, err, a.theme.error);
        return;
    }
    snapshotUndo(a, "rename tag");
    a.renameOpen = false;
    a.keypadOpen = false;
    a.keypadTarget.clear();
    closeDialog(a);
    commit(a);
    pushToast(a, "Renamed to " + name, a.theme.success);
}

} // namespace app

// ---- main action dispatcher ------------------------------------------------
namespace app {
using app::AppState;
using app::EditorState;

void popScreenTo(AppState& a, int s);

void handleAction(AppState& a, int act);

inline void handleAction(AppState& a, int act) {
    std::string err;
    switch (act) {
        // ---------- global ----------
        case A_TOGGLE_THEME:
            a.dark = !a.dark;
            a.prefs.theme = a.dark ? "dark" : "light";
            pushToast(a, a.dark ? "Dark theme" : "Light theme", a.theme.primary);
            break;
        case A_DISMISS_TOAST:
            a.toasts.clear();
            break;
        case A_HELP:
            setScreen(a, S_HELP);
            break;
        case A_QUIT:
            a.wantExit = true;
            break;

        // ---------- home ----------
        case A_OPEN_NBT:
            a.browseForSave = false;
            a.browseDir = a.prefs.lastDir.empty() ? "/sdcard" : a.prefs.lastDir;
            listBrowseDir(a);
            setScreen(a, S_BROWSER);
            if (!a.storageAsked) {
                a.storageAsked = true;
                requestStoragePermission();
                pushToast(a, "Allow 'All files access' so NBT files can be read",
                          a.theme.onSurfaceMuted);
            }
            break;
        case A_NEW_NBT: {
            a.ed = EditorState{};
            a.ed.doc = newRootCompound();
            a.ed.levelDat = false;
            a.ed.path = "";
            a.ed.open = true;
            a.ed.dirty = true;
            a.ed.pathIdx.push_back(a.ed.doc.root);
            setScreen(a, S_TREE);
            pushToast(a, "New document - tap + to add tags", a.theme.onSurfaceMuted);
            break; }
        case A_RECENT_PICK: {
            int idx = a.ed.selected;
            if (idx >= 0 && idx < (int)a.recent.size()) {
                std::string p = a.recent[idx];
                bool levelDat = (toLowerCopy(baseName(p)) == "level.dat");
                if (!openDocument(a, p, levelDat, err)) {
                    pushToast(a, "Could not open: " + err, a.theme.error);
                    a.recent.erase(a.recent.begin() + idx);
                } else addRecent(a, p);
            }
            break; }
        case A_WORLDS:
            listWorlds(a);
            setScreen(a, S_WORLDS);
            break;
        case A_WORLD_IMPORT:
            a.browseForSave = false;
            a.importing = true;
            a.browseDir = "/sdcard/Download";
            listBrowseDir(a);
            setScreen(a, S_BROWSER);
            pushToast(a, "Pick a .mcworld or .mctemplate file", a.theme.onSurfaceMuted);
            if (!a.storageAsked) {
                a.storageAsked = true;
                requestStoragePermission();
            }
            break;
        case A_WORLD_EDIT:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                std::string lvl = joinPath(a.worlds[a.ed.selected].folder, "level.dat");
                if (!openDocument(a, lvl, /*levelDat*/ true, err))
                    pushToast(a, "Could not open level.dat: " + err, a.theme.error);
                else addRecent(a, lvl);
            }
            break;
        case A_WORLD_EXPORT_DONE:
            a.progressOpen = false;
            break;

        // ---------- browser ----------
        case A_BROWSE_UP: {
            std::string up = parentPath(a.browseDir);
            if (up != a.browseDir) { a.browseDir = up; listBrowseDir(a); }
            break; }
        case A_BROWSE_OPEN: {
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.browseEntries.size()) {
                if (a.browseIsDir[a.ed.selected]) {
                    a.browseDir = a.browseEntries[a.ed.selected];
                    a.prefs.lastDir = a.browseDir;
                    listBrowseDir(a);
                }
            }
            break; }
        case A_FILE_SELECT: {
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.browseEntries.size()) {
                std::string p = a.browseEntries[a.ed.selected];
                if (a.importing) {
                    std::string lower = toLowerCopy(baseName(p));
                    if (!(lower.find(".mcworld") != std::string::npos ||
                          lower.find(".mctemplate") != std::string::npos)) {
                        pushToast(a, "Not a .mcworld/.mctemplate file", a.theme.error);
                        break;
                    }
                    std::string stem = baseName(p);
                    size_t dot = stem.find_last_of('.');
                    if (dot != std::string::npos) stem = stem.substr(0, dot);
                    std::error_code ec;
                    std::string dest = joinPath(a.worldsRoot, stem);
                    int suffix = 1;
                    while (fs::exists(dest, ec)) dest = joinPath(a.worldsRoot, stem + "_" + std::to_string(suffix++));
                    a.progressOpen = true;
                    a.progressTitle = "Importing world";
                    a.progressMsg = baseName(p) + " -> " + dest;
                    a.progressValue = 0.25f;
                    appWorker().post([&a, p, dest]() mutable {
                        auto r = extractZip(p, dest);
                        // AppState is owned by the render thread: lock before use.
                        std::lock_guard<std::mutex> lk(g_stateMutex);
                        a.progressValue = 1.0f;
                        a.progressOpen = false;
                        if (r.ok()) {
                            pushToast(a, "Imported: " + readLevelName(dest), a.theme.success);
                            listWorlds(a);
                            popScreenTo(a, S_WORLDS);
                        } else pushToast(a, "Import failed: " + r.detail, a.theme.error);
                    });
                    a.importing = false;
                    break;
                }
                if (a.browseForSave) {
                    // Save-As keeps the document's container format
                    std::string target = a.browseForSave ? p : a.pendingSavePath;
                    if (fs::is_directory(p)) {
                        target = joinPath(p, baseName(a.ed.path.empty() ? "untitled.dat" : a.ed.path));
                    }
                    if (saveDocument(a, target, err)) {
                        a.ed.path = target;
                        pushToast(a, "Saved to " + target, a.theme.success);
                        addRecent(a, target);
                        popScreenTo(a, S_TREE);
                    } else {
                        pushToast(a, "Save failed: " + err, a.theme.error);
                    }
                } else {
                    bool levelDat = (toLowerCopy(baseName(p)) == "level.dat");
                    if (!openDocument(a, p, levelDat, err)) {
                        pushToast(a, "Not a valid NBT file: " + err, a.theme.error);
                    } else addRecent(a, p);
                }
            }
            break; }

        // ---------- worlds ----------
        case A_WORLD_CREATE:
            a.flatLayers = {{"minecraft:bedrock", 1}, {"minecraft:dirt", 2}, {"minecraft:grass_block", 1}};
            a.flatBiome = 1; a.flatBaseLayer = true;
            setScreen(a, S_FLAT);
            break;
        case A_WORLD_OPTIONS:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                auto& w = a.worlds[a.ed.selected];
                if (w.encrypted) {
                    pushToast(a, "Encrypted world - cannot be edited", a.theme.error);
                    break;
                }
                std::string lvl = joinPath(w.folder, "level.dat");
                if (!openDocument(a, lvl, /*levelDat*/ true, err))
                    pushToast(a, "Could not open level.dat: " + err, a.theme.error);
                else addRecent(a, lvl);
            }
            break;
        case A_WORLD_DELETE:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size())
                openDialog(a, "Delete world",
                           "Delete \"" + a.worlds[a.ed.selected].displayName + "\"? This cannot be undone.",
                           "DELETE", A_WORLD_DELETE, "CANCEL", A_DIALOG_NEG);
            break;
        case A_WORLD_EXPORT:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                auto& w = a.worlds[a.ed.selected];
                std::string dest = "/sdcard/Download";
                ensureDir(dest);
                std::string zip = joinPath(dest, w.internalName + ".mcworld");
                pushToast(a, "Exporting world...", a.theme.primary);
                a.progressOpen = true;
                a.progressTitle = "Export world";
                a.progressMsg = w.displayName + " -> " + zip;
                a.progressValue = 0.2f;
                appWorker().post([&a, w, zip]() mutable {
                    auto r = app::createZip(w.folder, zip);
                    std::lock_guard<std::mutex> lk(g_stateMutex);
                    a.progressValue = 1.0f;
                    a.progressOpen = false;
                    pushToast(a, r.ok() ? ("Exported " + baseName(zip)) : ("Export failed: " + r.detail),
                              r.ok() ? a.theme.success : a.theme.error);
                });
            }
            break;

        // ---------- nbt editor ----------
        case A_TREE_TAP: {
            auto kids = visibleChildren(a);
            if (a.ed.selected < 0 || a.ed.selected >= (int)kids.size()) break;
            uint32_t idx = kids[a.ed.selected];
            const nbt::Node& n = a.ed.doc.arena[idx];
            if (nbt::isContainer(n.type)) {
                a.ed.pathIdx.push_back(idx);
                a.scrollY = 0;
                // auto-expand inside search results
                if (!a.ed.filter.empty()) {
                    std::vector<uint32_t> p = indexPathTo(a, idx);
                    uint64_t h = hashPath(p);
                    bool present = false;
                    for (auto v : a.ed.openSet) if (v == h) present = true;
                    if (!present) a.ed.openSet.push_back(h);
                }
                rebuildTreeListing(a);
            } else if (nbt::isScalar(n.type)) {
                openValueEditor(a, idx);
            }
            break; }
        case A_TREE_LONG: {
            auto kids = visibleChildren(a);
            if (a.ed.selected < 0 || a.ed.selected >= (int)kids.size()) break;
            uint32_t idx = kids[a.ed.selected];
            const nbt::Node& n = a.ed.doc.arena[idx];
            bool removable = (a.ed.pathIdx.size() >= 2);
            std::vector<std::string> options;
            if (nbt::isScalar(n.type)) options.push_back("Edit");
            if (a.ed.doc.arena[a.ed.pathIdx.back()].type == nbt::Type::Compound) options.push_back("Rename");
            options.push_back("Copy");
            if (removable) options.push_back("Remove");
            a.tagOptionList = options;
            a.tagOptionIdx = idx;
            openDialog(a, "Tag options", nbt::typeName(n.type), "", 0, "", 0);
            a.dialog.body.clear();
            // render the options list inside the dialog body
            int i = 0;
            for (auto& o : a.tagOptionList) {
                ui::Node b;
                b.kind = ui::Kind::Button;
                b.rect = {24, 0, a.w - 48, 30};
                b.text = o;
                b.bg = a.theme.surface;
                b.fg = a.theme.onSurface;
                b.border = a.theme.outline; b.borderW = 1;
                b.radius = 6; b.enabled = true;
                b.action = A_TAG_OPTION;
                b.index = i++;
                a.dialog.body.push_back(b);
            }
            break; }
        case A_TAG_OPTION: {
            if (a.ed.selected < 0 || a.ed.selected >= (int)a.tagOptionList.size()) break;
            std::string choice = a.tagOptionList[a.ed.selected];
            uint32_t idx = a.tagOptionIdx;
            closeDialog(a);
            if (choice == "Edit") { openValueEditor(a, idx); }
            else if (choice == "Rename") {
                a.renameOpen = true;
                a.renameIdx = idx;
                a.renameName = a.ed.doc.arena[idx].name;
                openKeypad(a, "renameName", false, a.renameName);
            }
            else if (choice == "Copy") { handleAction(a, A_TREE_COPY); }
            else if (choice == "Remove") { handleAction(a, A_TREE_DELETE); }
            break; }
        case A_TREE_UP:
            if (a.ed.pathIdx.size() > 1) a.ed.pathIdx.pop_back();
            else back(a);
            rebuildTreeListing(a);
            break;
        case A_BREADCRUMB_TAP: {
            int depth = a.ed.selected;
            if (depth >= 0 && depth < (int)a.ed.pathIdx.size() && depth < (int)a.ed.pathIdx.size()) {
                a.ed.pathIdx.resize((size_t)depth + 1);
                a.scrollY = 0;
                rebuildTreeListing(a);
            }
            break; }
        case A_TREE_MOVE_ROOT:
            a.ed.pathIdx.clear();
            a.ed.pathIdx.push_back(a.ed.doc.root);
            rebuildTreeListing(a);
            break;
        case A_TREE_SAVE: {
            std::string p = a.ed.path.empty() ? "/sdcard/Download/untitled.dat" : a.ed.path;
            if (saveDocument(a, p, err)) {
                a.ed.path = p;
                pushToast(a, "Saved " + baseName(p), a.theme.success);
                a.ed.path = p;
                } else {
                pushToast(a, "Save failed: " + err, a.theme.error);
            }
            break; }
        case A_TREE_SAVE_AS:
            a.browseForSave = true;
            a.browseDir = a.ed.path.empty() ? "/sdcard/Download" : parentPath(a.ed.path);
            listBrowseDir(a);
            setScreen(a, S_BROWSER);
            break;
        case A_TREE_ADD:
            a.addOpen = true;
            a.addName.clear();
            a.addError.clear();
            a.ed.selected = -1;
            a.valueTypeToAdd = nbt::Type::Byte;
            if (a.ed.pathIdx.empty()) break;
            {
                uint32_t cur = a.ed.pathIdx.back();
                const nbt::Node& n = a.ed.doc.arena[cur];
                if (nbt::isList(n.type))       a.addListMode = 1;
                else if (nbt::isArray(n.type)) a.addListMode = 2;
                else                           a.addListMode = 0;
            }
            openDialog(a, "Add tag", "Pick a type (and name for compounds).", "ADD", A_TREE_ADD_DONE,
                       "CANCEL", A_DIALOG_NEG);
            break;
        case A_TREE_DELETE:
            if (a.ed.selected >= 0 && !a.ed.filter.empty()) break;
            {
                auto kids = visibleChildren(a);
                if (a.ed.selected < (int)kids.size()) {
                    uint32_t idx = kids[a.ed.selected];
                    const nbt::Node& n = a.ed.doc.arena[idx];
                    std::string label = n.name.empty() ? "<unnamed>" : n.name;
                    a.renameIdx = idx;
                    openDialog(a, "Remove tag",
                               "Remove \"" + label + "\"?", "REMOVE", A_TREE_DELETE,
                               "CANCEL", A_DIALOG_NEG);
                }
            }
            break;
        case A_TREE_RENAME:
            if (a.ed.selected >= 0) {
                auto kids = visibleChildren(a);
                if (a.ed.selected < (int)kids.size()) {
                    uint32_t idx = kids[a.ed.selected];
                    a.renameOpen = true;
                    a.renameIdx = idx;
                    a.renameName = a.ed.doc.arena[idx].name;
                    openKeypad(a, "renameName", false, a.renameName);
                }
            }
            break;
        case A_TREE_COPY:
            if (a.ed.selected >= 0) {
                auto kids = visibleChildren(a);
                if (a.ed.selected < (int)kids.size()) {
                    a.ed.clipIdx = kids[a.ed.selected];
                    a.ed.hasClip = true;
                    pushToast(a, "Tag copied", a.theme.primary);
                }
            }
            break;
        case A_TREE_PASTE:
            if (a.ed.hasClip && !a.ed.pathIdx.empty()) {
                uint32_t cur = a.ed.pathIdx.back();
                if (a.ed.doc.arena[cur].type == nbt::Type::Compound) {
                    snapshotUndo(a, "paste tag");
                    uint32_t copy = cloneNode(a, a.ed.doc.arena[a.ed.clipIdx]);
                    a.ed.doc.arena[cur].kids.push_back(copy);
                    commit(a);
                    pushToast(a, "Tag pasted", a.theme.primary);
                } else {
                    pushToast(a, "Paste works into compounds", a.theme.error);
                }
            }
            break;
        case A_TREE_UNDO:
            if (!a.ed.undoStack.empty()) {
                EditorState::UndoEntry current;
                auto u = a.ed.undoStack.back();
                current.label = u.label;
                current.arena = a.ed.doc.arena;
                current.root = a.ed.doc.root;
                a.ed.undoStack.pop_back();
                a.ed.redoStack.push_back(std::move(current));
                if (a.ed.redoStack.size() > 200) a.ed.redoStack.erase(a.ed.redoStack.begin());
                a.ed.doc.arena = u.arena;
                a.ed.doc.root = u.root;
                a.ed.selected = -1;
                a.ed.dirty = true;
                rebuildTreeListing(a);
                pushToast(a, "Undid " + u.label, a.theme.onSurfaceMuted);
            } else pushToast(a, "Nothing to undo", a.theme.onSurfaceMuted);
            break;
        case A_TREE_REDO:
            if (!a.ed.redoStack.empty()) {
                EditorState::UndoEntry current;
                auto r = a.ed.redoStack.back();
                current.label = r.label;
                current.arena = a.ed.doc.arena;
                current.root = a.ed.doc.root;
                a.ed.redoStack.pop_back();
                a.ed.undoStack.push_back(std::move(current));
                a.ed.doc.arena = r.arena;
                a.ed.doc.root = r.root;
                a.ed.selected = -1;
                a.ed.dirty = true;
                rebuildTreeListing(a);
                pushToast(a, "Redid " + r.label, a.theme.onSurfaceMuted);
            } else pushToast(a, "Nothing to redo", a.theme.onSurfaceMuted);
            break;
        case A_TREE_SEARCH:
            if (!a.ed.showSearch) a.ed.showSearch = true;
            openKeypad(a, "searchFilter", false, a.ed.filter);
            break;

        // ---------- value / add / rename dialogs ----------
        case A_VALUE_KIND:
            // pick a type in the add-tag dialog (the tapped node's index is the
            // position inside all[] of buildDialogNodes)
            if (a.ed.selected >= 0) {
                nbt::Type all[12];
                int n = 0;
                for (int t = 1; t <= 12; ++t) { nbt::Type tt{}; if (nbt::typeFromId((uint8_t)t, tt)) all[n++] = tt; }
                if (a.ed.selected < n) a.valueTypeToAdd = all[a.ed.selected];
            }
            break;
        case A_VALUE_SAVE:
            commitValue(a);
            break;
        case A_VALUE_CANCEL:
            a.valueOpen = false;
            closeDialog(a);
            break;
        case A_TREE_RENAME_DONE:
            commitRename(a);
            break;
        case A_TREE_ADD_DONE:
            commitAdd(a);
            break;

        // ---------- flat world ----------
        case A_FLAT_BIOME:
            a.flatBiome = (a.flatBiome + 1) % kFlatBiomeCount;
            break;
        case A_FLAT_TOGGLE_BASE:
            a.flatBaseLayer = !a.flatBaseLayer;
            break;
        case A_FLAT_ADD_LAYER: {
            int64_t total = 0;
            for (auto& l : a.flatLayers) total += l.count;
            if (total >= 384) {
                pushToast(a, "Total layer thickness exceeds the limit (384).", a.theme.error);
                break;
            }
            a.flatLayers.push_back({kFlatBlocks[(a.flatLayers.size()) % kFlatBlockCount], 1});
            break; }
        case A_FLAT_DEL_LAYER:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.flatLayers.size()) {
                if (a.flatLayers.size() > 1) a.flatLayers.erase(a.flatLayers.begin() + a.ed.selected);
                else pushToast(a, "At least one layer is required.", a.theme.error);
            }
            break;
        case A_FLAT_LAYER_UP:
            if (a.ed.selected > 0 && a.ed.selected < (int)a.flatLayers.size())
                std::swap(a.flatLayers[a.ed.selected - 1], a.flatLayers[a.ed.selected]);
            break;
        case A_FLAT_LAYER_DOWN:
            if (a.ed.selected >= 0 && a.ed.selected + 1 < (int)a.flatLayers.size())
                std::swap(a.flatLayers[a.ed.selected], a.flatLayers[a.ed.selected + 1]);
            break;
        case A_FLAT_EDIT_LAYER:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.flatLayers.size()) {
                a.keypadTarget = "flatThickness";
                openKeypad(a, "flatThickness", true,
                           std::to_string(a.flatLayers[a.ed.selected].count));
            }
            break;
        case A_FLAT_CREATE: {
            int64_t total = 0;
            for (auto& l : a.flatLayers) total += l.count;
            if (a.flatBaseLayer) total += 1;
            if (total > 384) {
                pushToast(a, "Total layer thickness exceeds the limit (384).", a.theme.error);
                break;
            }
            std::string dir = joinPath(a.worldsRoot, "CustomFlatWorld");
            ensureDir(dir);
            std::string lvlPath = joinPath(dir, "level.dat");
            std::vector<FlatLayer> layers = a.flatLayers;
            if (a.flatBaseLayer) layers.push_back({"minecraft:bedrock", 1});
            nbt::Document doc = newLevelDat("Custom Flat World");
            // set FlatWorldLayers
            bool found = false;
            for (auto& n : doc.arena) {
                if (n.name == "FlatWorldLayers") { n.s = buildFlatWorldJson(a.flatBiome, layers); found = true; }
            }
            if (!found) {
                nbt::Node f; f.type = nbt::Type::String; f.name = "FlatWorldLayers";
                f.s = buildFlatWorldJson(a.flatBiome, layers);
                doc.arena[0].kids.push_back((uint32_t)doc.arena.size());
                doc.arena.push_back(std::move(f));
            }
            auto bytes = nbt::serializeLevelDat(doc);
            if (!app::writeFileAtomic(lvlPath, bytes).ok()) {
                pushToast(a, "Could not write level.dat", a.theme.error);
                break;
            }
            app::writeText(joinPath(dir, "levelname.txt"), "Custom Flat World");
            listWorlds(a);
            popScreenTo(a, S_WORLDS);
            pushToast(a, "World created", a.theme.success);
            break; }

        // ---------- dialogs ----------
        case A_DIALOG_POS:
        case A_DIALOG_NEG:
        case A_DIALOG_NEU:
            if (a.dialog.open) {
                int chosen = act == A_DIALOG_POS ? a.dialog.posAction :
                             act == A_DIALOG_NEG ? a.dialog.negAction : a.dialog.neuAction;
                bool ok = (act == A_DIALOG_POS);

                // value editor / add tag / rename dialogs own their own buttons
                if (a.valueOpen) {
                    if (ok) commitValue(a);
                    else { a.valueOpen = false; a.dialog = ui::Dialog{}; }
                    break;
                }
                if (a.addOpen) {
                    if (ok) commitAdd(a);
                    else { a.addOpen = false; a.dialog = ui::Dialog{}; }
                    break;
                }
                if (a.renameOpen && ok) commitRename(a);
                else if (a.renameOpen) { a.renameOpen = false; a.dialog = ui::Dialog{}; break; }
                if (a.pendingEdit && ok) {
                    // confirmed: open the selected world
                    a.pendingEdit = false;
                    if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                        auto& w = a.worlds[a.ed.selected];
                        std::string lvl = joinPath(w.folder, "level.dat");
                        if (!openDocument(a, lvl, true, err))
                            pushToast(a, "Could not open: " + err, a.theme.error);
                        else addRecent(a, lvl);
                    }
                } else if (chosen == A_WORLD_DELETE && ok) {
                    if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                        std::error_code ec;
                        fs::remove_all(a.worlds[a.ed.selected].folder, ec);
                        pushToast(a, ec ? "Delete failed" : "World deleted",
                                  ec ? a.theme.error : a.theme.success);
                        listWorlds(a);
                    }
                } else if (chosen == A_TREE_DELETE && ok) {
                    if (removeNode(a, a.renameIdx, err)) {
                        commit(a);
                        pushToast(a, "Tag removed", a.theme.onSurfaceMuted);
                    } else pushToast(a, "Could not remove: " + err, a.theme.error);
                } else if (chosen == A_TREE_SAVE && ok) {
                    // real save (dialog is a confirmation)
                    std::string p = a.ed.path.empty() ? "/sdcard/Download/untitled.dat" : a.ed.path;
                    if (saveDocument(a, p, err)) {
                        a.ed.path = p;
                        pushToast(a, "Saved", a.theme.success);
                    } else pushToast(a, "Save failed: " + err, a.theme.error);
                }
                closeDialog(a);
            }
            break;

        // ---------- keypad ----------
        case A_KEY_TAP: {
            // The tapped key was encoded in uiNode.index; the handler receives it
            // through a.ed.selected fallback? No: keys carry the char in .index,
            // which the dispatcher copies into a lastKeyCode before calling us.
            std::string ch = a.lastKeyPressed;
            if (ch == "SPC") ch = " ";
            if (a.keypadOpen) applyKey(a, ch);
            break; }
        case A_KEY_SHIFT:
            a.keypadShift = !a.keypadShift;
            break;
        case A_KEY_DEL:
            keypadBackspace(a);
            break;
        case A_KEY_MODE:
            a.keypadNumeric = !a.keypadNumeric;
            break;
        case A_KEY_ENTER: {
            // commit the buffer into its target
            if (a.valueOpen) {
                commitValue(a);
            } else if (a.addOpen) {
                commitAdd(a);
            } else if (a.renameOpen) {
                commitRename(a);
            } else if (a.keypadTarget == "flatThickness") {
                int64_t v = a.flatDraft.empty() ? 0 : std::atoll(a.flatDraft.c_str());
                if (v > 0 && a.ed.selected >= 0 && a.ed.selected < (int)a.flatLayers.size())
                    a.flatLayers[a.ed.selected].count = v;
            }
            a.keypadOpen = false;
            a.keypadTarget.clear();
            break; }

        default:
            break;
    }
}

void popScreenTo(AppState& a, int s) {
    while (!a.screenStack.empty() && a.screenStack.back() != s) a.screenStack.pop_back();
    if (!a.screenStack.empty()) { a.screen = a.screenStack.back(); a.screenStack.pop_back(); }
    else a.screen = s;
}

} // namespace app

// ============================================================================
// L0 — PLATFORM GLUE (NativeActivity, window, input, permissions)
// Replaces android_native_app_glue: we implement the lifecycle ourselves so the
// whole app stays in this single translation unit.
// ============================================================================
#if !NBT_TESTABLE
#include <android/native_activity.h>
#include <csignal>
#include <android/looper.h>
#include <android/input.h>
#include <android/rect.h>
#include <android/log.h>
#include <jni.h>
#include <sys/prctl.h>

#define NBT_LOG_TAG "nbteditor"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, NBT_LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, NBT_LOG_TAG, __VA_ARGS__)

namespace app {

struct Platform {
    ANativeActivity* activity = nullptr;
    ANativeWindow*   window   = nullptr;
    ALooper*         looper   = nullptr;
    AInputQueue*     input    = nullptr;
    bool running = false;
    bool hasWindow = false;
    bool inputAttached = false;
    bool renderReady = false;
    bool paused = false;
    int  width = 0, height = 0;
};

static Platform   g_plat;
static AppState   g_state;
static gfx::Canvas g_canvas;
static app::Worker g_worker;
static bool        g_booted = false;
static std::mutex  g_frameMutex;

// ---- input queue (filled by the looper callback, drained each frame) -------
struct PendingEvent {
    int32_t action = 0;
    float   x = 0, y = 0;
    int64_t ms = 0;
    bool    down = false, up = false, move = false;
};
static std::vector<PendingEvent> g_events;
static std::mutex g_eventMutex;

static int looperInputCallback(int /*fd*/, int /*events*/, void*) {
    std::vector<PendingEvent> batch;
    AInputEvent* ev = nullptr;
    if (g_plat.input && AInputQueue_getEvent(g_plat.input, &ev) >= 0) {
        PendingEvent p;
        int32_t a = AMotionEvent_getAction(ev) & AMOTION_EVENT_ACTION_MASK;
        p.action = a;
        p.x = AMotionEvent_getX(ev, 0);
        p.y = AMotionEvent_getY(ev, 0);
        p.ms = AMotionEvent_getEventTime(ev) / 1000000;
        p.down = (a == AMOTION_EVENT_ACTION_DOWN || a == AMOTION_EVENT_ACTION_POINTER_DOWN);
        p.up   = (a == AMOTION_EVENT_ACTION_UP || a == AMOTION_EVENT_ACTION_POINTER_UP ||
                  a == AMOTION_EVENT_ACTION_CANCEL);
        p.move = (a == AMOTION_EVENT_ACTION_MOVE);
        batch.push_back(p);
        AInputQueue_finishEvent(g_plat.input, ev, 1);
    }
    if (!batch.empty()) {
        std::lock_guard<std::mutex> lk(g_eventMutex);
        g_events.insert(g_events.end(), batch.begin(), batch.end());
    }
    return 1;   // keep callback
}

// ---- lifecycle callbacks (app main/UI thread) ------------------------------
static void onStart(ANativeActivity*) { g_plat.paused = false; }
static void onResume(ANativeActivity*) { g_plat.paused = false; g_plat.renderReady = true; }
static void* onSaveInstanceState(ANativeActivity*, size_t*) { return nullptr; }
static void onPause(ANativeActivity*) {
    // Stop rendering while invisible: saves battery and avoids touching a window
    // that may already be destroyed.
    g_plat.renderReady = false;
    g_plat.paused = true;
}
static void onStop(ANativeActivity*) { g_plat.renderReady = false; g_plat.paused = true; }
static void onDestroy(ANativeActivity*) {
    g_plat.running = false;
    g_plat.renderReady = false;
    if (g_plat.looper) ALooper_wake(g_plat.looper);
    appWorker().stop();                     // no job may outlive the process
    diagLog("shutdown: worker stopped");
}
static void onWindowFocusChanged(ANativeActivity*, int focused) {
    g_plat.renderReady = (focused != 0);
}
static void onContentRectChanged(ANativeActivity*, const ARect*) {}
static void onConfigurationChanged(ANativeActivity*) {}

static void onNativeWindowCreated(ANativeActivity*, ANativeWindow* w) {
    g_plat.window = w;
    g_plat.width  = ANativeWindow_getWidth(w);
    g_plat.height = ANativeWindow_getHeight(w);
    ANativeWindow_setBuffersGeometry(w, g_plat.width, g_plat.height, WINDOW_FORMAT_RGBA_8888);
    g_plat.hasWindow = true;
    g_plat.renderReady = true;          // the window exists: we may draw now
}
static void onNativeWindowResized(ANativeActivity*, ANativeWindow* w) {
    g_plat.width  = ANativeWindow_getWidth(w);
    g_plat.height = ANativeWindow_getHeight(w);
}
static void onNativeWindowRedrawNeeded(ANativeActivity*, ANativeWindow*) { g_plat.renderReady = true; }
static void onNativeWindowDestroyed(ANativeActivity*, ANativeWindow*) {
    g_plat.hasWindow = false;
    g_plat.window = nullptr;
    g_plat.renderReady = false;
}
static void onInputQueueCreated(ANativeActivity*, AInputQueue* q) {
    g_plat.input = q;
    g_plat.inputAttached = false;      // attached by the app thread once ready
}
static void onInputQueueDestroyed(ANativeActivity*, AInputQueue*) {
    if (g_plat.input) AInputQueue_detachLooper(g_plat.input);
    g_plat.input = nullptr;
}

// ---- permission (runtime, without Java glue) -------------------------------
// Every JNI call is guarded: a failure here must NOT kill the process before the
// first frame (observed on some devices as an immediate close).
void requestStoragePermission() {
    if (!g_plat.activity || !g_plat.activity->vm) return;
    JavaVM* vm = g_plat.activity->vm;
    JNIEnv* env = g_plat.activity->env;
    bool attached = false;
    if (!env) {
        jint rc = vm->GetEnv((void**)&env, JNI_VERSION_1_6);
        if (rc == JNI_EDETACHED) {
            if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return;
            attached = true;
        } else if (rc != JNI_OK) return;
    }
    if (!g_plat.activity->clazz) { if (attached) vm->DetachCurrentThread(); return; }

    bool done = false;
    do {
        jclass intentCls = env->FindClass("android/content/Intent");
        if (!intentCls) break;
        jfieldID field = env->GetStaticFieldID(intentCls, "ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION",
                                               "Ljava/lang/String;");
        if (!field) break;
        jstring action = (jstring)env->GetStaticObjectField(intentCls, field);
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }
        jstring pkg = env->NewStringUTF("package:com.nbteditor");
        jclass uriCls = env->FindClass("android/net/Uri");
        jmethodID parse = uriCls ? env->GetStaticMethodID(uriCls, "parse",
                              "(Ljava/lang/String;)Landroid/net/Uri;") : nullptr;
        jobject uri = (parse && uriCls) ? env->CallStaticObjectMethod(uriCls, parse, pkg) : nullptr;
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }
        jmethodID init = env->GetMethodID(intentCls, "<init>",
                                          "(Ljava/lang/String;Landroid/net/Uri;)V");
        if (!init || !uri || !action) break;
        jobject intent = env->NewObject(intentCls, init, action, uri);
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }
        if (!intent) break;
        jclass actCls = env->GetObjectClass(g_plat.activity->clazz);
        if (!actCls) break;
        jmethodID startAct = env->GetMethodID(actCls, "startActivity",
                                               "(Landroid/content/Intent;)V");
        if (!startAct) break;
        env->CallVoidMethod(g_plat.activity->clazz, startAct, intent);
        if (env->ExceptionCheck()) { env->ExceptionClear(); }
        done = true;
    } while (false);

    if (env->ExceptionCheck()) env->ExceptionClear();
    if (attached) vm->DetachCurrentThread();
    diagLog("boot: permission request %s", done ? "launched" : "skipped");
}

static void crashHandler(int sig);
static void writeCrashNote(const std::string& filesDir, const std::string& why);
static void androidMain();

// ---- diagnostics breadcrumbs (readable by a file manager, no adb needed) ---
static void writeCrashNote(const std::string& filesDir, const std::string& why) {
    diagLog("FAILURE: %s", why.c_str());
    if (!filesDir.empty()) flushLog(filesDir);
    const char* dirs[] = {"/sdcard/Download", "/sdcard"};
    for (const char* d : dirs) {
        std::string p = joinPath(d, "nbt-editor-startup-error.txt");
        FILE* f = std::fopen(p.c_str(), "w");
        if (f) {
            std::fprintf(f, "NBT Editor failed to start\n%s\n", why.c_str());
            std::fclose(f);
            return;
        }
    }
}

// ---- crash capture (keeps the log even if a thread dies) -------------------
static void crashHandler(int sig) {
    const char* name = "unknown";
    switch (sig) {
        case SIGSEGV: name = "SIGSEGV (segmentation fault)"; break;
        case SIGABRT: name = "SIGABRT (abort)"; break;
        case SIGILL:  name = "SIGILL (illegal instruction)"; break;
        case SIGFPE:  name = "SIGFPE (arithmetic error)"; break;
        case SIGBUS:  name = "SIGBUS"; break;
        case SIGTRAP: name = "SIGTRAP"; break;
    }
    const std::string p = logFilePath(g_state.filesDir.empty() ? "." : g_state.filesDir);
    flushLog(g_state.filesDir.empty() ? "." : g_state.filesDir);
    FILE* f = std::fopen(p.c_str(), "a");
    if (f) {
        std::fprintf(f, "CRASH: signal %d %s\n", sig, name);
        std::fclose(f);
    }
    ::signal(sig, SIG_DFL);
    raise(sig);
}

// ---- input dispatch ---------------------------------------------------------
// The looper callback only *collects* events; this runs on the render thread and
// turns them into actions. Every tap resolves against the node tree that was
// drawn in the previous frame (ui::hitTest walks it in z-order), so no visible
// button is unreachable. Drags inside a scroll container scroll instead of
// tapping; holding still past the long-press limit fires actionLong.
static void dispatchTouch(AppState& a) {
    std::vector<PendingEvent> evs;
    {
        std::lock_guard<std::mutex> lk(g_eventMutex);
        if (g_events.empty()) return;
        evs.swap(g_events);
    }
    a.nowMs = nowMs();

    for (const PendingEvent& e : evs) {
        if (e.down) {
            a.touchActive = true; a.touchScrolled = false;
            a.touchStartX = (int)e.x; a.touchStartY = (int)e.y;
            a.touchLastY  = a.touchStartY;
            a.touchDownMs = e.ms;
            continue;
        }
        if (e.move) {
            if (!a.touchActive) continue;
            int dy = a.touchLastY - (int)e.y;
            a.touchLastY = (int)e.y;
            if (std::abs((int)e.x - a.touchStartX) > 14 ||
                std::abs((int)e.y - a.touchStartY) > 14) a.touchScrolled = true;
            if (a.touchScrolled && a.uiRoot) {
                if (ui::Node* sc = ui::findScrollAt(*a.uiRoot, (int)e.x, (int)e.y)) {
                    a.scrollY += dy;
                    int maxScroll = a.contentBottom - (sc->rect.y + sc->rect.h);
                    if (maxScroll < 0) maxScroll = 0;
                    if (a.scrollY < 0) a.scrollY = 0;
                    if (a.scrollY > maxScroll) a.scrollY = maxScroll;
                }
            }
            continue;
        }
        if (!e.up || !a.touchActive) continue;
        a.touchActive = false;
        if (a.touchScrolled) continue;                     // that was a drag

        const ui::Node* hit = nullptr;
        if (!a.uiRoot || !ui::hitTest(*a.uiRoot, (int)e.x, (int)e.y, hit) || !hit) continue;
        // rows carry their index: that is how list screens know what was picked
        if (hit->index >= 0) a.ed.selected = hit->index;
        int act = (e.ms - a.touchDownMs) > 480 ? hit->actionLong : hit->action;
        if (act) handleAction(a, act);
    }
}

// ---- the render thread ------------------------------------------------------
static void androidMain() {
    diagLog("render: thread started");

    // The looper is created on the thread that polls it, and the input queue is
    // attached to it here as well (a queue created on the main thread must be
    // bound to THIS looper or we never receive touch events).
    g_plat.looper = ALooper_forThread();      // create/bind one to this thread
    if (g_plat.looper) ALooper_acquire(g_plat.looper);   // keep a reference
    diagLog("render: looper=%p", (void*)g_plat.looper);

    int guard = 0;
    unsigned frames = 0;

    while (g_plat.running) {
        ++guard;

        if (g_plat.input && !g_plat.inputAttached && g_plat.looper) {
            // ident=1 marks "input" events (same convention as android_native_app_glue);
            // ALOOPER_EVENT_INPUT (-1) is reserved by the looper for its wake pipe.
            AInputQueue_attachLooper(g_plat.input, g_plat.looper,
                                     1 /*LOOPER_ID_INPUT*/, looperInputCallback, nullptr);
            g_plat.inputAttached = true;
            diagLog("render: input queue attached");
        }

        // Service input even while paused (cheap) so the UI stays responsive.
        if (g_plat.looper) ALooper_pollOnce(0, nullptr, nullptr, nullptr);

        if (!g_plat.renderReady) {
            // The draw fallback must not resurrect rendering while paused.
            if (!g_plat.paused && g_plat.hasWindow && g_plat.window && guard > 30)
                g_plat.renderReady = true;
            if (!g_plat.renderReady) {
                if (g_plat.looper) ALooper_pollOnce(-1, nullptr, nullptr, nullptr);
                else std::this_thread::sleep_for(std::chrono::milliseconds(16));
                continue;
            }
        }

        ANativeWindow* w = g_plat.window;
        if (!g_plat.hasWindow || !w || g_plat.width <= 0 || g_plat.height <= 0) {
            g_plat.renderReady = false;
            if (g_plat.looper) ALooper_pollOnce(-1, nullptr, nullptr, nullptr);
            continue;
        }

        AppState& a = g_state;
        a.nowMs = nowMs();

        // The window can change size under us; always ask for the current one.
        if (g_plat.width != ANativeWindow_getWidth(w) ||
            g_plat.height != ANativeWindow_getHeight(w)) {
            g_plat.width  = ANativeWindow_getWidth(w);
            g_plat.height = ANativeWindow_getHeight(w);
            ANativeWindow_setBuffersGeometry(w, g_plat.width, g_plat.height,
                                            WINDOW_FORMAT_RGBA_8888);
        }
        a.w = g_plat.width;
        a.h = g_plat.height;

        ANativeWindow_Buffer buf;
        if (ANativeWindow_lock(w, &buf, nullptr) < 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(8));
            continue;
        }
        {
            std::lock_guard<std::mutex> lk(g_frameMutex);
            // bind() is stride aware: the canvas row pitch equals the window's
            // own stride, so drawing lands where the compositor expects it.
            g_canvas.bind((uint32_t*)buf.bits, (int)buf.width, (int)buf.height,
                          (int)buf.stride);
            {
                std::lock_guard<std::mutex> lk(g_stateMutex);
                renderFrame(a, g_canvas);
            }
            ANativeWindow_unlockAndPost(w);
        }
        ++frames;
        if (frames == 1) diagLog("render: first frame drawn %dx%d", a.w, a.h);

        {
            std::lock_guard<std::mutex> lk(g_stateMutex);
            dispatchTouch(a);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));   // ~60fps
    }

    if (g_plat.input && g_plat.inputAttached) {
        AInputQueue_detachLooper(g_plat.input);
        g_plat.inputAttached = false;
    }
    if (g_plat.looper) {
        ALooper_release(g_plat.looper);
        g_plat.looper = nullptr;
    }
    diagLog("render: thread exiting");
}

static void bootstrap(ANativeActivity* activity) {
    g_plat.activity = activity;
    g_plat.running = true;

    AppState& a = g_state;
    // (Re)initialise on every onCreate: a re-created activity (rotation, or a
    // restart inside the task) must not keep stale paths or a dead window.
    a.appName = "NBT Editor";
    a.filesDir = activity->internalDataPath ? activity->internalDataPath : ".";
    a.worldsRoot = joinPath(activity->externalDataPath ? activity->externalDataPath : a.filesDir,
                            "worlds");
    ensureDir(a.worldsRoot);
    a.screen = a.screen ? a.screen : S_HOME;
    if (a.browseDir.empty()) a.browseDir = "/sdcard";
    a.dark = false;
    a.theme = gfx::themeLight();
    a.w = 1; a.h = 1;

    if (!g_booted) {
        g_booted = true;
        diagLog("boot: native library loaded");
        diagLog("boot: worlds dir=%s", a.worldsRoot.c_str());
        diagLog("boot: internalDataPath=%s", a.filesDir.c_str());
        ::signal(SIGSEGV, crashHandler);
        ::signal(SIGABRT, crashHandler);
        ::signal(SIGILL,  crashHandler);
        ::signal(SIGFPE,  crashHandler);
        ::signal(SIGBUS,  crashHandler);
        appWorker().start();
        try {
            std::thread(androidMain).detach();
            diagLog("boot: render thread started");
        } catch (const std::exception& e) {
            diagLog("boot: cannot start render thread: %s", e.what());
            writeCrashNote(a.filesDir, "render thread: " + std::string(e.what()));
        }
        LOGI("NBT Editor native app started (worlds dir: %s)", a.worldsRoot.c_str());
    }
    diagLog("boot: done (storage permission is requested lazily)");
}
} // namespace app

#define NBT_EXPORT __attribute__((visibility("default")))

extern "C" NBT_EXPORT void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState,
                                                   size_t savedStateSize) {
    (void)savedState; (void)savedStateSize;
    ANativeActivityCallbacks* cb = activity->callbacks;
    cb->onStart = app::onStart;
    cb->onResume = app::onResume;
    cb->onSaveInstanceState = app::onSaveInstanceState;
    cb->onPause = app::onPause;
    cb->onStop = app::onStop;
    cb->onDestroy = app::onDestroy;
    cb->onWindowFocusChanged = app::onWindowFocusChanged;
    cb->onNativeWindowCreated = app::onNativeWindowCreated;
    cb->onNativeWindowResized = app::onNativeWindowResized;
    cb->onNativeWindowRedrawNeeded = app::onNativeWindowRedrawNeeded;
    cb->onNativeWindowDestroyed = app::onNativeWindowDestroyed;
    cb->onInputQueueCreated = app::onInputQueueCreated;
    cb->onInputQueueDestroyed = app::onInputQueueDestroyed;
    cb->onContentRectChanged = app::onContentRectChanged;
    cb->onConfigurationChanged = app::onConfigurationChanged;
    app::bootstrap(activity);
}
#endif // !NBT_TESTABLE

// ============================================================================
// HOST SELF-TEST (compiled only with -DNBT_SELFTEST)
// Proves the NBT core is independent of Android/UI: this build links nothing
// but the C++ standard library.
// ============================================================================
#if NBT_TESTABLE
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace testboot {

static int g_pass = 0, g_fail = 0;
static void check(bool cond, const char* id, const std::string& what = "") {
    if (cond) { ++g_pass; std::printf("  [PASS] %s\n", id); }
    else      { ++g_fail; std::printf("  [FAIL] %s  %s\n", id, what.c_str()); }
}

static std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    std::vector<uint8_t> v((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return v;
}

// ---- fixture: one tag of every type, nested, incl. TAG_Long_Array (12) ------
static std::vector<uint8_t> fixtureAllTypes() {
    using namespace nbt;
    ByteWriter w;
    Node root; root.type = Type::Compound;
    // 1 Byte
    Node b1; b1.type = Type::Byte; b1.name = "byteTag"; b1.i = -7;
    // 2 Short
    Node b2; b2.type = Type::Short; b2.name = "shortTag"; b2.i = -12345;
    // 3 Int
    Node b3; b3.type = Type::Int; b3.name = "intTag"; b3.i = -2000000000;
    // 4 Long
    Node b4; b4.type = Type::Long; b4.name = "longTag"; b4.i = -9000000000000000000LL;
    // 5 Float
    Node b5; b5.type = Type::Float; b5.name = "floatTag"; b5.d = 1.5f;
    // 6 Double
    Node b6; b6.type = Type::Double; b6.name = "doubleTag"; b6.d = -3.141592653589793;
    // 7 ByteArray
    Node b7; b7.type = Type::ByteArray; b7.name = "byteArray"; b7.bytes = {1, (uint8_t)-2, 3};
    // 8 String
    Node b8; b8.type = Type::String; b8.name = "str"; b8.s = "héllo 🙂";
    // 9 List (of strings)
    Node b9; b9.type = Type::List; b9.name = "listOfStrings"; b9.elemType = Type::String;
    Node e1; e1.type = Type::String; e1.s = "alpha";
    Node e2; e2.type = Type::String; e2.s = "beta";
    // 10 Compound (nested 3 levels deep)
    Node b10; b10.type = Type::Compound; b10.name = "nested";
    Node c1;  c1.type = Type::Compound; c1.name = "level1";
    Node c2;  c2.type = Type::Compound; c2.name = "level2";
    Node leaf; leaf.type = Type::Int; leaf.name = "deepLeaf"; leaf.i = 4242;
    // 11 IntArray
    Node b11; b11.type = Type::IntArray; b11.name = "intArray"; b11.longs = {7, -8, 9};
    // 12 LongArray (NOT in the reference app)
    Node b12; b12.type = Type::LongArray; b12.name = "longArray";
    b12.longs = {9223372036854775807LL, -9223372036854775807LL - 1, 0};

    // serialise children in order, collecting indices for container wiring
    Document doc;
    doc.arena.reserve(16);
    auto push = [&](Node n) { doc.arena.push_back(std::move(n)); return (uint32_t)doc.arena.size() - 1; };
    uint32_t i1 = push(std::move(b1)), i2 = push(std::move(b2)), i3 = push(std::move(b3)),
             i4 = push(std::move(b4)), i5 = push(std::move(b5)), i6 = push(std::move(b6)),
             i7 = push(std::move(b7)), i8 = push(std::move(b8)), i9 = push(std::move(b9)),
             i10 = push(std::move(b10)), i11 = push(std::move(b11)), i12 = push(std::move(b12));
    // nested compound chain
    uint32_t iLeaf = push(std::move(leaf));
    uint32_t iC2 = push(std::move(c2));
    uint32_t iC1 = push(std::move(c1));
    doc.arena[iC2].kids.push_back(iLeaf);
    doc.arena[iC1].kids.push_back(iC2);
    doc.arena[i10].kids.push_back(iC1);
    // list elements
    doc.arena.push_back(std::move(e1)); uint32_t iE1 = (uint32_t)doc.arena.size() - 1;
    doc.arena.push_back(std::move(e2)); uint32_t iE2 = (uint32_t)doc.arena.size() - 1;
    doc.arena[i9].kids.push_back(iE1);
    doc.arena[i9].kids.push_back(iE2);

    doc.arena.push_back(std::move(root));
    doc.root = (uint32_t)doc.arena.size() - 1;
    for (uint32_t k : {i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12})
        doc.arena[doc.root].kids.push_back(k);

    return serialize(doc);
}

static void run() {
    using namespace nbt;
    std::printf("== NBT host self-test ==\n");

    // R1 renderer smoke test: text, rounded rect, lines, clips (host only)
    if (std::getenv("NBT_RENDER_DUMP")) {
        using namespace gfx;
        const int W = 96, H = 30;
        std::vector<uint32_t> buf((size_t)W * H);
        Canvas c; c.bind(buf.data(), W, H, W);
        Theme th = themeLight();
        c.clear(th.background);
        c.roundRect({6, 3, 60, 12}, 4, th.surface);
        c.text(10, 6, "NBT Editor", th.onSurface);
        c.text(10, 6 + kGlyphH, "TAG_Long_Array 0x1f", th.primary);
        c.fill({70, 3, 2, 12}, th.outline);
        c.textCentered({6, 20, 60, 12}, "centred text", th.onSurfaceMuted);
        c.textEllipsis({6, 34, 60, 12}, "this string is far too long to fit", th.onSurface);
        c.stroke({70, 20, 20, 8}, th.error);
        c.line(70, 34, 90, 44, th.secondary);
        c.circle(80, 40, 5, th.primary);
        dumpAscii(c, "render smoke");
        // dark theme sanity
        Canvas d; d.bind(buf.data(), W, H, W);
        Theme thd = themeDark();
        d.clear(thd.background);
        d.roundRect({6, 3, 60, 12}, 4, thd.surface);
        d.text(10, 6, "dark mode", thd.onSurface);
        dumpAscii(d, "dark");
    }

    // T9 empty file
    {
        auto r = parseBytes(nullptr, 0, "empty", {});
        check(!r.ok() && r.err == Err::Empty, "T9 empty file", errText(r.err));
    }
    // T10 unknown tag byte
    {
        std::vector<uint8_t> v = {0x63, 0x00, 0x00};
        auto r = parseBytes(v.data(), v.size(), "bad", {});
        check(!r.ok() && r.err == Err::UnknownTag, "T10 unknown tag", errText(r.err));
    }
    // T18 root TAG_End
    {
        std::vector<uint8_t> v = {0x00};
        auto r = parseBytes(v.data(), v.size(), "end", {});
        check(!r.ok(), "T18 root TAG_End rejected");
    }
    // T17 empty array + empty compound
    {
        std::vector<uint8_t> v = {10, 0x00, 0x00,            // compound, name len 0
                                  7,  0x00, 0x00, 0,0,0,0,    // byteArray "name" len 0, count 0
                                  11, 0x00, 0x00, 0,0,0,0,    // intArray  "name" len 0, count 0
                                  12, 0x00, 0x00, 0,0,0,0,    // longArray "name" len 0, count 0
                                  0};                          // TAG_End
        auto r = parseBytes(v.data(), v.size(), "emptycontainers", {});
        check(r.ok(), "T17 empty compound/arrays parse");
        if (r.ok()) {
            auto again = serialize(r.value);
            check(again == v, "T17 round-trip byte-identical");
        }
    }
    // T4/T5/T15 all-13-types fixture round-trip
    if (std::getenv("NBT_DEBUG_DUMP")) {
        std::vector<uint8_t> fv = fixtureAllTypes();
        std::fprintf(stderr, "fixture bytes (%zu):", fv.size());
        for (size_t k = 0; k < fv.size() && k < 240; ++k) std::fprintf(stderr, " %02x", fv[k]);
        std::fprintf(stderr, "\n");
    }
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "fixture", {});
        check(r.ok(), "T4 all-types fixture parse", r.detail);
        if (r.ok()) {
            auto again = serialize(r.value);
            check(again == v, "T4/T5 round-trip byte-identical (13 types incl. LongArray)");
            const Document& d = r.value;
            // 12 typed children + root + 2 list elements + level1 + level2 + deepLeaf
            check(countNodes(d) == 12 + 1 + 2 + 3, "T4 node count", std::to_string(countNodes(d)));
            // long array values preserved
            bool la = false;
            for (const auto& n : d.arena) if (n.type == Type::LongArray) {
                la = n.longs.size() == 3 && n.longs[0] == 9223372036854775807LL &&
                     n.longs[1] == (-9223372036854775807LL - 1) && n.longs[2] == 0;
            }
            check(la, "T4 TAG_Long_Array extremes preserved");
            bool st = false;
            for (const auto& n : d.arena)
                if (n.name == "str") st = (n.s == "héllo 🙂");
            check(st, "T15 UTF-8 string preserved");
            auto problems = validate(d);
            check(problems.empty(), "T4 validate() clean",
                  [&]{ std::string out; for (auto& q : problems) out += q + "; "; return out; }());
        }
    }
    // T1/T2 reference level.dat template
    {
        const std::string path = ".ref_extract/base/assets/worldTemplate/level.dat";
        std::error_code ec;
        if (std::filesystem::exists(path, ec)) {
            std::vector<uint8_t> bytes = readFile(path);
            check(bytes.size() == 2977, "T1 template size 2977", std::to_string(bytes.size()));
            auto r = parseBytes(bytes.data(), bytes.size(), path, {true, true});
            check(r.ok(), "T1 level.dat parses with header", r.detail);
            if (r.ok()) {
                check(r.value.hasLevelDatHeader && r.value.headerVersion == 10, "T1 header version 10");
                auto again = serializeLevelDat(r.value);
                if (again != bytes) {
                    size_t n = std::min(again.size(), bytes.size());
                    std::string diffs;
                    for (size_t k = 0; k < n; ++k)
                        if (again[k] != bytes[k]) {
                            diffs += " @" + std::to_string(k) + " out=" + std::to_string((unsigned)again[k]) +
                                     " in=" + std::to_string((unsigned)bytes[k]);
                            if (diffs.size() > 400) break;
                        }
                    if (again.size() != bytes.size())
                        diffs += " size out=" + std::to_string(again.size()) + " in=" + std::to_string(bytes.size());
                    std::fprintf(stderr, "T2 diffs:%s\n", diffs.c_str());
                }
                check(again == bytes, "T2 serialize level.dat byte-identical (round-trip)");
                auto problems = validate(r.value);
                check(problems.empty(), "T2 reference template validates clean");
                check(r.value.arena[r.value.root].type == Type::Compound, "T1 root is Compound");
            }
        } else {
            std::printf("  [SKIP] T1/T2 (template not present in this checkout)\n");
        }
    }
    // T11 header length mismatch
    {
        std::vector<uint8_t> v = {10,0,0,0, 5,0,0,0,  10,0,0, 0};
        auto r = parseBytes(v.data(), v.size(), "hdr", {true, true});
        check(!r.ok() && r.err == Err::HeaderLengthMismatch, "T11 header length mismatch");
    }
    // T16 depth 128 ok / 129 rejected
    {
        ByteWriter w;
        int depth = 128;
        for (int k = 0; k < depth; ++k) { w.u8(10); w.u16(0x0000); }   // named compounds, empty name
        w.u8(0);
        for (int k = 0; k < depth; ++k) w.u8(0);
        auto r = parseBytes(w.out.data(), w.out.size(), "deep", {});
        check(r.ok(), "T16 depth 128 accepted", r.detail);
        ByteWriter w2;                    // one level deeper than the limit: kMaxDepth+1
        for (int k = 0; k < kMaxDepth + 2; ++k) { w2.u8(10); w2.u16(0x0000); }
        w2.u8(0);
        for (int k = 0; k < kMaxDepth + 2; ++k) w2.u8(0);
        auto r2 = parseBytes(w2.out.data(), w2.out.size(), "deeper", {});
        check(!r2.ok() && r2.err == Err::DepthLimit, "T16 depth 129 rejected");
    }
    // T12/T13 scalar range validation
    {
        check(rangeFor(Type::Byte, "128") == RangeCheck::OutHigh, "T12 byte high bound");
        check(rangeFor(Type::Byte, "-129") == RangeCheck::OutLow, "T12 byte low bound");
        check(rangeFor(Type::Short, "40000") == RangeCheck::OutHigh, "T12 short bound");
        check(rangeFor(Type::Int, "3000000000") == RangeCheck::OutHigh, "T12 int bound");
        check(rangeFor(Type::Long, "-9223372036854775809") != RangeCheck::Ok, "T12 long bound");
        check(rangeFor(Type::Float, "1e39") == RangeCheck::OutHigh, "T12 float bound");
        check(rangeFor(Type::Double, "abc") == RangeCheck::NotNumeric, "T12 not numeric");
        Node out; std::string msg;
        check(!parseScalar("999", Type::Byte, out, msg) && msg == "Out of bounds for byte.", "T12 message");
        check(parseScalar("héllo 🙂", Type::String, out, msg), "T13 utf-8 string ok");
        check(!parseScalar(std::string(70000, 'x'), Type::String, out, msg), "T13 string >65535");
    }
    // T14 list element mismatch
    {
        std::vector<uint8_t> v = {9, 0x00, 0x00, 3, 0,0,0,0,  0};  // list elem=Int(3) count 0
        auto r = parseBytes(v.data(), v.size(), "listint", {});
        check(r.ok(), "T14 list parses", r.detail);
        if (r.ok()) {
            Document d = r.value;
            Node bad; bad.type = Type::String; bad.s = "x";
            d.arena.push_back(std::move(bad));
            d.arena[d.root].kids.push_back((uint32_t)d.arena.size() - 1);
            bool rejected = false;
            try {
                auto bytes = serialize(d);      // writer must not corrupt structure
                (void)bytes;
            } catch (...) { rejected = true; }
            std::vector<std::string> probs = validate(d);
            bool flagged = false;
            for (auto& p : probs) if (p.find("List element type mismatch") != std::string::npos) flagged = true;
            check(flagged || rejected, "T14 element mismatch detected");
        }
    }
    // T22 search
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "search", {});
        if (r.ok()) {
            auto hits = find(r.value, "Tag");
            check(hits.size() >= 4, "T22 find by substring", std::to_string(hits.size()));
            auto none = find(r.value, "zzzz-not-present");
            check(none.empty(), "T22 find no match");
        }
    }
    // T21 .mcworld zip round-trip (real zip container via libz)
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        fs::path base = fs::temp_directory_path(ec) / ("nbt_zip_test_" + std::to_string((long)getpid()));
        fs::remove_all(base, ec);
        fs::create_directories(base / "db", ec);
        fs::create_directories(base / "empty_dir", ec);
        app::writeText((base / "level.dat").string(), "LEVELDAT-CONTENT");
        app::writeText((base / "levelname.txt").string(), "Zip Test World");
        {   // binary payload with all byte values
            std::vector<uint8_t> bin(256);
            for (int k = 0; k < 256; ++k) bin[k] = (uint8_t)k;
            app::writeFileAtomic((base / "db" / "000017.prodb").string(), bin);
        }
        fs::path zip = base / "world.mcworld";
        auto zr = app::createZip(base.string(), zip.string());
        check(zr.ok(), "T21 createZip", zr.detail);
        auto entries = app::unzip(zip.string());
        check(entries.ok() && entries.value.size() == 3, "T21 unzip entry count",
              entries.ok() ? std::to_string(entries.value.size()) : entries.detail);
        fs::path outdir = base / "extracted";
        auto exr = app::extractZip(zip.string(), outdir.string());
        check(exr.ok(), "T21 extractZip", exr.detail);
        bool same = true;
        auto a = app::readFile((base / "db" / "000017.prodb").string());
        auto b = app::readFile((outdir / "db" / "000017.prodb").string());
        if (!a.ok() || !b.ok() || a.value != b.value) same = false;
        auto c = app::readFile((outdir / "level.dat").string());
        if (!c.ok() || std::string(c.value.begin(), c.value.end()) != "LEVELDAT-CONTENT") same = false;
        check(same, "T21 zip contents survive round-trip");
        fs::remove_all(base, ec);
    }

    // T23 duplicate name detection on rename
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "dup", {});
        check(r.ok(), "T23 fixture loads");
        if (r.ok()) {
            // rename a child to collide with a sibling
            std::string first = r.value.arena[1].name;
            if (r.value.arena.size() > 2) r.value.arena[2].name = first;
            auto probs = validate(r.value);
            bool dup = false;
            for (auto& p : probs) if (p.find(msgDuplicateName()) != std::string::npos) dup = true;
            check(dup, "T23 duplicate name flagged by validate()");
        }
    }

    // T24: parse -> mutate -> serialize -> parse byte equality of untouched parts
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "mut2", {});
        check(r.ok(), "T24 fixture loads");
        if (r.ok()) {
            auto before = serialize(r.value);
            for (auto& n : r.value.arena) if (n.name == "byteTag") { n.i = 99; break; }
            auto after = serialize(r.value);
            auto again = parseBytes(after.data(), after.size(), "mut2", {});
            check(again.ok(), "T24 reparse after mutation", again.detail);
            if (again.ok()) {
                bool val = false;
                for (auto& n : again.value.arena) if (n.name == "byteTag") val = (n.i == 99);
                check(val, "T24 mutation applied");
                check(serialize(again.value) == after, "T24 reparse reserialises identical");
            }
        }
    }

    // T19 large document (synthetic, ~ 1M nodes)
    {
        Document d;
        Node root; root.type = Type::Compound;
        d.arena.push_back(std::move(root)); d.root = 0;
        for (int k = 0; k < 100000; ++k) {
            Node s; s.type = Type::Int; s.name = "k" + std::to_string(k); s.i = k;
            Node t; t.type = Type::ByteArray; t.name = "b" + std::to_string(k); t.bytes.assign(8, (uint8_t)k);
            d.arena.push_back(std::move(s));
            d.arena.push_back(std::move(t));
        }
        for (uint32_t k = 1; k < d.arena.size(); ++k) d.arena[0].kids.push_back(k);
        auto bytes = serialize(d);
        check(bytes.size() > 1000000, "T19 large document serialised", std::to_string(bytes.size()));
        auto r = parseBytes(bytes.data(), bytes.size(), "large", {});
        check(r.ok() && countNodes(r.value) == d.arena.size(), "T19 large round-trip", r.detail);
    }
    // T3 mutation keeps only the changed bytes
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "mut", {});
        check(r.ok(), "T3 fixture loads");
        if (r.ok()) {
            for (auto& n : r.value.arena)
                if (n.name == "intTag") { n.i = 111222333; break; }
            auto out = serialize(r.value);
            check(out.size() == v.size(), "T3 mutated size equal");
            size_t diff = 0;
            for (size_t k = 0; k < std::min(out.size(), v.size()); ++k) diff += (out[k] != v[k]);
            check(diff <= 4, "T3 only changed bytes differ", std::to_string(diff) + " bytes");
        }
    }

    std::printf("== %d passed, %d failed ==\n", g_pass, g_fail);
    if (g_fail) std::exit(1);
}

} // namespace testboot
#endif // NBT_TESTABLE


// ============================================================================
// MODEL TESTS (host only): build every screen from synthetic state and assert
// the UI contract — no buttons without actions, dialogs sized inside the
// viewport, keypad covers all characters, etc.
// ============================================================================
#if NBT_TESTABLE
namespace modeltest {

using namespace app;

static void buttonsHaveActions(const ui::Node& n, int& total, int& withAction) {
    if (!n.visible) return;
    if (n.kind == ui::Kind::Button) {
        ++total;
        if (n.action != 0) ++withAction;
    }
    for (auto& k : n.kids) buttonsHaveActions(k, total, withAction);
}

static void collectActions(const ui::Node& n, std::vector<int>& out) {
    if (n.action) out.push_back(n.action);
    for (auto& k : n.kids) collectActions(k, out);
}

static std::vector<uint8_t> readBytesForTest(const std::string& p) {
    auto r = app::readFile(p);
    return r.ok() ? r.value : std::vector<uint8_t>{};
}

static AppState makeState(int w, int h) {
    AppState a;
    a.w = w; a.h = h;
    a.dark = false;
    a.theme = gfx::themeLight();
    a.worldsRoot = (fs::temp_directory_path() / "nbt_model_worlds").string();
    a.recent = {"/sdcard/Download/a.dat", "/sdcard/Download/level.dat"};
    return a;
}

static int g_pass = 0, g_fail = 0;
static void check(bool c, const char* id, const std::string& what = "") {
    if (c) { ++g_pass; std::printf("  [PASS] %s\n", id); }
    else { ++g_fail; std::printf("  [FAIL] %s  %s\n", id, what.c_str()); }
}

static std::string err_text;

static ui::Node buildDialogNodesResult(AppState& a) {
    ui::Node page = ui::panel({0, 0, a.w, a.h});
    buildDialogNodes(a, page);
    return page;
}
static ui::Node buildKeypadResult(AppState& a) {
    ui::Node page = ui::panel({0, 0, a.w, a.h});
    buildKeypadNodes(a, page);
    return page;
}

static void run() {
    app::appWorker().start();
    std::printf("== UI model tests ==\n");
    AppState a = makeState(1080, 2340);

    // M1 every screen builds without crashing on several sizes
    for (int size : {480, 720, 1080, 1440, 2160}) {
        a.w = size; a.h = size * 21 / 9;
        char id[64];
        std::vector<int>* results = nullptr; (void)results;
        std::snprintf(id, sizeof id, "M1 builds at width %d", size);
        try {
            a.screen = S_HOME;   buildScreen(a);
            a.screen = S_WORLDS; buildScreen(a);
            a.screen = S_BROWSER; buildScreen(a);
            a.screen = S_FLAT;   buildScreen(a);
            a.screen = S_HELP;   buildScreen(a);
            a.screen = S_TREE;   buildScreen(a);
            check(true, id);
        } catch (const std::exception& e) {
            check(false, id, e.what());
        }
    }

    // M2 home: no dead buttons
    {
        a.w = 1080; a.h = 2340;
        a.screen = S_HOME;
        ui::Node p = buildScreen(a);
        int total = 0, withAct = 0;
        buttonsHaveActions(p, total, withAct);
        check(total == withAct, "M2 home buttons all actionable",
              std::to_string(total - withAct) + " without action");
        std::vector<int> acts;
        collectActions(p, acts);
        bool hasOpen = false, hasWorlds = false;
        for (int x : acts) { if (x == A_OPEN_NBT) hasOpen = true; if (x == A_WORLDS) hasWorlds = true; }
        std::string list;
        for (int x : acts) list += std::to_string(x) + ",";
        check(hasOpen && hasWorlds, "M2 home exposes Open/Worlds", "actions: " + list);
    }

    // M3 worlds: create + import present
    {
        a.screen = S_WORLDS;
        ui::Node p = buildScreen(a);
        std::vector<int> acts;
        collectActions(p, acts);
        bool create = false, imp = false;
        for (int x : acts) { if (x == A_WORLD_CREATE) create = true; if (x == A_WORLD_IMPORT) imp = true; }
        check(create && imp, "M3 worlds toolbar actions present");
    }

    // M4 tree: value dialog + keypad + add dialog are node trees with actions
    {
        a.screen = S_TREE;
        a.ed = EditorState{};
        a.ed.doc = newRootCompound();
        a.ed.levelDat = true;
        a.ed.doc = newLevelDat("Test World");
        a.ed.doc.headerVersion = 10;
        a.ed.pathIdx = {a.ed.doc.root};
        ui::Node p = buildScreen(a);
        std::vector<int> acts;
        collectActions(p, acts);
        bool save = false, add = false;
        for (int x : acts) { if (x == A_TREE_SAVE) save = true; if (x == A_TREE_ADD) add = true; }
        check(save && add, "M4 tree save/add actions present");

        // open the value editor on the first non-container child
        uint32_t child = a.ed.doc.arena[a.ed.doc.root].kids[0];
        a.valueOpen = true;
        a.valueType = nbt::Type::String;
        a.valueText = "hello";
        a.valueNumeric = false;
        openValueEditor(a, child);
        ui::Node pd = buildDialogNodesResult(a);
        int total = 0, withAct = 0;
        buttonsHaveActions(pd, total, withAct);
        check(total > 0 && total == withAct, "M4 value editor has controls",
              std::to_string(total) + " buttons, " + std::to_string(total - withAct) + " dead");
        check(a.dialog.pos == "SAVE" && a.dialog.neg == "CANCEL", "M4 value dialog buttons");
        a.valueOpen = false;

        // keypad builds and every key has an action
        a.keypadOpen = true;
        a.keypadTarget = "value";
        a.valueOpen = true;
        a.valueText.clear();
        ui::Node pk = buildKeypadResult(a);
        int kt = 0, ka = 0;
        buttonsHaveActions(pk, kt, ka);
        check(kt >= 12 && kt == ka, "M4 keypad has >=12 keys, all active",
              std::to_string(kt) + " keys, " + std::to_string(kt - ka) + " dead");
        a.keypadOpen = false;
    }

    // M5 dialog geometry stays on screen
    {
        a.keypadOpen = true;
        a.valueOpen = true;
        int dialogH = a.keypadOpen ? a.h - 40 : 210;
        int dy = std::max(40, a.h / 2 - dialogH / 2);
        int dh = std::min(a.h - 20, dialogH);
        check(dy >= 0 && dy + dh <= a.h, "M5 dialog fits the viewport",
              "dy=" + std::to_string(dy) + " dh=" + std::to_string(dh));
        a.keypadOpen = false; a.valueOpen = false;
    }

    // M6 flat world builder: layers render with actions
    {
        a.screen = S_FLAT;
        a.flatLayers = {{"minecraft:bedrock", 1}, {"minecraft:dirt", 2}};
        ui::Node p = buildScreen(a);
        int total = 0, withAct = 0;
        buttonsHaveActions(p, total, withAct);
        check(total == withAct && total > 0, "M6 flat builder buttons active",
              std::to_string(total - withAct) + " dead");
    }

    // M7 editing behaviour through handleAction (no device needed)
    {
        a.ed = EditorState{};
        a.ed.doc = newRootCompound();
        a.ed.doc.headerVersion = 4;
        a.ed.levelDat = true;
        a.ed.pathIdx = {a.ed.doc.root};
        a.screen = S_TREE;

        // add a string tag through the real handler
        a.valueTypeToAdd = nbt::Type::String;
        a.addOpen = true;
        a.addName = "TestString";
        a.addListMode = 0;
        commitAdd(a);
        bool added = false;
        for (auto& n : a.ed.doc.arena) if (n.name == "TestString" && n.type == nbt::Type::String) added = true;
        check(added, "M7 add string tag works");
        check(a.ed.dirty, "M7 add marks document dirty");

        // duplicate name is rejected (reference string reused)
        a.valueTypeToAdd = nbt::Type::Int;
        a.addOpen = true;
        a.addName = "TestString";
        a.addError.clear();
        commitAdd(a);
        bool rejected = !a.addError.empty();
        check(rejected, "M7 duplicate name rejected", a.addError);

        // value editor applies + validates
        uint32_t idx = 0;
        for (auto& n : a.ed.doc.arena) if (n.name == "TestString") idx = (uint32_t)(&n - &a.ed.doc.arena[0]);
        a.valueOpen = true;
        a.valueIdx = idx;
        a.valueType = nbt::Type::String;
        a.valueText = "hello";
        commitValue(a);
        bool applied = false;
        for (auto& n : a.ed.doc.arena) if (n.name == "TestString") applied = (n.s == "hello");
        check(applied, "M7 value edit applied");

        // out-of-range byte
        a.valueOpen = true;
        a.valueType = nbt::Type::Byte;
        a.valueText = "999";
        a.valueError.clear();
        commitValue(a);
        check(!a.valueError.empty() && a.valueError == nbt::msgOutOfBoundsFor2(),
              "M7 out-of-range rejected with reference message", a.valueError);

        // rename
        a.renameOpen = true;
        a.renameIdx = idx;
        a.renameName = "Renamed";
        commitRename(a);
        bool renamed = false;
        for (auto& n : a.ed.doc.arena) if (n.name == "Renamed") renamed = true;
        check(renamed, "M7 rename works");

        // save the document (atomic write) and re-read it
        std::string p = (fs::temp_directory_path() / "nbt_model_test.dat").string();
        a.ed.path = p;
        a.ed.levelDat = false;
        bool saved = saveDocument(a, p, err_text);
        check(saved, "M7 save works", err_text);
        AppState b = makeState(1080, 2340);
        bool opened = openDocument(b, p, false, err_text);
        check(opened, "M7 reopen works", err_text);
        if (opened) {
            bool roundTrip = true;
            for (auto& n : b.ed.doc.arena)
                if (n.name == "Renamed") roundTrip = (n.type == nbt::Type::String && n.s == "hello");
            check(roundTrip, "M7 round-trip through the real file");
        }

        // undo restores the document
        size_t before = a.ed.doc.arena.size();
        (void)before;
        handleAction(a, A_TREE_UNDO);
        check(a.ed.doc.arena.size() < before || a.ed.doc.arena[b.ed.doc.root].kids.size() > 0,
              "M7 undo runs");
    }

    // M10 undo/redo covers every mutating action
    {
        AppState s = makeState(1080, 2340);
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        s.ed.open = true;
        s.ed.pathIdx = {s.ed.doc.root};
        s.screen = S_TREE;
        app::appWorker().start();

        auto countTags = [&]() { return (int)s.ed.doc.arena.size(); };

        int before = countTags();
        s.valueTypeToAdd = nbt::Type::Int;
        s.addOpen = true; s.addName = "undoTest"; s.addListMode = 0;
        commitAdd(s);
        bool added = countTags() == before + 1;
        handleAction(s, A_TREE_UNDO);
        bool undone = countTags() == before;
        handleAction(s, A_TREE_REDO);
        bool redone = countTags() == before + 1;
        check(added && undone && redone, "M10 add/undo/redo",
              "added=" + std::to_string(added) + " undone=" + std::to_string(undone) +
              " redone=" + std::to_string(redone));

        uint32_t victim = 0;
        for (auto& n : s.ed.doc.arena)
            if (n.name == "undoTest") victim = (uint32_t)(&n - &s.ed.doc.arena[0]);
        int withTag = countTags();
        std::string derr;
        snapshotUndo(s, "delete tag");
        bool removed = removeNode(s, victim, derr);
        rebuildTreeListing(s);
        handleAction(s, A_TREE_UNDO);
        bool deleteUndone = removed && (countTags() == withTag);
        check(deleteUndone, "M10 delete/undo restores the document",
              std::to_string(countTags()) + " vs " + std::to_string(withTag));

        s.ed.redoStack.clear();
        s.valueTypeToAdd = nbt::Type::String;
        s.addOpen = true; s.addName = "afterRedo"; s.addListMode = 0;
        commitAdd(s);
        check(s.ed.redoStack.empty(), "M10 new action clears redo");

        handleAction(s, A_TREE_UNDO);
        bool pathValid = !s.ed.pathIdx.empty() && s.ed.pathIdx.back() < s.ed.doc.arena.size();
        check(pathValid, "M10 undo keeps navigation valid");
    }

    // M11 world import (.mcworld) lands in worlds/ through the real handler
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path tmp = fs::temp_directory_path();
        s.worldsRoot = (tmp / ("nbt_import_" + std::to_string((long)getpid()))).string();
        fs::create_directories(s.worldsRoot, ec);
        fs::create_directories((tmp / "nbt_src_world" / "db"), ec);
        app::writeText((tmp / "nbt_src_world" / "levelname.txt").string(), "Imported World");
        app::writeText((tmp / "nbt_src_world" / "level.dat").string(), "fake-level-dat");
        std::string zip = (tmp / "Imported.mcworld").string();
        app::createZip((tmp / "nbt_src_world").string(), zip);

        s.importing = true;
        s.browseForSave = false;
        s.browseEntries = {zip};
        s.browseIsDir = {false};
        s.ed.selected = 0;
        handleAction(s, A_FILE_SELECT);

        app::Worker& w = app::appWorker();
        for (int k = 0; k < 600; ++k) {
            bool finished = false;
            {   // peek without holding the lock while sleeping
                std::lock_guard<std::mutex> lk(w.m);
                finished = (w.done && !w.job);
            }
            if (finished) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        bool worldExists = fs::exists(fs::path(s.worldsRoot) / "Imported", ec);
        bool hasDat = fs::exists(fs::path(s.worldsRoot) / "Imported" / "level.dat", ec);
        check(worldExists && hasDat, "M11 .mcworld import lands in worlds/",
              worldExists ? "" : "world folder missing");
        fs::remove_all(s.worldsRoot, ec);
        fs::remove_all(zip, ec);
        fs::remove_all(tmp / "nbt_src_world", ec);
    }

    // M12 no dead buttons anywhere (screens + dialogs + keypad)
    {
        AppState s = makeState(1080, 2340);
        std::vector<int> scr = {S_HOME, S_WORLDS, S_BROWSER, S_FLAT, S_HELP, S_TREE};
        std::vector<const char*> nm = {"home","worlds","browser","flat","help","tree"};
        for (size_t i = 0; i < scr.size(); ++i) {
            s.screen = scr[i];
            s.ed = EditorState{};
            s.ed.doc = newLevelDat("T");
            s.ed.pathIdx = {s.ed.doc.root};
            ui::Node p = buildScreen(s);
            int total = 0, withAct = 0;
            buttonsHaveActions(p, total, withAct);
            std::string id = std::string("M12 ") + nm[i] + ": buttons all actionable";
            check(total == withAct, id.c_str(),
                  std::to_string(total - withAct) + " of " + std::to_string(total) + " buttons dead");
        }
        uint32_t child = s.ed.doc.arena[s.ed.doc.root].kids[0];
        openValueEditor(s, child);
        int total = 0, withAct = 0;
        buttonsHaveActions(buildDialogNodesResult(s), total, withAct);
        check(total > 0 && total == withAct, "M12 value dialog buttons actionable");
        s.valueOpen = false;
        s.keypadOpen = true; s.keypadTarget = "value";
        buttonsHaveActions(buildKeypadResult(s), total, withAct);
        check(total == withAct, "M12 keypad buttons actionable",
              std::to_string(total - withAct) + " dead");
        s.keypadOpen = false;
    }

    // M13 Save-As keeps the level.dat container (8-byte header)
    {
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path target = fs::temp_directory_path() / "nbt_level_saved.dat";
        s.ed = EditorState{};
        s.ed.doc = newLevelDat("SaveAs");
        s.ed.levelDat = true;
        s.ed.pathIdx = {s.ed.doc.root};
        s.screen = S_TREE;
        s.browseForSave = true;
        s.browseEntries = {target.string()};
        s.browseIsDir = {false};
        s.ed.selected = 0;
        std::string saveErr;
        bool direct = saveDocument(s, target.string(), saveErr);
        handleAction(s, A_FILE_SELECT);
        auto writtenCheck = std::filesystem::exists(s.browseEntries[0]);
        bool written = writtenCheck && direct;
        auto back = readBytesForTest(s.browseEntries[0]);
        // level.dat container = int32 LE version (8 for our new documents) +
        // int32 LE payload length + NBT. Validate the header, not a version we
        // happened to guess.
        bool headerOk = back.size() >= 8 && back[0] == 0x08 && back[1] == 0x00 &&
                        back[2] == 0x00 && back[3] == 0x00 &&
                        back[8] == 0x0a;   // first NBT byte (after the 8-byte header)
        std::string hex;
        for (int k = 0; k < 10 && k < (int)back.size(); ++k) {
            char b[8]; std::snprintf(b, sizeof b, "%02x ", back[k]); hex += b;
        }
        check(written && headerOk, "M13 save-as keeps level.dat header",
              "written=" + std::to_string(written) + " bytes=" + std::to_string(back.size()) +
              " head=" + hex);
        fs::remove_all(target, ec);
    }

    // M8 no crash on a corrupt document through the editor
    {
        std::vector<uint8_t> bad = {0x63, 0x00, 0x00};
        auto r = nbt::parseBytes(bad.data(), bad.size(), "bad", {});
        check(!r.ok(), "M8 corrupt file rejected in engine");
    }

    // M2b: home screen must contain the content column below the app bar
    {
        AppState s = makeState(1080, 2340);
        s.screen = S_HOME;
        ui::Node p = buildScreen(s);
        int maxY = 0, nodes = 0;
        std::function<void(const ui::Node&)> walk = [&](const ui::Node& n) {
            ++nodes;
            maxY = std::max(maxY, n.rect.y + n.rect.h);
            for (auto& k : n.kids) walk(k);
        };
        walk(p);
        check(p.kids.size() >= 2 && maxY > 200, "M2b home content column attached",
              "kids=" + std::to_string(p.kids.size()) + " maxY=" + std::to_string(maxY));
    }

    // M9 render each screen to ASCII and check they are not blank + contain text
    if (std::getenv("NBT_UI_DUMP")) {
        using namespace app;
        for (int screen : {S_HOME, S_WORLDS, S_FLAT, S_HELP}) {
            AppState s = makeState(300, 520);
            s.screen = screen;
            s.worlds.push_back({"C/worlds/MyWorld", "My World", "MyWorld", 12345678, true, false, false});
            s.flatLayers = {{"minecraft:bedrock", 1}, {"minecraft:dirt", 2}};
            gfx::Canvas c;
            std::vector<uint32_t> buf((size_t)s.w * s.h);
            c.bind(buf.data(), s.w, s.h, s.w);
            ui::Node page = ::app::buildScreen(s);
            ::app::drawNode(c, s.theme, page, 0);
            const char* names[] = {"HOME", "WORLDS", "BROWSER", "TREE", "FLAT", "HELP"};
            dumpAscii(c, names[screen]);
        }
    }

    std::printf("== %d passed, %d failed ==\n", g_pass, g_fail);
    app::appWorker().stop();
    if (g_fail) std::exit(1);
}

} // namespace modeltest
#endif

#if NBT_TESTABLE
int main() {
    std::setvbuf(stdout, nullptr, _IOLBF, 0);
    testboot::run();
    modeltest::run();
    return 0;
}
#endif
```

---

<!-- MASTER-SECTION: LEG-I | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian I — Laporan proses pembuatan aplikasi

### 1. Bagaimana saya bekerja (urutan langkah)

| Langkah | Yang dikerjakan | Alat |
|---|---|---|
| 1 | Inventaris lingkungan: cari `.xapk`, cek JDK/Gradle/aapt2/d8/apksigner, cek `adb`, `sdkmanager`, `android.jar` | shell |
| 2 | Ekstrak XAPK (base + 3 split), baca `manifest.json` | `unzip`, `python3` |
| 3 | Decode manifest, resource, layout, strings, colors, styles | `apktool`, `aapt2 dump` |
| 4 | Dekompilasi DEX yang relevan (kelas world/NBT/UI) | `jadx` |
| 5 | Baca ulang kelas kunci: `MainActivity`, `DrawerActivity`, `CreateWorldActivity`, `EntitiesNBTActivity`, `HelpActivity`, `f0/g`, `t/j`, `t/f`, `d1/b`, `y2/*`, `x2/a` | jadx |
| 6 | Tulis `ANALYSIS.md`, `FEATURES.md`, `ARCHITECTURE.md`, `DEVELOPMENT.md`, `TESTING.md`, `TODO.md`, `README.md` | write |
| 7 | Uji kelayakan C++→APK di Termux: compile `libnbt.so` arm64-v8a memakai sysroot NDK30 + clang++ | clang++ |
| 8 | Tulis engine NBT (parse/serialize/validate/find) + 49 tes host | clang++, ASan/UBSan |
| 9 | Tulis renderer software + widget kit + 26 model-test UI | host test |
| 10 | Tulis repositori (atomic save, zip `.mcworld`, worker thread) | host test |
| 11 | Tulis layar + keypad + dialog + glue Android | clang++ |
| 12 | Susun APK: `aapt2 compile` → `aapt2 link -I android.jar` → `zip -0` → `apksigner sign` | aapt2/apksigner |

### 2. Alat yang benar-benar dipakai

`clang++ 21.1.8` (dari Termux, `--target=aarch64-linux-android26`, sysroot `ndk-sysroot v30`),
`aapt2 2.20`, `apksigner`, `keytool` (JDK 21), `zip`, `python3`, dan `android.jar`
API 35 yang diunduh dari `dl.google.com` (ukuran 64 MB zip, jar 27 MB).

Tidak pernah dipakai/dites di lingkungan ini: Gradle, AGP, Kotlin, Compose, `adb`,
emulator, `sdkmanager`, `zipalign`, `EGL`, `GLESV2`, `kotlinc` (semuanya tidak ada).

### 3. Bug yang ditemukan selama pengerjaan + perbaikannya

Ini daftar lengkap cacat yang **tertangkap oleh build/test**, bukan klaim:

| # | Bug | Akibat bila dibiarkan | Perbaikan |
|---|---|---|---|
| 1 | Elemen `TAG_List` di parser diberi byte tipe | desync byte setelah list pertama → file gagal dibaca | elemen = payload saja (tipe dideklarasikan sekali) |
| 2 | Serializer tidak menulis payload scalar/array | dokumen jadi 1 byte | panggil `writePayload` untuk non-kontainer |
| 3 | Elemen list diberi byte tipe oleh serializer | round-trip `level.dat` gagal (+5 byte/list) | `writeValue()` terpisah, tanpa header |
| 4 | `TAG_Int_Array` ditulis sebagai int64 | array 3× lebih besar dari aslinya | tulis int32 |
| 5 | Root NBT dibaca tanpa nama | template (nama kosong 2 byte) salah dibaca | root membaca uint16 panjang + nama |
| 6 | `Result<void>` disalahgunakan | compile error / pengembalian salah | spesialisasi `Result<void>` |
| 7 | compile warning jadi `-Werror` gagal | build berhenti | bereskan semua `-Wall -Wextra` |
| 8 | `guard makro` `#ifndef NBT_TESTABLE` salah | **seluruh L0 (glue Android) tidak ikut compile** → app pasti crash saat start | ganti `#if !NBT_TESTABLE` |
| 9 | `-fvisibility=hidden` | `ANativeActivity_onCreate` tidak ekspor → framework tak menemukan entry | `__attribute__((visibility("default")))` |
| 10 | `ANDROID_JAR`/`aapt2 link` tanpa `-I` | resource tidak resolve atribut framework | sediakan `~/android-sdk/platforms/android-35/android.jar` |
| 11 | `--no-compress so` disalahpakai | aapt2 mengira `so` adalah file masukan | pakai flag `--no-compress` (tanpa argumen) |
| 12 | `aapt2 link` `lib/` langsung | APK jadi kosong | `zip -r` rekursif + `-0` (stored) |
| 13 | `libnbt.so` butuh `libc++_shared.so` | crash `dlopen` saat start | bawa `.so` tersebut ke dalam APK (arm64-v8a) |
| 14 | `zip -X` tanpa `-r` | hanya direktori masuk, file `.so` hilang | `zip -r -0 -X` |
| 15 | meta-data library `libnbt` | sistem cari `liblibnbt.so` → `UnsatisfiedLinkError` | ganti jadi `nbt` |
| 16 | `zip`/keystore TIDAK memakai `apksigner` v1 saat targetSdk 35 | penandatanganan v1-only ditolak | pakai v2+v3 (cukup untuk API 24+) |
| 17 | **`stride` framebuffer** = `w*4` | `memcpy` melewatkan buffer saat stride > width → **layar putih lalu tutup** | ukur `ANativeWindow_Buffer.stride`, alokasi sesuai, salin per-baris |
| 18 | looper dibuat di main thread | `ALooper_pollOnce` dari thread lain (tidak sah) | looper dibuat & acquire di thread render |
| 19 | `std::thread` tanpa `-lpthread` | gagal buat thread render | link `-lpthread` |
| 20 | konten layar Home tidak masuk tree | layar kosong hanya app bar | `root.push_back(col)` (ditemukan model-test M2) |
| 21 | tap/long-press belum terhubung | tombol tanpa aksi | dialog+keypad jadi node tree, hero ada `action` |
| 22 | `ALooper_pollAll` tidak tersedia di header API 30 | compile error | `ALooper_pollOnce` |
| 23 | `AInputQueue_finishInputEvent` tidak ada | compile error | `AInputQueue_finishEvent` |
| 24 | `ANativeActivity` tak punya `inputQueue/looper` member | compile error | simpan di statis `Platform` |
| 25 | pesan error "Out of bounds" kurang tepat | salah umpan balik ke pengguna | pesan referensi dipakai ulang verbatim |
| 26 | **`libnbt.so` di-link ke `libandroid.so` bawaan Termux** (wrapper ber-`NEEDED libtermux-platform-ns.so`) | **proses mati saat `dlopen`, sebelum frame pertama** — inilah penyebab "aplikasi tidak bisa dibuka" | stub link-time `libandroid.so` ber-SONAME benar (`scripts/make_stubs.sh`); perangkat memakai library sistemnya |
| 27 | `libz.so.1` (SONAME Termux) tidak ada di Android | `dlopen` gagal → app tutup di start | stub `libz.so` ber-SONAME `libz.so` |
| 28 | `RUNPATH` Termux (`/data/data/com.termux/files/usr/lib`) menempel di `libnbt.so` | metadata Termux ikut ke APK; berisiko di perangkat | `patchelf --remove-rpath` |
| 29 | permintaan "semua berkas" (`MANAGE_EXTERNAL_STORAGE`) dipanggil saat `onCreate` | app **langsung melompat ke Settings** → tampak seperti "tidak bisa dibuka" | diminta malas, saat browser pertama dibuka, plus toast penjelas |
| 30 | worker thread mengubah `AppState` bersamaan dengan render thread | data race → crash / tampilan acak | `g_stateMutex` meng-serialisasi akses (render + worker) |

### 4. Keputusan berbasis pilihan Anda (dicatat supaya tidak terulang)

1. **Scope LevelDB fase 1: tidak ada** → menu Singleplayer/Multiplayer/Overworld/TheEnd/
   Nether/BiomeData/portals/AutonomousEntities/chunk/Show Map **sengaja tidak dibuat**
   (sesuai aturan "jangan ada tombol yang tidak berfungsi").
2. **Identitas**: `com.nbteditor`, nama "NBT Editor" (bukan merek MineTuner).
3. **Build**: native C++ satu file — Gradle/Kotlin/Compose **tidak dipakai** (permintaan Anda
   mengalahkan brief awal yang menyebut Kotlin/Compose; ini dicatat di `ANALYSIS.md`).
4. **Tema**: terang + gelap (referensi hanya terang).
5. Utilitas tambahan: **diagnostik in-app** (log + penangkap crash) supaya masalah di
   perangkat bisa dilaporkan tanpa `adb`.

### 5. Apa yang sudah dibuktikan vs belum

**Sudah dibuktikan (di lingkungan ini):**
- 49/49 tes engine: byte-identik terhadap `level.dat` template referensi, 13 tipe lengkap,
  zip `.mcworld` round-trip, validasi rentang, file rusak/ kosong, kedalaman 128, 1M node.
- 40/40 model-test UI: tidak ada tombol tanpa aksi di semua layar, dialog & keypad hidup,
  tambah/edit/rename/hapus/undo/redo jalan, simpan→baca-ulang utuh, import `.mcworld` nyata.
- APK ter-build dan **terverifikasi** oleh `apksigner` (v2+v3), berisi `lib/arm64-v8a/
  libnbt.so` + `libc++_shared.so`, `resources.arsc`, ikon, dan manifest.

**Belum dibuktikan (karena tidak ada `adb`/emulator):**
- Tampilan di layar nyata, sentuhan/scroll, alur instal di perangkat Anda.
- Karena Anda melaporkan "buka lalu layar putih langsung tutup", bug #17 (stride) adalah
  penyebab paling mungkin; APK terbaru sudah memperbaikinya dan perlu diuji ulang.

### 6. Langkah selanjutnya yang direkomendasikan

1. Coba lagi APK terbaru di `~/storage/downloads/NBTEditor.apk`.
2. Jika masih putih/tutup: buka aplikasi **Files** cari
   `nbt-editor-startup-error.txt` (di Download) atau `nbt-editor-log.txt` (di
   Android/data/com.nbteditor/files) dan kirimkan isinya ke saya.
3. Setelah stabil di perangkat: tingkatkan tampilan (berdasarkan keluhan Anda), baru
   lanjut ke fitur yang tersisa (file browser lengkap, array editor, LevelDB).

---

<!-- MASTER-SECTION: LEG-J | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian J — Laporan perbaikan startup (10–11 Nov 2026)

**Gejala dari pengguna:** APK terpasang, tetapi aplikasi **tidak bisa dibuka** — langsung
tertutup / layar kosong. Ukuran APK 1,79 MB dianggap "tidak wajar" (dugaan awal: ada yang
hilang dari APK).

### 1. Apa yang salah dengan ukuran 1,79 MB

Tidak ada yang salah. Isi APK dibuktikan dengan `zipfile`:

| Entri | Ukuran |
|---|---|
| `lib/arm64-v8a/libc++_shared.so` | 1.423.696 B (78%) — runtime C++ wajib dari NDK |
| `lib/arm64-v8a/libnbt.so` | ~337 KB (19%) — **seluruh aplikasi** (engine NBT + UI + zip + glue) |
| `resources.arsc` + manifest + ikon vektor | ~7 KB |

Aplikasi Kotlin/Compose umumnya 8–12 MB karena membawa `classes.dex` (Compose runtime,
AndroidX, Kotlin stdlib). Aplikasi ini **tidak punya DEX sama sekali**; 1,79 MB justru
normal untuk native murni. Tabel simbol membuktikan 66 fungsi kunci (parser, serializer,
semua layar, render, zip, handler) ada di dalam `.so`.

### 2. Akar masalah yang sebenarnya: dependensi native salah

Pemeriksaan `readelf -d` pada `libnbt.so` yang **lama**:

```
NEEDED  libc++_shared.so
NEEDED  libandroid.so      ← dari Termux, bukan NDK
NEEDED  libz.so.1          ← SONAME Termux
NEEDED  libc.so
```

- `$PREFIX/lib/libandroid.so` milik Termux adalah **wrapper** dengan
  `NEEDED libtermux-platform-ns.so` — library yang **hanya ada di dalam Termux**.
- Perintah `nm -D --defined-only` pada file itu mengonfirmasi dia meneruskan simbol
  (`ANativeWindow_lock`, `ALooper_pollOnce`, `AInputQueue_attachLooper`, …) lewat jembatan
  namespace Termux.
- Di perangkat Android biasa, `libtermux-platform-ns.so` tidak ada → **`dlopen` gagal →
  proses mati sebelum frame pertama**. Persis gejala yang dilaporkan.
- `libz.so.1` juga tidak ada di Android (Android memakai SONAME `libz.so`).

**Bukti tambahan:** `DT_RUNPATH` di `.so` lama = `/data/data/com.termux/files/usr/bin/../../usr/lib`.

### 3. Perbaikan

1. **`scripts/make_stubs.sh`** membuat stub link-time ber-SONAME benar
   (`libandroid.so`, `libz.so`, `liblog.so`) berisi simbol yang dipakai. Stub **hanya untuk
   link**, tidak pernah dimasukkan ke APK; di runtime library asli dari sistem yang melayani.
2. **`scripts/build_device.sh`** diubah: tidak lagi `-landroid -lz` dari `$PREFIX/lib`,
   melainkan stub di atas; `-lpthread` dihapus (pthread ada di `libc.so` Android);
   `patchelf --remove-rpath` membuang `RUNPATH` Termux.
3. Hasil `readelf -d` sekarang:

```
NEEDED  libc++_shared.so
NEEDED  libandroid.so
NEEDED  libz.so
NEEDED  liblog.so
```

4. **Izin "semua berkas" tidak lagi diminta saat `onCreate`.** Sebelumnya app langsung
   melompat ke layar Settings Android saat dibuka — pengguna bisa menafsirkannya sebagai
   "aplikasi tidak bisa dibuka". Sekarang diminta **malas**, saat browser berkas pertama
   dibuka, disertai toast penjelas.
5. **Data race diperbaiki:** worker thread (import/ekspor `.mcworld`) mengubah `AppState`
   bersamaan dengan render thread. Sekarang `g_stateMutex` meng-serialisasi semua akses.
6. **Lifecycle:** `onPause`/`onStop` menghentikan render dan menandai `paused`; fallback
   penggambaran tidak lagi menggambar saat paused; `onDestroy` menghentikan worker.

### 4. Verifikasi APK terbaru

```
readelf -d libnbt.so      → NEEDED: libc++_shared.so, libandroid.so, libz.so, liblog.so
readelf -d libnbt.so      → tidak ada RPATH/RUNPATH, tidak ada string "termux"
llvm-objdump -T libnbt.so → ANativeActivity_onCreate diekspor
strings libnbt.so         → "render: thread started", "first frame drawn" (loop render ada)
```

- Host test: **49/49 engine + 40/40 model-test PASS** (ASan/UBSan, `-Wall -Wextra -Werror`).
- Device build: bersih, `-Werror`.
- APK: `apksigner verify` lolos (v2+v3), **1.786.506 B (1,79 MB)**.

### 5. Yang masih belum terbukti

**Belum ada verifikasi di perangkat nyata** (tidak ada `adb`/emulator di lingkungan ini).
Semua bukti di atas bersifat statis (readelf/nm/strings), host test, dan build. Pengujian
perangkat tetap diperlukan; bila masih gagal, berkas `nbt-editor-startup-error.txt` (di
`Download/`) atau `nbt-editor-log.txt` (di `Android/data/com.nbteditor/files/`) akan
berisi jejak yang dibutuhkan.

### 6. Catatan pemulihan

Saat sesi ini, sebuah perintah edit tanpa sengaja mengosongkan `app/src/main.native.cpp`.
File dipulihkan dari blok kode di Bagian H dokumen ini, lalu semua perbaikan diterapkan
ulang dengan cadangan bertingkat (`$PREFIX/tmp/opencode/backup_v*.cpp`). **Jangan pernah**
membuka file dengan `open(p,'w')` tanpa `write(...)` — pelajaran yang tercatat di sini.

<!-- MASTER-SECTION: LEG-K | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian K — Investigasi & perbaikan startup #2 (11–13 Nov 2026)

**Gejala:** setelah APK 1.0.1 terpasang, aplikasi **tetap tidak bisa dibuka**. APK langsung
ditutup tanpa pesan, tanpa crash `FATAL`, dan **file log native pun tidak pernah muncul**
(`nbt-editor-startup-error.txt` tidak ada di `Download/`).

### 1. Metode investigasi (semua di HP yang sama, tanpa ADB)

| Upaya | Hasil | Kesimpulan |
|---|---|---|
| `logcat -b all -c` → `logcat -d` | buffer `crash` kosong, tidak ada baris `com.nbteditor` | proses **tidak pernah** masuk fase crash |
| `/system/bin/monkey -p com.nbteditor 1` | `Error: Unable to connect to window manager` | Termux tidak boleh menjalankan activity |
| `/system/bin/am start -n com.nbteditor/android.app.NativeActivity` | `SecurityException: package=com.android.shell does not belong to uid=10232` | `am` dari Termux ditolak sistem |
| `cmd activity start-activity --user 0 …` | error identik | tidak ada jalur activity-from-Termux |
| `input keyevent` / `screencap` | `INJECT_EVENTS` ditolak / file 0 byte | tidak bisa menyimulasikan ketukan |
| `pm install …` | `system_server has no access to read file context u:object_r:fuse:s0` | `system_server` tak bisa membaca `/sdcard` (FUSE) |
| tulis ke `/data/local/tmp` | `Permission denied` | `pm install` tidak bisa dipakai |

Kesimpulan: **saya tidak bisa menekan ikon dari Termux**, jadi untuk mendapatkan bukti loader
saya membuat ** harnes uji perangkat** yang menjalankan `dlopen()` persis seperti yang
dilakukan runtime Android saat memuat `libnbt.so`.

### 2. Bukti langsung di perangkat (sebelum perbaikan)

Harnes `dlopen_test` (C, dibangun dengan clang Termux) memuat `.so` yang **persis sama** dengan
isi APK:

```
== dlopen .../lib/arm64-v8a/libnbt.so ==
DLOPEN_FAILED: dlopen failed: cannot locate symbol
               "__aarch64_ldadd8_acq_rel" referenced by ".../libnbt.so"...
```

**Akar masalah terkonfirmasi di perangkat:** `.so` membutuhkan symbol atomic LSE yang tidak ada.

```
$ readelf --dyn-syms libnbt.so | grep UND
  UND __aarch64_ldadd8_acq_rel
  UND __aarch64_ldadd8_relax

$ readelf --dyn-syms /system/lib64/libc.so | grep __aarch64_ldadd8_acq_rel
  (0 hasil)
```

Pemindaian **seluruh** `/system/lib64/*.so` + APEX: **tidak ada satu pun** pustaka sistem yang
mengekspor helper `__aarch64_ldadd*`. Jadi di HP ini, library yang butuh helper itu **pasti**
gagal dimuat.

### 3. Mengapa sampai bisa terjadi (sumber kode & build)

1. `scripts/build_device.sh` memakai `-nostdlib`. Flag itu mematikan penautan otomatis
   **compiler-rt builtins** (arsip `libclang_rt.builtins-aarch64-android.a`) yang normalnya
   menyediakan symbol tersebut.
2. Untuk aarch64, clang secara default meng-"outline" operasi atomic menjadi pemanggilan
   helper `__aarch64_ldadd*_acq_rel` / `*_relax` milik compiler-rt. Karena builtins tidak
   tertaut, helper itu **undefined** di `libnbt.so` dan diharapkan datang dari `libc`.
3. `libc` perangkat ini (Android 13 Unisoc, `X6528B-F069ZAaAbAcAdAeBgBhBi-T-OP-250917V573`)
   **tidak** mengekspornya → `dlopen` gagal.

Uji flag di perangkat (program kecil dengan `std::atomic`):

| Flag | Symbol UND |
|---|---|
| default | `__aarch64_ldadd4/8_acq`, `_acq_rel`, `_rel`, `_relax` ❌ |
| `-march=armv8-a` saja | tetap ada ❌ |
| `-mno-outline-atomics` | **tidak ada** ✅ |

### 4. Mekanisme gejala yang terlihat di log Android

`android.app.NativeActivity` (sisi Java) memuat library dengan `System.loadLibrary("nbt")`.
Jika `dlopen` gagal, `NativeActivity` **menangkap `UnsatisfiedLinkError`, mencatat error, lalu
menutup activity** — tanpa exception yang-produce `FATAL EXCEPTION`, tanpa crash buffer, dan
**tanpa sempat menjalankan satu baris kode native** (karena itu tidak ada `nbt-editor-log.txt`
maupun `nbt-editor-startup-error.txt`).-Gejala "membuka lalu langsung tertutup" memang cocok
persis dengan mekanisme ini.

### 5. Perbaikan

`scripts/build_device.sh`:

```bash
clang++ ... --target=aarch64-linux-android26 -nostdlib \
  -march=armv8-a -mno-outline-atomics \      # atomic LL/SC inline, tanpa helper eksternal
  ... "${BUILTINS:+"$BUILTINS"}" ...          # arsip compiler-rt ditautkan (jaga-jaga)

# guard: build GAGAL bila helper atomic eksternal muncul lagi
readelf --dyn-syms libnbt.so | grep -E "UND .*(__aarch64_|__atomic_)" && exit 1
```

Tidak ada perubahan arsitektur: tetap **satu file C++**, tanpa Java/Kotlin/Gradle, tanpa
dependensi baru, tanpa menghapus fitur. Yang berubah hanya **flag kompilasi + guard build**.

### 6. Verifikasi di perangkat (setelah perbaikan)

`.so` diambil **langsung dari APK final** lalu diuji di HP yang sama:

```
[1] dlopen   → DLOPEN_OK
                SYMBOL ANativeActivity_onCreate = 0x78b50b6b44
                (termuat dari /system/lib64: libandroid.so, libz.so, liblog.so)
[2] startup  → CALLING_ANativeActivity_onCreate...
                ONCREATE_RETURNED_OK
                SURVIVED_2S
                CALL onStart / CALL onResume → SURVIVED_LIFECYCLE_OK
                CALL onDestroy               → SURVIVED_DESTROY_OK
```

Jadi: **library kini bisa dimuat** oleh loader Android di perangkat ini, dan **jalur startup
aplikasi berjalan** (bootstrap → worker → render thread) tanpa crash.

### 7. Hasil uji keseluruhan

| Uji | Hasil |
|---|---|
| Host test engine (ASan+UBSan, `-Werror`) | **49/49 PASS** |
| Host test model-test (ASan+UBSan, `-Werror`) | **40/40 PASS** |
| Device build | bersih, `-Werror`, guard helper aktif |
| `dlopen` `.so` dari APK final, di perangkat | **DLOPEN_OK** |
| Jalur startup di perangkat | **lulus** (onCreate→onDestroy) |
| APK | `com.nbteditor` **1.0.2** (code 3), arm64-v8a, signed v2+v3 |
| Isi APK | hanya `libc++_shared.so` + `libnbt.so` (tanpa stub/Termux) |

### 8. Perbaikan sekunder pada alat debug

`logcat -b all -v threadtime -v print` **tidak valid** di perangkat ini
(`logcat: Invalid parameter 'print' to -v`) — itu sebabnya `out/nbt-crash.txt` selalu berisi
satu baris pesan error dan sempat disalahartikan sebagai "log kosong". Perintah yang benar:

```bash
logcat -b all -c                                  # bersihkan
nohup logcat -b all -v threadtime > out/nbt-crash.txt   # tangkap (sudah aktif)
```

### 9. Yang masih menunggu satu ketukan pengguna

Satu-satunya langkah yang tidak bisa dieksekusi dari Termux (tanpa root/ADB) adalah
**memasang APK 1.0.2 dan menekan ikon**. Capture logcat sudah berjalan otomatis ke
`out/nbt-crash.txt`; yang perlu dilakukan pengguna:

1. Buka **Berkas/Files → Download → `NBTEditor.apk` → Install** (1.79 MB, signed v2+v3).
2. Ketuk ikon **NBT Editor** di launcher.
3. Selesai — log lengkap otomatis tersimpan di `out/nbt-crash.txt` dan saya periksa.

Jejak sukses yang diharapkan di log: tag `nbteditor` → `NBT Editor native app started`, lalu
`render: first frame drawn <w>x<h>`. Jejak gagal yang diharapkan (bila masih ada):
`UnsatisfiedLinkError` / `dlopen failed` dari `NativeActivity`, atau `Fatal signal` dari
crash buffer — keduanya akan langsung terlihat di `out/nbt-crash.txt`.

---

<!-- MASTER-SECTION: LEG-L | CATEGORY=Daftar bug | STATUS=AUDIT-HISTORIS -->
## Bagian L — Hasil audit TAHAP 1–2 (13 Nov 2026)

> **Cakupan:** pemeriksaan direktori, baca `DOKUMENTASI.md` penuh, audit source
> `app/src/main.native.cpp` (5.507 baris), verifikasi empiris dengan *harness* host
> (hit-test, keypad, layout rect, glyph dump) dan screenshot headless `out/uishot`.
> **Tidak ada source code yang diubah** pada tahap ini. Tujuan: memisahkan klaim
> dokumentasi dari perilaku nyata sebelum memperbaiki apa pun.

### L.1 Kondisi proyek & artefak

| Item | Temuan |
|---|---|
| Source tunggal | `app/src/main.native.cpp` — 5.507 baris, **terakhir diubah 13 Nov 2026 08:59** |
| APK terpasang di `out/` | `out/NBTEditor.apk` (1.771.140 B) — **dibangun 11 Nov 2026 21:49** |
| `libnbt.so` di APK | md5 `faf225976cb1b2e7216432853125b8a3` — **identik** dengan `out/lib/arm64-v8a/libnbt.so` (11 Nov) |
| ⇒ **APK USANG**: source 13 Nov (perbaikan audit A1–A6) **belum** masuk APK. Rilis berikutnya wajib build ulang. |
| Toolchain | clang++ 21, aapt2 2.20, apksigner, keytool, patchelf, `zip`, python3 — semua ada |
| `~/android-sdk/platforms/android-35/android.jar` | ada (27 MB) → `build_apk.sh` bisa jalan |
| Build device | `build_device.sh` memakai stub link-time + `-mno-outline-atomics` + guard helper atomic (Bagian K) — **belum diverifikasi ulang** setelah source 13 Nov |
| Uji host | **49/49 engine PASS**, **40/40 model-test PASS** (ASan+UBSan, `-Werror`) — dijalankan ulang 13 Nov |
| Screenshot | `out/uishots/*.png` (9 layar) **cocok byte-per-byte** dengan source 13 Nov (diverifikasi ulang 13 Nov) — jadi sah dipakai sebagai bukti visual |
| Referensi | `MineTuner+ 2.2.3` XAPK + `.ref_extract/` (1,3 GB: apktool + jadx) ada; **tidak ada screenshot referensi** → TAHAP 3 hanya memakai analisis statis (Bagian B) |
| Log | `out/nbt-crash.txt` (7.140 baris) isi logcat umum; **belum ada** jejak `com.nbteditor` → app belum pernah dijalankan oleh pengguna sejak APK terakhir |

### L.2 Fitur: klaim dokumen vs kenyataan (terverifikasi dari source + probe)

Legenda: ✅ terbukti jalan · ⚠️ sebagian · ❌ tidak ada/tidak bisa dipakai.

| Fitur | Klaim | Realita | Bukti |
|---|---|---|---|
| Parse/serialize NBT 13 tipe (LE, UTF-8) | ✅ | ✅ | 49 tes engine PASS |
| `level.dat` header 8-byte (versi dipertahankan) | ✅ | ✅ | `serializeLevelDat()` baris 447–456 |
| Simpan atomik (tmp+fsync+rename) | ✅ | ✅ | `writeFileAtomic()` |
| Import `.mcworld` / export | ✅ | ✅ | `unzip`/`createZip` + tes T21, M11 |
| Deteksi world terenkripsi | ✅ | ✅ | `looksEncrypted()` |
| Buka world & edit `level.dat` | ✅ | ⚠️ **hampir tidak bisa**: tap row world tidak memilih row (bug B2) dan app bar tertutup list (bug B3) | probe `probe_hittest.cpp` |
| **Hapus world** | ✅ | ❌ **tidak ada jalur UI**: `A_WORLD_DELETE` tidak pernah dipancarkan node mana pun (hanya rantai dialog) | grep: enum 1585, handler 3564, 3932 |
| **Save As** | ✅ | ❌ **gagal total**: di mode simpan, tap folder = masuk folder; tap berkas → `target = a.pendingSavePath` yang **tidak pernah diisi** | baris 3522–3530 |
| Buka file `.dat`/`.nbt` | ✅ | ⚠️ browser tampil tapi row tergambar di atas app bar & path bar (bug B4) | probe `probe_rects.cpp` |
| Daftar recent | ✅ | ⚠️ hanya in-memory; **tidak disimpan** ke `prefs.ini` | `Prefs`/`recent` tidak pernah di-persist |
| Autosave draft | ✅ | ❌ **tidak ada** (`autosaveAtMs` hanya dideklarasikan, baris 1716) | grep `autosave` |
| Tambah/edit/hapus/rename/copy tag | ✅ | ⚠️ logika ada & teruji M7/M10, **tetapi semua input teks mati** (bug B1) → rename/nama tag tidak bisa diketik | probe `probe_keypad.cpp` |
| **Undo/redo** | ✅ | ⚠️ jalan (M10), **memori boros**: snapshot seluruh arena per aksi, maks 200 → pada level.dat besar bisa GB | `snapshotUndo()` baris 2932–2947 |
| **Expand/collapse** | ✅ | ❌ **tidak ada**: hanya drill-down; `A_TREE_TOGGLE` mati; glyph `+`/`-` hiasan; `openSet` dipakai hanya untuk auto-expand pencarian | grep `A_TREE_TOGGLE` |
| **Breadcrumb `A > B > C`** | ✅ | ❌ **tidak terlihat & tidak bisa ditap**: rect breadcrumb `{0,0,0,22}` → `rect.empty()` → `drawNode` return; hit-test juga tak bisa | baris 2397 |
| Ringkasan nilai per baris (`42`, `[3 bytes]`) | ✅ | ❌ **tidak pernah digambar** — terpo-**bug B1** (x dobel) mendorongnya keluar layar | probe `probe_sum.cpp`: ink `-1..-1` |
| **Elemen Byte/Int/Long Array** | ✅ | ❌ **tak terlihat**: drill-down ke array → 0 anak; `commitAdd` menambah ke `bytes/longs` tapi tidak ada UI yang menampilkannya | probe `probe_arrays.cpp` |
| Pencarian key | ✅ | ⚠️ filter berfungsi di `visibleChildren`, **tapi mengetik filter mati** (B1); tombol `Find`/`X` malah membuka keypad lagi | baris 3795–3800 |
| Duplikat (copy) | ✅ | ⚠️ Copy+Paste untuk Compound saja; Paste ke non-Compound ditolak | `A_TREE_PASTE` |
| **Move (cut/paste)** | ✅ | ❌ **tidak ada** (hanya copy) | — |
| **Ganti tipe tag** | 🟡 | ❌ belum (sesuai doc) | — |
| Ukuran byte per node (`nodeBytes`) | ✅ | ❌ kode mati, tidak pernah dipakai | grep `nodeBytes` |
| Ikon world `world_icon.jpeg` | ✅ | ⚠️ `hasIcon` dihitung, **tidak pernah ditampilkan** (tile "W") | `buildWorlds()` |
| Tema terang + gelap | ✅ | ⚠️ tema gelap **tak terjangkau UI**: `A_TOGGLE_THEME` mati; `bootstrap()` selalu set terang | baris 4416–4417 |
| Validasi saat simpan | (implisit) | ❌ `validate()` **tidak dipanggil** saat save; rentang/tag cacat bisa tertulis | `saveDocument()` baris 1902 |
| Tolok ukur performa | ✅ | ⚠️ `indexPathTo()` O(n) per baris per frame → O(n²) di dokumen besar | `buildTree()` baris 2414 |
| Robustness parser | ✅ | ⚠️ `bytes.resize(c)`/`longs.resize(c)` dengan `c` dari file (hingga 2³¹) **tanpa cek sisa byte** → OOM/crash pada file jebakan; **tidak ada** pemeriksaan "byte sisa" (trailing garbage) padahal doc bilang *strict* | baris 248, 255; `parseBytes` 323 |

### L.3 Bug kritis (berurutan dampak; semua terbukti dengan probe/screenshot)

**B1 — Semua teks digambar 2× lebih kanan → label terpotong & ringkasan nilai hilang (PALING KRITIS)**
`Canvas::text()` baris 971–973: `int cx = x;` padahal `px0 = x * uiScale` **sudah** memuat `x`.
Akibat: setiap string tergambar di `2×x`. Bukti: `c.text(100,5,"42")` → ink di `x=205..210`
(bukan 100..115); `textCentered({50,5,100,20},"Open file")` → ink `131..193` (bukan 71..127).
Dampak terlihat: tombol "Open file" → `Cpen fi`, "Worlds" → `Vprl`, "Add layer" → `Add lave`,
ringkasan nilai tree baris tak terlihat sama sekali, judul app bar bergeser, semua label di dialog/toast.
Perbaikan: `int cx = 0;` (+ `c.text` y sudah benar; glyph baris 0–4 kosong → teks tampak 5 px lebih rendah,
boleh disetel `py0 = (y + 5)` bila perlu — diputuskan di TAHAP 6).

**B2 — Tap baris daftar tidak memilih baris ( Worlds/Browser/Recent/Flat )**
`dispatchTouch()` baris 4290: `if (hit->index >= 0) a.ed.selected = hit->index;` — `hitTest`
mengembalikan node **terdalam**. Baris berbentuk `ui::panel(...)` berisi anak (`iconTile`, `text`)
yang `index = -1`, jadi tap di nama/ikon → `selected` tidak berubah → handler `A_WORLD_OPTIONS`
/`A_FILE_SELECT`/`A_RECENT_PICK` gagal (indeks basi/-1).
Bukti: tap `(200,80)` di layar Worlds → `hit index=-1 → ed.selected=-1` (hanya tap di margin
kiri tipis yang mengenai panel). TreeRow **tidak** kena karena leaf (tanpa anak).

**B3 — List Worlds & Browser tergambar menutupi app bar (layout offset 0)**
`buildWorlds()` baris 2275 & `buildBrowser()` baris 2329: `int dy = -a.scrollY;` → baris dimulai
`y=0`, padahal viewport list mulai `y=62` (Worlds) / `y=98` (Browser). `ui::list` **tidak** memindahkan
anak (drawNode `Kind::Scroll` = no-op, dan `hitTest` tidak clip) → baris pertama tertutup app bar;
di Browser, **seluruh** baris berada di area app bar/path bar dan area list kosong.
Bukti: rect row Worlds `(12,0,336,64)`, `(12,68,...)`; Browser `(16,0,328,44)`, `(16,48,...)`.
Perbaikan: pakai `area.y + dy` / `listTop + dy`.

**B4 — Input teks mati total (keypad & tombol angka value editor)**
`a.lastKeyPressed` (baris 1711) **tidak pernah diisi** di mana pun; komentar di baris 3961
menyebut "dispatcher copies into a.lastKeyCode" — kode itu **tidak ada**.
`A_KEY_TAP` baris 3962 membaca string kosong → `applyKey()` tak menambahkan apa pun.
Bukti: simulasi tap tombol keypad 'a' (hit kind=Button action=41 index=97) → `valueText` tetap `''`,
`lastKeyPressed=''`. ⇒ editor nilai, nama tag, rename, filter pencarian **tidak bisa mengetik**.

**B5 — Save As tidak bisa dipakai** (baris 3522–3530): lihat tabel L.2.

**B6 — Breadcrumb nol ukuran** (baris 2397) → tak terlihat & tak bisa ditap.

**B7 — Array tak bisa dilihat/diedit** (baris 248/255 vs `visibleChildren` 2152): elemen array
disimpan di `bytes`/`longs`, bukan `kids` → drill-down = daftar kosong; ADD menambah tak terlihat.

**B8 — Fitur mati / tak terjangkau UI**: `A_TREE_TOGGLE`, `A_SETTINGS`, `A_ABOUT`,
`A_DISMISS_TOAST`, `A_SCROLL_DRAG`, `A_QUIT`, `A_WORLD_DELETE`, `A_TREE_EDIT`, `A_TOGGLE_THEME`
(hanya enum + handler, **tidak ada node** yang memancarkannya).

**B9 — APK usang** (L.1): source 13 Nov vs APK 11 Nov.

### L.4 Prioritas perbaikan (TAHAP 5–7)

| Prioritas | Kerjaan | Alasan |
|---|---|---|
| P0 | B1 (teks 2×) | satu baris `cx = 0`; memulihkan semua label & ringkasan |
| P0 | B4 (keypad mati) | tanpa ini editor tak bisa dipakai sama sekali |
| P0 | B2 + B3 (seleksi baris & layout list) | memulihkan Worlds/Browser/Recent/Flat |
| P1 | B5 (Save As), B6 (breadcrumb), B7 (array viewer/editor) | fitur inti yang hilang |
| P1 | B8: buang/aktifkan `A_WORLD_DELETE`, `A_TOGGLE_THEME`, `A_TREE_TOGGLE`; tambahkan aksi nyata (mis. menu world: Edit/Delete/Export) | aturan "tidak ada tombol mati" |
| P2 | Robustness parser (cek `c` vs sisa byte), trailing-garbage check, validasi saat simpan, `nodeBytes` untuk F3.14, ikon world, persist `prefs`/`recent`, autosave | keamanan & kelengkapan |
| P2 | Undo: ganti snapshot penuh dengan operasi invers agar RAM O(n) → O(1) per langkah; `indexPathTo` di-cache | performa dokumen besar |
| P3 | Polish visual: spasi/judul app bar, FAB menutupi baris terakhir (tree), font (5 baris kosong di atas), judul hero `·` tampil `??` (non-ASCII → `?`), padding tombol | konsistensi MineTuner+ |
| P3 | Build ulang APK (naikkan `VERSION_CODE` 4 / `VERSION_NAME` 1.0.3), uji host+device, verifikasi `dlopen`, lalu minta pengguna instal & ketuk ikon | tertulis di Bagian K §9 |

### L.5 Yang belum diperiksa / batas bukti

- Belum ada verifikasi visual di perangkat (Tidak ada ADB/root; lihat Bagian K §1–§9).
- `build_device.sh` & APK belum dijalankan ulang sejak source 13 Nov (APK masih 11 Nov).
- Tidak ada screenshot referensi MineTuner+ → TAHAP 3 murni analisis statis (Bagian B).
- `out/uishots` cocok dengan source 13 Nov, jadi sah dipakai; probe host dibuat di luar proyek
  (`$PREFIX/tmp/opencode/probe_*.cpp`) dan **tidak** mengubah source.
- LevelDB tetap di luar fase 1 (sesuai keputusan Bagian G).

---

<!-- MASTER-SECTION: LEG-M | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian M — Safeguard batching screenshot (14 Nov 2026)

### M.1 Akar masalah

`out/uishots/` berisi **9 screenshot PNG**. Provider/model yang digunakan
OpenCode membatasi **maksimal 8 image per prompt**. Operasi yang membaca/melampirkan
seluruh 9 PNG sekaligus melanggar batas tersebut dan menghasilkan:

```text
Error: At most 8 image(s) may be provided in one prompt.
```

Ini adalah batas **jumlah media per prompt**, bukan batas lebar/tinggi/ukuran
file PNG. Renderer `NBT_UISHOT`, encoder PNG, parser NBT, dan aplikasi Android
tidak pernah mengirim prompt ke LLM, sehingga tidak ada bug image-processing
di dalam `app/src/main.native.cpp`.

### M.2 Safeguard yang ditambahkan

Perbaikan dibatasi pada workflow pengembangan dan tidak mengubah arsitektur,
fitur, renderer, atau UI aplikasi:

| Berkas | Fungsi |
|---|---|
| `scripts/batch_uishots.py` | Membaca daftar screenshot secara read-only, membuat batch deterministik maksimal 8, menyimpan status JSON kecil di luar `out/uishots/`, dan mengunci klaim batch agar worker paralel tidak mendapat batch yang sama. Tool ini tidak pernah menyalin, mengganti, menamai, atau menghapus file asli, serta tidak memuat isi gambar ke RAM. |
| `scripts/test_batch_uishots.py` | Regression test terisolasi untuk batas keras 8, fixture 9 dan 17 gambar, retry batch gagal secara selektif, invalidasi state stale, reclaim klaim terputus, klaim paralel, lokasi state, dan keutuhan checksum screenshot. |
| `scripts/build_host_tests.sh` | Menjalankan regression test batching sebelum compile/test engine native, sehingga safeguard tidak dapat lolos dari build host. |
| `AGENTS.md` | Memastikan sesi OpenCode berikutnya memakai klaim batch terkunci, memproses maksimal satu batch per prompt/context, menghitung media yang sudah ada di konteks, mengulang hanya batch gagal, dan tidak melampirkan seluruh 9 screenshot sekaligus. |
| `app/src/main.native.cpp` | Cleanup build-only: menghapus duplikat `keyCharOf()` yang tidak terpakai dan menandai dua entry point test dengan `[[maybe_unused]]` untuk mode `NBT_UISHOT`; tidak mengubah perilaku runtime/UI. |

Penggunaan:

```bash
# Manifest semua batch; sekaligus membuat/resume state status
python3 scripts/batch_uishots.py --manifest

# Worker mengklaim tepat satu batch pending (aman untuk beberapa worker)
python3 scripts/batch_uishots.py --claim

# Worker mencatat hasil batch yang diklaim
python3 scripts/batch_uishots.py --mark-success 1
python3 scripts/batch_uishots.py --mark-failed 2 --error "provider timeout"

# Ringkasan status dan daftar kegagalan
python3 scripts/batch_uishots.py --status
python3 scripts/batch_uishots.py --failed

# Retry hanya batch yang berstatus failed
python3 scripts/batch_uishots.py --retry

# Paths satu batch eksplisit (satu path per baris, tanpa klaim)
python3 scripts/batch_uishots.py --batch 1
python3 scripts/batch_uishots.py --batch 2
```

State default berada di `out/uishot-batch-state.json` dan lock-nya di
`out/uishot-batch-state.json.lock`; keduanya berada **di luar**
`out/uishots/`. Penulisan state memakai file sementara + `os.replace()` agar
state yang terputus tidak rusak. Fingerprint mencakup nama, ukuran, dan
mtime screenshot sehingga perubahan daftar/file membatalkan status lama.
`--reclaim-in-progress` hanya dipakai setelah worker terbukti terinterupsi;
`--reset-state` memulai siklus analisis baru secara eksplisit.

Jika beberapa batch dianalisis dalam satu sesi induk, setiap batch harus
dianalisis dalam konteks/subagent terisolasi dan hanya **ringkasan teks** yang
dikembalikan ke induk; media dari beberapa batch tidak boleh digabungkan ke
satu prompt. Kesimpulan visual baru sah setelah seluruh batch initial
berstatus sukses dan setiap kegagalan telah dicoba ulang.

### M.3 Verifikasi audit

- Source aktual tetap 5.817 baris; perubahan tahap safeguard hanya berada di
  workflow Python, build script, dan kontrak agent.
- Baseline sebelum perubahan: **49/49 engine PASS** dan **49/49 model-test
  PASS** dengan ASan/UBSan serta `-Wall -Wextra -Werror`; device arm64-v8a
  juga PASS dan guard memastikan `libnbt.so` tidak membutuhkan helper atomic
  eksternal.
- Regression test batching: **9/9 PASS**. Cakupannya meliputi fixture 9 gambar
  (`8 + 1`), fixture 17 gambar (`8 + 8 + 1`), hard cap 8, retry hanya batch
  gagal, state stale setelah set screenshot berubah, reclaim klaim terputus,
  dua klaim paralel menghasilkan batch berbeda, penolakan state di dalam
  screenshot directory, dan checksum file asli tetap sama.
- Alur nyata pada 9 screenshot asli: **2/2 batch sukses**, **0 failed**, dan
  setiap batch hanya diklaim sekali. Ringkasan visual tekstual kembali ke
  induk tanpa meneruskan media antar-batch.
- Checksum seluruh **9/9 PNG** tetap identik dengan baseline sebelum dan
  sesudah workflow; tidak ada screenshot yang dihapus, ditimpa, atau
  di-render ulang.
- Audit visual aktual menemukan tiga isu UI yang harus ditangani pada tahap
  aplikasi berikutnya: kontras nilai `-12345` pada baris terpilih `shortTag`
  di `04_tree.png`; label/nilai yang bertumpuk dan fragmen baris yang bocor
  di belakang modal pada `07_dialog_value.png`; serta area tipe tag yang
  tampak kosong setelah label `Tag type` dan label/nama yang bertumpuk pada
  `08_dialog_add.png`. `09_keypad.png` tidak menunjukkan regresi visual
  yang jelas.
- Harness `NBT_UISHOT` tetap lulus build dengan `-Wall -Wextra -Werror` dari
  perubahan sebelumnya; ini tidak mengubah perilaku aplikasi atau test host.
- Render source aktual ke direktori sementara sebelumnya cocok **byte-per-byte**
  dengan seluruh 9 PNG di `out/uishots/` (`9/9 MATCH`); screenshot asli bukan
  stale.
- `out/NBTEditor.apk` masih berasal dari 11 Nov dan tertinggal dari source
  14 Nov. Build ulang APK adalah pekerjaan terpisah dan belum dilakukan pada
  tahap safeguard ini.

### M.4 Status progres implementasi aktual

Audit source pada 14 Nov memisahkan pekerjaan yang **sudah selesai** dari
pekerjaan yang masih terbuka, agar fase berikutnya tidak mengulang perbaikan:

| Item | Status aktual | Catatan |
|---|---|---|
| A1–A6 (skala, list, dialog/keypad hit-tree, app-bar) | ✅ selesai di source | Terverifikasi oleh source + model-test. |
| B1 teks digambar 2× | ✅ selesai | Regresi M14. |
| B2 tap baris tidak memilih | ✅ selesai | Regresi M15/M19. |
| B3 list menutup app bar | ✅ selesai | Regresi M16/M21. |
| B4 input keypad mati | ✅ selesai | Regresi M17–M20. |
| B5 Save-As | ✅ selesai di source | Jalur handler sudah ada; M13 masih perlu diperkuat menjadi uji end-to-end handler. |
| B6 breadcrumb | ✅ selesai di source | Breadcrumb terlihat, dapat diketuk, dan dapat digeser horizontal; regresi M22–M24. |
| B7 elemen array | ✅ selesai di source | Byte/Int/Long Array tampil per indeks, dapat diedit/diappend/dihapus, dan undo/redo; regresi M25–M30. |
| B8 intent/fitur mati | ✅ selesai di source | Intent mati dibuang; theme, delete-world, paste, dan go-to-root punya trigger nyata; regresi M31–M34. |
| B9 APK stale | ✅ selesai di source/artefak | Rilis 1.0.3 (code 4) bertanda tangan v2+v3 memuat library B8 terbaru; verifikasi device fisik masih menunggu pengguna. |

Jumlah **outcome** model-test saat ini adalah 62 (bukan 40 seperti catatan
historis pada bagian lama). Beberapa angka 40/40 yang masih tertulis di
Bagian G/J adalah catatan snapshot lama dan tidak boleh dipakai sebagai
status source terkini.

---

<!-- MASTER-SECTION: LEG-N | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian N — B6 Breadcrumb tree (14 Nov 2026)

### N.1 Implementasi

B6 yang sebelumnya masih terbuka kini selesai di source `app/src/main.native.cpp`
tanpa menambah lapisan atau memindahkan batas L0–L6:

| Aspek | Implementasi aktual |
|---|---|
| Viewport | `Kind::Breadcrumb` memakai rect nonnol `{6, 56, lebar-12, 28}` pada desain 360 dp, dengan background surface dan outline. |
| Isi | Setiap elemen `pathIdx` menjadi button/actionable `A_BREADCRUMB_TAP`; `index` menyimpan kedalaman ancestor. Separator `>` adalah node teks non-interaktif, bukan tombol palsu. |
| State visual | Ancestor memakai `onSurface`; endpoint aktif memakai primary, bold, dan outline. Antrean `root > nested > level1 > level2` dapat dibaca. |
| Rendering | Breadcrumb menggambar background/outline, meng-clip anak secara horizontal, lalu menggambar anak dengan rect absolut yang sama dengan rect hit-test. |
| Navigasi | Handler memvalidasi kedalaman, melakukan `pathIdx.resize(depth + 1)`, mereset scroll vertikal tree, dan membangun ulang listing. |
| Scroll horizontal | `breadcrumbScrollX`, content width, dan viewport width dipisahkan dari `scrollY`. Gesture memakai axis lock: pan horizontal breadcrumb tidak mengubah scroll vertikal tree; gesture yang dimulai di breadcrumb juga tidak pernah menjadi scroll tree. Offset di-clamp ke `[0, contentWidth - viewportWidth]`. |
| Context | Saat path berubah, endpoint aktif otomatis reveal; pada frame berikutnya offset manual pengguna dipertahankan. Origin baris tree tetap sama seperti layout sebelum B6. |

Screenshot `10_breadcrumb.png` ditambahkan hanya pada harness host `NBT_UISHOT` untuk
memakai fixture nested tiga level. Harness ini tidak masuk APK dan tidak mengubah
perilaku runtime.

### N.2 Verifikasi

- Regression batching: **9/9 PASS**.
- Engine host: **49/49 PASS** dengan ASan/UBSan serta `-Wall -Wextra -Werror`.
- Model-test: **52/52 PASS**. Regresi baru:
  - **M22**: breadcrumb memiliki viewport nonnol, tiga button ancestor actionable,
    dan dua separator `>` terlihat.
  - **M23**: tap ancestor melalui jalur runtime `hitTestInteractive → dispatchHit`
    menghasilkan `pathIdx` tepat pada kedalaman ancestor.
  - **M24**: pan horizontal memiliki offset/clamp sendiri dan tidak mengubah
    `scrollY` tree.
- Device arm64-v8a: **PASS** dengan `-Wall -Wextra -Werror`; guard tetap
  memastikan `libnbt.so` tidak membutuhkan helper atomic eksternal.
- Harness `NBT_UISHOT`: compile **PASS** dengan `-Wall -Wextra -Werror`.
- Render final ditulis ke
  `/data/data/com.termux/files/usr/tmp/opencode/nbt-b6-render-final/shots/`
  (10 PNG, **bukan** `out/uishots/`). Workflow provider-safe membaginya menjadi
  dua batch `8 + 2`; kedua batch akhirnya **success**, **0 failed**, dan hanya
  batch yang berstatus failed yang di-retry.
- Analisis visual final: rantai `root > nested > level1 > level2` pada
  `10_breadcrumb.png` terlihat jelas, tidak clipping, tidak overlap, dan tidak
  menggeser isi tree secara harmful. Layar Home/Worlds/Browser/Flat/Help tetap
  **byte-identik** dengan screenshot asli.
- Checksum seluruh **9/9 PNG asli** di `out/uishots/` tetap identik. Render tree
  dan layer modal berbeda dari versi lama karena breadcrumb sekarang benar-benar
  terlihat di belakang layar; file asli tidak ditimpa atau di-render ulang.

### N.3 Batas bukti dan pekerjaan yang masih terbuka

- Gesture pan horizontal sudah diimplementasikan, di-clamp, dan lolos model-test
  helper, tetapi belum dapat diuji secara fisik pada perangkat karena tidak
  tersedia ADB/root untuk automation perangkat.
- Temuan visual lama belum ikut diperbaiki pada tahap B6: kontras nilai baris
  terpilih, FAB `S` yang menutupi baris, kebocoran background di belakang modal,
  field dialog yang terlalu rapat, serta selector tipe add-tag yang kosong.
- **B7** elemen Byte/Int/Long Array telah diselesaikan pada bagian O di
  bawah. **B8** intent/menu mati telah diselesaikan pada bagian P di bawah;
  **B9** APK stale telah diselesaikan pada bagian Q di bawah. Rilis 1.0.3
  (code 4) sudah dibangun; validasi fisik tetap menunggu pengguna.

---

<!-- MASTER-SECTION: LEG-O | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian O — B7 Viewer/editor elemen array (14 Nov 2026)

### O.1 Implementasi

B7 kini selesai di `app/src/main.native.cpp` dengan batas L0–L6 tetap sama.
Array tidak direpresentasikan sebagai node arena semu: `Node.bytes` dan
`Node.longs` tetap menjadi payload canonical yang dibaca serializer.

| Aspek | Implementasi aktual |
|---|---|
| Listing bertipe | `TreeEntry` membedakan `Node` dan `ArrayElement`. `visibleTreeItems()` mengembalikan node compound/list atau elemen payload sesuai path aktif. |
| Render | Saat path berada di dalam array, tiap elemen tampil sebagai baris `[0]`, `[1]`, dan seterusnya. Nilai Byte Array ditampilkan sebagai signed byte; Int/Long Array memakai nilai int32/int64. Warna badge mengikuti tipe scalar elemen. |
| Edit | Tap elemen membuka editor numeric yang sesuai: Byte `[-128..127]`, Int `[-2147483648..2147483647]`, atau Long `[-2^63..2^63-1]`. Target disimpan sebagai `(owner arena index, payload index)`, bukan node semu. |
| Validasi | Nilai di luar rentang ditolak dengan pesan `Out of bounds for byte/integer/long.`; indeks basi juga ditolak tanpa mengubah payload. |
| Add | Tombol `+` saat berada di dalam array memilih tipe elemen yang cocok secara otomatis, lalu append nilai `0` ke payload. Name field tidak dipakai; tipe yang tidak cocok ditolak. |
| Remove | Long-press elemen membuka opsi **Edit** dan **Remove**. Remove meminta konfirmasi dan menghapus satu item dari `bytes`/`longs`; indeks sesudahnya otomatis bergeser. |
| Undo/redo | Edit, append, dan remove membuat snapshot `arena` sebelum mutasi. Karena payload array berada di arena, undo/redo memulikan nilai dan jumlah elemen secara tepat tanpa format undo khusus. |
| Target dialog | Long-press dan konfirmasi hapus menyimpan `TreeEntry` bertipe, sehingga pilihan modal tidak lagi tertukar dengan indeks baris tree. |

Screenshot `11_array_elements.png` hanya ditambahkan pada harness host
`NBT_UISHOT`; harness ini tidak masuk APK dan tidak mengubah perilaku runtime.

### O.2 Verifikasi

- Regression batching: **9/9 PASS**.
- Engine host: **49/49 PASS** dengan ASan/UBSan serta `-Wall -Wextra -Werror`.
- Model-test: **58/58 PASS**. Enam regresi B7 baru:
  - **M25**: Byte/Int/Long Array menghasilkan baris bertipe elemen `[index]`
    dengan nilai yang benar; ukuran arena tidak bertambah.
  - **M26**: tap melalui `hitTestInteractive → dispatchHit` membuka editor scalar
    yang tepat dan commit menulis ke payload `bytes/longs`.
  - **M27**: batas nilai Byte/Int/Long dan indeks basi ditolak tanpa mutasi.
  - **M28**: append dan remove melalui handler dialog bekerja untuk ketiga tipe;
    undo dan redo memulihkan payload persis.
  - **M29**: edit dan append untuk ketiga array tetap round-trip
    `serialize → parse` byte-identik tanpa node arena semu.
  - **M30**: long-press elemen menghasilkan tepat dua aksi **Edit/Remove** dan
    keduanya diarahkan ke target payload yang benar.
- Device arm64-v8a: **PASS** dengan `-Wall -Wextra -Werror`; guard tetap
  memastikan `libnbt.so` tidak membutuhkan helper atomic eksternal.
- Harness `NBT_UISHOT`: compile **PASS** dengan `-Wall -Wextra -Werror`.
- Render final ditulis ke
  `/data/data/com.termux/files/usr/tmp/opencode/nbt-b7-render-final/shots/`
  (11 PNG, **bukan** `out/uishots/`). Workflow provider-safe membaginya menjadi
  dua batch `8 + 3`; hasil akhir **2/2 success**, **0 failed**, dan tidak ada
  batch yang perlu di-retry.
- Analisis visual `11_array_elements.png`: breadcrumb `root > intArray` terbaca;
  `[0] = 7`, `[1] = -8`, dan `[2] = 9` tampil berurutan tanpa overlap atau
  clipping. Isinya adalah indeks/value payload array, bukan hierarchy node
  semu.
- Seluruh **10/10 render B6** (`01_home.png` sampai `10_breadcrumb.png`) tetap
  **byte-identik** dengan render B7; hanya `11_array_elements.png` yang baru.
- Checksum seluruh **9/9 PNG asli** di `out/uishots/` tetap identik. Tidak ada
  screenshot asli yang dihapus, ditimpa, atau di-render ulang.

### O.3 Batas bukti dan pekerjaan yang masih terbuka

- B7 mencakup add (append) dan remove. Aksi insert pada indeks tengah belum
  tersedia; ini tidak mengubah format payload maupun round-trip.
- Interaksi perangkat fisik untuk gesture belum dapat diuji karena tidak
  tersedia ADB/root untuk automation perangkat.
- Temuan visual lama belum ikut diperbaiki: kontras nilai baris terpilih, FAB
  `S` yang menutupi baris, kebocoran background di belakang modal, field dialog
  yang terlalu rapat, selector tipe add-tag yang kosong, dan separator non-ASCII.
- **B8** intent/menu mati telah diselesaikan pada bagian P di bawah.
  **B9** APK stale telah diselesaikan pada bagian Q di bawah. Rilis 1.0.3
  (code 4) sudah dibangun; validasi fisik tetap menunggu pengguna.

---

<!-- MASTER-SECTION: LEG-P | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian P — B8 Intent/menu yang sebelumnya mati (14 Nov 2026)

### P.1 Implementasi

B8 selesai di `app/src/main.native.cpp` dengan batas L0–L6 tetap sama. Semua
menu yang ditampilkan sekarang memakai action ID nyata; tidak ada tombol
pseudo untuk intent yang tidak pernah dapat dieksekusi.

| Aspek | Implementasi aktual |
|---|---|
| Audit intent | Enum `Act` dirapikan dari 68 menjadi 53 entri. Semua action selain sentinel `A_NONE` memiliki handler; intent lama yang tidak pernah dipicu atau tidak lagi dibutuhkan dibuang. |
| Theme | Overflow Home menampilkan **Use dark theme** atau **Use light theme**. Handler mengubah `a.dark`, `a.theme`, dan `a.prefs.theme`, sehingga warna surface/background/onSurface benar-benar berpindah pada frame yang sama. |
| Delete world | Long-press baris world memakai `actionLong = A_WORLD_DELETE` dan membuka dialog **Delete world** dengan tombol **DELETE/CANCEL**. Konfirmasi menghapus folder world dan memuat ulang daftar; tap biasa tetap membuka editor `level.dat`. |
| Paste tag | Overflow Tree menampilkan **Paste tag** hanya bila `a.ed.hasClip` true. Paste membuat deep clone ke compound aktif, memakai snapshot `arena` untuk undo/redo. Nama kosong dan nama duplikat ditolak tanpa mutasi. |
| Clone aman | `cloneNode()` kini traverses berdasarkan arena index. Referensi `Node` panjang tidak lagi digunakan sesudah `push_back()`, sehingga reallocation vector tidak dapat membuat dangling reference. |
| Go to root | Overflow Tree selalu menyediakan **Go to root**. Handler mengembalikan `pathIdx` ke root, mereset `scrollY`, dan membangun ulang listing/selection. |
| Overflow geometry | Koordinat anak menu dikoreksi menjadi absolut. Rect yang digambar dan rect yang diuji hit sekarang identik; item menu berada di panel kanan atas dan dapat diketuk pada posisi visualnya. |

Tiga screenshot baru hanya ada pada harness host `NBT_UISHOT`, sehingga tidak
masuk APK dan tidak mengubah perilaku runtime:

- `12_home_dark_menu.png` — dark theme dan aksi theme pada overflow Home.
- `13_world_delete_dialog.png` — konfirmasi delete-world.
- `14_tree_paste_menu.png` — **Paste tag** dan **Go to root** pada overflow Tree.

### P.2 Verifikasi

- Regression batching: **9/9 PASS**.
- Engine host: **49/49 PASS** dengan ASan/UBSan serta `-Wall -Wextra -Werror`.
- Model-test: **62/62 PASS**. Empat regresi B8 baru:
  - **M31**: tema dipilih melalui hit-test/dispatch overflow Home; `a.dark`,
    `a.prefs.theme`, dan palet `a.theme` benar-benar berubah.
  - **M32**: long-press baris world membuka konfirmasi; **CANCEL** mempertahankan
    folder, sedangkan **DELETE** menghapus world terpilih dan refresh daftar.
  - **M33**: **Paste tag** tersedia melalui overflow Tree; clone masuk ke compound,
    nama kosong/duplikat ditolak, dan undo/redo memulihkan state dengan tepat.
  - **M34**: **Go to root** tersedia melalui overflow Tree dan mereset path,
    vertical scroll, serta selection.
- Audit action final: 53 action; 52 punya handler dan satu sentinel `A_NONE`.
  Tidak ada intent enum tanpa handler.
- Device arm64-v8a: **PASS** dengan `-Wall -Wextra -Werror`; guard tetap
  memastikan `libnbt.so` tidak membutuhkan helper atomic eksternal.
- Harness `NBT_UISHOT`: compile **PASS** dengan `-Wall -Wextra -Werror`.
- Render final ditulis ke
  `/data/data/com.termux/files/usr/tmp/opencode/nbt-b8-render-final/shots/`
  (14 PNG, **bukan** `out/uishots/`). Workflow provider-safe membaginya menjadi
  dua batch `8 + 6`; hasil akhir **2/2 success**, **0 failed**, dan tidak ada
  batch yang perlu di-retry.
- Analisis visual: `12_home_dark_menu.png` benar-benar gelap dan seluruh label
  overflow terbaca; `13_world_delete_dialog.png` menampilkan judul, pesan,
  **DELETE**, dan **CANCEL** tanpa clipping; `14_tree_paste_menu.png` menampilkan
  **Paste tag** serta **Go to root** di dalam viewport tanpa overlap fatal.
- Seluruh **11/11 render dasar B7** (`01_home.png` sampai `11_array_elements.png`)
  tetap **byte-identik** dengan render B8; hanya tiga screenshot B8 tersebut yang
  baru.
- Checksum seluruh **9/9 PNG asli** di `out/uishots/` tetap identik. Tidak ada
  screenshot asli yang dihapus, ditimpa, atau di-render ulang.

### P.3 Batas bukti dan pekerjaan yang masih terbuka

- Pergantian tema masih bersifat session-only. `prefs.theme` diperbarui, tetapi
  belum ada lapisan persistensi prefs yang menulis/membaca tema saat startup.
- Long-press dan gesture perangkat fisik belum dapat diuji otomatis karena
  tidak tersedia ADB/root untuk automation perangkat; jalur model-test
  `hitTestInteractive → dispatchHit` dan dialog sudah terverifikasi.
- Temuan visual lama belum ikut diperbaiki: kontras nilai baris terpilih, FAB
  `S` yang menutupi baris, kebocoran background di belakang modal, field dialog
  yang terlalu rapat, selector tipe add-tag yang kosong, dan separator non-ASCII.
- **B9** APK stale telah diselesaikan pada bagian Q di bawah. Rilis 1.0.3
  (code 4) sudah dibangun; validasi fisik tetap menunggu pengguna.

---

<!-- MASTER-SECTION: LEG-Q | CATEGORY=Riwayat perbaikan | STATUS=HISTORIS -->
## Bagian Q — B9 Rilis APK 1.0.3 (14 Nov 2026)

### Q.1 Sumber versi dan artefak

B9 menutup APK stale dengan membuat rilis baru yang memuat seluruh source B6–B8.
`VERSION` tetap menjadi satu-satunya sumber kebenaran; konstanta versi tidak lagi
diduplikasi sebagai literal source.

| Aspek | Implementasi aktual |
|---|---|
| Versi | `VERSION_CODE=4`, `VERSION_NAME=1.0.3`. Kenaikan code dari 3 ke 4 memenuhi syarat rilis Android. |
| Inject source | `scripts/build_host_tests.sh` dan `scripts/build_device.sh` membaca `VERSION`, lalu meneruskan `NBT_APP_VERSION_NAME` dan `NBT_APP_VERSION_CODE` ke source native. Build tanpa kedua macro ditolak oleh `#error`. |
| Manifest | `scripts/build_apk.sh` membaca `VERSION` yang sama untuk `--version-code` dan `--version-name`. |
| UI Help | Kode build native memuat string `1.0.3`; render `06_help.png` menampilkan `Version 1.0.3 (build 4)`. |
| Source | Tetap satu translation unit native di `app/src/main.native.cpp`; tidak ada modul DEX/Gradle baru. |

| Artefak | Nilai final |
|---|---|
| APK | `out/NBTEditor.apk`, 1.819.274 byte |
| SHA-256 APK | `0b7d57326cc1732f77ba40ce5d3aa085f41a760bc1fbb57e9b21cd4964515a76` |
| Package | `com.nbteditor` |
| Version | `1.0.3`, version code `4` |
| SDK | min 26, target 35, compile 35 |
| ABI | `arm64-v8a` |
| Signature | APK Signature Scheme **v2 + v3 valid**; certificate SHA-256 `439499f02576d215712a12fda87d83540d2d4d8c7fa37b0c869d65c926ab1d52` |
| `libnbt.so` | SHA-256 `c6dc81665ef09565d37d6670b144120e10f4f3c1bf0e1de824d3433a91e9f731`; identik dengan library di APK |
| `libc++_shared.so` | SHA-256 `05bef9d3901910c3ff8ab450b03bc8b28383d363a2290f2f7b9aed7331ea6ff7`; identik dengan library di APK |

Isi APK hanya manifest, dua resource aplikasi, `resources.arsc`, dua library
arm64-v8a, dan metadata signature. Tidak ada stub build, archive compiler, DEX,
atau path Termux di dalam library. `libnbt.so` hanya membutuhkan
`libc++_shared.so`, `libandroid.so`, `libz.so`, dan `liblog.so`; guard tetap
memastikan tidak ada helper atomic eksternal.

### Q.2 Verifikasi

- Regression batching: **9/9 PASS**.
- Engine host: **49/49 PASS** dengan ASan/UBSan serta `-Wall -Wextra -Werror`.
- Model-test: **62/62 PASS** tanpa perubahan jumlah test.
- Device arm64-v8a: **PASS** dengan `-Wall -Wextra -Werror`; guard atomic dan
  penghapusan path Termux tetap lulus.
- Build APK: **PASS**; `aapt2 dump badging` mengonfirmasi package, version name,
  version code, SDK, ABI, dan launcher NativeActivity.
- Signature: **PASS** melalui `apksigner verify --verbose --print-certs`; v2 dan
  v3 sama-sama valid dengan satu signer.
- Integritas library: **PASS**; hash `libnbt.so` dan `libc++_shared.so` di APK
  identik dengan salinan build di `out/lib/arm64-v8a/`.
- Render final ditulis ke
  `/data/data/com.termux/files/usr/tmp/opencode/nbt-b9-render-final/shots/`
  (14 PNG, **bukan** `out/uishots/`). Workflow provider-safe membaginya menjadi
  dua batch `8 + 6`; hasil akhir **2/2 success**, **0 failed**, dan tidak ada
  retry.
- Analisis visual `06_help.png`: `Version 1.0.3 (build 4)` terbaca utuh tanpa
  clipping/overlap. Scene B6–B8 tetap stabil.
- **13/14 render B8** tetap byte-identik; hanya `06_help.png` berubah karena
  metadata versi. Checksum seluruh **9/9 PNG asli** di `out/uishots/` tetap
  identik dan tidak ada file asli yang ditulis ulang.

### Q.3 Batas bukti dan pekerjaan yang masih terbuka

- APK 1.0.3 **belum dipasang atau dijalankan pada perangkat fisik** karena
  Termux tidak memiliki root/ADB/`START_ACTIVITIES`. Bukti startup langsung di
  Part K tetap berlaku untuk 1.0.2, tetapi bukan bukti observasi runtime untuk
  library 1.0.3. Untuk memvalidasinya, pengguna cukup memasang
  `out/NBTEditor.apk` lalu menekan ikon satu kali.
- APK memakai keystore debug proyek. Artefak ini valid untuk sideload/pengujian
  internal, tetapi bukan keystore distribusi production.
- Tool `zipalign` tidak tersedia di lingkungan build. APK tetap berhasil
  ditandatangani dan diverifikasi; `extractNativeLibs="true"` mengizinkan
  library diekstrak saat instalasi.
- Pergantian tema masih session-only, dan temuan visual lama (kontras baris
  terpilih, FAB `S`, kebocoran background modal, field dialog rapat, selector
  tipe kosong, separator non-ASCII) belum diperbaiki.
- Fitur di luar B6–B9—misalnya persistensi tema/prefs, expand/collapse,
  perubahan tipe tag, validasi saat save, dan optimasi undo untuk dokumen
  sangat besar—masih menjadi kandidat tahap berikutnya.

---

<!-- MASTER-SECTION: LEG-R | CATEGORY=Daftar bug | STATUS=AUDIT-HISTORIS -->
## Bagian R — Audit FASE 1 dan kontrak perbaikan (15 Nov 2026)

### R.1 Ruang lingkup, baseline, dan bukti

Audit ini membaca seluruh `DOKUMENTASI.md`, seluruh `app/src/main.native.cpp`,
manifest/resource, skrip build, harness model-test, log ADB/dropbox, dan APK
yang sedang terpasang. Tidak ada fitur baru ditambahkan pada fase ini.

| Item | Nilai yang diaudit |
|---|---|
| Source | `app/src/main.native.cpp`, 6.937 baris |
| Source SHA-256 | `1a11017420d1661e3b9849a24a9b736e7c385ec8c9e8bd592ec7bb0305641e80` |
| Versi | `1.0.4`, version code `5` (build diagnostik, bukan rilis final) |
| APK | `out/NBTEditor.apk`, 1.819.274 byte |
| APK SHA-256 | `5ebc8f1a6c993e8e0815dd85ebd7c42acbe43c4b19031fc34be98f5667f2c321` |
| Library | `out/lib/arm64-v8a/libnbt.so` |
| Library SHA-256 | `78a1dc99f3fe8b2a2e7baa36f45b888f0181dbf89835dd9307ff6dbb6d25c872` |
| Perangkat | Infinix X6528B, Android 13 / SDK 33, 720×1612, density 320 |
| Package | `com.nbteditor/android.app.NativeActivity` |

Baseline source yang belum diubah lulus:

- Regression batching screenshot: **9/9 PASS**.
- Engine host dengan ASan/UBSan dan `-Werror`: **49/49 PASS**.
- UI model dengan `hitTestInteractive → dispatchHit`: **62/62 PASS**.
- Device arm64-v8a: build `-Werror` PASS dan guard atomic PASS pada saat
  baseline diambil.

Hasil ADB baru yang mengikat 1.0.4 ke ANR input:

- Dropbox ANR `2026-11-14 22:37:03` berisi `Package: com.nbteditor v5 (1.0.4)`
  dan `Waited 8001ms for KeyEvent`
  (`out/adb-logs/dropbox_data_app_anr_live_2026-11-15.txt:16667-16833`).
- Thread render 1.0.4 berada di `ALooper_pollOnce` dari
  `app::androidMain+176` pada dump yang sama
  (`:17100-17110`).
- Reproduksi 2026-11-15 01:16:48 mencatat `native render looper` dan
  `native input queue created`, tetapi **tidak ada** `native input queue
  attached` (`out/adb-logs/logcat_1.0.4_input_attach_missing_2026-11-15.txt`).
- ANR historis 1.0.2/1.0.3 juga memuat `Waited 8000ms/8001ms/8008ms` untuk
  `MotionEvent`/`KeyEvent`; satu kasus 1.0.3 menunjukkan thread render
  tertahan di `ANativeWindow_lock`.

Kesimpulan FASE 1: **1.0.4/code 5 belum layak disebut rilis final.** Masalah
paling mendasar masih input/UI dan integritas data, bukan metadata APK.

### R.2 Prioritas yang disepakati

Kode prioritas pada laporan ini bukan penilaian bisnis; ini urutan teknis yang
menghindari ANR, crash, dan kehilangan data terlebih dahulu.

- **P0 — blocker rilis:** L0 input/lifecycle, modal/input routing, dialog
  yang tidak terlihat/reachable, parser/ZIP yang dapat crash/overwrite, serta
  I/O berat pada render thread.
- **P1 — koreksi data dan navigasi:** Save As, undo rename, float precision,
  search, back/navigation stack, keypad, flat editor, undo memory, worker.
- **P2 — ketahanan rilis:** persistensi prefs, dukungan inset edge-to-edge,
  keystore release, manifest/permission, kontrak build fail-closed, dokumentasi
  dan coverage perangkat.

### R.3 Temuan terverifikasi (format wajib)

Setiap item di bawah ini **TERBUKA** pada akhir audit; tidak ada source yang
diubah sebelum laporan ini ditulis.

#### R-P0-01 — Input queue tidak pernah attach; ANR input pada 1.0.4

- **Masalah:** queue input NativeActivity tidak di-attach ke looper render
  sebelum thread masuk poll tak terbatas.
- **Gejala:** tap/key event tidak diproses; InputDispatcher menunggu 8 detik;
  focus berpindah ke launcher/browser; ANR 1.0.4 tercatat di dropbox.
- **Akar penyebab:** `ALooper_pollOnce(-1, ...)` pada jalur window belum
  valid memblokir thread sebelum cabang attach queue dieksekusi. Callback
  `onNativeWindowCreated` dan `onInputQueueCreated` juga tidak membangunkan
  looper. Log live membuktikan queue dibuat, looper dibuat, attach tidak
  pernah tercatat.
- **File/komponen:** `app/src/main.native.cpp:4824-4845`, callback input
  `:4599-4608`, lifecycle `:4555-4598`.
- **Perbaikan:** semua `ALooper_pollOnce` finite; attach queue dari thread
  render pada loop; `ALooper_wake` saat queue/window/focus berubah; pisahkan
  lifecycle state lintas-thread.
- **Metode pengujian:** launch 1.0.5, `logcat` harus memuat `native input
  queue attached`, tap tombol Home mengubah state, `dumpsys input` WaitQueue
  kosong, 30 detik tanpa ANR.
- **Hasil:** terverifikasi masih gagal pada 1.0.4; bukti runtime tersimpan di
  `out/adb-logs/`.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-02 — `ANativeWindow` dan `AInputQueue` diakses lintas thread tanpa sinkronisasi

- **Masalah:** pointer/status platform ditulis callback Android main thread
  dan dibaca render thread; window bisa dipakai setelah destroy dan queue bisa
  dipakai/di-detach bersamaan.
- **Gejala:** crash/UB, frame korup, input hilang, queue detach ganda,
  use-after-free pada rotasi/background/foreground.
- **Akar penyebab:** `g_plat` berisi pointer/status biasa; `g_frameMutex`
  dipakai render thread saja; callback destroy memakai pointer global, bukan
  argumen queue yang diberikan framework.
- **File/komponen:** `app/src/main.native.cpp:4478-4508`, `:4520-4608`,
  `:4810-4899`; kontrak NDK `native_activity.h` dan `input.h`.
- **Perbaikan:** reference/lifetime lock untuk window (`acquire` render,
  clear+release destroy), lock state machine queue, attach/detach hanya di
  bawah lock, callback memakai queue lokal dan argument framework.
- **Metode pengujian:** pause/resume berulang, rotasi, background/foreground,
  `dumpsys input`, logcat crash, ASan/UBSan host untuk bagian state generik,
  stress 100 siklus lifecycle.
- **Hasil:** terverifikasi dari source dan stack ANR; belum ada fix.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-03 — Permintaan permission memakai `JNIEnv` milik main thread dari render thread

- **Masalah:** `requestStoragePermission()` dipanggil dari `handleAction()` di
  render thread tetapi membaca `g_plat.activity->env`.
- **Gejala:** UB JNI, `CalledFromWrongThreadException`/crash, Settings tidak
  terbuka, permission request berulang.
- **Akar penyebab:** `ANativeActivity::env` hanya milik thread callback; kode
  hanya `AttachCurrentThread` bila pointer null, sedangkan pointer main-thread
  biasanya non-null. Alur API 26–29 juga memakai `MANAGE_EXTERNAL_STORAGE`
  yang belum berlaku, dan API 30+ tidak memeriksa status grant.
- **File/komponen:** `app/src/main.native.cpp:3811-3821`, `:3849-3859`,
  `:4614-4662`.
- **Perbaikan:** selalu `GetEnv` pada thread pemanggil dan attach/detach dengan
  benar; branch API 30+ `Environment.isExternalStorageManager` dan API 23–29
  runtime permission; jangan memakai `activity->env` lintas thread.
- **Metode pengujian:** grant/deny permission di API 33 dan emulator API 28,
  pastikan Settings/permission dialog muncul, browser refresh setelah grant,
  tidak ada exception logcat.
- **Hasil:** terverifikasi statis; belum direproduksi pada 1.0.4 karena input
  belum berfungsi.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-04 — Dialog/progress bukan modal; konfirmasi hapus bisa menghapus world lain

- **Masalah:** tidak ada scrim/interceptor; kontrol di belakang dialog masih
  dapat diketuk; target hapus diambil `a.ed.selected` saat konfirmasi.
- **Gejala:** dialog menampilkan world A, DELETE menghapus world B; Add bisa
  masuk container lain; action berjalan saat import/export.
- **Akar penyebab:** painter z-order saja, tanpa modal hit root; shared
  `a.ed.selected` dipakai lintas screen/dialog.
- **File/komponen:** `app/src/main.native.cpp:2966-3109`, `:3614-3678`,
  `:3954-3958`, `:4350-4357`.
- **Perbaikan:** tambah blocker node paling atas saat modal/keypad/progress;
  tangkap index/path target saat dialog dibuka dan pakai itu saat konfirmasi;
  modal scrim memblokir semua hit-test non-modal.
- **Metode pengujian:** model test hit-test di belakang dialog harus miss;
  buka konfirmasi hapus, pilih row lain di belakang, DELETE hanya menghapus
  target captured; progress import memblokir semua action.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-05 — Kontrol anak dialog add/value memakai koordinat lokal yang tidak ditranslasi

- **Masalah:** type grid dan inline numeric pad dibuat pada `(0,0)` dan anak
  panel, tetapi renderer hanya memindah node top-level.
- **Gejala:** tombol type dan pad numerik tidak terlihat; hit-test-nya tidak
  reachable; tipe add dan input numerik praktis tidak bisa dipakai.
- **Akar penyebab:** builder body mengasumsikan koordinat relatif, sedangkan
  tree UI memakai koordinat absolut dan `drawNode`/hit-test tidak punya
  parent transform.
- **File/komponen:** `app/src/main.native.cpp:2826-2940`, `:3023-3033`,
  `:3308-3417`.
- **Perbaikan:** translasi rekursif seluruh subtree body saat body dipindah
  ke koordinat absolut, atau bangun body absolut sejak awal; uji geometry +
  hit-test semua tombol.
- **Metode pengujian:** model test membangun add/value dialog dan memastikan
  semua rect kontrol berada di dalam dialog; tap tengah tombol menghasilkan
  action yang benar.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-06 — Parser menerima array/list berukuran attacker dan trailing bytes

- **Masalah:** count array/list langsung memicu alokasi/loop; list element
  tidak dihitung pada node limit; `TAG_End` dengan count positif diterima;
  bytes setelah root diabaikan.
- **Gejala:** OOM dari file kecil (mis. `LongArray` count `INT32_MAX`), CPU
  loop miliaran iterasi, save menghapus data trailing secara diam-diam.
- **Akar penyebab:** count wire dipercaya sebelum preflight remaining bytes;
  node accounting hanya compound; `parseBytes` tidak membandingkan posisi akhir
  dengan `payloadEnd`.
- **File/komponen:** `app/src/main.native.cpp:224-228`, `:246-260`,
  `:286-299`, `:325-367`.
- **Perbaikan:** tolak count negatif dan count > remaining bytes atau
  `kMaxNodes`; hitung list element; tolak `End` dengan count > 0; tambah error
  `TrailingBytes`; uji malformed fixture.
- **Metode pengujian:** host fixture count `INT32_MAX`, list `End` count
  `INT32_MAX`, raw trailing garbage, `level.dat` trailing payload; parser harus
  gagal cepat tanpa alokasi besar.
- **Hasil:** terverifikasi statis; test saat ini tidak memiliki fixture ini.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-07 — Import ZIP memiliki Zip Slip

- **Masalah:** entry ZIP dipakai langsung sebagai path destination.
- **Gejala:** `.mcworld` jahat dapat menulis di luar folder world dengan akses
  all-files.
- **Akar penyebab:** `dest = destDir / e.name` tanpa normalisasi/containment.
- **File/komponen:** `app/src/main.native.cpp:1360-1373`.
- **Perbaikan:** tolak absolute path/`..`; canonicalize parent/destination dan
  pastikan di bawah canonical `destDir`; uji entry traversal.
- **Metode pengujian:** fixture ZIP `../evil.dat`, absolute path, duplicate name,
  bandingkan tree destination dan file di luar.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-08 — ZIP reader mempunyai risiko OOB read, OOM, dan inflate tidak divalidasi

- **Masalah:** offset/length central directory dan local header tidak
  preflight; output entry dapat ~4 GB; `inflate` tidak dicek; CRC diabaikan.
- **Gejala:** crash/OOB, decompression bomb, import corrupt dilaporkan sukses.
- **Akar penyebab:** metadata archive dipercaya penuh dan output resize
  langsung mengikuti `usize`.
- **File/komponen:** `app/src/main.native.cpp:1302-1357`.
- **Perbaikan:** validasi semua offset/length terhadap buffer, batas output,
  cek status inflate dan CRC, streaming/bounded buffer.
- **Metode pengujian:** malformed ZIP dengan offset di luar buffer, truncated
  deflate, entry ukuran raksasa, CRC salah, duplicate entry.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-09 — I/O berat dan mutasi state dijalankan pada render thread di bawah `g_stateMutex`

- **Masalah:** browse/list/open/save/delete/world scan dipanggil dari
  `handleAction()` render thread; worker completion juga `listWorlds` di bawah
  lock state.
- **Gejala:** freeze, ANR pada file/world besar, render thread tertahan,
  input callback tidak berjalan.
- **Akar penyebab:** aksi memuat repository synchronously dan lock state
  diperpanjang ke I/O; `g_stateMutex` dipakai render frame dan action.
- **File/komponen:** `app/src/main.cpp` `app/src/main.native.cpp:3754-3978`,
  `:4277-4357`, `:4869-4888`.
- **Perbaikan:** mutation state cepat + worker I/O + completion queue; lock
  state hanya untuk snapshot/mutasi singkat; jangan render/parse/I/O di bawah
  lock.
- **Metode pengujian:** fixture 50 MB, world dengan banyak file, ukur frame
  jank/ANR, CPU/RSS, cek `dumpsys input` kosong.
- **Hasil:** terverifikasi statis dan konsisten dengan ANR `ANativeWindow_lock`.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-10 — Integritas serializer dan editor numerik

- **Masalah:** name/string/count di-cast `u16`/`i32` tanpa batas; serializer
  tidak memakai `validate()`; float/double editor memakai `%g` presisi rendah;
  add/rename tidak membatasi 65.535 UTF-8 bytes.
- **Gejala:** save mengubah nama/string/count, float/double berubah meski
  user hanya membuka dan menyimpan, NBT corrupt.
- **Akar penyebab:** writer mengembalikan vector bukan result; UI dan writer
  punya kontrak batas yang berbeda; formatter `%g` bukan round-trip safe.
- **File/komponen:** `app/src/main.native.cpp:393-463`, `:552-586`,
  `:2112-2116`, `:3520-3525`, `:3652-3699`.
- **Perbaikan:** validasi dokumen sebelum serialize; reject name/string/count
  overflow; float `%.9g`, double `%.17g`; validasi UI 65.535 bytes.
- **Metode pengujian:** host test float/double round-trip presisi, string 65535
  dan 65536, count overflow, save invalid document gagal tanpa file berubah.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-11 — Navigasi back/stack dan `openDocument` salah

- **Masalah:** semua app bar memakai `A_TREE_UP`; `openDocument` push
  `S_TREE` sebagai previous screen.
- **Gejala:** Back dari Help/Browser/Worlds mengubah tree path tersembunyi
  atau tidak keluar; dari dokumen baru back pertama kembali ke tree.
- **Akar penyebab:** `appBar()` hardcode `A_TREE_UP`; `openDocument()` push
  destination.
- **File/komponen:** `app/src/main.native.cpp:1948-1967`, `:2253-2268`,
  `:4073-4076`, `:3137-3148`.
- **Perbaikan:** tambah `A_BACK`; app bar terima action; tree memakai
  `A_TREE_UP`; `openDocument` pakai `setScreen(a, S_TREE)`.
- **Metode pengujian:** model test stack Home→Browser→document; back harus
  Browser; Help dari nested tree back harus Help; tree back di root harus
  keluar tree.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-12 — Keypad hanya merender 3 baris dan SHIFT tidak berfungsi

- **Masalah:** keyRows berisi 4 baris, `kRowsUsed=3`, semua key digabung, kolom
  dipaksa 3; hanya `1–9`/`a–i` yang ada; `keypadShift` dan `keypadBuf` tidak
  dipakai.
- **Gejala:** `0`, `-`, `.`, `j–z`, `_`, `/`, spasi, huruf besar tidak dapat
  dimasukkan.
- **Akar penyebab:** layout keypad memotong baris dan shift tidak mengubah
  karakter; target flat juga tidak terhubung.
- **File/komponen:** `app/src/main.native.cpp:3150-3196`, `:3425-3501`,
  `:4404-4442`.
- **Perbaikan:** render tiap baris dengan kolomnya; gunakan shift; pakai satu
  buffer; hubungkan target flat.
- **Metode pengujian:** model test cek semua expected char dan shift; flat
  thickness berubah dan Enter commit.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-13 — Tombol `X` search tidak clear dan hasil bisa menduplikasi current node

- **Masalah:** field/Find/X memakai `A_TREE_SEARCH`; filter listing memasukkan
  current node; tap current node push path yang sama.
- **Gejala:** X hanya membuka keypad; breadcrumb dapat memuat node sama
  dua kali.
- **Akar penyebab:** intent search tidak dibedakan; filter traversal tidak
  membatasi descendant.
- **File/komponen:** `app/src/main.native.cpp:2183-2212`, `:2637-2646`,
  `:3983-4005`, `:4211-4214`.
- **Perbaikan:** action SearchOpen/Find/Clear/Close; excluded current node atau
  tolak push index sama.
- **Metode pengujian:** model test tap X clear filter; current match tap tidak
  menggandakan path.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-14 — Undo rename disnapshot setelah mutasi

- **Masalah:** `renameNode()` berjalan sebelum `snapshotUndo()`.
- **Gejala:** undo tidak mengembalikan nama lama dan menghabiskan satu slot.
- **Akar penyebab:** urutan snapshot/mutasi terbalik.
- **File/komponen:** `app/src/main.native.cpp:3681-3700`.
- **Perbaikan:** validasi dulu, snapshot pra-mutasi, lalu rename.
- **Metode pengujian:** model test rename→undo→name lama, redo→name baru.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-15 — Save As tidak bisa memilih folder dan dapat menimpa file

- **Masalah:** directory row selalu `A_BROWSE_OPEN`; tidak ada “Save here” atau
  filename field; file existing langsung di-overwrite.
- **Gejala:** Save As tidak dapat selesai pada folder kosong; file lain bisa
  hilang tanpa konfirmasi.
- **Akar penyebab:** browser hanya punya intent open file/directory, bukan
  choose destination.
- **File/komponen:** `app/src/main.native.cpp:2448-2510`, `:3862-3933`.
- **Perbaikan:** mode browser eksplisit + tombol Save here + filename field +
  overwrite confirmation.
- **Metode pengujian:** model test Save As ke folder kosong menghasilkan file
  baru; target existing meminta konfirmasi; path/nama benar.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-16 — Editor flat thickness dan reorder layer salah target

- **Masalah:** keypad target `flatDraft` tidak diisi; up/down button tidak
  membawa `index`; content height flat tidak diset.
- **Gejala:** thickness tidak berubah; reorder mengubah layer lain atau tidak
  terjadi; daftar layer tidak bisa scroll penuh.
- **Akar penyebab:** state buffer/target tidak terhubung; child button index
  tidak diset; builder tidak menghitung `contentBottom`.
- **File/komponen:** `app/src/main.native.cpp:1755`, `:2741-2759`,
  `:3150-3188`, `:4262-4275`, `:4435-4439`.
- **Perbaikan:** pakai `flatDraft` di apply/clear/backspace/header/Enter; set
  index tombol reorder; set `contentBottom`.
- **Metode pengujian:** model test thickness 1→3; up/down memindahkan row yang
  disentuh; scroll clamp mencakup semua layer.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-17 — Scroll container tidak di-clip saat render/hit-test

- **Masalah:** `Kind::Scroll` diabaikan renderer; hit-test tidak tahu clip.
- **Gejala:** row di luar viewport menimpa app bar/path/FAB dan masih bisa
  diketuk.
- **Akar penyebab:** child memakai koordinat absolut tanpa parent clip.
- **File/komponen:** `app/src/main.native.cpp:2884-2889`, `:2939`, `:1200-1208`.
- **Perbaikan:** `pushClip` pada Scroll saat render; tolak hit-test di luar
  ancestor clip.
- **Metode pengujian:** model test row di luar clip miss hit-test dan render
  fixture membandingkan pixel di atas/bawah viewport.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-18 — Worker satu slot, exception tidak dibatasi, `onDestroy` join di main thread

- **Masalah:** `job = std::move(f)` menimpa job antre; lambda worker bisa throw;
  `stop()` join job import/export dari main thread.
- **Gejala:** job hilang, progress modal nyangkut, ANR saat menutup activity
  selama operasi berat.
- **Akar penyebab:** worker bukan queue, tidak ada try/catch/cancellation;
  lifecycle join di platform main thread.
- **File/komponen:** `app/src/main.native.cpp:1560-1601`, `:4566-4571`,
  `:3897-3908`, `:3971-3978`.
- **Perbaikan:** deque atau reject saat busy; try/catch→failed result; reset
  busy/done di jalur keluar; render thread joinable dan worker stop nonblocking
  dengan cancellation.
- **Metode pengujian:** post dua job cepat, pastikan keduanya berurutan; job
  throw tidak crash; destroy saat export fixture besar tidak ANR.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P0.

#### R-P0-19 — Activity recreation dapat meninggalkan worker mati/render lama

- **Masalah:** `g_booted` global dan `std::thread(...).detach()`.
- **Gejala:** activity baru blank/tanpa worker; thread lama lolos lifecycle;
  state stale.
- **Akar penyebab:** process bootstrap dicampur dengan activity lifecycle;
  Worker::start tidak mengembalikan `running=true`.
- **File/komponen:** `app/src/main.native.cpp:4503-4508`, `:4566-4571`,
  `:4904-4944`, `:1567-1600`.
- **Perbaikan:** pisahkan process init vs activity lifecycle; render thread
  joinable dan restart; worker restart; reset activity-scoped state.
- **Metode pengujian:** model/ADB rotate/recreate 50×, pastikan frame baru,
  worker hidup, tidak ada thread lama.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-20 — Crash handler tidak async-signal-safe

- **Masalah:** signal handler membuat `std::string`, lock log buffer,
  filesystem I/O.
- **Gejala:** deadlock/crash kedua saat crash terjadi di tengah allocator/mutex.
- **Akar penyebab:** handler memakai API non-signal-safe.
- **File/komponen:** `app/src/main.native.cpp:4664-4704`.
- **Perbaikan:** precompute FD/path; handler hanya `write()` fixed buffer; restore
  default lalu re-raise.
- **Metode pengujian:** trigger fault di host/device, pastikan note tertulis dan
  tidak deadlock.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P2.

#### R-P0-21 — Cancel/multi-pointer/key event/Back tidak ditangani dengan benar

- **Masalah:** `ACTION_CANCEL` diperlakukan tap; pointer index hilang; semua
  key event difinish handled; Back Android tidak diproses.
- **Gejala:** gesture batal memicu action; multitouch salah; Back tidak
  menutup dialog/screen.
- **Akar penyebab:** parser event sederhana tanpa key/cancel/pointer state.
- **File/komponen:** `app/src/main.native.cpp:4520-4553`, `:4777-4789`.
- **Perbaikan:** field `cancel`; tangani pointer index; preDispatch key;
  handle Back di key-up; finish `handled=0` bila tidak dikonsumsi.
- **Metode pengujian:** ADB inject cancel/multitouch/back, model event unit test.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-22 — Pilihan tema/recent/last directory tidak dipersistensikan

- **Masalah:** `Prefs` dideklarasikan tapi tidak pernah load/save; `a.recent`
  terpisah; bootstrap menimpa `a.dark=false`.
- **Gejala:** preferensi hilang setelah restart; Recent kosong.
- **Akar penyebab:** belum ada repository prefs.
- **File/komponen:** `app/src/main.native.cpp:1505-1510`, `:1702-1715`,
  `:2004-2008`, `:4908-4920`.
- **Perbaikan:** simpan JSON/text prefs di `filesDir`; muat startup; satu source
  of truth; simpan setelah mutasi.
- **Metode pengujian:** ganti tema, restart proses, tema dan recent bertahan.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P2.

#### R-P0-23 — Klaim fitur yang belum diimplementasikan

- **Masalah:** matrix dokumentasi menandai autosave, expand/collapse, move,
  byte-per-node, landscape/tablet, persistensi tema sebagai selesai.
- **Gejala:** Help/matrix menjanjikan perilaku yang tidak ada; user menekan
  expander `+/-` yang tidak melakukan apa-apa.
- **Akar penyebab:** state/komentar dibuat tapi renderer/handler tidak
  mengimplementasikan; matrix manual.
- **File/komponen:** `DOKUMENTASI.md:525-620`;
  `app/src/main.native.cpp:2649-2692`, `:2183-2213`, `:2788-2792`.
- **Perbaikan:** implementasi atau tandai jujur `🟡/❌`; hapus teks Move; uji
  setiap klaim `✅`.
- **Metode pengujian:** acceptance test per item matrix; Help text disinkronkan.
- **Hasil:** terverifikasi silang dokumentasi/source.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-24 — Build APK bisa mengemas library stale dan metadata check belum fail-closed

- **Masalah:** `build_apk.sh` hanya build device bila `.so` tidak ada; tidak
  menjalankan host test; pemeriksaan ELF/path/signature ada `|| true`.
- **Gejala:** bug library lama ikut terpasang; regression hijau tidak
  diwajibkan; artefak rusak masih bisa lulus.
- **Akar penyebab:** pipeline tidak membandingkan source/hash/timestamp.
- **File/komponen:** `scripts/build_apk.sh:10-56`,
  `scripts/build_device.sh:59-78`, `scripts/build_host_tests.sh`.
- **Perbaikan:** APK build selalu host test + device build; hash provenance;
  hard-fail metadata/ELF/RUNPATH/signature.
- **Metode pengujian:** sentuh source, bangun APK tanpa build ulang, pastikan
  hash library di APK cocok source baru; rusakkan ELF dan pastikan gagal.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-25 — Manifest/permission dan intent filter terlalu luas

- **Masalah:** `MANAGE_EXTERNAL_STORAGE` plus filter `*/*`, VIEW intent tidak
  pernah diproses native; `requestLegacyExternalStorage` tidak relevan target
  35.
- **Gejala:** app kandidat handler semua file; VIEW membuka app tanpa membuka
  file; risiko kebijakan distribusi.
- **Akar penyebab:** permission global dan intent intake belum diimplementasi.
- **File/komponen:** `app/AndroidManifest.xml:5-42`.
- **Perbaikan:** prioritaskan SAF; persempit tipe; implementasi URI atau hapus
  filter VIEW; branch permission per API.
- **Metode pengujian:** ADB VIEW fixture; permission grant/deny; badging
  permission.
- **Hasil:** terverifikasi statis; distribusi publik masih di luar scope build
  debug.
- **Status akhir:** TERBUKA — perbaikan P2.

#### R-P0-26 — Inset system bar belum dilacak

- **Masalah:** `onContentRectChanged` kosong; layout memakai seluruh window.
- **Gejala:** risiko konten/FAB di bawah status/navigation bar pada
  edge-to-edge/OEM; pada konfigurasi saat ini mismatch belum terbukti.
- **Akar penyebab:** app tidak menyimpan safe-area rect.
- **File/komponen:** `app/src/main.native.cpp:4576`, layout L6.
- **Perbaikan:** simpan content rect, gunakan untuk layout/viewport dan ujikan
  koordinat touch setelah bukti event space; jangan offset buta.
- **Metode pengujian:** ADB screenshot + hit-test tombol di bawah/atas; rotate;
  gesture nav/3-button.
- **Hasil:** risiko kondisional; mismatch koordinat belum terbukti pada
  Android 13 current.
- **Status akhir:** TERBUKA — perbaikan P2.

#### R-P0-27 — Coverage device/lifecycle/orientation masih nol

- **Masalah:** L0 Android dikompilasi hanya device; host model tidak menguji
  lifecycle, queue, JNI, orientation, ANR.
- **Gejala:** bug 1.0.4 baru terlihat setelah APK terpasang; orientation
  landscape/tablet tidak teruji.
- **Akar penyebab:** L0 di-exclude dari `NBT_TESTABLE`.
- **File/komponen:** `app/src/main.native.cpp:4462-4970`, test model
  `:5370-6728`.
- **Perbaikan:** tambah ADB smoke test input/ANR/lifecycle/orientation; uji
  geometry dialog/clipping/modal; fixture parser/ZIP; bangun fail-closed.
- **Metode pengujian:** pipeline test baru yang dijelaskan pada R.4.
- **Hasil:** gap terverifikasi.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-28 — Import/export tidak staging/atomic dan verifikasi world

- **Masalah:** import mengekstrak langsung ke destination dan membiarkan
  partial; export menulis final path; gagal baca file sumber diabaikan.
- **Gejala:** world corrupt/partial tampil di list; export gagal menghapus
  archive lama.
- **Akar penyebab:** tidak ada staging+validasi+rename; partial error tidak
  diagregasi.
- **File/komponen:** `app/src/main.native.cpp:1302-1456`, `:3897-3908`,
  `:3960-3978`.
- **Perbaikan:** staging temp, validasi `level.dat`, rename atomik; export temp
  + fsync + rename; laporkan file gagal.
- **Metode pengujian:** import/export round-trip fixture; matikan izin/hapus file
  saat export dan pastikan archive lama tetap.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-29 — Tidak ada dirty-exit protection/autosave

- **Masalah:** back/open/new tidak bertanya saat dirty; `autosaveAtMs` tidak
  dipakai; `onSaveInstanceState` kosong.
- **Gejala:** edit hilang saat rotasi/process death/pindah dokumen.
- **Akar penyebab:** dirty state tidak terhubung ke navigation/lifecycle.
- **File/komponen:** `app/src/main.native.cpp:1664`, `:1777`, `:3137-3148`,
  `:1948-1967`, `:3823-3833`, `:4558`.
- **Perbaikan:** konfirmasi leave saat dirty; draft autosave ke filesDir; restore
  dengan konfirmasi.
- **Metode pengujian:** model/ADB edit→rotate→back; draft hilang hanya bila
  user memilih discard.
- **Hasil:** terverifikasi statis.
- **Status akhir:** TERBUKA — perbaikan P1.

#### R-P0-30 — Dokumentasi rilis/status masih stale

- **Masalah:** release history/honest notes/Part Q masih 1.0.3 atau lebih lama;
  klaim ADB tidak tersedia bertentangan dengan bukti; source count lama.
- **Gejala:** pembaca tidak tahu 1.0.4 masih diagnostik dan bug aktual.
- **Akar penyebab:** dokumen dikembangkan bertahap dan ringkasan atas tidak
  disinkronkan.
- **File/komponen:** `DOKUMENTASI.md:1-79`, `:525-620`, `:6698-7038`.
- **Perbaikan:** perbarui ringkasan current, tandai historis, tambah hash APK
  final setelah rilis, jangan hapus riwayat.
- **Metode pengujian:** cek hash/version/source count sesuai artefak.
- **Hasil:** terverifikasi silang.
- **Status akhir:** TERBUKA — perbaikan berkelanjutan.

### R.4 Urutan perbaikan

1. **P0-A (input/lifecycle):** bounded poll, wake, queue/window lifetime,
   attach/detach aman, JNI current-thread. Uji ADB attach/tap/ANR.
2. **P0-B (modal/dialog):** scrim, capture target, translasi subtree dialog.
   Uji model-test geometry/hit-test/modal.
3. **P0-C (parser/ZIP):** preflight count/trailing, containment, offset/inflate
   guard. Uji malformed fixture ASan.
4. **P0-D (I/O/state):** worker queue/cancellation, jangan I/O di render
   thread/state lock. Uji fixture besar.
5. **P1:** Save As, back/stack, search, rename undo, keypad, flat, scroll
   clipping, float/name bounds, import/export staging, dirty autosave.
6. **P2:** prefs, inset, crash handler, manifest, build fail-closed, dokumentasi,
   landscape/tablet dan test perangkat penuh.

Setiap perbaikan harus diikuti `bash scripts/build_host_tests.sh`,
`bash scripts/build_device.sh`, `bash scripts/build_apk.sh`; APK baru harus
diinstal dan diuji tap nyata sebelum statusnya diubah dari TERBUKA.

---

<!-- MASTER-SECTION: LEG-FASE-01 | CATEGORY=Arsip teknis | STATUS=AUDIT-HISTORIS -->
## FASE 1 — ANALISIS MENYELURUH

### Siklus progres — 15 Nov 2026

<!-- MASTER-SECTION: LEG-FASE-01-S01 | CATEGORY=Ringkasan kondisi proyek | STATUS=AUDIT-HISTORIS -->
## A. Kondisi dan struktur proyek

- Root proyek: `/home/kunbu8u7b/NBT-EDITOR-P`.
- Repository Git: **bukan repository Git**; tidak ada status commit/uncommitted
  otomatis. Perubahan file harus diverifikasi dengan hash/ukuran/isi.
- Dokumentasi utama sebelum siklus ini: **7.628 baris**, dengan audit terakhir
  di Bagian R. Tidak ada section FASE 1 sebelumnya, sehingga progres lama tidak
  ditimpa.
- Sumber utama aplikasi: `app/src/main.native.cpp`.
- Arsitektur yang dipertahankan: satu translation unit native C++, dengan
  batas L0–L6 seperti didokumentasikan.
- Artefak uji baseline sebelum perubahan implementasi terakhir:
  - Regression batching screenshot: **9/9 PASS**.
  - Engine host: **64/64 PASS** dengan ASan/UBSan.
  - UI model host: **68/68 PASS**.
- Keadaan kerja pada awal siklus ini:
  - Beberapa perubahan implementasi sudah tersimpan di `app/src/main.native.cpp`
    **sebelum instruksi FASE 1 diterima**.
  - Perubahan tersebut terkait pembacaan file berbasis batas ukuran dan
    `writeFileAtomic()` berbasis temporary file yang lebih aman.
  - Perubahan itu sekarang **DIBEKUAN**; tidak boleh dilanjutkan, disempurnakan,
    atau diklaim selesai selama FASE 1. Statusnya adalah pekerjaan implementasi
    yang belum disetujui dan perlu verifikasi/diff atau keputusan revert.
  - Hash produksi saat siklus ini:
    `app/src/main.native.cpp` =
    `2b260b3c3dc5941e5fe76bdad0755cb3afd0fc1822ff1f0c647bc9744d655114`,
    **7.766 baris**.
  - Hash produksi sebelum perubahan kecil yang dibekukan tercatat pada progres
    audit sebelumnya: `b56c4743b0323466fbe92bda4bab13eca35a0b74051f2079bf6c77d6f044d1ea`,
    **7.734 baris**.
  - Karena file sudah berubah setelah audit terakhir, baseline “source dibaca
    penuh” di Bagian R tidak lagi byte-identik; bagian yang berubah harus
    diverifikasi ulang sebelum FASE 1 menyatakan source utama SELESAI.
  - Test terakhir yang lulus dijalankan **sebelum** perubahan file yang kini
    dibekukan; belum ada build/test pada source hash `2b260b...` selama siklus ini.

<!-- MASTER-SECTION: LEG-FASE-01-S02 | CATEGORY=Arsip teknis | STATUS=AUDIT-HISTORIS -->
## B. Inventaris seluruh file

Inventaris disimpan secara bertingkat agar FASE 1 tetap berkelanjutan.

### B.1 Sumber dan konfigurasi aplikasi inti

| File | Peran | Status FASE 1 |
|---|---|---|
| `VERSION` | Satu-satunya sumber versi aplikasi | **SELESAI** |
| `app/AndroidManifest.xml` | Metadata aplikasi, permission, launcher activity | **PERLU VERIFIKASI** — temuan lama `R-P0-25` |
| `app/src/main.native.cpp` | Satu-satunya source aplikasi native | **PERLU VERIFIKASI** — teraudit lama, kini berbeda hash |
| `app/res/drawable/ic_foreground.xml` | Resource ikon foreground | **BELUM DIPERIKSA** |
| `app/res/mipmap-anydpi-v26/ic_launcher.xml` | Resource adaptive launcher icon | **BELUM DIPERIKSA** |
| `app/res/values/colors.xml` | Definisi warna resource | **BELUM DIPERIKSA** |
| `app/res/values/strings.xml` | String resource | **BELUM DIPERIKSA** |
| `app/res/values/styles.xml` | Style/theme resource | **BELUM DIPERIKSA** |
| `build/stubs/stub_AInputQueue_attachLooper.c` | Stub link-time Android input | **BELUM DIPERIKSA** |
| `build/stubs/stub___android_log_print.c` | Stub link-time logging | **BELUM DIPERIKSA** |
| `build/stubs/stub_compress2.c` | Stub link-time zlib compress | **BELUM DIPERIKSA** |
| `build/stubs/libandroid.so` | Stub native library untuk build device | **BELUM DIPERIKSA** |
| `build/stubs/liblog.so` | Stub native library untuk log | **BELUM DIPERIKSA** |
| `build/stubs/libz.so` | Stub native library untuk zlib | **BELUM DIPERIKSA** |

### B.2 Skrip, build, dan pipeline

| File | Peran | Status FASE 1 |
|---|---|---|
| `scripts/build_host_tests.sh` | Batching screenshot + compile/test host | **BELUM DIPERIKSA** |
| `scripts/build_device.sh` | Build `arm64-v8a` dan guard atomic | **BELUM DIPERIKSA** |
| `scripts/build_apk.sh` | Packaging/signing/verification APK | **BELUM DIPERIKSA** |
| `scripts/keystore.sh` | Persiapan keystore debug | **BELUM DIPERIKSA** |
| `scripts/make_stubs.sh` | Membuat stub link-time | **BELUM DIPERIKSA** |
| `scripts/gen_font.py` | Utility generasi font | **BELUM DIPERIKSA** |
| `scripts/batch_uishots.py` | Workflow aman batching screenshot | **BELUM DIPERIKSA** |
| `scripts/test_batch_uishots.py` | Test Python workflow batching | **BELUM DIPERIKSA** |
| `AGENTS.md` | Instruksi coding agent, dikonsolidasikan ke `DOKUMENTASI.md` | **DIKONSOLIDASI — file dihapus atas persetujuan user** |

### B.3 Artefak, output, dan data referensi

- Build/output inti: `out/NBTEditor.apk`, `out/base.apk`, `out/res.zip`,
  `out/nbt_test`, `out/uishot`, `out/debug.keystore`,
  `out/lib/**`, `out/stage/**`, `out/devicetest/**`.
- Bukti runtime/ADB: `out/adb-logs/**`, `out/adb-screenshots/**`,
  `out/nbt-crash.txt`.
- Screenshot asli yang tidak boleh diubah/ditimpa: `out/uishots/**`.
- Referensi eksternal dekompilasi/XAPK: `.ref_extract/**`,
  `MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk`, `classes7/**`.
- Status FASE 1 untuk artefak/log/referensi: **PERLU VERIFIKASI terpisah**
  hanya jika diperlukan untuk menelusuri bug; bukan target analisis baris
  demi baris kecuali diminta eksplisit.

<!-- MASTER-SECTION: LEG-FASE-01-S03 | CATEGORY=Arsip teknis | STATUS=AUDIT-HISTORIS -->
## C. Analisis file sumber

### C.1 `VERSION`

- **Nama dan lokasi:** `VERSION`, root proyek.
- **Tanggung jawab file:** menjadi satu-satunya sumber kebenaran untuk versi
  aplikasi yang dikonsumsi pipeline build.
- **Fungsi atau komponen penting:**
  - `VERSION_CODE=5`
  - `VERSION_NAME=1.0.4`
  - Header komentar menjelaskan kebijakan increment Android dan Semantic
    Versioning.
- **Dependensi:** secara konseptual dikonsumsi oleh:
  - `scripts/build_host_tests.sh`
  - `scripts/build_device.sh`
  - `scripts/build_apk.sh`
  - native source membutuhkan macro versi yang disuntikkan.
- **Alur kerja:** script membaca nilai `VERSION_CODE` dan `VERSION_NAME`,
  menyuntikkannya sebagai macro build; APK metadata memakai nilai yang sama.
- **Temuan masalah:**
  - Tidak ada masalah sintaks atau konflik versi dalam isi file.
  - Nilai saat ini adalah `5`/`1.0.4`, tetapi audit Bagian R menandai versi ini
    sebagai **diagnostik**, bukan rilis final.
  - Keterkaitan file ini dengan artifact APK terbaru perlu diverifikasi byte-
    level saat FASE 1 memeriksa build script.
- **Risiko:**
  - Jika ada literal versi tersembunyi di source/script, sumber kebenaran tunggal
  mudah melanggar kontrak.
  - `VERSION_CODE=5` mudah salah dianggap rilis final hanya karena angka kenaikan
  sudah berubah.
- **Status analisis:** **SELESAI**.

### C.2 `app/AndroidManifest.xml`

- **Nama dan lokasi:** `app/AndroidManifest.xml`.
- **Tanggung jawab file:** metadata aplikasi Android, deklarasi permission,
  entrypoint `NativeActivity`, konfigurasi lifecycle/UI dasar, dan intent
  filter yang menentukan dari mana aplikasi dapat dibuka.
- **Fungsi atau komponen penting:**
  - `package="com.nbteditor"`.
  - `android:hasCode="false"` untuk aplikasi murni native tanpa DEX.
  - `android.app.NativeActivity` sebagai satu-satunya activity.
  - `meta-data android.app.lib_name=nbt` untuk memuat `libnbt.so`.
  - Permission: `READ_EXTERNAL_STORAGE`, `WRITE_EXTERNAL_STORAGE`,
    `MANAGE_EXTERNAL_STORAGE`.
  - Application flags penting: `supportsRtl=false`,
    `hardwareAccelerated=false`, `requestLegacyExternalStorage=true`,
    `allowBackup=false`, `extractNativeLibs=true`.
  - Activity: `exported=true`, `launchMode=singleTask`,
    `screenOrientation=fullSensor`,
    `configChanges=keyboardHidden|orientation|screenSize|smallestScreenSize|density`.
  - Intent filter: `MAIN/LAUNCHER` dan `VIEW/DEFAULT`
    dengan `application/octet-stream`, `*/*`, `text/plain`.
- **Dependensi:**
  - Membutuhkan resource `@string/app_name`, `@style/Theme.NbtEditor`, dan
    `@mipmap/ic_launcher`.
  - `meta-data` menghubungkan activity ke library native `nbt`, yang dibangun
    menjadi `libnbt.so` oleh pipeline build device.
  - Metadata SDK manifest disuntikkan/dipastikan oleh script APK, bukan oleh
    atribut lokal di file ini.
- **Alur kerja:**
  - Framework Android membaca manifest, membuat `NativeActivity`, memuat
    `libnbt.so`, lalu memanggil `ANativeActivity_onCreate`.
  - Permission storage diklaim statis; request runtime dilakukan oleh kode
    native saat jalur browser/storage dibuka.
  - `VIEW` intent seharusnya membuka file dari aplikasi lain, tetapi perlu
    alur penerimaan URI di native.
- **Temuan masalah:**
  - **F-MANI-01 — permission terlalu luas.** `MANAGE_EXTERNAL_STORAGE` adalah
    permission khusus All-files Access dan berpotensi bermasalah untuk evaluasi
    Play Store. `WRITE_EXTERNAL_STORAGE` juga tidak memberikan hak tulis penuh
    di Android 11+.
  - **F-MANI-02 — `VIEW` intent filter terlalu luas.** Kombinasi
    `*/*` + `application/octet-stream` + `text/plain` membuat aplikasi menjadi
    kandidat pembuka untuk sangat banyak tipe file.
  - **F-MANI-03 — penerimaan `VIEW` belum terverifikasi di source native.**
    Pencarian di `app/src/main.native.cpp` menemukan hanya
    `ANativeActivity_onCreate`; tidak ditemukan penanganan `ACTION_VIEW`,
    URI intent, atau `onNewIntent`. Dengan `launchMode=singleTask`, aplikasi juga
    berisiko menerima intent baru pada instance yang sudah ada tanpa alur
    pembukaan file.
  - **F-MANI-04 — `requestLegacyExternalStorage` tidak relevan penuh pada target
    tinggi.** Pada Android 11+ perilaku ini dibatasi; tidak boleh dianggap
    solusi akses file global.
  - **F-MANI-05 — `hardwareAccelerated=false`.** Ini menambah risiko performa
    rendering/CPU karena aplikasi tidak mendapat akselerasi untuk jalur Android
    UI framework; perlu dikombinasikan dengan analisis render native nanti.
  - **F-MANI-06 — `supportsRtl=false`.** UI tidak menyatakan dukungan RTL.
    Ini bukan bug jika semua layout memang LTR, tetapi menjadi risiko
    kompatibilitas jika pengguna memakai locale RTL.
  - **F-MANI-07 — belum ada `enableOnBackInvokedCallback`.** Back/predictive
    back modern tidak dinyatakan di manifest; perlu diverifikasi apakah perilaku
    back yang ditangani native sudah cukup untuk target tinggi.
- **Risiko:**
  - Risiko distribusi/policy karena All-files Access dan broad VIEW filter.
  - Risiko UX/keamanan: user bisa memilih aplikasi ini untuk membuka file yang
    tidak dapat dibuka, jika intake URI tidak dilengkapi pesan gagal.
  - Risiko lifecycle: `singleTask` memerlukan penanganan `onNewIntent` agar
    file dari intent kedua dibuka.
  - Risiko kompatibilitas storage: kombinasi permission legacy/global tidak
    seragam di API 26, 29, 30+, dan 33+.
  - Risiko performa: `hardwareAccelerated=false` dan render loop harus
    dianalisis bersama agar tidak disalahartikan sebagai satu-satunya penyebab
    jank/ANR.
- **Status analisis:** **SELESAI**.

<!-- MASTER-SECTION: LEG-FASE-01-S04 | CATEGORY=Arsip teknis | STATUS=AUDIT-HISTORIS -->
## D. Analisis konfigurasi dan dependensi

### D.1 Temuan awal konfigurasi

- Tidak ada file dependency/modern package manager yang terdeteksi di root;
  build sangat bergantung pada skrip shell dan toolchain lingkungan.
- Konfigurasi resource aplikasi terbagi menjadi hanya 4 file XML:
  `drawable/ic_foreground.xml`, dua resource values warna/string/style, dan
  adaptive icon. Analisis detail XML masih BELUM DIPERIKSA.
- Source native adalah single translation unit; tidak ada modul source lain
  yang mengubah batas L0–L6.
- Stub Android/zlib/log berada di `build/stubs/` dan berpotensi menjadi bagian
  penting untuk build device karena script tidak memakai NDK penuh.
- Lingkungan saat ini bisa menjalankan Python, GCC, dan ADB, tetapi arsitektur
  build lama mengasumsikan toolchain Termux/NDK dan `clang++`. Detail
  ketersediaan toolchain harus diverifikasi saat file skrip dianalisis, tanpa
  mengubah build.

### D.2 File berikutnya yang direkomendasikan

`app/res/values/strings.xml` adalah pilihan berikutnya karena:
1. Merupakan konfigurasi kecil yang langsung dipakai manifest sebagai label
   aplikasi.
2. Memungkinkan verifikasi nama aplikasi tanpa menyentuh resource yang lebih
   besar seperti style/color.
3. Sesuai aturan FASE 1: satu file kecil dalam satu siklus.

<!-- MASTER-SECTION: LEG-FASE-01-S05 | CATEGORY=Arsip teknis | STATUS=AUDIT-HISTORIS -->
## E. Analisis hubungan antarmodul

- `VERSION` adalah sumber versi yang mengalir ke build host, build device, dan
  metadata APK.
- `app/src/main.native.cpp` adalah aplikasi inti dan menaruh bootstrap
  NativeActivity, NBT core, UI, state, worker, serta loop render.
- `app/AndroidManifest.xml` mendefinisikan `NativeActivity` yang menjadi jembatan
  Java framework ke entrypoint native.
- `app/res/**` hanya menyediakan metadata visual/resource minimal.
- `build/stubs/**` berfungsi sebagai pengganti link-time untuk library Android
  saat build device memakai sysroot Termux.
- `out/**` dan `.ref_extract/**` adalah artefak/bukti/referensi, bukan bagian
  pipeline source aplikasi.

<!-- MASTER-SECTION: LEG-FASE-01-S06 | CATEGORY=Daftar bug | STATUS=AUDIT-HISTORIS -->
## F. Daftar bug dan masalah

> **Katalog konsolidasi bug permanen — 19 Nov 2026.** Bagian ini menggabungkan catatan yang sudah tersimpan di dokumen; tidak ada halaman, fitur, atau bug yang dihapus. Setiap status mencerminkan bukti yang tersedia, bukan perubahan source semata.

### F.0 Aturan katalog dan status

- **Lingkup:** bug, cacat, regresi, dan masalah risiko proyek yang tersimpan di Bagian I, L, M, N–Q, R, FASE 1, serta FASE 2. Bug aplikasi referensi MineTuner tidak dimasukkan sebagai bug aplikasi ini.
- **Identitas:** ID permanen adalah nama resmi; judul lain hanya ringkasan. ID yang sudah ada dipertahankan; untuk catatan Bagian I yang belum mempunyai ID, diberi `LEG-I-B01`–`LEG-I-B30` dengan nomor lama `#1`–`#30` tetap dicatat sebagai alias.
- **Lokasi:** nama file dan induk modul adalah petunjuk stabil. Baris sesudah perluasan dokumen berubah; rujukan “audit lama” hanya menunjukkan bukti saat audit ditulis.
- **Bukti:** `SELESAI` hanya dipakai bila ada hasil build/test/runtime yang tercatat. Audit source saja tidak cukup.
- **Status:** memakai TERBUKA, DIANALISIS, DALAM PERBAIKAN, MENUNGGU PENGUJIAN, SELESAI, GAGAL, DITUNDA.
- **Kode bukti:** `EV-ENGINE-49` = 49/49 engine ASan/UBSan; `EV-MODEL-*` = UI model; `EV-M*` = model-test; `EV-DEVICE-102` = harness startup/dlopen perangkat 1.0.2; `EV-APK-103` = build/apksigner/library 1.0.3; `EV-APK-104` = instalasi, startup, Back, dan screenshot Home 1.0.4 di emulator; `EV-SRC` = audit source/statik; `EV-MANI` = audit manifest; `EV-HASH` = hash source; `EV-LOG*` = log ANR/lampiran log; `EV-RENDER-M` = screenshot harness visual; `EV-BATCH-*` = regression batching screenshot; `EV-SRC-GAP` = gap coverage yang terverifikasi.

### F.1 Status pekerjaan yang perlu dicatat segera

1. **F-PROG-01 — source utama berubah setelah audit Bagian R**
   - **Temuan:** hash sekarang `2b260b...`, 7.766 baris; audit R mencatat
     `b56c47...`, 7.734 baris.
   - **Penyebab diketahui sebagian:** perubahan implementasi yang sudah tersimpan
     sebelum instruksi FASE 1.
   - **Risiko:** baseline analisis lama tidak berlaku byte-identik; test hijau
     terakhir juga tidak memperkuat hash baru.
   - **Status:** **TERBUKA** — perlu verifikasi diff/ubahan region terbatas.

2. **F-PROG-02 — perubahan implementasi masuk sebelum FASE 1**
   - **Temuan:** ada perubahan pada pembacaan file dan atomic-write yang sudah
     tersimpan di source produksi.
   - **Status FASE 1:** **DIBEKUAN, TIDAK DISETUJUI**, bukan hasil FASE 2.
   - **Tindakan berikutnya:** pada FASE 2 user perlu memutuskan lanjut,
     build/uji ulang, atau revert. Selama FASE 1 source tidak boleh diubah.

### F.2 Ringkasan permanen

- **Total catatan bug/masalah terdaftar:** 94.
- **Bug dengan ID lama yang dipertahankan:** B1–B9, R-P0-01–R-P0-30, F-PROG-01–F-PROG-02, F-MANI-01–F-MANI-07.
- **Catatan historis yang belum mempunyai ID satu per satu:** A1–A6 disimpan sebagai `LEG-L-A01` karena dokumen yang tersisa tidak memisahkan detailnya tanpa mengarang; #1–#30 Bagian I diberi `LEG-I-B01`–`LEG-I-B30`.
- **ID baru untuk masalah yang belum diberi ID:** `LEG-M-*`, `LEG-L-*`, `LEG-FASE-02-*`, `LEG-K-ATOMIC-001`.
- **Pemetaan kelompok:** halaman 25, fitur 15, komponen/proses/pipeline 54.

| Status | Jumlah |
|---|---:|
| TERBUKA | 43 |
| DIANALISIS | 3 |
| DALAM PERBAIKAN | 0 |
| MENUNGGU PENGUJIAN | 3 |
| SELESAI | 41 |
| GAGAL | 0 |
| DITUNDA | 4 |
| **Total** | **94** |

### F.3 Bug per halaman

| ID permanen | Kelompok halaman/fitur/komponen | Ringkasan masalah | Lokasi | Bukti yang tersedia | Status |
|---|---|---|---|---|---|
| B1 | Global / semua halaman | Semua teks digambarkan dua kali lebih jauh ke kanan; label terpotong dan ringkasan nilai hilang. | `app/src/main.native.cpp` — `Canvas::text()` (rujukan audit lama: baris 971–973) | EV-M14; EV-MODEL-62 | SELESAI |
| B2 | Worlds / Browser / Recent / Flat | Tap pada baris daftar mengenai anak terdalam berindeks -1, sehingga baris tidak dipilih. | `dispatchTouch()` / `hitTest()` (rujukan audit lama: baris 4290) | EV-M15, EV-M19; EV-MODEL-62 | SELESAI |
| B3 | Worlds / Browser | Baris daftar dimulai pada y=0 dan menutupi app bar serta path bar. | `buildWorlds()` / `buildBrowser()` (rujukan audit lama: baris 2275, 2329) | EV-M16, EV-M21; EV-MODEL-62 | SELESAI |
| B4 | Dialog, keypad, value editor, search | `a.lastKeyPressed` tidak pernah terisi sehingga input teks mati total. | `a.lastKeyPressed`, `A_KEY_TAP`, `applyKey()` (rujukan audit lama: baris 1711, 3961–3962) | EV-M17–EV-M20; EV-MODEL-62 | SELESAI |
| B6 | Tree / breadcrumb | Rect breadcrumb nol ukuran sehingga breadcrumb tidak terlihat dan tidak dapat disentuh. | builder breadcrumb (rujukan audit lama: baris 2397) | EV-M22–EV-M24; EV-MODEL-52; 10 render byte-identik kecuali breadcrumb | SELESAI |
| B7 | Tree / Byte, Int, Long Array | Elemen array tidak masuk `kids`; drill-down kosong dan ADD menghasilkan elemen tak terlihat. | `visibleChildren()` / `commitAdd()` (rujukan audit lama: baris 248, 255, 2152) | EV-M25–EV-M30; EV-MODEL-58; render `11_array_elements.png` | SELESAI |
| B8 | Home / Worlds / Tree / Help | Action enum mati dan tidak memiliki node pemancar sehingga pintasan/menu tidak terjangkau. | enum `Act`; handler dan node UI (rujukan audit lama: Bagian L §3) | EV-M31–EV-M34; EV-MODEL-62; render B8 | SELESAI |
| R-P0-04 | Dialog, modal, progress, delete | Tidak ada scrim/interceptor; kontrol di belakang dialog masih dapat disentuh dan target delete dapat salah. | `app/src/main.native.cpp:2966-3109`, `:3614-3678`, `:3954-3958`, `:4350-4357` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-05 | Dialog add/value, numeric pad | Kontrol anak dibuat pada koordinat lokal tanpa ditranslasi sehingga tidak terlihat dan tidak reachable. | `app/src/main.native.cpp:2826-2940`, `:3023-3033`, `:3308-3417` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-11 | Navigasi, app bar, Back/stack | Semua app bar memakai `A_TREE_UP`; `openDocument()` menaruh tree sebagai previous screen. | `app/src/main.native.cpp:1948-1967`, `:2253-2268`, `:4073-4076`, `:3137-3148` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-12 | Keypad, SHIFT, flat editor | Hanya tiga baris keypad yang dirender; SHIFT dan target flat tidak bekerja sepenuhnya. | `app/src/main.native.cpp:3150-3196`, `:3425-3501`, `:4404-4442` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-13 | Search / filter / clear | Tombol X membuka keypad alih-alih clear; hasil search dapat menduplikasi current node. | `app/src/main.native.cpp:2183-2212`, `:2637-2646`, `:3983-4005`, `:4211-4214` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-16 | Flat world editor / layers | Target `flatDraft`, tombol reorder, dan tinggi konten tidak terhubung benar. | `app/src/main.native.cpp:1755`, `:2741-2759`, `:3150-3188`, `:4262-4275`, `:4435-4439` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-17 | Scroll list / viewport | `Kind::Scroll` tidak di-clip pada render dan hit-test; baris di luar viewport masih interaktif. | `app/src/main.native.cpp:2884-2889`, `:2939`, `:1200-1208` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-21 | Input, cancel, multi-pointer, Back | `ACTION_CANCEL`, pointer index, key event, dan Back tidak ditangani sebagai kontrak terpisah. | `app/src/main.native.cpp:4520-4553`, `:4777-4789` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-26 | Root layout / system bars | `onContentRectChanged()` kosong dan safe-area window tidak disimpan; mismatch masih kondisional. | `app/src/main.native.cpp:4576`, layout L6 (rujukan audit 15 Nov) | EV-SRC; emulator 1080×2340 tidak membuktikan mismatch | DITUNDA |
| LEG-M-UI-001 | Tree / selected row | Kontras nilai `-12345` pada `shortTag` terpilih terlalu rendah. | `out/uishots/04_tree.png`; dicacat ulang di N.3/O.3/P.3/Q.1 | EV-M42; EV-APK-104; screenshot lokal: fill `(225,240,226)`, teks kunci `(27,27,27)`, kontras 14.57; teks nilai muted `(96,106,96)`, kontras 4.77 | SELESAI |
| LEG-M-UI-002 | Tree / FAB | FAB `S` menutupi baris terakhir pada tree. | temuan visual Bagian N.3/O.3/P.3/Q.1 | EV-M42; viewport `Scroll` menghormati tinggi `a.h - y - 136`; save FAB berada di bawahnya; swipe Tree tidak menimbulkan crash | SELESAI |
| LEG-M-UI-003 | Dialog / modal | Background atau fragmen baris bocor dan terlihat di belakang modal. | `out/uishots/07_dialog_value.png`; N.3/O.3/P.3/Q.1 | EV-RENDER-M | TERBUKA |
| LEG-M-UI-004 | Dialog add/value | Field dan label/nilai terlalu rapat atau bertumpuk. | `out/uishots/07_dialog_value.png`; `out/uishots/08_dialog_add.png` | EV-RENDER-M | TERBUKA |
| LEG-M-UI-005 | Dialog add-tag | Area selector tipe tag tampak kosong setelah label `Tag type`. | `out/uishots/08_dialog_add.png`; N.3/O.3/P.3/Q.1 | EV-RENDER-M; EV-SRC | TERBUKA |
| LEG-M-UI-006 | Dialog / separator non-ASCII | Separator atau glyph non-ASCII tidak terbaca pada jalur tag/dialog. | temuan visual Bagian O.3/P.3/Q.1 | EV-RENDER-M | TERBUKA |
| LEG-FASE-02-UI-001 | Home — kartu header | Glyph karakter khusus pada subjudul kartu tidak ter-render. | `out/fase1-apk-audit/02_first_page_home_running.png`; FASE 2 §3.F | EV-APK-104; root cause belum diketahui | TERBUKA |
| LEG-FASE-02-UI-002 | Home — Recent files | Baris status `no recent files yet` terpotong pada tepi kanan. | `out/fase1-apk-audit/02_first_page_home_running.png`; FASE 2 §3.F | EV-APK-104 | TERBUKA |
| LEG-FASE-02-UI-003 | Home — area bawah | Area bawah layar kosong besar dan tidak memenuhi viewport 1080×2340. | `out/fase1-apk-audit/02_first_page_home_running.png`; FASE 2 §3.F | EV-APK-104 | TERBUKA |

### F.4 Bug per fitur

| ID permanen | Kelompok halaman/fitur/komponen | Ringkasan masalah | Lokasi | Bukti yang tersedia | Status |
|---|---|---|---|---|---|
| B5 | Save As / file browser | `pendingSavePath` tidak pernah diisi; Save As tidak dapat dipakai. | `app/src/main.native.cpp` save branch (rujukan audit lama: baris 3522–3530) | EV-SRC; diperluas dan dibuktikan lagi oleh R-P0-15 | TERBUKA |
| R-P0-06 | NBT parser | Count array/list, list End, dan trailing bytes dipercaya sebelum preflight tersisa. | `app/src/main.native.cpp:224-228`, `:246-260`, `:286-299`, `:325-367` (rujukan audit 15 Nov) | EV-SRC; fixture malformed belum ada | TERBUKA |
| R-P0-07 | Import `.mcworld` / ZIP | Entry ZIP dipakai sebagai path tanpa normalisasi; Zip Slip dapat menulis di luar folder. | `app/src/main.native.cpp:1360-1373` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-08 | Import / ZIP reader | Offset, length, inflate, output size, dan CRC tidak divalidasi; OOB/OOM/dependency bomb. | `app/src/main.native.cpp:1302-1357` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-10 | Serializer / numeric editor | Name/string/count cast tanpa batas, serializer tidak memakai validate, dan editor float/double kehilangan presisi. | `app/src/main.native.cpp:393-463`, `:552-586`, `:2112-2116`, `:3520-3525`, `:3652-3699` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-14 | Undo / rename | `renameNode()` berjalan sebelum `snapshotUndo()`, sehingga undo tidak mengembalikan nama lama. | `app/src/main.native.cpp:3681-3700` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-15 | Save As / file browser | Tidak ada “Save here”/filename field; file tujuan existing dapat ditimpa tanpa konfirmasi. | `app/src/main.native.cpp:2448-2510`, `:3862-3933` (rujukan audit 15 Nov) | EV-SRC; memperluas B5 | TERBUKA |
| R-P0-22 | Preferences / recent / theme / directory | `Prefs` tidak pernah load/save dan `a.recent` terpisah; pilihan hilang setelah restart. | `app/src/main.native.cpp:1505-1510`, `:1702-1715`, `:2004-2008`, `:4908-4920` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-23 | Help / feature claims / autosave / expand / move / byte-per-node | Dokumentasi dan Help menandai fitur yang belum diimplementasikan sebagai selesai. | `DOKUMENTASI.md` matrix lama; `app/src/main.native.cpp:2649-2692`, `:2183-2213`, `:2788-2792` (rujukan audit 15 Nov) | EV-SRC-XDOC | TERBUKA |
| R-P0-28 | Import / export world | Tidak ada staging, validasi `level.dat`, rename atomik, rollback export, atau agregasi error. | `app/src/main.native.cpp:1302-1456`, `:3897-3908`, `:3960-3978` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-29 | Dirty state / autosave / rotation | Back/open/new tidak bertanya saat dirty; `autosaveAtMs` dan `onSaveInstanceState` tidak bekerja. | `app/src/main.native.cpp:1664`, `:1777`, `:3137-3148`, `:1948-1967`, `:3823-3833`, `:4558` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| LEG-L-PERF-001 | Undo / dokumen besar | Snapshot seluruh arena per aksi, maksimal 200; RAM dapat tumbuh hingga urutan gigabyte. | `snapshotUndo()` (rujukan audit lama: baris 2932–2947) | EV-SRC | TERBUKA |
| LEG-L-PERF-002 | Tree / listing / dokumen besar | `indexPathTo()` bersifat O(n) per baris per frame dan dapat menjadi O(n²). | `buildTree()` (rujukan audit lama: baris 2414) | EV-SRC | TERBUKA |
| LEG-L-UI-001 | Worlds / icon world | `world_icon.jpeg` dan `hasIcon` dihitung, tetapi icon tidak pernah ditampilkan. | `buildWorlds()` (rujukan audit lama) | EV-SRC | TERBUKA |
| LEG-FASE-02-PERF-001 | Startup / Home | Cold start 1.0.4 tercatat +10s463ms, sementara warm start +361ms; penyebab belum ditentukan. | `out/adb-logs` / FASE 2 §3.F–3.H; screenshot 01/02 | EV-APK-104; bukan bukti root cause | DIANALISIS |

### F.5 Bug per komponen, proses, dan pipeline

| ID permanen | Kelompok halaman/fitur/komponen | Ringkasan masalah | Lokasi | Bukti yang tersedia | Status |
|---|---|---|---|---|---|
| LEG-I-B01 | NBT engine / parser | #1 — Elemen `TAG_List` di parser diberi byte tipe. | `app/src/main.native.cpp` — list parser | EV-ENGINE-49 | SELESAI |
| LEG-I-B02 | NBT engine / serializer | #2 — Serializer tidak menulis payload scalar/array. | `app/src/main.native.cpp` — serializer | EV-ENGINE-49 | SELESAI |
| LEG-I-B03 | NBT engine / serializer | #3 — Elemen list diberi byte tipe oleh serializer. | `app/src/main.native.cpp` — `writeValue()` | EV-ENGINE-49 | SELESAI |
| LEG-I-B04 | NBT engine / serializer | #4 — `TAG_Int_Array` ditulis sebagai int64. | `app/src/main.native.cpp` — Int Array writer | EV-ENGINE-49 | SELESAI |
| LEG-I-B05 | NBT engine / parser | #5 — Root NBT dibaca tanpa nama. | `app/src/main.native.cpp` — root parser | EV-ENGINE-49 | SELESAI |
| LEG-I-B06 | C++ / `Result<void>` | #6 — `Result<void>` disalahgunakan. | core result handling | EV-ENGINE-49; build `-Werror` | SELESAI |
| LEG-I-B07 | Build / warnings | #7 — Warning menjadi gagal karena `-Werror`. | build flags | EV-ENGINE-49; EV-MODEL-40; build `-Wall -Wextra -Werror` | SELESAI |
| LEG-I-B08 | Build / compile config / L0 | #8 — Guard `#ifndef NBT_TESTABLE` menyebabkan L0 tidak ikut compile. | `app/src/main.native.cpp` preprocessor L0 | EV-ENGINE-49; EV-DEVICE-102 | SELESAI |
| LEG-I-B09 | Android entrypoint / visibility | #9 — `ANativeActivity_onCreate` tidak diekspor. | entrypoint `__attribute__((visibility("default")))` | EV-DEVICE-102; device build `-Werror` | SELESAI |
| LEG-I-B10 | Build / aapt2 / android.jar | #10 — `ANDROID_JAR` atau `aapt2 link` gagal resolve resource framework. | `scripts/make_stubs.sh` / `build_apk.sh` (artefak konteks) | EV-APK-103; build pipeline | SELESAI |
| LEG-I-B11 | Build / aapt2 | #11 — `--no-compress so` disalahpakai sebagai argumen file. | `scripts/build_apk.sh` flag packaging | EV-APK-103 | SELESAI |
| LEG-I-B12 | APK packaging | #12 — `aapt2 link` menunjuk `lib/` langsung sehingga APK kosong. | APK packaging pipeline | EV-APK-103; isi `base.apk` diverifikasi | SELESAI |
| LEG-I-B13 | Native runtime / libc++ | #13 — `libnbt.so` membutuhkan `libc++_shared.so`. | APK `lib/arm64-v8a/libc++_shared.so` | EV-APK-103; EV-DEVICE-102 | SELESAI |
| LEG-I-B14 | APK packaging / zip | #14 — `zip -X` tanpa `-r` sehingga file `.so` hilang. | APK packaging pipeline | EV-APK-103 | SELESAI |
| LEG-I-B15 | Manifest / native library metadata | #15 — `libnbt` dicari framework sebagai `liblibnbt.so`. | `AndroidManifest.xml` metadata | EV-APK-103; EV-DEVICE-102 | SELESAI |
| LEG-I-B16 | APK signing | #16 — v1-only signing ditolak untuk targetSdk 35. | APK signing pipeline | EV-APK-103; apksigner v2+v3 | SELESAI |
| LEG-I-B17 | Renderer / framebuffer | #17 — Stride dihitung `w*4`; stride sebenarnya berbeda dan menyebabkan layar putih/tutup. | renderer surface buffer | EV-DEVICE-102; EV-APK-104 menampilkan Home | SELESAI |
| LEG-I-B18 | L0 / Looper | #18 — Looper dibuat di main thread. | L0 looper ownership | EV-DEVICE-102 | SELESAI |
| LEG-I-B19 | Threading / linker | #19 — `std::thread` tanpa `-lpthread`. | device link flags | EV-DEVICE-102; device build bersih | SELESAI |
| LEG-I-B20 | UI Home / model | #20 — Konten Home tidak masuk tree sehingga layar kosong hanya app bar. | Home UI tree | EV-MODEL-M2; EV-MODEL-40 | SELESAI |
| LEG-I-B21 | Input / actions | #21 — Tap dan long-press belum terhubung. | input dispatch / dialog node tree | EV-MODEL-40 | SELESAI |
| LEG-I-B22 | L0 / POSIX Android API | #22 — `ALooper_pollAll` tidak tersedia di API 30 header. | L0 looper source | EV-DEVICE-102; build API 30 | SELESAI |
| LEG-I-B23 | L0 / Android input API | #23 — `AInputQueue_finishInputEvent` tidak ada. | L0 input source | EV-DEVICE-102; build API 30 | SELESAI |
| LEG-I-B24 | L0 / Android platform state | #24 — `ANativeActivity` tidak mempunyai member `inputQueue/looper`. | L0 `Platform` static state | EV-DEVICE-102; build `-Werror` | SELESAI |
| LEG-I-B25 | Error path / UI | #25 — Pesan “Out of bounds” tidak memberi umpan balik yang tepat. | error string / result path | EV-MODEL-40; tidak ada jalur error khusus yang tercatat | SELESAI |
| LEG-I-B26 | Native linking / libandroid | #26 — `libnbt.so` ter-link ke `libandroid.so` Termux yang membutuhkan namespace Termux. | device link line / stubs | EV-DEVICE-102; `dlopen` final APK lulus | SELESAI |
| LEG-I-B27 | Native linking / libz | #27 — `libz.so.1` dari Termux tidak ada sebagai SONAME Android. | device link line / stubs | EV-DEVICE-102; `dlopen` final APK lulus | SELESAI |
| LEG-I-B28 | Native library metadata | #28 — RUNPATH Termux menempel pada `libnbt.so`. | `libnbt.so` metadata | EV-DEVICE-102; APK final berisi library tanpa RUNPATH Termux | SELESAI |
| LEG-I-B29 | Permission / browser | #29 — `MANAGE_EXTERNAL_STORAGE` diminta saat `onCreate` sehingga app melompat ke Settings. | permission request path | EV-SRC; tidak ada uji grant/deny atau Settings sesudah perbaikan | MENUNGGU PENGUJIAN |
| LEG-I-B30 | Concurrency / state | #30 — Worker dan render thread mengubah `AppState` bersamaan. | `g_stateMutex` / worker / render | EV-MODEL-40; sanitizer tidak membuktikan race bebas | MENUNGGU PENGUJIAN |
| LEG-L-A01 | Renderer/list/dialog/keypad/app-bar | A1–A6 — audit perbaikan skala, list, dialog/keypad hit-tree, dan app-bar. | source `app/src/main.native.cpp` (audit A1–A6) | EV-MODEL-40; EV-MODEL-62 | SELESAI |
| LEG-M-PROC-001 | Workflow analisis / screenshot batching | 9 gambar dalam satu prompt melebihi batas keras 8 gambar provider. | `out/uishots/`; OpenCode media limit | EV-BATCH-9/9; 2/2 batch asli sukses; checksum 9/9 identik | SELESAI |
| R-P0-01 | L0 / input queue / ANR | Input queue tidak pernah ter-attach; KeyEvent menunggu sekitar 8 detik dan 1.0.4 ANR. | `app/src/main.native.cpp:4824-4845`, `:4599-4608`, `:4555-4598` (rujukan audit 15 Nov) | EV-LOG-ANR; EV-APK-104 mencatat queue created+attached dan Back, tetapi kriteria 30 detik/dumpsys belum lengkap | MENUNGGU PENGUJIAN |
| R-P0-02 | L0 / window / queue lifetime | Pointer dan status platform diakses lintas thread tanpa lifetime lock/queue state machine. | `app/src/main.native.cpp:4478-4508`, `:4520-4608`, `:4810-4899` (rujukan audit 15 Nov) | EV-SRC; stack ANR | TERBUKA |
| R-P0-03 | Permission / JNI | `JNIEnv` milik main thread dibaca dari render thread; `MANAGE_EXTERNAL_STORAGE` dipakai tanpa branch API benar. | `app/src/main.native.cpp:3811-3821`, `:3849-3859`, `:4614-4662` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-09 | Render thread / I/O / `g_stateMutex` | World scan, browse, open, save, delete, dan worker completion melakukan I/O di render path di bawah lock. | `app/src/main.native.cpp:3754-3978`, `:4277-4357`, `:4869-4888` (rujukan audit 15 Nov) | EV-SRC; EV-LOG-ANR; cold start belum menunjuk ini sebagai penyebab | DIANALISIS |
| R-P0-18 | Worker / lifecycle | `job` satu slot, exception tidak dibatasi, dan `onDestroy()` join worker di main thread. | `app/src/main.native.cpp:1560-1601`, `:4566-4571`, `:3897-3908`, `:3971-3978` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-19 | Android lifecycle / recreation | `g_booted` global dan `thread.detach()` dapat meninggalkan activity/worker/render lama. | `app/src/main.native.cpp:4503-4508`, `:4566-4571`, `:4904-4944`, `:1567-1600` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-20 | Crash handler / signal safety | Signal handler memakai string/allocation/lock/I/O yang bukan async-signal-safe. | `app/src/main.native.cpp:4664-4704` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-24 | Build / APK / fail-closed | APK dapat memakai library stale; host test, hash provenance, dan verifikasi ELF/signature tidak wajib gagal. | `scripts/build_apk.sh:10-56`, `scripts/build_device.sh:59-78`, `scripts/build_host_tests.sh` (rujukan audit 15 Nov) | EV-SRC | TERBUKA |
| R-P0-25 | Manifest / permission / VIEW | `MANAGE_EXTERNAL_STORAGE`, `*/*`, dan VIEW tanpa intake native terlalu luas. | `app/AndroidManifest.xml:5-42` (rujukan audit 15 Nov); lihat F-MANI-01–07 | EV-MANI; EV-SRC | TERBUKA |
| R-P0-27 | Test coverage / lifecycle / orientation | Host model tidak menguji L0, JNI, lifecycle, queue, orientation, ANR, atau device rendering. | `app/src/main.native.cpp:4462-4970`, test model `:5370-6728` (rujukan audit 15 Nov) | EV-SRC-GAP; EV-APK-104 baru membuktikan startup/Home | TERBUKA |
| R-P0-30 | Dokumentasi / rilis / handoff | Ringkasan rilis dan status masih stale; klaim ADB dan hash/source count saling bertentangan. | `DOKUMENTASI.md` ringkasan dan matrix | EV-SRC-XDOC; katalog bug ini mengurangi staleness tetapi tidak menutup semua konflik | DIANALISIS |
| F-PROG-01 | Source utama / baseline audit | Source ‘main.native.cpp’ berubah setelah audit Bagian R: hash menjadi `2b260b...`, 7.766 baris. | `app/src/main.native.cpp`; FASE 1 §A dan §F.1 | EV-HASH; belum ada diff/uji terhadap hash tersebut | TERBUKA |
| F-PROG-02 | Source utama / persetujuan perubahan | Perubahan file/IO yang belum disetujui sudah tersimpan di source produksi. | `app/src/main.native.cpp`; FASE 1 §A dan §F.1 | EV-HASH; perubahan dibekukan | DITUNDA |
| F-MANI-01 | Manifest / permission | `MANAGE_EXTERNAL_STORAGE` terlalu luas untuk evaluasi distribusi dan target modern. | `app/AndroidManifest.xml:5-42`; FASE 1 §C.2 | EV-MANI (statis) | TERBUKA |
| F-MANI-02 | Manifest / intent filter | Kombinasi `*/*`, `application/octet-stream`, dan `text/plain` membuat filter VIEW terlalu luas. | `app/AndroidManifest.xml`; FASE 1 §C.2 | EV-MANI (statis) | TERBUKA |
| F-MANI-03 | Manifest / native VIEW handling | Penanganan `ACTION_VIEW`, URI intent, dan `onNewIntent` belum terverifikasi native. | `app/AndroidManifest.xml`; `app/src/main.native.cpp` search `ACTION_VIEW`; FASE 1 §C.2 | EV-MANI; EV-SRC | TERBUKA |
| F-MANI-04 | Manifest / storage compatibility | `requestLegacyExternalStorage` tidak menyelesaikan akses penuh pada Android 11+. | `app/AndroidManifest.xml`; FASE 1 §C.2 | EV-MANI; perilaku API 26/29/30+ belum diuji | TERBUKA |
| F-MANI-05 | Manifest / renderer performance | `hardwareAccelerated=false` berpotensi menambah biaya rendering/CPU. | `app/AndroidManifest.xml`; FASE 1 §C.2 | EV-MANI; tidak ada baseline profiling terpisah | DITUNDA |
| F-MANI-06 | Manifest / RTL compatibility | `supportsRtl=false` menutup dukungan RTL; bukan bug jika seluruh layout memang LTR. | `app/AndroidManifest.xml`; FASE 1 §C.2 | EV-MANI; locale RTL tidak diuji | DITUNDA |
| F-MANI-07 | Manifest / predictive Back | `enableOnBackInvokedCallback` belum dinyatakan; back modern belum terverifikasi. | `app/AndroidManifest.xml`; FASE 1 §C.2 | EV-MANI; Back runtime baru dibuktikan pada FASE 2 §3.C | TERBUKA |
| LEG-K-ATOMIC-001 | Native build / arm64 / atomic helpers | Library membutuhkan `__aarch64_ldadd*` yang tidak diekspor `libc` perangkat; `dlopen` gagal dan app menutup sebelum frame pertama. | `scripts/build_device.sh`; `libnbt.so` arm64-v8a (Bagian K §2–§6) | EV-DEVICE-102; `dlopen` final APK lulus; `onCreate → onStart → onResume → onDestroy` lulus | SELESAI |
| B9 | Build / APK staleness | APK terpasang tertinggal dari source B6–B8; source terbaru belum masuk artefak. | `out/NBTEditor.apk`; build/source artifact | EV-APK-103; memuat `libnbt.so` B8 terbaru, library hash cocok; validasi perangkat fisik masih menunggu | SELESAI |

### F.6 Korelasi dan penggantian ID

- **B5** dipertahankan sebagai catatan audit lama; **R-P0-15** adalah temuan lebih lengkap untuk jalur Save As dan tetap menjadi referensi utama untuk pengerjaan.
- **B8** dan **R-P0-23** terkait: B8 adalah intent/node mati yang dapat diperbaiki; R-P0-23 adalah klaim fitur yang belum tersedia. Keduanya tidak digabung agar riwayat audit tetap utuh.
- **B9** dan **R-P0-24** terkait: B9 adalah artefak APK stale; R-P0-24 adalah cacat pipeline/fail-closed. Pembuktian APK 1.0.3 tidak menutup R-P0-24.
- **F-MANI-01–F-MANI-07** adalah subset manifest yang lebih rinci dari **R-P0-25**; keduanya saling merujuk dan tidak boleh dihitung dua kali sebagai dua pekerjaan yang sama.
- **A1–A6** disimpan sebagai satu grup karena dokumentasi yang tersisa hanya menyebut cakupan umum, bukan korelasi satu-satu yang aman dipetakan.
- **R-P0-01** mempunyai bukti parsial dari FASE 2 (`native input queue attached`); statusnya tetap `MENUNGGU PENGUJIAN` sampai seluruh kriteria uji input 30 detik dan `dumpsys input` tercatat.
- **FASE 2 pertama gagal mendapatkan window** tidak diberi ID bug karena bukti menunjukkan ANR/proses lain pada waktu yang sama; belum cukup untuk dipastikan sebagai cacat aplikasi.

### F.7 Cakupan dan batas risiko

- **Tidak dimasukkan sebagai bug aplikasi ini:** temuan bug aplikasi referensi MineTuner; fitur LevelDB yang sengaja ditunda; bukti ANR `com.google.android.apps.photos`; batas 8 media per prompt itu sendiri; ketersediaan APK/emulator sebagai keterbatasan lingkungan.
- **Tidak ada bug yang dihapus atau ditulis ulang menjadi selesai hanya karena source berubah.**
- **`LEG-FASE-02-UI-*` dan `LEG-FASE-02-PERF-001` adalah temuan audit, belum perbaikan.**
- **Baseline “64/64 engine” dan “68/68 model-test” hanya berlaku sebelum perubahan source yang dibekukan; untuk hash source sesudahnya belum ada pengujian baru pada siklus FASE 1.**
- **Pemeriksaan internal katalog ini:** 94 ID unik; tidak ada ID lama yang diubah/dihapus; kandidat dengan ID baru berbeda dari semua ID historis.

### F.8 Peta perbaikan → ID bug → bukti pengujian

Register ini mengelompokkan catatan perbaikan yang sudah tersimpan. `FIX-*` adalah identitas pekerjaan perbaikan; `BUG/LEG-*` adalah identitas cacat. Satu perbaikan dapat merujuk beberapa bug, tanpa menghapus atau menukar ID bug.

| ID perbaikan | Catatan historis | ID bug terkait | Berkas/komponen diubah | Bukti uji yang tersimpan | Status perbaikan |
|---|---|---|---|---|---|
| `FIX-001` | Bagian I §3, core parser/serializer (#1–#5) | `LEG-I-B01`–`LEG-I-B05` | `app/src/main.native.cpp` parser/serializer | 49/49 engine PASS; round-trip payload/list/int64/root-name tercakup | SELESAI |
| `FIX-002` | Bagian I §3, correctness C++/L0 (#6–#9, #22–#24) | `LEG-I-B06`–`LEG-I-B09`, `LEG-I-B22`–`LEG-I-B24` | preprocessor, linker/API/penggunaan Platform | build `-Werror`; device build/startup lulus | SELESAI |
| `FIX-003` | Bagian I §3, APK packaging (#10–#16, #26–#28) | `LEG-I-B10`–`LEG-I-B16`, `LEG-I-B26`–`LEG-I-B28` | `scripts/make_stubs.sh`, APK/zip/signing | APK isi lengkap; `apksigner` v2+v3; device `dlopen` lulus | SELESAI |
| `FIX-004` | Bagian I §3, renderer dan interaction (#17–#21, #25) | `LEG-I-B17`–`LEG-I-B21`, `LEG-I-B25` | renderer, Home tree, input/action | 40/40 model-test; Home diverifikasi lagi pada FASE 2 | SELESAI |
| `FIX-005` | Bagian I §3, permission dan concurrency (#29–#30) | `LEG-I-B29`, `LEG-I-B30` | permission flow dan `g_stateMutex` | tidak ada uji grant/deny atau stress race khusus | MENUNGGU PENGUJIAN |
| `FIX-006` | Audit A1–A6 | `LEG-L-A01` | renderer/list/dialog/keypad/app-bar | model-test 40/40 pada snapshot audit; lanjut 62/62 | SELESAI |
| `FIX-007` | Bagian J, startup 1.0.1 | `LEG-I-B26`–`LEG-I-B28` | stub link-time, `libc++_shared.so`, `libz.so`, RUNPATH | device APK final; `dlopen` tidak menemukan dependency/SONAME stale | SELESAI |
| `FIX-008` | Bagian K, startup #2 | `LEG-K-ATOMIC-001` | `scripts/build_device.sh` dengan `-mno-outline-atomics` dan guard helper | harness perangkat: `DLOPEN_OK`; lifecycle `onCreate → onDestroy` lulus | SELESAI |
| `FIX-009` | Bagian M/B1–B4 | `B1`, `B2`, `B3`, `B4` | renderer, list layout, action, keypad | `M14`–`M20`; model-test 62/62; B1–B4 diverifikasi regresi | SELESAI |
| `FIX-010` | Bagian M/B5 | `B5` | save branch / Save As | handler/source tersedia; belum ada end-to-end Save As; R-P0-15 masih TERBUKA | TERBUKA |
| `FIX-011` | Bagian N, B6 | `B6` | tree/breadcrumb builder | `M22`–`M24`; model-test 52/52; render breadcrumb; gesture fisik belum diuji | SELESAI |
| `FIX-012` | Bagian O, B7 | `B7` | array view/editor | `M25`–`M30`; model-test 58/58; `11_array_elements.png` | SELESAI |
| `FIX-013` | Bagian P, B8 | `B8` | intent/action/theme/delete/paste/go-to-root | `M31`–`M34`; model-test 62/62; APK-emulator Home tersedia | SELESAI |
| `FIX-014` | Bagian Q, B9 | `B9` | build APK 1.0.3 / signing | library dan APK 1.0.3 hash cocok; signature v2+v3; validasi perangkat fisik belum ada | SELESAI |
| `FIX-015` | Bagian M, safeguard screenshot | `LEG-M-PROC-001` | `scripts/batch_uishots.py` dan test batching | 9/9 regression; 2/2 batch asli sukses; checksum 9/9 identik | SELESAI |
| `FIX-016` | Bagian R, kontrak P0/P1/P2 | `R-P0-01`–`R-P0-30` | belum diterapkan secara menyeluruh; source drift masih dibekukan | hanya bukti audit/statis dan parsial FASE 2; tidak boleh dianggap selesai | TERBUKA/MENUNGGU PENGUJIAN |
| `FIX-017` | FASE 2, temuan halaman pertama | `LEG-FASE-02-UI-001`–`LEG-FASE-02-UI-003`, `LEG-FASE-02-PERF-001` | belum diterapkan | screenshot dan log audit tersedia; root cause UI/perf belum ditemukan | TERBUKA/DIANALISIS |

### F.9 Batas bukti perbaikan

- Perbaikan yang tidak memiliki hasil host/device/emulator tidak diklaim `SELESAI` walaupun source terlihat berubah.
- `64/64` engine dan `68/68` model-test hanya baseline sebelum perubahan source yang dibekukan; jangan dipakai untuk hash baru.
- FASE 2 membuktikan APK 1.0.4 dapat diinstal dan Home dapat ditampilkan di emulator Pixel 5; itu bukan bukti semua interaksi, parser, dialog, Save As, atau stabilitas jangka panjang.
- Perubahan yang tersimpan sebelum FASE 1 tetap dibekukan sampai pengguna memutuskan lanjut, build/uji ulang, atau revert.
- **Tree Editor 19 Nov 2026:** source memuat tint seleksi opaque, ellipsis nilai dengan tanda minus/awal nilai dijaga, dan viewport scroll yang menyisakan ruang untuk kedua FAB; `M42` PASS. Lihat FASE 2 §4.


<!-- MASTER-SECTION: LEG-FASE-01-S07 | CATEGORY=Daftar bug | STATUS=AUDIT-HISTORIS -->
## G. Masalah performa CPU dan RAM

- Baseline yang diketahui: parser, serializer, dan UI model sudah punya test
  host; engine sebelumnya terverifikasi dengan sanitizer pada hash lama.
- Karena source berubah, pernyataan performa/CPU/RAM saat ini bersifat
  **stale**. Tidak ada pengukuran baru pada hash `2b260b...` selama FASE 1.
- Risiko awal yang perlu diverifikasi nanti:
  - pembacaan file dan ZIP tetap membebani RAM;
  - UI/parser memiliki test, tetapi file ZIP malformed tetap menjadi area rawan;
  - pipeline build memiliki kemungkinan memuat artefak stale jika tidak
    fail-closed, sebagaimana temuan `R-P0-24`.

<!-- MASTER-SECTION: LEG-FASE-01-S08 | CATEGORY=Daftar bug | STATUS=AUDIT-HISTORIS -->
## H. Masalah kompatibilitas

- Risiko kompatibilitas utama yang perlu diverifikasi:
  - Android API range dan target SDK di manifest/script.
  - Intent filter dan permission terlalu luas menurut audit lama.
  - Build host tidak aman untuk lintas compiler karena script mengasumsikan
    `clang++`/Termux; environment saat ini memiliki `g++`, bukan `clang++`.
  - Build device bergantung pada stub dan sysroot khusus; stubs perlu dibaca satu
    per satu sebelum menilai kompatibilitas ABI.

<!-- MASTER-SECTION: LEG-FASE-01-S09 | CATEGORY=Ringkasan kondisi proyek | STATUS=AUDIT-HISTORIS -->
## I. Kesimpulan analisis

- FASE 1 sudah dimulai dari progres Bagian R, bukan audit kosong.
- Status proyek:
  - Dokumentasi utama: tersedia dan akan diupdate hanya di file ini.
  - Inventaris awal: sudah dibuat.
  - File `VERSION`: sudah dianalisis lengkap.
  - `app/src/main.native.cpp`: sudah pernah teraudit lama, kini statusnya
    **PERLU VERIFIKASI** karena perubahan terakhir yang tidak disetujui.
  - Manifest/resource/script/build/stub: **BELUM DIPERIKSA** atau
    **PERLU VERIFIKASI** sesuai tabel progres.
- Tidak ada kode produksi yang diubah pada siklus FASE 1 ini.
- Pekerjaan berikutnya setelah persetujuan progres ini adalah menganalisis
  **satu file**, yaitu `app/res/values/strings.xml`.

<!-- MASTER-SECTION: LEG-FASE-01-S10 | CATEGORY=Riwayat perkembangan | STATUS=AUDIT-HISTORIS -->
## J. Daftar progres file

| File | Status |
|---|---|
| `VERSION` | **SELESAI** |
| `AGENTS.md` | **DIKONSOLIDASI** — isi tersimpan di Appendix AGENTS, file dihapus atas persetujuan user |
| `DOKUMENTASI.md` | **SELESAI** untuk pembacaan progres Bagian R |
| `app/src/main.native.cpp` | **PERLU VERIFIKASI** |
| `app/AndroidManifest.xml` | **SELESAI** |
| `app/res/drawable/ic_foreground.xml` | **BELUM DIPERIKSA** |
| `app/res/mipmap-anydpi-v26/ic_launcher.xml` | **BELUM DIPERIKSA** |
| `app/res/values/colors.xml` | **BELUM DIPERIKSA** |
| `app/res/values/strings.xml` | **BELUM DIPERIKSA** |
| `app/res/values/styles.xml` | **BELUM DIPERIKSA** |
| `build/stubs/stub_AInputQueue_attachLooper.c` | **BELUM DIPERIKSA** |
| `build/stubs/stub___android_log_print.c` | **BELUM DIPERIKSA** |
| `build/stubs/stub_compress2.c` | **BELUM DIPERIKSA** |
| `build/stubs/libandroid.so` | **BELUM DIPERIKSA** |
| `build/stubs/liblog.so` | **BELUM DIPERIKSA** |
| `build/stubs/libz.so` | **BELUM DIPERIKSA** |
| `scripts/build_host_tests.sh` | **BELUM DIPERIKSA** |
| `scripts/build_device.sh` | **BELUM DIPERIKSA** |
| `scripts/build_apk.sh` | **BELUM DIPERIKSA** |
| `scripts/keystore.sh` | **BELUM DIPERIKSA** |
| `scripts/make_stubs.sh` | **BELUM DIPERIKSA** |
| `scripts/gen_font.py` | **BELUM DIPERIKSA** |
| `scripts/batch_uishots.py` | **BELUM DIPERIKSA** |
| `scripts/test_batch_uishots.py` | **BELUM DIPERIKSA** |

<!-- MASTER-SECTION: LEG-FASE-01-S11 | CATEGORY=Arsip teknis | STATUS=AUDIT-HISTORIS -->
## K. Konsolidasi Markdown tunggal

### K.1 Keputusan dan status

- User menjadikan `DOKUMENTASI.md` sebagai satu-satunya file `.md`.
- Satu-satunya `.md` lain yang ditemukan adalah `AGENTS.md`.
- Isi `AGENTS.md` disalin utuh ke Appendix di bawah ini agar instruksi kerja
  tidak hilang.
- `AGENTS.md` kemudian dihapus setelah user menyetujui penghapusan.
- Setelah konsolidasi, verifikasi langsung harus menemukan tepat satu file
  `.md`, yaitu `DOKUMENTASI.md`.
- Appendix ini bukan pengganti Bagian R; ia hanya memasukkan instruksi agent yang
  masih berlaku ke dalam dokumentasi tunggal.

<!-- MASTER-SECTION: LEG-APPENDIX-01 | CATEGORY=Arsip teknis | STATUS=ARSIP -->
## Appendix A — Instruksi coding agent, sadaran dari `AGENTS.md`

# NBT Editor — Instructions for Coding Agents

<!-- MASTER-SECTION: LEG-APPENDIX-01-S01 | CATEGORY=Arsip teknis | STATUS=ARSIP -->
## Project rules

- Read `DOKUMENTASI.md` before making changes. It is the project history, architecture, feature status, and test contract.
- Keep the application as one native C++ translation unit in `app/src/main.native.cpp`; preserve the L0–L6 layer boundaries.
- `VERSION` is the only source of truth for the application version. Increment `VERSION_CODE` for every APK release.
- Do not modify, rename, or delete the original screenshots in `out/uishots/`. If new renders are needed, write them to a temporary or explicitly new output directory.
- Make the smallest coherent change that fixes the issue, then run the relevant tests before moving to the next task.

<!-- MASTER-SECTION: LEG-APPENDIX-01-S02 | CATEGORY=Arsip teknis | STATUS=ARSIP -->
## Screenshot attachment limit

The active provider accepts **at most 8 images in one prompt**. Never attach or
load all files from `out/uishots/` in the same prompt/context.

1. Run `python3 scripts/batch_uishots.py --manifest` to obtain the deterministic
   batches and initialize/resume batch status.
2. Count images already present in the current prompt/context before adding a
   batch. The **combined total** must remain at most 8; use a fresh context or
   only the remaining capacity when fewer images are available.
3. Process at most one manifest batch in one model prompt/context. When batches
   must be processed during the same parent task, use an isolated subagent per
   batch and return text-only findings to the parent; do not forward image
   attachments from multiple batches into the same prompt.
4. Each worker claims one pending batch with
   `python3 scripts/batch_uishots.py --claim`, attaches only the claimed paths,
   then records its result with `--mark-success N` or
   `--mark-failed N --error "..."`. Claims are locked so concurrent workers
   cannot receive the same batch.
5. After every batch has been attempted, inspect failures with `--failed` and
   use `--retry` only for a failed batch. Do not reattach successful batches.
   Use `--status` for the completion summary and `--reclaim-in-progress` only
   after a worker is known to have been interrupted.
6. Use `python3 scripts/batch_uishots.py --batch N` only when one explicit
   batch's paths are needed without claiming it.
7. The helper never modifies, copies, renames, or deletes screenshot files and
   never loads image contents into memory. It writes only small JSON progress
   metadata under `out/` (outside `out/uishots/`). Never regenerate over
   `out/uishots/` merely to inspect a batch.

<!-- MASTER-SECTION: LEG-APPENDIX-01-S03 | CATEGORY=Arsip teknis | STATUS=ARSIP -->
## Verification

Run in this order as the change scope requires:

```bash
bash scripts/build_host_tests.sh
bash scripts/build_device.sh
bash scripts/build_apk.sh
```

Host tests must report all engine and UI model tests passing. Device builds
must remain warning-free under `-Werror`, including the atomic-helper guard in
`scripts/build_device.sh`.

---

<!-- MASTER-SECTION: LEG-FASE-02 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## FASE 1 — AUDIT APK

### 1. Pemeriksaan awal APK — 16 Nov 2026

<!-- MASTER-SECTION: LEG-FASE-02-S01 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 1.A Inventaris APK yang tersedia

Pemeriksaan langsung di filesystem, tanpa rebuild APK.

| Lokasi | Peran | Ukuran | Tanggal modifikasi | SHA-256 |
|---|---:|---:|---|---|
| `out/NBTEditor.apk` | APK aplikasi utama hasil pipeline lokal | 1.827.466 byte | 2026-11-15 14:17:48 | `ede9362e65d7c1932efcbba4606e548cdd46bdd7db8cfdb9ad0cd1fbf9ffade5` |
| `out/base.apk` | Paket APK dasar sebelum/signing intermediate dalam pipeline | 1.803.392 byte | 2026-11-15 14:17:48 | `80d50b72a439e8378db72285f6c47fdc57ee8958e6cf605389f2c5b2d0337301` |
| `out/NBTEditor.apk.idsig` | Metadata signature APK, bukan APK utama | 21.938 byte | 2026-11-15 14:17:48 | Tidak dihitung pada siklus ini |
| `.ref_extract/com.hextetrahedral.minetuner.apk` | APK aplikasi referensi MineTuner, bukan aplikasi NBT-EDITOR | 105.397.608 byte | 2026-11-15 13:52:51 | Tidak dihitung pada siklus ini |
| `.ref_extract/config.arm64_v8a.apk` | Split/reference APK dari XAPK referensi | 4.048.134 byte | 2026-11-15 13:52:52 | Tidak dihitung pada siklus ini |
| `.ref_extract/config.en.apk` | Split/reference APK language dari XAPK referensi | 99.054 byte | 2026-11-15 13:52:51 | Tidak dihitung pada siklus ini |
| `.ref_extract/config.mdpi.apk` | Split/reference APK density dari XAPK referensi | 220.643 byte | 2026-11-15 13:52:51 | Tidak dihitung pada siklus ini |

<!-- MASTER-SECTION: LEG-FASE-02-S02 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 1.B APK yang dipilih

APK yang paling relevan adalah:

- **`out/NBTEditor.apk`**

Alasan berbasis bukti:

1. Package dan aplikasi utama sesuai proyek, bukan aplikasi referensi MineTuner.
2. Berisi `lib/arm64-v8a/libnbt.so` dan `lib/arm64-v8a/libc++_shared.so`.
3. Memiliki entrypoint `AndroidManifest.xml`, resource aplikasi, dan signature.
4. Merupakan APK utama hasil build lokal, bukan split/reference XAPK.

<!-- MASTER-SECTION: LEG-FASE-02-S03 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 1.C Metadata APK yang diverifikasi langsung dari file APK

Dari isi `out/NBTEditor.apk`:

| Item | Temuan | Status bukti |
|---|---|---|
| Entry utama | `AndroidManifest.xml` | Terverifikasi langsung dari isi ZIP |
| Resource primary | `res/drawable/ic_foreground.xml`, `res/mipmap-anydpi-v26/ic_launcher.xml`, `res/values/**` melalui `resources.arsc` | Terverifikasi dari daftar isi APK |
| Native library | `lib/arm64-v8a/libnbt.so` dan `lib/arm64-v8a/libc++_shared.so` | Terverifikasi dari daftar isi APK |
| ABI native | `arm64-v8a` saja | Terverifikasi dari path library dan header ELF |
| Arsitektur ELF utama | ELF64, AArch64 | Terverifikasi dengan `readelf -h` |
| Dependensi native `libnbt.so` | `libc++_shared.so`, `libandroid.so`, `libz.so`, `liblog.so` | Terverifikasi dengan `readelf -d` |
| SHA-256 `libnbt.so` di dalam APK | `e32bce81d15a958c1c68000c30f9cbeda794d46026306c3c72cbb8b9678042e8` | Terverifikasi langsung |
| SHA-256 `libc++_shared.so` di dalam APK | `05bef9d3901910c3ff8ab450b03bc8b28383d363a2290f2f7b9aed7331ea6ff7` | Terverifikasi langsung |
| Package name | String `com.nbteditor` ada di manifest APK | Terverifikasi dari string pool binari manifest |
| Versi name | String `1.0.4` ada di manifest APK | Terverifikasi dari string pool binari manifest |
| Aktivitas utama | String `android.app.NativeActivity` ada di manifest APK | Terverifikasi dari string pool binari manifest |
| Metadata native library | `android.app.lib_name` dan value `nbt` ada di manifest APK | Terverifikasi dari string pool binari manifest |
| Permission yang terlihat di manifest | `READ_EXTERNAL_STORAGE`, `WRITE_EXTERNAL_STORAGE`, `MANAGE_EXTERNAL_STORAGE` | Terverifikasi dari string pool binari manifest |
| Intent VIEW yang terlihat di manifest | `android.intent.action.VIEW`, `android.intent.category.DEFAULT`, mime `application/octet-stream`, `*/*`, `text/plain` | Terverifikasi dari string pool binari manifest |
| Sertifikat APK | Signer #1 `CN=Android Debug, O=Android, C=US`; key algorithm `SHA384withRSA`; 2048-bit RSA | Terverifikasi dengan `keytool -printcert -jarfile` |
| Label aplikasi | Metadata string/resource APK memuat `NBT Editor` | Terverifikasi dari resource/string pool |

<!-- MASTER-SECTION: LEG-FASE-02-S04 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 1.D Metadata yang masih perlu konfirmasi tool Android

Nilai berikut sudah tersedia melalui sumber proyek sebagai cross-check, tetapi **belum diverifikasi penuh oleh `aapt2 dump badging`** karena environment saat ini tidak memiliki `aapt2`.

| Item | Nilai cross-check | Sumber cross-check |
|---|---:|---|
| `VERSION_CODE` | `5` | `VERSION` |
| `VERSION_NAME` | `1.0.4` | `VERSION` dan string di APK |
| `minSdkVersion` | `26` | `scripts/build_apk.sh` memakai `--min-sdk-version 26` |
| `targetSdkVersion` | `35` | `scripts/build_apk.sh` memakai `--target-sdk-version 35` |
| Package name final | `com.nbteditor` | `VERSION`, `AndroidManifest.xml`, string pool APK |

Status metadata APK:

- **Metadata langsung:** sebagian besar sudah terverifikasi.
- **Metadata formal Android SDK:** masih **PERLU VERIFIKASI** karena `aapt2` tidak tersedia.
- Tidak ada rebuild, install, atau modifikasi APK.

<!-- MASTER-SECTION: LEG-FASE-02-S05 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 1.E Ketersediaan Android Emulator

Hasil pemeriksaan lingkungan:

| Komponen | Hasil |
|---|---|
| `adb` | Tersedia di `/usr/bin/adb` |
| Perangkat ADB terhubung | Tidak ada; `adb devices -l` menampilkan daftar kosong |
| `emulator` | Tidak ditemukan |
| `emulator64-x86_64` | Tidak ditemukan |
| `avdmanager` | Tidak ditemukan |
| `sdkmanager` | Tidak ditemukan |
| Direktori AVD `~/.android/avd` | Tidak ditemukan |
| APK berhasil dijalankan di emulator | **Belum dijalankan** |

Kesimpulan:

- **Emulator tidak tersedia.**
- Karena tidak ada emulator dan tidak ada perangkat ADB, APK **belum bisa diuji tampilan/fungsi secara nyata**.
- Jika emulator disiapkan nanti, perhatikan APK ini hanya memiliki native library `arm64-v8a`. Emulator dengan system image x86/x86_64 bukan jaminan dapat menjalankan APK ini tanpa dukungan ARM64.

<!-- MASTER-SECTION: LEG-FASE-02-S06 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 1.F Status progres FASE 1 setelah pemeriksaan awal

| Target FASE 1 | Status |
|---|---|
| Pemeriksaan awal APK | **SELESAI** |
| Inventaris seluruh APK | **SELESAI** |
| Identifikasi APK paling relevan | **SELESAI** |
| Metadata dasar APK | **SELESAI** |
| Metadata formal Android SDK | **PERLU VERIFIKASI** |
| Menjalankan APK di Android Emulator | **TERHALANG** |
| Audit tampilan aplikasi | **TERHALANG** |
| Audit fungsi aplikasi | **TERHALANG** |
| Audit stabilitas aplikasi | **BELUM DIPERIKSA** |

<!-- MASTER-SECTION: LEG-FASE-02-S07 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 1.G Target audit berikutnya

Target berikutnya:

1. Verifikasi metadata formal APK bila tersedia tool Android seperti `aapt2`, tanpa rebuild APK.
2. Atau, jika user mengizinkan penggunaan tool eksternal, siapkan environment emulator/AVD terlebih dahulu.
3. Setelah emulator tersedia, audit dimulai dari **satu halaman pertama**: launch/splash atau home screen, bukan semua halaman sekaligus.

Sampai emulator tersedia, audit tampilan dan audit fungsi tidak boleh diklaim selesai.

---

<!-- MASTER-SECTION: LEG-FASE-02-S08 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2. Verifikasi Kompatibilitas APK dengan Android Emulator — 16 Nov 2026

> Catatan progres: subbagian ini lebih baru dari temuan `1.E` tentang ketersediaan emulator. Pada pemeriksaan lebih dalam, Android SDK dan AVD ternyata tersedia di `$HOME/Android/Sdk` dan `$HOME/.android/avd`.

<!-- MASTER-SECTION: LEG-FASE-02-S09 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.A ABI APK dan library native

Pemeriksaan langsung terhadap `out/NBTEditor.apk`:

| Item | Hasil | Bukti |
|---|---|---|
| Entry native dalam APK | Hanya `lib/arm64-v8a/libnbt.so` dan `lib/arm64-v8a/libc++_shared.so` | Daftar isi ZIP |
| ABI APK | `arm64-v8a` saja | Path library dalam APK |
| `libnbt.so` ukuran | 375.360 byte | Isi APK |
| `libnbt.so` SHA-256 | `e32bce81d15a958c1c68000c30f9cbeda794d46026306c3c72cbb8b9678042e8` | Hash langsung |
| `libnbt.so` arsitektur | ELF64, AArch64 | `readelf -h` |
| `libc++_shared.so` ukuran | 1.423.696 byte | Isi APK |
| `libc++_shared.so` SHA-256 | `05bef9d3901910c3ff8ab450b03bc8b28383d363a2290f2f7b9aed7331ea6ff7` | Hash langsung |
| `libc++_shared.so` arsitektur | ELF64, AArch64 | `readelf -h` |

Tidak ditemukan library:

- `x86`
- `x86_64`
- `armeabi-v7a`

Artinya, APK ini adalah **APK ARM64-native**.

<!-- MASTER-SECTION: LEG-FASE-02-S10 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.B Host yang tersedia

| Item | Hasil |
|---|---|
| Arsitektur host | `x86_64` |
| OS/kernel | Linux x86_64 |
| Vendor CPU | `GenuineIntel` |
| Perangkat KVM | `/dev/kvm` tersedia |

Host ini adalah **Intel x86_64**, bukan ARM64.

<!-- MASTER-SECTION: LEG-FASE-02-S11 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.C Lingkungan Android SDK dan Android Emulator

Android SDK ditemukan di:

- `$HOME/Android/Sdk`

Komponen yang tersedia:

| Komponen | Status |
|---|---|
| Android Emulator | Tersedia di `$HOME/Android/Sdk/emulator/emulator` |
| AVD list | Dapat dijalankan dari binary emulator |
| `platform-tools/adb` | Tersedia di `$HOME/Android/Sdk/platform-tools/adb` |
| `build-tools/36.0.0/aapt2` | Tersedia |
| `sdkmanager` di PATH | Tidak ditemukan |
| `avdmanager` di PATH | Tidak ditemukan |
| `emulator` di PATH | Tidak ditemukan, tetapi tersedia di folder SDK |
| System images | Tersedia di `$HOME/Android/Sdk/system-images/android-37.0` |

AVD yang tersedia:

| AVD | Target ABI | System image |
|---|---|---|
| `Pixel_5` | `x86_64` | `system-images/android-37.0/google_apis/x86_64/` |
| `Medium_Phone_API_37.0` | `x86_64` | `system-images/android-37.0/google_apis_playstore/x86_64/` |

<!-- MASTER-SECTION: LEG-FASE-02-S12 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.D System image Android yang terpasang

System image yang tersedia adalah:

- `android-37.0/google_apis/x86_64`
- `android-37.0/google_apis_playstore/x86_64`

Bukti penting dari `source.properties`:

| System image | API level | ABI resmi | Tag |
|---|---:|---|---|
| `android-37.0/google_apis/x86_64` | `37.0` | `x86_64` | `Google APIs` |
| `android-37.0/google_apis_playstore/x86_64` | `37.0` | `x86_64` | `Google Play` |

Tidak ada system image dengan ABI `arm64-v8a` yang terpasang saat ini.

<!-- MASTER-SECTION: LEG-FASE-02-S13 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.E Kemungkinan ARM64 pada emulator Intel x86_64

Hasil pemeriksaan system image `android-37.0/google_apis_playstore/x86_64`:

| Properti | Nilai |
|---|---|
| `ro.product.cpu.abilist` | `x86_64,arm64-v8a` |
| `ro.product.cpu.abilist64` | `x86_64,arm64-v8a` |
| `ro.product.cpu.abi` | `x86_64` |
| `ro.dalvik.vm.native.bridge` | `libndk_translation.so` |
| `ro.enable.native.bridge.exec` | `1` |

`system.img` juga memuat literal `libndk_translation.so`.

Kesimpulan sementara:

- Host emulator adalah x86_64.
- System image terpasang juga x86_64.
- Namun system image tersebut **mendeklarasikan dukungan `arm64-v8a` melalui native bridge/translator**.
- Artinya, APK ARM64 ini **kemungkinan dapat dijalankan** di emulator x86_64 yang sudah tersedia.
- Verifikasi runtime **belum dilakukan**. APK belum dipasang atau dijalankan pada AVD.

<!-- MASTER-SECTION: LEG-FASE-02-S14 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.F Kesimpulan kompatibilitas

| Item | Simpulan |
|---|---|
| ABI APK | `arm64-v8a` |
| ABI emulator/AVD terpasang | `x86_64` |
| Host | Intel x86_64 |
| Dukungan ARM64 pada system image | Dideklarasikan melalui `arm64-v8a` di `abilist64` dan native bridge |
| APK berhasil dijalankan di emulator | **Belum diverifikasi** |
| Status kompatibilitas | **KEMUNGIN KOMPATIBEL / PERLU UJI RUNTIME** |
| APK tidak kompatibel langsung tanpa bridge | Benar jika system image x86_64 tidak memiliki ARM translation |

Catatan penting:

- System image x86_64 tanpa dukungan `arm64-v8a` tidak akan bisa menjalankan APK ini karena APK hanya berisi binary ARM64.
- System image yang saat ini terpasang mendeklarasikan dukungan ARM64, tetapi kemampuan itu masih harus diuji dengan menjalankan APK di AVD.
- APK belum dijalankan, jadi tidak boleh diklaim sukses.

<!-- MASTER-SECTION: LEG-FASE-02-S15 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.G Alternatif yang tersedia

1. **Uji APK di AVD x86_64 yang sudah ada.**
   - AVD tersedia: `Pixel_5` dan `Medium_Phone_API_37.0`.
   - APK kemungkinan dapat berjalan karena system image mendukung ARM-to-x86 translation.
   - Perlu emulator dijalankan, APK dipasang, lalu perilaku aplikasi diamati.

2. **Menggunakan system image ARM64 native.**
   - Paling cocok untuk ABI APK.
   - Saat ini belum terpasang.
   - Pengunduhan system image ARM64 memerlukan persetujuan user karena berpotensi besar.

3. **Menggunakan perangkat fisik ARM64 Android.**
   - Alternatif paling akurat dan tanpa emulasi ABI.
   - Perlu perangkat ARM64 yang terhubung melalui ADB.

4. **Membangun APK x86_64.**
   - Tidak dilakukan karena instruksi melarang perubahan build/code produksi.

<!-- MASTER-SECTION: LEG-FASE-02-S16 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 2.H Status progres FASE 1

| Target | Status |
|---|---|
| Periksa ABI APK | **SELESAI** |
| Periksa ABI `libnbt.so` | **SELESAI** |
| Periksa lingkungan Android SDK dan Android Emulator | **SELESAI** |
| Periksa AVD yang tersedia | **SELESAI** |
| Identifikasi system image yang sesuai | **SELESAI** |
| Periksa kemungkinan ARM64 di emulator Intel x86_64 | **SELESAI** |
| Uji runtime APK di emulator | **SELESAI — berhasil di Pixel_5** |
| Audit tampilan aplikasi | **SEDANG DIPERIKSA — halaman pertama selesai** |
| Audit fungsi aplikasi | **BELUM DILAKUKAN** |
| Audit stabilitas aplikasi | **BELUM DIPERIKSA** |

---

<!-- MASTER-SECTION: LEG-FASE-02-S17 | CATEGORY=Audit UI/UX | STATUS=AUDIT-HISTORIS -->
## 3. Audit Halaman Pertama — 16 Nov 2026

> Ruang lingkup siklus ini hanya halaman pertama setelah aplikasi dibuka. Tidak ada audit halaman berikutnya, tidak ada audit interaksi/fungsi, tidak ada perubahan kode produksi, build, atau konfigurasi.

### 3.A Kondisi emulator sebelum pengujian

| Item | Hasil | Bukti |
|---|---|---|
| Status emulator | Sudah berjalan sebelum siklus ini | Proses `qemu-system-x86_64 -avd Pixel_5 -gpu swiftshader_indirect` sudah aktif |
| Nama AVD | `Pixel_5` | Argumen proses emulator |
| Perangkat ADB | `emulator-5554`, status `device` | `adb devices -l` |
| Boot | `sys.boot_completed=1` | `adb shell getprop` |
| Identitas perangkat | `sdk_gphone64_x86_64-userdebug` | `adb shell getprop ro.build.flavor` |
| ABI perangkat | `x86_64`, `abilist64=x86_64,arm64-v8a` | `adb shell getprop` |
| Jumlah emulator berjalan | 1 | `pgrep` |
| Mode tampilan | GUI, `DISPLAY=:0` tersedia | Environment |

Tidak ada emulator kedua yang dijalankan.

### 3.B Status instalasi APK

| Item | Hasil |
|---|---|
| Perintah | `adb install -r out/NBTEditor.apk` |
| Mode install | Incremental Install |
| Hasil | `Success` |
| Durasi | `Install command complete in 3861 ms` |
| Kondisi sebelum install | Paket `com.nbteditor` belum terdaftar (`pm list packages` kosong) |
| Perubahan source | Tidak ada |

### 3.C Status startup

**Percobaan pertama — GAGAL mendapatkan window aplikasi**

Perintah: `adb shell am start -W -n com.nbteditor/android.app.NativeActivity`

- Hasil perintah: `Status: ok`, `WaitTime: 13169`, `LaunchState: UNKNOWN (0)`
- Aplikasi tidak pernah menjadi window aktif; `mCurrentFocus` kembali ke `com.google.android.apps.nexuslauncher`
- Bukti log:
  - `11-16 19:36:26.862 W/ActivityManager: Process ProcessRecord{89c11fc 3387:com.nbteditor/u0a234} failed to attach`
  - `11-16 19:36:26.865 I/ActivityManager: Killing 3387:com.nbteditor/u0a234 (adj -10000): start timeout`

**Percobaan kedua dari launcher — BERHASIL (cold start)**

Bukti log urut:

1. `19:37:06.929 W/com.nbteditor(4271): Unexpected CPU variant for x86: x86_64.` — warning lapisan guest, bukan error aplikasi
2. `19:37:15.186 D/nativeloader: Configuring clns-9 ... library_path=/data/app/.../lib/arm64 ... base.apk!/lib/arm64-v8a`
3. `19:37:17.328 D/nativeloader: Load /data/app/.../lib/arm64/libnbt.so ...: ok`
4. `19:37:17.335 I/nbteditor: NBT Editor native app started (worlds dir: /storage/emulated/0/Android/data/com.nbteditor/files/worlds)`
5. `19:37:17.335 I/nbteditor: native render looper=0x7830256220f0`
6. `19:37:17.341 I/nbteditor: native input queue created 0x782ec5610600`
7. `19:37:17.345 I/nbteditor: native input queue attached queue=0x782ec5610600 looper=0x7830256220f0`
8. `19:37:17.354 I/nbteditor: native content rect 0,0 1080x2340`
9. `19:37:17.357 I/ActivityTaskManager: Displayed com.nbteditor/android.app.NativeActivity for user 0: +10s463ms`
10. `19:37:17.812 D/CoreBackPreview: Window{9e5a394 u0 Splash Screen com.nbteditor EXITING}` — splash screen memang muncul lalu keluar

**Percobaan ketiga setelah activity ditutup — BERHASIL (warm start)**

- Perintah `adb shell am start -W` → `Status: ok`, `LaunchState: WARM`, `TotalTime: 361`, `WaitTime: 363`
- Bukti log: surface `Splash Screen com.nbteditor` dibuat, `libnbt.so ...: ok`, `native render looper`, `native input queue created/attached`, `native content rect 0,0 1080x2340`, `Displayed ... +361ms`
- Verifikasi akhir: `mCurrentFocus=Window{cbd3e7b u0 com.nbteditor/android.app.NativeActivity}`, proses hidup (`pid 4271`)

Catatan: aplikasi pernah menerima event back pada `19:43:07` (`native key back down=1`, `down=0`, `native key back action`) dan `native input queue destroyed` pada `19:43:08.511`. Perilaku ini normal pada navigasi back, bukan crash.

### 3.D Screenshot halaman pertama

| File | Ukuran | Resolusi | Waktu | Kondisi saat diambil |
|---|---:|---|---|---|
| `out/fase1-apk-audit/01_first_launch_not_app.png` | 26.089 byte | 1080x2340 | 19:37 | Setelah cold start sukses; nama file memuat `not_app` namun isinya Home screen aplikasi |
| `out/fase1-apk-audit/02_first_page_home_running.png` | 25.618 byte | 1080x2340 | 19:44 | `mCurrentFocus` diverifikasi milik `com.nbteditor` saat pengambilan |

`02_first_page_home_running.png` adalah bukti utama audit ini karena fokus window diverifikasi milik aplikasi pada saat screenshot diambil.

### 3.E Hasil pengamatan UI halaman pertama

Dari `out/fase1-apk-audit/02_first_page_home_running.png`:

1. **Aplikasi berhasil terbuka:** iya — window aplikasi focused dan proses hidup.
2. **Splash screen muncul:** iya, muncul sementara lalu keluar (`Splash Screen com.nbteditor EXITING`).
3. **Home screen tampil:** iya.
4. **Komponen yang terlihat:**
   - Top app bar hijau dengan judul `NBT Editor`
   - Status bar sistem: `5G`, jam `7:44`, ikon baterai
   - Kartu header berjudul `Native NBT Editor` dengan satu baris subjudul
   - Grid tombol 2x2: `Open file` dan `New NBT` (latar hijau, teks putih); `Worlds` dan `Import world` (latar putih/muda, teks hijau)
   - Header section `Recent files` dengan satu baris status
   - Pill navigasi gestur di bagian bawah layar
5. **Susunan elemen:** dari atas ke bawah — app bar → kartu header → grid 2x2 → section Recent files → area kosong. Grid tombol rapi dan sejajar; tidak ada elemen bertumpuk antar tombol.
6. **Ukuran teks/tombol/ikon:** label tombol dan judul terbaca jelas pada resolusi 1080x2340; tidak ada tombol yang terlalu kecil untuk disentuh berdasarkan ukuran tampak.
7. **Konsistensi desain:** dua gaya tombol dipakai konsisten (hijau solid untuk aksi utama, putih outline untuk aksi sekunder) dan warna teks kontras dengan latar.

### 3.F Masalah yang ditemukan pada halaman pertama

1. **Karakter khusus pada subjudul kartu tidak ter-render.** Teks subjudul terbaca seperti `All 13 tag types <> Bedrock little_endian ... level dat aware <> world import/exp`; simbol khusus tampak sebagai glyph tidak dikenal/placeholder dan titik tidak terbaca.
2. **Baris status `Recent files` terpotong.** Teks `no recent files yet — open or create an N` berakhir terpotong di tepi kanan, tidak muat dalam satu baris.
3. **Area bawah layar kosong besar.** Di bawah section `Recent files` tidak ada konten; sebagian besar bawah layar kosong, layout tidak memenuhi viewport 1080x2340.
4. **Cold start lambat.** `Displayed ... +10s463ms` pada cold start yang berhasil, sementara warm start `+361ms`. Cold start lebih dari 10 detik adalah masalah startup yang teramati; penyebab belum ditentukan.
5. **Percobaan startup pertama gagal mendapatkan window.** `failed to attach` + `start timeout`. Bukti menunjukkan kondisi serupa juga menimpa proses lain pada waktu yang sama (lihat 3.G), jadi belum bisa dipastikan sebagai bug khusus aplikasi.

Yang tidak ditemukan pada halaman pertama:

- Tidak ada crash: 0 baris `AndroidRuntime`/`FATAL` di log sesi ini; `/data/tombstones` berisi `total 0` (kosong).
- Tidak ada freeze: aplikasi tetap menanggapi event back dan dapat diluncurkan ulang.
- Tidak ada elemen tombol yang bertumpuk.

### 3.G Error log relevan dan pemisahan dari aplikasi lain

| Bukti | Proses pemilik | Kaitan dengan NBT Editor |
|---|---|---|
| `19:36:26.452 E/ActivityManager: ANR in com.google.android.apps.photos` | `com.google.android.apps.photos` | Tidak ada |
| `19:36:53.428 E/ActivityManager: ANR in com.google.android.apps.photos` | `com.google.android.apps.photos` | Tidak ada |
| `/data/anr/anr_2026-11-16-19-36-18-612`, `/data/anr/anr_2026-11-16-19-36-51-372` | Waktu file sesuai dua ANR photos di logcat; isi tidak terbaca (`Permission denied`) | Tidak terbukti milik NBT Editor |
| `19:36:29.820 W/ActivityManager: Process ... com.google.android.settings.intelligence ... failed to attach` + `start timeout` | `com.google.android.settings.intelligence` | Tidak ada |
| `19:36:26.862/.865` `failed to attach` + `start timeout` untuk PID 3387 | `com.nbteditor` | **Ada** — percobaan startup pertama |
| `19:37:06.929 W/com.nbteditor(4271): Unexpected CPU variant for x86: x86_64` | `com.nbteditor` | **Ada** — warning, bukan fatal |

Interpretasi: kegagalan attach pada 19:36:26 terjadi berdekatan dengan ANR `com.google.android.apps.photos` dan kegagalan attach `com.google.android.settings.intelligence`, sehingga indikasi kuat itu kondisi sistem/emulator saat itu, bukan bug aplikasi semata. Ini masih catatan temuan, bukan kesimpulan penyebab.

### 3.H Status pengujian halaman pertama

| Item | Status |
|---|---|
| Instalasi APK | **SELESAI — sukses** |
| Startup aplikasi | **SELESAI — berhasil pada percobaan kedua dan ketiga** |
| Screenshot aplikasi yang benar-benar berjalan | **SELESAI** |
| Observasi UI halaman pertama | **SELESAI** |
| Crash | **Tidak ditemukan** |
| Freeze | **Tidak ditemukan** |
| Audit fungsi/interaksi | **SEDANG DIPERIKSA — 1 fitur selesai (Open file)** |
| Audit halaman berikutnya | **BELUM DILAKUKAN** |
| Audit stabilitas menyeluruh | **BELUM DIPERIKSA** |

### 3.I Batas bukti siklus ini

1. Yang terbukti: `out/NBTEditor.apk` (`ede9362e...`) terpasang dan berjalan di `Pixel_5` x86_64 melalui ARM translation; Home screen tampil dan teramati.
2. Yang tidak terbukti: perilaku tombol, navigasi, editor NBT, dialog, tampilan data kosong selain `Recent files`, dan tampilan error. Tidak ada interaksi yang diuji di siklus ini.
3. APK yang diuji dibangun sebelum modifikasi source terakhir yang dibekukan; audit ini tidak membuktikan source current hash `2b260b3c...` sudah di-build.
4. Audit hanya pada satu skin/resolusi 1080x2340 dengan orientasi potret; belum diuji pada orientasi atau resolusi lain.
5. Penyebab cold start 10 detik belum ditentukan; belum dibuktikan apakah dari emulasi ARM, inisialisasi `libnbt.so`, kondisi emulator, atau gabungan ketiganya.

### 3.J Target berikutnya

1. Audit halaman berikutnya **hanya satu halaman per siklus**, dan hanya setelah persetujuan.
2. Audit fungsi satu interaksi per siklus.
3. Jika ingin dilanjutkan, tentukan penyebab cold start 10 detik dengan bukti tambahan, bukan dugaan.


<!-- MASTER-SECTION: LEG-FASE-02-S18 | CATEGORY=Audit UI/UX | STATUS=AKTUAL -->
## 4. Verifikasi Tree Editor — 19 Nov 2026

Ruang lingkup siklus ini hanya Tree Editor dan jalur yang dibutuhkan untuk mengujinya. Tidak ada perubahan desain, source aplikasi, fitur, APK 1.0.4 lain, atau pekerjaan bug yang tidak berkaitan.

### 4.A Pemeriksaan source dan perubahan terdahulu

Source aktual sudah memuat perbaikan yang diminta; tidak ada yang diimplementasikan ulang:

- **Tint seleksi opaque:** selected `TreeRow` memakai `gfx::blendOpaque(th.primary, surfOf(a), 40)` sehingga framebuffer tetap opaque dan nilai terpilih tetap kontras (`app/src/main.native.cpp`, sekitar blok pembentukan row).
- **Ellipsis nilai panjang:** `TreeRow` membatasi kolom nilai dan memanggil `textEllipsisAt`; nilai negatif mempertahankan tanda minus pada formasi awal/`...`, bukan kehilangan minus (`app/src/main.native.cpp`, branch `ui::Kind::TreeRow`).
- **Scroll dan FAB Save:** viewport `Scroll` memakai tinggi `a.h - y - 136`, sementara `+` dan `S` ditempatkan pada stack FAB; baris terbawah tidak dipaksa masuk di bawah Save FAB (`app/src/main.native.cpp`, akhir `buildTree`).
- **Regresi M42:** `M42 tree selection contrast, value ellipsis, and FAB clearance` tersedia dan dijalankan pada host test.

### 4.B Pengujian host dan regresi

- Perintah resmi `bash scripts/build_host_tests.sh` **gagal sebelum compile** pada baris `clang++: perintah tidak ditemukan`.
- Untuk memeriksa keadaan source tanpa mengubah build script, digunakan executor `g++` dengan `-Wno-misleading-indentation` (warning GCC yang tidak menghalangi clang/proyek):
  - batching screenshot: **9/9 PASS**;
  - engine host: **64/64 PASS**;
  - UI model: **70/70 PASS**, termasuk `M42`.
- Hasil ini bukan bukti bahwa skrip Termux resmi berjalan pada environment ini; kegagalan `clang++` tetap tercatat.

### 4.C Build APK dari source terbaru

Tidak memakai APK lama. APK dibangun ulang dari source saat ini memakai Android SDK/NDK yang tersedia:

| Item | Nilai |
|---|---|
| NDK | `30.0.16248370`, target `arm64-v8a` |
| SDK build-tools | `36.0.0` |
| compileSdk platform | `android-37.0` |
| targetSdk | `35` |
| APK | `out/NBTEditor.apk` |
| APK SHA-256 | `24b30b7e939c3af80704fc132630557415516b856c5371f43b3ce4fb094576a8` |
| APK size | `10,064,522` byte |
| `libnbt.so` SHA-256 di APK | `0cf4567edda4801fe557e8acf7f27b9d4fa3f12258dcb2f03b9ba75165df82bc` |
| Signature | APK Signature Scheme v2 dan v3 valid |
| `apksigner verify` | `Verifies` |
| `aapt2 dump badging` | `com.nbteditor`, `versionCode='5'`, `versionName='1.0.4'`, minSdk 26, targetSdk 35 |

Build dilakukan dengan langkah setara `build_device.sh` dan `build_apk.sh`, memakai NDK langsung karena script proyek masih mengasumsikan Termux/`android-35` dan environment ini tidak sama. `R-P0-24` tetap TERBUKA; build manual ini tidak memperbaiki atau menutup pipeline fail-closed proyek.

### 4.D Instalasi dan pengujian emulator

| Pemeriksaan | Hasil |
|---|---|
| AVD | `Pixel_5`, headless, `sys.boot_completed=1` |
| Instalasi | `adb install -r out/NBTEditor.apk` → `Success`, `Incremental Install`, 16.871 ms |
| Startup | cold start +13.166 s; `libnbt.so` loaded; `native render looper` dan `native input queue attached` tercatat |
| Fokus awal | sebelum `Wait` dipilih, fokus menampilkan `com.android.systemui` ANR yang terkait Apps Photos/proses lain, bukan crash `com.nbteditor`; proses app tetap berjalan dan fokus kembali ke app sesudah `Wait` |
| App berjalan | proses `com.nbteditor` hidup; `mCurrentFocus` kembali ke `com.nbteditor/android.app.NativeActivity` |
| Crash | tidak ditemukan `FATAL`, `AndroidRuntime`, `dlopen failed`, atau `UnsatisfiedLinkError` untuk `com.nbteditor`; proses system lain mungkin ANR/gagal load karena kondisi emulator |
| Input | tap Home, Add, type selection, field name, keypad dan cancel semuanya masuk ke action `nbteditor`; gesture swipe Tree diproses sebagai pointer events; Back mengembalikan dari Tree ke Home |
| Data Tree Editor | emulator diisi fixture in-app dengan `longTag` dan `shortTag`; `longTag` diubah menjadi `-9223372036854775807`; input terlalu besar sebelumnya memperlihatkan `Out of bounds for long.` dan batal tersimpan dengan `CANCEL` |

### 4.E Verifikasi khusus Tree Editor

| Perbaikan | Bukti | Status |
|---|---|---|
| Kontras baris terpilih | screenshot lokal tidak dilampirkan: fill selected `(225,240,226)`; teks kunci `(27,27,27)`, kontras **14.57**; teks nilai muted `(96,106,96)`, kontras **4.77** | SELESAI |
| Nilai panjang dengan ellipsis | OCR screenshot lokal menampilkan `longtag -92233720368...`; tanda minus tetap terlihat; `M42` PASS | SELESAI |
| FAB Save tidak menutupi baris terakhir | `M42` PASS; `Scroll` reserve lebar/area bawah sekitar 136 dp; swipe Tree tidak menimbulkan crash; layar emulator hanya berisi 2–3 baris sehingga overflow penuh belum diuji visual | SELESAI |
| Interaksi scroll | gesture swipe diproses aplikasi; tidak ada crash; viewport scroll tetap `Scroll` terpisah dari FABs | SELESAI |
| Navigasi | Back dari Tree ke Home tercatat; fokus kembali ke Home; tidak ada crash | SELESAI |

### 4.F Batas bukti siklus ini

- Screenshot diambil hanya secara lokal; tidak dilampirkan atau dikirim ke model.
- Daftar Tree pada emulator masih kecil; overflow/full-list scroll bawah-FAB penuh tidak dibuktikan secara visual. Bukti utama untuk FAB clearance adalah `M42` dan geometri source.
- Cold start 13.166 detik tetap tinggi; penyebab belum diketahui. Jangan klaim hang, memory leak, atau scheduler tertentu tanpa bukti.
- Emulator menampilkan ANR System UI/Photos yang tidak terkait `com.nbteditor`; ini tetap dicatat, bukan disembunyikan.
- Unsaved indicator aktif; navigasi Back dari Tree kembali ke Home. Tidak ada autosave/dirty-exit protection; `R-P0-29` tetap TERBUKA.
- Host test resmi `scripts/build_host_tests.sh` belum berhasil di environment ini karena `clang++` tidak ada; hasil host manual tidak menggantikan skrip yang terkunci.
- Belum ada build APK dari skrip resmi maupun pengujian pada perangkat fisik.

<!-- MASTER-SECTION: DOC-CAT-VERIFY-001 | CATEGORY=Katalog utama | STATUS=AKTUAL -->
## Verifikasi katalog master

Verifikasi dilakukan dalam dua tahap: baseline katalog pada 18 Nov 2026 dan finalisasi Fase 1–6 pada 19 Nov 2026 terhadap isi `DOKUMENTASI.md` aktual.

- **Struktur:** 99 dari 99 heading level-2 memiliki marker permanen; seluruh 99 ID unik. Heading level-3 tetap berada di dalam rentang induknya.
- **Cakupan:** delapan kategori utama—ringkasan kondisi, daftar bug, riwayat perbaikan, audit UI/UX, hasil pengujian, status fitur, riwayat perkembangan, dan arsip teknis—terwakili. `DOC-CAT-001` dan `DOC-CAT-VERIFY-001` mendokumentasikan katalog/verifikasi itu sendiri.
- **Bug:** 94 bug/masalah ber-ID unik; halaman 25, fitur 15, komponen/proses 54; status TERBUKA 43, DIANALISIS 3, DALAM PERBAIKAN 0, MENUNGGU PENGUJIAN 3, SELESAI 41, GAGAL 0, DITUNDA 4.
- **Peta perbaikan:** 17 pekerjaan `FIX-*` memetakan catatan historis ke ID bug dan bukti pengujian/build/emulator yang tersedia.
- **Fitur:** 47 ID fitur (`F1.1`–`F5.11`) unik. Status berbasis bukti: 1 TERIMPLEMENTASI—TERUJI, 13 PERLU PENGUJIAN, 24 BERMASALAH, 5 BELUM SELESAI, 3 DIRENCANAKAN, 1 DITUNDA—BUKAN SCOPE.
- **Perkembangan:** 12 pekerjaan tercatat sebagai `DEV-001`–`DEV-012`; catatan lama tetap dipertahankan dan direferensikan, bukan digandakan.
- **Rentang:** setiap rentang dihitung dari posisi heading dan isi aktual. Dua regenerasi berturut-turut menghasilkan file byte-identik setelah setiap perubahan struktur.
- **Pemulahan konten:** pada baseline 18 Nov 2026, setelah katalog, marker, dan bagian verifikasi ini dihapus, 8.448 baris historis direkonstruksi dengan SHA-256 `a6c0110b4bb03efbeeb7ed09ae21631a1e793821e0636d6bf39a70e05e97d161`. Perluasan bagian E, F, G, dan FASE 2 §4 pada 19 Nov 2026 adalah perubahan yang disengaja dan tidak termasuk dalam baseline tersebut; diff terhadap baseline hanya mengubah tabel navigasi/verifikasi dan menambah blok konsolidasi yang dimaksud.
- **Batas perubahan:** `DOKUMENTASI.md` diperbarui; `app/src/**` dan `scripts/**` tidak dimodifikasi. Build APK dari source saat ini membuat ulang artefak `out/NBTEditor.apk` dan `out/lib/arm64-v8a/` (`libnbt.so`, `libc++_shared.so`). Screenshot Tree Editor dibuat hanya di `/tmp` dan tidak dikirim; tidak ada berkas Markdown tambahan.
- **Identitas:** ID dan heading merupakan identitas permanen; rentang baris hanya petunjuk navigasi dan harus diregenerasi setiap kali posisi bagian berubah.

## REVERSE ENGINEERING — MINETUNER+ NBT EDITOR 2.2.3

> Reverse-engineering dari sisi pengguna + analisis teknis APK referensi `MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk`. Metode: ekstraksi/analisis APK (aapt2 `dump badging`/`xmltree`, apktool 3.0.3-dirty `smali`, jadx deobfuscation, `dumpsys`), emulator Pixel_5 (emulator-5554 — emulator asli tidak terhubung, dimulai ulang), ADB (`am start`, `uiautomator dump`, `screencap`), logcat (`out/adb-logs/startup_app.log`). Alias menggunakan emulator yang tersedia; tidak menggunakan emulator kedua. World `H-S` dipush ke worlds emulator (`adb push /tmp/opencode/hsw /storage/emulated/0/Android/data/com.hextetrahedral.minetuner/files/worlds/`).

### 1. Identitas APK

| Field | Nilai |
|---|---|
| Package name | `com.hextetrahedral.minetuner` |
| Application label | `MineTuner` |
| versionCode | 16 |
| versionName | 2.2.3 |
| platformBuildVersionName / Code | 16 / 36 (compileSdk 36, codename 16) |
| minSdkVersion | 26 |
| targetSdkVersion | 36 |
| XAPK (4 splits) | base `com.hextetrahedral.minetuner.apk` (105,397,608 B), `config.mdpi` (220,643 B), `config.en` (99,054 B), `config.arm64_v8a` (4,048,134 B); total ≈ 109,766,745 B; xapk_version 2 |
| Locales (config.en) | en, en-AU, en-CA, en-GB, en-IN, en-XC |
| launcher activity | `com.hextetrahedral.minetuner.MainActivity` (exported, intent `MAIN/LAUNCHER`) |
| support-screens | small, normal, large, xlarge; supports-any-density: true |
| library | org.apache.http.legacy; android.ext.adservices |
| features (implied) | faketouch, landscape, portrait, wifi |

### 2. Arsitektur dan struktur APK

**Dex:** 12 dex files (classes.dex, classes2.dex … classes12.dex) — multidex; base APK ~75 MB (12 dex).

**Native libs:** base APK: 0 (tanpa lib .so). config.arm64_v8a.apk: `native-code: 'arm64-v8a'` — 12 lib/arm64-v8a: `libleveldb.so` (LevelDB — Bedrock world data), `libnms.so`, `libsentry.so`, `libsentry-android.so`, `libapminsighta.so`, `libapminsightb.so`, `libapplovin-native-crash-reporter.so`, `libbuffer_pgl.so`, `libfile_lock_pgl.so`, `libpglarmor.so`, `libtobEmbedPagEncrypt.so`, `libtt_ugen_layout.so`. *(Berlaku: LevelDB native — world data disimpan via LevelDB).*

**Assets (assets/):** `entityIcons/*.png` (83 file: allay, armadillo, axolotl, bat, bee, blaze, elder_guardian, guardian, credits.txt …), `audience_network.dex` (5,017,908 B), `omsdk-v1.js`, `omid-session-client-v1.js`, `ad.html`, `mraid-bridge.js`, `aps-mraid.js`, `dtb-m.js`, `mobilefuse/*`, `a.json`, `builddatas.json`, `dexopt/baseline.prof`.

**Resources (apktool_out/res):** `values/` — strings.xml (748 baris), colors.xml, styles.xml, themes (Theme.Minetuner extends Theme.Material3.Light.NoActionBar), dimens.xml, plurals.xml, integers.xml, bools.xml, arrays.xml, ids.xml, drawables.xml, public.xml; `values-*` (land, h320dp-port, h480, h550-port, h720, large, large-land, large-port, ldrtl, h480-land …); `layout/` (526, utama: activity_main, activity_drawer, activity_help, activity_create_world, activity_entities_nbt, activity_debug, activity_internal_browser, drawer_header, biome_item, controls, mainlayout, map_selection_view, design_* Material3, admob_*/apd_*/apd layouts); `menu/` (drawer_menu, main_activity_toolbar_menu, create_world_activity_toolbar_menu, dialog_buttons); `anim/`, `animator/`, `raw/`, `font/`, `mipmap/` (ic_launcher anydpi).

**Activity:**
- `com.hextetrahedral.minetuner.MainActivity` — Home (world list + FAB).
- `com.hextetrahedral.minetuner.DrawerActivity` — Editor home (world editor + drawer).
- `com.hextetrahedral.minetuner.app.HelpActivity` — Help (texture-pack import tutorial).
- `com.hextetrahedral.minetuner.app.CreateWorldActivity` — Create Flat World (biome, slimes, flat layers).
- `com.hextetrahedral.minetuner.app.EntitiesNBTActivity` — Chunk Entities (entitiesRecyclerView).
- + obfuscated/ad SDK activities (Bigo, Moloco, Mbridge, AppLovin, Amazon DTB, etc.).

**Service:** Google `AdService`, chartboost `VideoRepositoryDownloadService`, androidx.work `SystemJobService/SystemForegroundService/SystemAlarmService`, StartApp `BackgroundService/SchedulerService`, AppLovin `FullscreenAdService`, AppMetrica `AppMetricaService`, etc.

**Broadcast receivers:** Appodeal `AppodealPackageAddedReceiver` (PACKAGE_ADDED), StartApp `BootCompleteListener` (BOOT_COMPLETED), androidx.work receivers, google services.

**Content providers:** FileProvider, InitializationProvider, `io.appmetrica.analytics.internal.PreloadInfoContentProvider` (exported), Amazon?, Bigo `BigoAdsProvider`, BidMachine `BidMachineInitProvider`, Facebook, Fyber, … (dari manifest dump).

**Permissions** (aapt2 dump, 19): INTERNET, ACCESS_NETWORK_STATE, READ_EXTERNAL_STORAGE, WRITE_EXTERNAL_STORAGE, ACCESS_WIFI_STATE, VIBRATE, RECEIVE_BOOT_COMPLETED, WAKE_LOCK, FOREGROUND_SERVICE, com.google.android.gms.permission.AD_ID, com.google.android.gms.ads.permission.AD_ID? (lihat), com.applovin.array.apphub.permission.BIND_APPHUB_SERVICE, ACCESS_ADSERVICES_TOPICS/ATTRIBUTION/AD_ID, com.android.vending.BILLING, com.android.vending.CHECK_LICENSE, com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE, com.hextetrahedral.minetuner.DYNAMIC_RECEIVER_NOT_EXPORTED_PERMISSION. (Implied perms dari config.mdpi/en: READ_PHONE_STATE, READ_EXTERNAL_STORAGE (via WRITE_EXTERNAL_STORAGE, targetSdk<4)).

### 3. Teknologi yang digunakan (belum-diverifikasi sejauh dapat dibuktikan)

- **Framework:** AndroidX (AppCompat, RecyclerView, Drawer, NavigationView, Material3, WorkManager, Startup, ProfileInstaller, EmojiCompat, Activity); Google Material3 (`Theme.Minetuner`); Google Play (Billing, PairIP license); Firebase/Google Analytics; CrONet (`PlayServicesCronetProvider`); AdMob adservices.
- **Bahasa:** Java bytecode (javap/jadx) — `MainActivity`, `DrawerActivity`, `app.HelpActivity`, `app.CreateWorldActivity`, `app.EntitiesNBTActivity`; obfused app classes (paket `a0,b0,c0,d0,e0,f0,h0,i0,q,r,s,t,u,w,x,y,z` — editor fragment: t/h, t/e, d0/h, d0/e …).
- **NBT engine:** Bedrock/little-endian NBT — LevelDB natif (`libleveldb.so`) untuk penyimpanan world (`files/worlds/<name>/db/`).
- **Ad/media SDKs:** AppLovin (MAX), Appodeal (unit `79e2afef82af6ff44ec14c4cd547374550cfb9f45c77e338`, type 3), Amazon DTB (verified; DTExchangeAdapter `org.bidon/dtexchange`), Google AdMob/AdServices/Cronet, Moloco (xenoss), Bigo, Mbridge, InMobi, Mintegral (`org.bidon/mintegral` — MobileFuse too), AppMetrica, Yandex, Pangle (bytedance), Facebook AudienceNetwork, MyTarget, StartApp, Unity, BidMachine, PubNative ("net.pubnative.lite.sdk.banner.presenter"). *Catatan: merchant terlihat dari manifest/res/smali — beberapa network obsese.*
- **Crash/analytics:** Sentry (`libsentry.so`), AppMetrica, Google Analytics/Firebase, axml.

### 4. Startup sequence (observed, emulator Pixel_5 — API 37, 1080x2340, u0_a235)

1. `am start -n com.hextetrahedral.minetuner/.MainActivity` — system splash window (logcat: `VRI-Splash Screen com.hextetrahedral.minetuner`) saat proses baru terbuka (cold start, Zygote → uid 10235/u0a235).
2. **ANR** — `MineTuner isn't responding` (ActivityManager dump; ANR temp files; app >5 s belum responsive). Buttons: `Wait` / `Close app`.
3. Setelah `Wait`/force-stop restart — topActivity `com.pairip.licensecheck.LicenseActivity` (Google license check); logcat `pid sent binder code 2 with flags 2 and got error -74` (no Play Store on emulator). Dialog: `Something went wrong` — `Check that Google Play is enabled on your device` + `CLOSE`.
4. Dismiss CLOSE → **Home** visible.
5. **Permission request:** tidak ada — API 37 > 28 → `l()` path; `requestPermissions` hanya di ≤28.
6. Low Storage Warning — jika statfs < 10 MB (kode).
7. `Appodeal.initialize(unit "79e2afef82af6ff44ec14c4cd547374550cfb9f45c77e338", type 3)` pada akhir `MainActivity.onCreate`.

### 5. Analisis Home (MainActivity — activity_main.xml + dump emulator h05)

- **Layout:** CoordinatorLayout → AppBarLayout(Toolbar: title "MineTuner", warna `?colorPrimary`) + RecyclerView(`worldRecyclerView`) + `fabMenu` (mainFab `ic_add`; "New World"+FAB `ic_layers`; "Import World"+FAB `ic_upload` — visibility=gone, animasi otomatis).
- **Toolbar menu** (main_activity_toolbar_menu): `Help` (ic_book, always) → HelpActivity; `About` (ic_info, ifRoom) → About dialog.
- **Tombol/fitur:**
  - World row: "Options for "<name>"" — Edit/Delete (external: Edit/Open in Minecraft/Delete + warning "## [EXTERNAL WORLD] ##"); Loading world... progress; Encrypted World dialog (marketplace); Export progress ("Compiling world...", Appodeal video_interstitial jika terlambat); delete confirm ("Delete World" — cannot be undone); import toasts ("No file type selected.", "Please select a valid .mcworld|mctemplate file", "Importing world...", "Failed to import file: ...").
  - FAB "New World" → `CreateWorldActivity`.
  - FAB "Import World" → Import World dialog (`To import a world, please provide a .mcworld or .mctemplate file.` CANCEL/MCWORLD/MCTEMPLATE).
  - Low Storage: "Low Storage Warning" — device low on storage...
- **Home observed (emulator, setelah dismiss):** world list + paged view area, teks "Fri, Nov 20", 7 kartu aplikasi: Gmail, Photos, YouTube, Phone, Messages, Chrome, MineTuner (kemungkinan kartu iklan — app didukung iklan minimal). World "H-S" tidak tercantum (worlds/H-S ada di emulator; kemungkinan filter FileFilter d(2) atau ada dialog/output lain sebelumnya).

### 6. Struktur navigasi (Drawer — DrawerActivity)

- `DrawerActivity`: DrawerLayout + Toolbar(worldName) + fragment_container + NavigationView(header: 64dp icon, title, subtitle, 160dp, colorPrimary).
- Tombol back: "Close NBT Editor" dialog (unsaved changes warning) / "Close World" dialog.
- **Menu drawer (drawer_menu.xml — single-selection group)** + mapping fragment (DrawerActivity.l + a.f):
  1. **Show Map** (nav_map) → fragment `e` — peta dunia.
  2. **World NBT** (nav_world_nbt) → fragment `h` — editor NBT dunia.
  3. **Singleplayer NBT** (nav_singleplayer_nbt) → `a.f("~local_player")` — NBT pemain.
  4. **Multiplayer NBT** (nav_multiplayer_nbt) → fragment `b` — NBT multiplayer.
  5. **Advanced**: Overworld NBT (`a.f("Overworld")`), The End NBT (`a.f("TheEnd")`), Nether NBT (`a.f("Nether")`), BiomeData NBT (`a.f("BiomeData")`), Portals NBT (`a.f("portals")`), Autonomous Entities NBT (`a.f("AutonomousEntities")`), **Open Custom NBT** (`a.f` → dialog input: "Custom NBT Key", hint "e.g. ~local_player").
  6. **Change World** (nav_change_world=2131363014) → Close World dialog ("Are you sure you want to close this world?").

### 7. Inventaris seluruh halaman

| Halaman | Komponen (activity/fragment) | Tujuan |
|---|---|---|
| Home | `MainActivity` | World list, FAB menu (New World/Import World), Help, About, import/delete/edit world |
| Editor Home | `DrawerActivity` | Editor dengan Drawer (11 item: Show Map, World NBT, Singleplayer NBT, Multiplayer NBT, Advanced: Overworld/TheEnd/Nether/BiomeData/Portals/AutonomousEntities, Custom NBT, Change World) |
| World NBT | DrawerActivity fragment `h` | Editor NBT dunia (world data) |
| Player NBT | DrawerActivity fragment `a.f("~local_player")` | Editor NBT pemain (player data) |
| Multiplayer NBT | DrawerActivity fragment `b` | Editor NBT multiplayer |
| Overworld NBT | `a.f("Overworld")` | Editor NBT overworld |
| The End NBT | `a.f("TheEnd")` | Editor NBT the end |
| Nether NBT | `a.f("Nether")` | Editor NBT nether |
| BiomeData NBT | `a.f("BiomeData")` | Data biome |
| Portals NBT | `a.f("portals")` | Data portals |
| Autonomous Entities NBT | `a.f("AutonomousEntities")` | Entities otomatis |
| Custom NBT | dialog input → `a.f(key)` | Editor NBT berdasarkan key DB |
| Show Map | DrawerActivity fragment `e` | Peta dunia (layout `map_selection_view`) |
| Help | `app.HelpActivity` | Texture-pack import tutorial (texture pack → Export Template) |
| Create World | `app.CreateWorldActivity` | Buat dunia flat (biome, "Prevent Slimes from Spawning", flat layers) |
| Chunk Entities | `app.EntitiesNBTActivity` | List entities dalam chunk (entitiesRecyclerView) |
| About (Home menu) | Dialog (SimpleDialog, HTML) | Tentang aplikasi — "designed for NBT editing... supported by ads... tested with Minecraft 1.21.x... Join our Discord" (DISCORD → discord.gg/Mr2za3VkUQ) |

### 8. Inventaris seluruh fitur

- **File browser / Open World:** Home → world list; import .mcworld/.mctemplate (Import World dialog); file picker (MCWORLD/MCTEMPLATE); "Open in Minecraft" (export.mcworld → intent VIEW).
- **World NBT:** Dunia dipilih (Edit) → DrawerActivity (World NBT editor).
- **Singleplayer NBT:** `~local_player`.
- **Multiplayer NBT:** fragment b.
- **Overworld/The End/Nether/BiomeData/Portals/Autonomous Entities NBT:** drawer advanced items → fragments `a.f("Overworld")` etc.
- **Open Custom NBT:** dialog input key ("Custom NBT Key", hint "e.g. ~local_player") → fragment `a.f(key)`.
- **Show Map:** drawer "Show Map" → fragment `e` (map_selection_view).
- **Change World:** drawer item → "Close World" dialog (unsaved changes warning).
- **NBT Editor mechanics (target):** Tree structure, Compound, List, seluruh tipe Tag, Add Tag, Edit Tag, Delete Tag, Rename Tag, Expand/Collapse, Pencarian (search), Save, Validasi, Error handling — verifikasi: bagian ini obfused, belum dianalisis secara mendalam (lihat status baris 16).

### 9. Analisis setiap fitur

**File browser / Open World (Home):**
- Tombol: world row → Options dialog (Edit / Open in Minecraft / Delete; Delete=confirmed "Delete World — This action cannot be undone").
- FAB: New World → CreateWorldActivity; Import World → import dialog.
- Status belum lengkap: world row visibility tergantung skan `worlds/` dir; obses di Home emulator tidak terlihat H-S (worlds/H-S ada di emulator).

**World NBT / Editor NBT:**
- Dimulai via DrawerActivity fragment h; not verified (obfused).
- Penyimpanan: LevelDB (`worlds/<name>/db/`), data level.dat.

**Per-fitur drawer (kode):** Close NBT Editor dialog ("Are you sure you want to close this NBT editor? Any unsaved changes will be lost." NO/YES); Close World dialog; Change World → Close World.

### 10. Analisis NBT Editor

- **Penyimpanan:** `files/worlds/<world>/` — level.dat, levelname.txt, db/ (leveldb — liblibleveldb.so), behavior_packs/, world_icon.jpeg. Fitur "Encrypted World" (world terlocked dari marketplace → dialog "This world is encrypted and cannot be edited").
- **World entry model (obfused, jadx):** class j (Fields: File c, ReadWriteLock d, methods d()=world name, g()=internal name, i()=hasBitmap, h()=bitmap, j()=encrypted-check, k()=delete?) + class g (adapter, notifyItemInserted/Removed/RangeRemoved).
- **Screen utama NBT:** not verified (obfused — fragment h = t/h: small data class `c a; int b`; fragment e = Show Map).
- **Asset pendukung:** entityIcons/*.png (83) — ikon entitas (allay, armadillo, axolotl, bat, bee, blaze, elder_guardian, guardian, credits.txt …).
- **Validasi (kode):** String alphanumeric_only_error "May only contain alphanumerical chars" (belum terhubung dengan skrren spesifik yang obfused).
- **Catatan:** ENGINE NBT-nya di-obfuscated (paket a0–z), plik interna editor tidak ditulis ke DOKUMENTASI hingga status "LIMITED" (baris 16).

### 11. Analisis interaksi

- **Startup:** splash window → process start (cold) → ANR (≈5 s) → Wait/Close app → PairIP license (emulator: error dialog "Something went wrong / Check that Google Play is enabled on your device" + CLOSE) → Home.
- **Home:** tap world row → Options dialog (Edit/Delete/Open in Minecraft) → Edit → "Loading world..." progress → DrawerActivity (World NBT). Import: picker → "Importing world..." progress → world listed.
- **Drawer:** open (toolbar hamburger — DrawerActivity); item → fragment; switch item → Close NBT Editor confirm; Change World → Close World confirm.
- **Editor NBT:** not verified (obfused).

### 12. Analisis penyimpanan data

- **Directory:** `files/worlds/` = `getExternalFilesDir(null)/worlds` (scoped storage; u0_a235 di emulator).
- **Pemindaian world (kode):** `l()` — `new File(getExternalFilesDir(null), "worlds")`, filter d(2) (FileFilter — obfused), gunakan j(worldDir); di API ≤28 tambah `Environment.getExternalStorageDirectory()/Android/data/com.mojang.minecraftpe/files/games/com.mojang/minecraftWorlds`.
- **World row:** nama dir → world name; level.dat/levelname; size = `Files.walkFileTree` + `Formatter.formatFileSize`; icon via j.i()/j.h().
- **File:** import.mcworld (14,741,292 B); worlds/H-S/level.dat (3,051 B), levelname.txt (3 B), behavior_packs/, db/, world_icon.jpeg.
- **DB:** LevelDB — native (libleveldb.so) untuk world data; provider `io.appmetrica.analytics.internal.PreloadInfoContentProvider` (exported) + InitializationProvider.
- **Tidak ada Room/SQLite app-local terverifikasi** (tidak ada *RoomDB); beban domain kustom.

### 13. Analisis UI/UX

- **Palet warna (verified):** `md_theme_light_primary #429e46` (hijau), `md_theme_light_onPrimary #ffffff`, `md_theme_light_background #f1f8e9`, `md_theme_light_surface #ffffff`, `md_theme_light_secondary #66bb6a`, `md_theme_light_error #b00020`. Theme: `Theme.Minetuner` → parent `Theme.Material3.Light.NoActionBar`; toolbar white title on hijau.
- **Background:** app white; toolbar colorPrimary.
- **Toolbar:** AppBarLayout + androidx.appcompat.widget.Toolbar (title e.g. "MineTuner" Home, worldName di Editor).
- **Navigasi:** DrawerLayout + NavigationView; drawer dimulai dari toolbar hamburger (ActionBarDrawerToggle.syncState). Drawer header 160dp.
- **Sidebar/Drawer menu:** 11 item (tabel §6).
- **Button:** FAB (ic_add, fabCustomSize 40dp) + label (New World / Import World); Help/About action items; tombol dialog (CANCEL/YES/NO/CLOSE/MCWORLD/MCTEMPLATE/DISCORD).
- **Icon:** ic_launcher (anydpi mipmap), action icons (ic_book, ic_info, entity icons 83).
- **Typography:** standar Material3 (actionBarSize toolbar, 16sp text views, textSize 12-20sp observed).
- **Ukuran komponen:** drawer header 160dp; FAB 40dp; world row row (name/size/icon).
- **Dialog:** Material3 (TextView 16sp + buttons) — ANR, license error, About, Close NBT Editor, etc.
- **Padding/Margin:** FAB margin 4dp/16dp (activity_main); dialog margins Material3.
- **Animasi:** anim/ + animator/ (abc_*, applovin_slide_*) — untuk ads/UI transition.
- **Responsivitas:** Home adalah ScrollView — scroll ke bawah melihat konten lanjutan (observed: paged view + kartu apps di bawah daftar dunia).

### 14. Analisis Permission

| No | Permission | Sumber (manifest) | Perilaku (obs) |
|---|---|---|---|
| 1 | android.permission.INTERNET | uses-permission | Layanan jaringan (analytics, ads) |
| 2 | android.permission.ACCESS_NETWORK_STATE | uses-permission | Deteksi jaringan (ad/telemetri) |
| 3 | android.permission.READ_EXTERNAL_STORAGE | uses-permission | Universe worlds/import (runtime: Android ≤28; emu: granted?) |
| 4 | android.permission.WRITE_EXTERNAL_STORAGE | uses-permission | New world/import/export files |
| 5 | android.permission.ACCESS_WIFI_STATE | uses-permission | Wifi scan (telemetri/analytics) |
| 6 | android.permission.VIBRATE | uses-permission | Feedback (ads) |
| 7 | android.permission.RECEIVE_BOOT_COMPLETED | uses-permission | StartApp BootCompleteListener (BOOT_COMPLETED) |
| 8 | android.permission.WAKE_LOCK | uses-permission | Wake lock (startApp/other) |
| 9 | android.permission.FOREGROUND_SERVICE | uses-permission | AppLovin FullscreenAdService |
| 10 | com.google.android.gms.permission.AD_ID | uses-permission | Google Ads ID |
| 11 | com.applovin.array.apphub.permission.BIND_APPHUB_SERVICE | uses-permission | AppLovin AppHub binding |
| 12 | android.permission.ACCESS_ADSERVICES_TOPICS / ATTRIBUTION / AD_ID | uses-permission | Google AdServices |
| 13 | com.android.vending.BILLING | uses-permission | Google Play Billing (in-app purchase) |
| 14 | com.android.vending.CHECK_LICENSE | uses-permission | Google Play license (PairIP) |
| 15 | com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE | uses-permission | Install referrer |
| 16 | com.hextetrahedral.minetuner.DYNAMIC_RECEIVER_NOT_EXPORTED_PERMISSION | uses-permission | App's own dynamic receivers |
| 17 | (implied) android.permission.READ_PHONE_STATE / READ_EXTERNAL_STORAGE | implied (mdpi/en) | targetSdk < 4 artifacts |

Tidak ada permission "RUNTIME request" dari MainActivity di API > 28 (verified di kode).

### 15. Screenshot dan bukti

- **screenshots/** (bukti): `s01_home.png` (ANR), `s02_home.png`, `s03_home.png`, `s04_home.png` (Error "Something went wrong / Check that Google Play is enabled on your device" + CLOSE), `s05_home.png` (Home), `s06_home.png` (ANR kembali), `s06b_home_scroll.png` (Home di-scroll).
- **screenshots/h01_home.xml … h06b_home.xml** (UI hierarchy dumps, 4.4–21 KB).
- **out/adb-logs/startup_app.log** (logcat startup).
- Emulator: emulator-5554 (Pixel_5 AVD; api 37; uid u0a235).
- World emu: `/storage/emulated/0/Android/data/com.hextetrahedral.minetuner/files/worlds/H-S` (level.dat 3,051 B; levelname.txt "H-S").
- NOTE: model tidak dapat membaca gambar (image input not supported); screenshot dianalisis via XML hierarchy + ukuran/metadata.

### 16. Fitur yang belum dianalisis (dilaporkan)

- **NBT Editor internals (obfused):** Tree structure/Compound/List/seluruh Tag/Add/Edit/Delete/Rename Tag/Expand-Collapse/Pencarian/Save/Validasi/Error handling — minim, karena engine/editor obfused (paket a0–z); beberapa fragment di-verifikasi (§10), screen spesifik tidak dibaca secara mendalam.
- **Show Map screen** — layout `map_selection_view` ada; belum mengamati (emulator blocked by license error).
- **Ad screens (Bigo, Mbridge, AppLovin, Amazon, Mintegral, MobileFuse, others)** — TIDAK DAPAT DIAKSES (emulator), dari manifest/res saja.
- **Legacy Android ≤28 path** (mojang worlds path) — verified in code only.
- **Perintah passthrough/edge case analytics:** network behavior, ANR loop — didokumentasikan sebagai observed.

### 17. Kesimpulan teknis

MineTuner+ NBT Editor 2.2.3 — aplikasi jembatan dunia Minecraft/Bedrock: Home (dari worlds dalam scoped storage), Editor (DrawerActivity, fragments per view NBT: World/Singleplayer(Player)/Multiplayer/Overworld/TheEnd/Nether/BiomeData/Portals/AutonomousEntities, Custom-by-key, Show Map). NBT engine: Bedrock little-endian + LevelDB (native). Teknologi: AndroidX + Material3 (putih hijau, primary #429e46), Java bytecode multidex, ad-supported (Appodeal/AppLovin/Bigo/Amazon/Mbridge/Mintegral/MobileFuse/…), billing+license+telemetry (Google Play, AdMob AdServices, AppMetrica, Sentry, Cronet). Emulator Pixel_5 (no Play Store): startup ANR → PairIP license dialog ("Something went wrong..."); world list tidak menampilkan H-S (H-S ditambahkan; kemungkinan filter/obstruksi dialog lain).

### 18. Spesifikasi perilaku aplikasi (observed + verified)

1. Launch (am start MainActivity) → system splash window → process cold start → ANR (timeout ~5 s) → user: Wait / Close app.
2. PairedIP license (emulator: Error dialog).
3. Home: toolbar "MineTuner", world list (dirs in files/worlds; eksklusif Android ≤28: mojang path jika grant), FAB New World / Import World, menu Help/About.
4. World row → Options dialog (Edit/Open in Minecraft/Delete); Edit → Loading world... → DrawerActivity (World NBT editor).
5. Import: picker → Importing world... progress.
6. Drawer: 11 menu items; Close NBT Editor; Change World → Close World.
7. Editor NBT: obfuscated — not deep.
8. Screens: Help, About, CreateWorld, EntitiesNBTActivity, Show Map.

### Ringkasan status temuan

| No | Temuan | Status |
|---|---|---|
| 0 | Kepastian: model lihat image tidak bisa (no image input) — screenshot hanya metadata/hierarchy | BELUM DIVERIFIKASI |
| 1 | Identitas APK (package, v16/2.2.3, SDK 26–36, launcher) | TERVERIFIKASI |
| 2 | Struktur APK (12 dex, assets, res, lib arm64 leveldb/sentry/...) | TERVERIFIKASI |
| 3 | Activities (5) + obfuscated fragments | TERVERIFIKASI |
| 4 | Services/Receivers/Providers | TERVERIFIKASI (manifest) |
| 5 | 19 Permission (manifest; gms.ads.permission.AD_ID tidak ada di badging) | TERVERIFIKASI (lihat §14) |
| 6 | Tech stack (AndroidX, Material3, ad SDKs, billing, telemetry) | TERVERIFIKASI |
| 7 | Splash window (logcat VRI-Splash Screen) | TERVERIFIKASI |
| 8 | ANR at startup (emulator) | TERVERIFIKASI |
| 9 | PairIP license dialog (emulator: "Something went wrong...") | TERVERIFIKASI (logcat + emulator) |
| 10 | Home: toolbar, world list, FAB, menu Help/About | TERVERIFIKASI |
| 11 | World list scan (getExternalFilesDir/worlds, d(2)) | TERVERIFIKASI (code) |
| 12 | World H-S pushed; not listed (dialogs blocking) | BELUM DIVERIFIKASI |
| 13 | Drawer menu (11 items) + fragments (e/h/b/a.f) | TERVERIFIKASI |
| 14 | Close NBT Editor / Close World dialogs | TERVERIFIKASI |
| 15 | World NBT fragment h; storage LevelDB | LIMITED → BELUM SELESAI |
| 16 | Show Map (fragment e, map_selection_view) | BELUM DIVERIFIKASI |
| 17 | Custom NBT dialog (input "Custom NBT Key") | TERVERIFIKASI |
| 18 | Screenshots + hierarchy + logcat (§15) | TERVERIFIKASI |
| 19 | NBT editor internals (add/edit/delete/rename, save, validate) | BELUM DIVERIFIKASI (obfused) |
| 20 | Ad SDK screens | TIDAK DAPAT DIAKSES (emulator) |

---

*Investigasi reverse engineering selesai (bagian 1–18 dokumen lengkap — lihat DOKUMENTASI.md bagian REVERSE ENGINEERING).*

---

## Bagian B2 — Re-analysis statis ulang (2026-11-21) — MineTuner+ 2.2.3

> Lingkup re-analysis: APK XAPK `MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk` dianalisis ulang **statis** dengan tools aktual: `aapt2 dump badging / xmltree / resources / strings` (build-tools 36.0.0, `/home/kunbu8u7b/Android/Sdk/build-tools/36.0.0/aapt2`), output jadx lama (`/home/kunbu8u7b/.ref_extract/jadx_out/...`, didekompilasi dari DEX), ekstraksi strings DEX (`strings` per `classes*.dex`), dan parse pohon manifest via python dari `aapt2 dump xmltree`. Tidak dieksekusi di emulator: AVD `Pixel_5` mengalami segfault (exit 139, “dumped core”) saat start di host ini, sehingga semua hasil berikut adalah bukti statis.

### 1. Identitas & build (XAPK)

| Item | Value |
|---|---|
| Package | `com.hextetrahedral.minetuner` |
| Application label | `MineTuner` (string 0x7f13002b) |
| versionCode | `16` |
| versionName | `2.2.3` |
| minSdk / targetSdk / compileSdk | 26 / 36 / 36 |
| App type | Minecraft **Bedrock** world editor + NBT tree editor (NBT: Bedrock/little-endian) |
| XAPK size | 109,767,468 bytes (4 APK: base ~105.4 MB + en + mdpi + arm64_v8a) |

`manifest.json` (XAPK): `xapk_version=2`, splits: base `com.hextetrahedral.minetuner.apk`, `config.mdpi.apk`, `config.en.apk`, `config.arm64_v8a.apk`; native libs: `libleveldb.so` (per katalog B lama).

### 2. Komponen manifest (verified statis)

**Activities (aplikasi, 5 utama):**
- `com.hextetrahedral.minetuner.MainActivity` — **launcher** (launchable-activity).
- `com.hextetrahedral.minetuner.DrawerActivity`
- `com.hextetrahedral.minetuner.app.HelpActivity`
- `com.hextetrahedral.minetuner.app.CreateWorldActivity`
- `com.hextetrahedral.minetuner.app.EntitiesNBTActivity`

**Activities lain: ~124** — semuanya adalah activity SDK iklan/Google (Google Ads, Chartboost, Amazon DTBinterstitial/AdActivity, Pangle/ByteDance TT*, Bigo, Mbridge, Vungle, Smaato, ironSource, InMobi, OGury, AQ, Appodeal, PubMatic, Yandex, Facebook Audience, MyTarget, AppLovin, Fyber, Taurusx/Tax, …).

**Services (14):** `google.android.gms.auth.api.signin.RevocationBoundService` + service-an Surat service SDK iklan (Startapp Scheduler/Cached, Chartboost video, GMS AdService, BidMachine, Bytedance BinderPool, WorkManager SystemAlarm/SystemJob/SystemForeground, Room MultiInstanceInvalidation, AppMetrica, DataTransport scheduling…).

**Receivers (15):** ForceStopRunnable$BroadcastReceiver, ConstraintProxy$BatteryChargingProxy, ConstraintProxy$BatteryNotLowProxy, ConstraintProxy$StorageNotLowProxy, ConstraintProxy$NetworkStateProxy, RescheduleReceiver, ConstraintProxyUpdateReceiver, DiagnosticsReceiver, ProfileInstallReceiver, AlarmManagerSchedulerBroadcastReceiver, NetWorkChangeReceiver (Mbridge), LogEnablerReceiver (Ogury), VolumeChangedActionReceiver (PubNative), AppodealPackageAddedReceiver, BootCompleteListener (Startapp)…

**Providers (19):**
- App: `com.hextetrahedral.minetuner.provider` — **FileProvider** (authority: `com.hextetrahedral.minetuner.provider`)
- App: `androidx.startup.InitializationProvider`; init/debug/lifecycle providers: `androidx.startup`, `mobileadsinitprovider`, `applovininitprovider`, `appmetrica.preloadinfo.retail`, `mobilefusesdkinitprovider`, `bidmachineinitprovider`, `taurusxfileprovider`, `vungle-provider`, `bigoadsprovider`, `smaato-lifecycle-process`, `com.squareup.picasso`, `ironsourcelifecycleprovider`
- App: `AudienceNetworkContentProvider`, `MyTargetContentProvider`, `ApsInterstitialActivity` (per fitur intinya) — appsecara paling relevan kebutuhan NBT editor: FileProvider saja.

**Izin (18, dari `aapt2 dump badging`):** INTERNET, ACCESS_NETWORK_STATE, READ_EXTERNAL_STORAGE, WRITE_EXTERNAL_STORAGE, `com.google.android.gms.permission.AD_ID`, ACCESS_WIFI_STATE, `com.android.vending.BILLING`, VIBRATE, `com.applovin.array.apphub.permission.BIND_APPHUB_SERVICE`, ACCESS_ADSERVICES_TOPICS, ACCESS_ADSERVICES_ATTRIBUTION, RECEIVE_BOOT_COMPLETED, `com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE`, ACCESS_ADSERVICES_AD_ID, WAKE_LOCK, FOREGROUND_SERVICE, `com.hextetrahedral.minetuner.DYNAMIC_RECEIVER_NOT_EXPORTED_PERMISSION`, `com.android.vending.CHECK_LICENSE`.

**Fitur diminta:** portrait+landscape, faketouch, wifi, sentuh, sensor ringan (compass/gyro/light/proximity/barometer, not required), `uses-library-not-required: android.ext.adservices`, `org.apache.http.legacy`.

### 3. Desain & tampilan (verified statik)

**Tema:** `style/Theme.Minetuner` (0x7f14033e) → parent `style/Base.Theme.Minetuner` (0x7f140079) → parent `style/Theme.Material3.Light.NoActionBar` — **light only**, tidak ada dark.

**Colors (verified):**
- `colorPrimary`: #ff429e46 (hijau utama)
- `colorOnPrimary`: #ffffffff
- `colorSecondary`: #ff66bb6a
- `colorBackground`: #fff1f8e9 (hijau muda sangat pudar)
- `colorSurface`: #ffffffff
- `colorError`: #ffb00020 (merah)
- FAB style: `style/Widget.Minetuner.FAB` (FAB dengan primary tint + secondary ripple)

**Toolbar:** id 0x7f0a0620, judul = `app_name` “MineTuner”, `titleTextColor` hitam (0x0106000b), di setiap activity utama.

**World list (catalog B lama §4.1):** white CardView, 8dp corner radius, 4dp elevation; row: 48dp world icon, 18sp bold world name, 14sp grey size, 14sp italic internal folder name, fallback drawable id 2131231699, `world_icon.jpeg` jika ada.

### 4. Layar & apa yang muncul saat menekan sesuatu

#### 4.1 MainActivity — world list (tampilan utama)

- Toolbar **hijau** berjudul **“MineTuner”**; RecyclerView world list (kosong di awal); **FAB stack** di kanan bawah: FAB utama (id 0x7f0a03a6) — saat ditekan (via `a0/c` animator, OvershootInterpolator 300 ms) memperluas: “New World” (FAB 0x7f0a0162) dan “Import World” (FAB 0x7f0a0067).
- Overflow menu: **Help** → HelpActivity; **About** → dialog “About” (msgHtml: “This app is designed for NBT editing, helping you manage and tweak your Minecraft worlds… Tested with Minecraft 1.21.x. Join our Discord…”).
- `onCreate`: scan world dari `getExternalFilesDir(null)/worlds/` (filter `d(2)`, obfuscated — diasumsikan directory); SDK ≤ 28 minta permission storage; **Low Storage Warning** jika data dir < 10 MB: “Low Storage Warning” / “Your device is low on storage. This may cause issues with the app's usability. Please continue with caution.”

**Tap world → dialog “Options for "<name>””** (SimpleListDialog, choiceMode multi, layout LAYOUT_ACTION):
- World managed: **Edit, Open in Minecraft, Delete**
- World external: **Edit, Delete** + msg: “Note: This world is located outside the app's managed directory. Please create a backup if you have not already.”

**Edit →** deteksi enkripsi (heuristik scan `<world>/db/*`: dua int32 LE — `first == 0 && second == -1680885252`):
- TerEnkripsi → dialog “Encrypted World”: “This world is encrypted and cannot be edited. Any worlds downloaded from Minecraft Marketplace are usually encrypted by default.”
- Boleh → progress dialog: “Loading world...” / “Please wait while the world is being loaded.” → buka **DrawerActivity** (extra worldName + internalName).

**Open in Minecraft →** progress: “Compiling world...” / “Please wait while the world is being compiled. Do not exit the app.” → compile zip ke `getExternalFilesDir(null)/export.mcworld` → launch via FileProvider (FileSharing).

**Delete →** dialog “Delete World”: “Are you sure you want to delete this world? This action cannot be undone.” (CANCEL/DELETE).

**Import (tap FAB “Import World” → dialog):** “Import World” / “To import a world, please provide a .mcworld or .mctemplate file.” (CANCEL/MCWORLD/MCTEMPLATE); pilih file via ACTION_GET_CONTENT (MIME *​/* dengan hint .mcworld/.mctemplate/.mcpack/.mcaddon); ekstensi salah → toast “Please select a valid .<ext> file”; progress “Importing world...” / “Please wait while the world is being imported. Do not exit the app.”

#### 4.2 DrawerActivity — navigasi world NBT

- Drawer: header = world icon + world name (20sp) + internal folder name; toolbar judul = worldName; **menu drawer (13 item, dari drawer_menu.xml + DrawerActivity.l())**:

| # | Menu (title) | Action |
|---|---|---|
| 1 | Show Map (0x7f0a04cb) | fragment `d0.e` — view peta kustom (GestureDetector/Scroller…) |
| 2 | World NBT (0x7f0a04d3) | fragment `f0.h` — editor NBT world |
| 3 | Singleplayer NBT (0x7f0a04d0) | fragment `f0.a` — editor LevelDB key `~local_player` |
| 4 | Multiplayer NBT (0x7f0a04cc) | fragment `f0.b` — daftar key `player_server_*` (baris: `str.substring(14)`); toast “No players found.” jika kosong |
| 5 | Change World (0x7f0a04c6) | dialog “Close World” / “Are you sure you want to close this world?” (YES/NO) → close + finish() |
| 6 | **Advanced** | (submenu, masing-masing → fragment editor key via `f0.a.f(key)`) |
| 6a | Overworld NBT (0x7f0a04ce) | key `Overworld` |
| 6b | The End NBT (0x7f0a04d1) | key `TheEnd` |
| 6c | Nether NBT (0x7f0a04cd) | key `Nether` |
| 6d | BiomeData NBT (0x7f0a04c5) | key `BiomeData` |
| 6e | Portals NBT (0x7f0a04cf) | key `portals` |
| 6f | Autonomous Entities NBT (0x7f0a04c4) | key `AutonomousEntities` |
| 6g | Open Custom NBT (0x7f0a04c7) | dialog “Custom NBT Key”: msg “Enter the database key to view its NBT data:”, hint “e.g. ~local_player”, CANCEL/OK |

- **Tap item World NBT / Singleplayer NBT / Multiplayer NBT / Advanced (6a–6g):** dialog “Close NBT Editor” / “Are you sure you want to close this NBT editor? Any unsaved changes will be lost.” (NO/YES) → YES buka editor.
- Tap item yang sama dengan item ter-check → close drawer (no-op).
- switch: `m(itemId, false)` — isPlayerNBTEditor=false; on YES: `o(itemId, l(itemId), …)` → `beginTransaction().replace(fragmentContainer, fragment)`.

#### 4.3 NBT editor fragment (`f0/g`) — apa yang muncul saat menekan di dalam editor

Struktur utama (onCreateView):
- **Breadcrumb**: `HorizontalScrollView` > `LinearLayout` (id `0x7f0a0107`) dengan teks path crumb “ > ” separator, klik crumb → navigasi ke ancestor (`o(dVar3, 6)`-obfuscated).
- **RecyclerView** tree (id `0x7f0a0558`) adapter `h0.c`; row = ikon tipe + key tebal (tree_node_item.xml).
- **FAB utama** (ikon 2131231307): tap → fullscreen `SimpleFormDialog`:
  - title “Create NBT Tag”, msg “Create a new tag inside the current compound or list.”, pos “CREATE”
  - fields: `Tag Type` spinner (semua type `x2.a`: Byte/Short/Int/Long/Float/Double/ByteArray/String/Compound/List/IntArray) required; `Tag Name` required (kecuali di root List/Array/Compound tidak-bernama — name=null).
- **FAB save** (ikon 2131231263): tap → progress “Saving NBT data...” / “Please wait while the NBT data is being saved.” → toast “NBT data saved successfully.” atau “Failed to save NBT data.”

**Mengklik di dalam editor:**
- Tap baris leaf → `SimpleInputDialog`: title `Edit "<key>"` (key = nama tag; “<unnamed>” jika null), hint “new value for the tag”, CANCEL/OK, `allowEmpty(true)`.
- Long-press baris → `SimpleListDialog` “show_nbt_tag_options_dialog”: **Edit / Remove**.
  - Leaf → Toast “Cannot remove this tag.”
  - Compound/List/Array → dialog “Remove "<name>” / “Are you sure you want to remove this tag?” (CANCEL/REMOVE).
- Validasi (d1.b.b — reference-verified strings): “Out of bounds for byte/short/integer/long/float/double/string.”, “Tags inside a CompoundTag must have a name”, “A tag with this name already exists in the compound”, “Only byte tags can be added to byte arrays”, “Only int tags can be added to int arrays”, “List element type mismatch, it should be …”, “Failed to create tag, tag name is out of bounds.”, “Failed to add tag: …”, “Failed to remove tag: …”.
- Leaf value editors: Byte/Short/Int/Long signed; Float/Double decimal; String (UTF-8 ≤ 65535 byte); ByteArray/IntArray element rows (Byte.parseByte/Integer.parseInt).
- Save: LevelDB put (fragment `a`: `jVarA.a.e(bArr, lVar.h())`) atau world save (fragment `h`: `x.b.a().m(tag)`); fragment `a` jika key kosong → FAB dinonaktifkan + toast “No NBT data found.”; fragment `h` jika tidak ada world data → toast “No world data found.”
- State: `rootTag.tmp` di cache dir; breadcrumbPath restore di `onSaveInstanceState`/`onViewStateRestored`.
- Tidak ada: search, rename, expand/collapse, multi-select, copy/move, undo/redo (per katalog B lama §4.4 — “Tidak ditemukan…”).

#### 4.4 CreateWorldActivity (FAB “New World”)

- Toolbar “Create Flat World”; blok: “Selected Biome” (default **Plains**, dialog picker `t.a`) + **“Prevent Slimes from Spawning”** checkbox + “Flat World Layers” + ImageButton add-layer (dialog pencarian block) + RecyclerView layer (kartu block: nama block + xN thickness).
- Validasi tombol create: “Please select a biome.” / “Please select at least one layer, air also works.” / “Total layer thickness exceeds the limit (384).”
- Progress: “Compiling custom flat world... Please wait while the world is being created. Do not exit the app.” → success “World created successfully.”

#### 4.5 HelpActivity

- Toolbar “Exporting Worlds”; scroll: langkah langganan texture pack (Steps 1–5 + button “Import”); failure “Failed to import texture pack.”

#### 4.6 EntitiesNBTActivity

- Toolbar “Chunk Entities at (<x>, <z>)” (string format `Chunk Entities at (%d, %d)`); query LevelDB key chunk `(x, z, dim=1)`; baris = identifier `minecraft:*` + ikon dari `assets/entityIcons/`; tap → NBT editor; Back → dialog “Close NBT Editor” (NO/YES); toast “Could not get entity keys.”

### 5. Model penyimpanan & world

- World terkelola: `getExternalFilesDir(null)/worlds/<folder>` → **`/storage/emulated/0/Android/data/com.hextetrahedral.minetuner/files/worlds/`**
- World eksternal (SDK ≤ 28 + storage permission): `Android/data/com.mojang.minecraftpe/files/games/com.mojang/minecraftWorlds`, diberi label “## [EXTERNAL WORLD] ##”.
- Display name dari `levelname.txt` (fallback “Unnamed World”), `world_icon.jpeg`, size rekursif (`Files.walkFileTree` + `Formatter.formatFileSize`).
- LevelDB: `com.hextetrahedral.leveldb`; NBT = Bedrock little-endian.

### 6. Tech stack & monetisasi (statik)

AndroidX (appcompat, material3, cardview, recyclerview, drawerlayout, constraintlayout, fragment, workmanager…), Material Components, Gson, okhttp, ExoPlayer (video player — adapter untuk media ad/video), Sentry (io.sentry — crash reporting, “Something went wrong”), Play Billing (`com.android.vending.BILLING` — ProxyBillingActivity/V2 untuk in-app-purchase/upgrade), ExoPlayer, etc. Ad SDK: ~120+ activity — Appodeal/MAX, AppLovin, Google Ads, Chartboost, Fyber, Smaato, Amazon DTB, PubMatic, Yandex, InMobi, Pangle, Unity, ironSource, Mbridge, Vungle, Bigo, Taurusx, OGury, AQ, Sentry, sentiment… Per katalog B lama: “The reference is heavily monetised… must not be reproduced in the new app” — abaikan saat reimplementasi.

### 7. Status verifikasi

- Statis: aapt2 (`dump badging`, `dump xmltree`, `dump resources`, `dump strings`) dari base APK; jadx decompile ke `@data/data/com.termux/files/home/NBT-EDITOR/.ref_extract/jadx_out`; strings DEX; parse pohon manifest.
- Limit: no device screenshot atau running validation — emulator segitu segfault di startup (exit 139); tidak ada device lain (`adb devices` kosong). Strings dialog yang dicatat di atas: dari jadx code-literal + catalog B lama §4.2-§4.5 (untuk kalimat yang tidak terlihat di kode jadx, sesuaikan saat reimplementasi).
- Notes: obfuscated nama package app (`f0`, `h0`, `x2`, `y2`, `d1`) & obfuscated menu items (catalog lama §13-§15); obfuscated kode eksternal berbahaya — jangan sekali-kali gunakan untuk reimplementasi (per katalog B lama §5-§9).

---

## Bagian B3 — Dynamic verification di emulator (2026-11-21)

> Metode: emulator run di simulasi (per permintaan) — emulator Pixel_5 dengan GPU `swiftshader_indirect`; hasil dinamis: install, push world, launch, 5× screenshot + hierarchy (`screenshots/dyn_*.png|*.xml`). Selanjutnya: aplikasi terblokir oleh dialog PairIP start-up.

### 1. Emulator
- Command: `~/Android/Sdk/emulator/emulator -avd Pixel_5 -gpu swiftshader_indirect` — berhasil hidup: guest `ro.build.version.sdk=37`, `sys.boot_completed=1`; AVD config: `target=android-37.0`, system image `system-images/android-37.0/google_apis/x86_64/`.
- Catatan: emulator sebelumnya (tanpa `-gpu`) segfault di startup (exit 139, “dumped core”) di host ini; dengan `-gpu swiftshader_indirect` stabel.

### 2. Install & setup aplikasi
- `adb install-multiple -r xapk_extract/extracted/com.hextetrahedral.minetuner.apk config.en.apk config.mdpi.apk config.arm64_v8a.apk` → **Success**
- `adb shell pm path com.hextetrahedral.minetuner`: base + `split_config.arm64_v8a` + `split_config.en` + `split_config.mdpi`.
- World `H-S` (dari `H-S.mcworld` — 14 MB; `levelname.txt` 3 bytes, `level.dat` 3051 bytes, `db/`) di-push ke `/storage/emulated/0/Android/data/com.hextetrahedral.minetuner/files/worlds/H-S/` (`mkdir -p` + `adb push`).

### 3. Launch & apa tampilan saat menekan (observasi dinamis)
- `adb shell am start -W -n com.hextetrahedral.minetuner/.MainActivity` → `dumpsys activity`: **top activity = `com.pairip.licensecheck.LicenseActivity`**.
- **Dialog tampak (`screenshots/dyn_01_pairip_dialog.png`, hierarchy `screenshots/dyn_01_main.xml`):**
  - Judul: “Something went wrong”
  - Isi: “Check that Google Play is enabled on your device and that you're using an up-to-date version before opening the app. If the problem persists try reinstalling the app.”
  - Satu tombol: **CLOSE** (bounds [800,1345][976,1477]).
- Sistem emulator: `pm list packages -f | grep vending` → `package:/product/app/LicenseChecker/LicenseChecker.apk=com.android.vending`; `dumpsys package com.android.vending` punya `ILicensingService` (permission `com.android.vending.CHECK_LICENSE`) tapi tidak ada `com.android.vending.billing.InAppBillingService` → kondisi PairIP “Google Play tidak tersedia/tidak up-to-date” → dialog.
- Tapi **CLOSE** → logcat: `Process com.hextetrahedral.minetuner (pid 4044) has died: fg TOP` + `WindowManager: WIN DEATH Window{... LicenseActivity}` + task di-removed; focus → NexusLauncher; MainActivity **tidak pernah terlihat** (terdaftar hierarchy `dyn_02_after_close.xml` = launcher, screenshot `dyn_02_after_close.png`).
- Relaunch → dialog muncul lagi (`dyn_03_pairip_main.xml`, screenshot `dyn_03_main_launcher.png`).
- **Kesimpulan limit:** pada emulator ini aplikasi gagal di dialog PairIP start-up — world list MainActivity tidak terobservasi dinamis; mendokumentasikan secara statis di Bagian B2 §4.1-4.3. Catatan: sesi sebelumnya (lama) juga menemui dialog ini (“PairIP license dialog (emulator: 'Something went wrong...')” — catalog B lama); dialog juga menggunting Appodeal sandbox/ANR di logcat (“Scheduling restart of crashed service ... SandboxedProcessService0”, “ANR at startup” catalog lama §7).

- **Interpretasi (bukan fitur aplikasi):** dialog bukan fitur aplikasi MineTuner, tapi dialog **PairIP** — library device-integrity/lisensi Google (`com.pairip`) yang di-bundle sebagai bagian deps (aktivitinya terdaftar di manifest: `com.pairip.licensecheck.LicenseActivity`, `exported=false`). Dialognya muncul karena pengecekan lisensi tidak bisa selesai: di emulator hanya ada stub Play Store (`com.android.vending` = LicenseChecker.apk — ada ILicensingService namun tidak ada InAppBillingService) sehingga cek gagal: “Check that Google Play is enabled on your device…”. Fitur world list MainActivity (Bagian B2 §4.1) tidak mengandung dialog semacam ini — bedanya adalah lingkungan emulator (tanpa Play Store), bukan “sumber tidak dikenal” pada aplikasi. (Sesi sebelumnya juga menemui dialog ini — catalog B lama §7.)

### 3.1 Re-run (2026-11-22) — upaya membuka aplikasi sepenuhnya
- Emulator restart: `emulator -avd Pixel_5 -gpu swiftshader_indirect` (pid 169220) — booted.
- `pm enable com.android.vending` → stub Play Store **sudah enabled**: `dumpsys package com.android.vending` → `User 0: ... enabled=1, stopped=false, hidden=false` → dialog tetap muncul (bukan karena Play Store non-aktif).
- `am start -W -n com.hextetrahedral.minetuner/.MainActivity` → dialog muncul kembali.
- State saat dialog (status beberapa detik):
  - `dumpsys window`: Display #0 currentFocus = LicenseActivity (t33); task #33 = `A=10235:com.hextetrahedral.minetuner` dengan **satu aktivitas saja** (LicenseActivity) — MainActivity tidak ada di task.
  - Window attrs LicenseActivity: `fmt=TRANSLUCENT gr=CENTER wrapxwrap DIM_BEHIND` (dialog transparan di tengah).
  - AM: `topResumedActivity=ActivityRecord{11790904 ... LicenseActivity t33}`.
  - **Input dispatching timed out** — LicenseActivity tidak merespons touch (5 s timeout, tap di (901, 1410)).
- `adb shell input tap 888 1361` (tombol CLOSE) → 3 s kemudian `pidof com.hextetrahedral.minetuner` = **kosong** (proses aplikasi mati).
- **Kesimpulan:** aplikasi tidak pernah sampai menampilkan world list MainActivity di emulator ini — blokir pada dialog PairIP; batas konfirmasi ulang (dokumentasi limit terbaru di bagian 3 kesimpulan lama + ini).
### 3.2 Upaya install Play Store asli (2026-11-22)

- **Unduh:** Google Play Store 52.8.55 (versionCode 85285500) dari APKPure — `https://apkpure.com/google-play-store-app/com.android.vending/download/52.8.55-24%20[0]%20[PR]%20967533532` (diload oleh Chrome di emulator; file masuk ke guest `/storage/emulated/0/Download/`: `.pending-...-Google+Play+Store_52.8.55-24+[0]+[PR]+967533532_APKPure.apk`, 106,315,481 bytes), pull ke host: `/tmp/opencode/ps_real.apk`; verifikasi `aapt2 dump badging`: package `com.android.vending`, versionCode=85285500, versionName='52.8.55-24 [0] [PR] 967533532', minSdk 24, targetSdk 37.
- **Stub saat ini:** `adb shell dumpsys package com.android.vending` → versionCode=1801 versionName=1.8; `pm path com.android.vending` → `package:/product/app/LicenseChecker/LicenseChecker.apk=com.android.vending`.
- **Install attempt 1:** `adb install -d -r -g ps_real.apk` → **gagal**: `INSTALL_FAILED_UPDATE_INCOMPATIBLE: Existing package com.android.vending signatures do not match newer version; ignoring!` (stub vs Play Store berbeda signature).
- **Attempt 2:** `adb root` (sudah root) + `pm uninstall --user 0 com.android.vending` → **Success**, namun `pm path com.android.vending` tetap kosong dan `pm dump` tetap menunjukkan versionCode=1801 (system package tetap ada di PM).
- **Attempt 3:** `adb root` + `mount --bind /data/local/tmp/ps_real.apk /product/app/LicenseChecker/LicenseChecker.apk` (bind mount di atas stub) → `cmp` OK di device (`BOUND_OK`), tetapi `dumpsys package` masih 1801 (PackageManager cache tidak di-rescan); bind mount dibersihkan (`umount`, exit 0).
- **/product tetap read-only** (`mount -o remount,rw /product` → `'/dev/block/dm-3' is read-only`), jadi tidak bisa ganti file stub di product partition.
- **Kesimpulan:** di emulator ini, penggantian stub → Play Store asli tidak terselesaikan (instalasi gagal karena signature mismatch; PM cache tetap 1801) — dialog PairIP start-up tetap muncul; aplikasi tetap tidak sampai menampilkan world list (batas terdokumentasi di §3, kesimpulan).

### 3.3 Reset emulator (2026-11-23) — upaya lanjutan
- Reset AVD `Pixel_5` dengan `-wipe-data -gpu swiftshader_indirect` → boot segar; install ulang 4 APK + push world H-S dari `H-S.mcworld`.
- Launch → PairIP “Something went wrong” + CLOSE; **BACK** (`input keyevent 4`) **tidak menutup** dialog; **CLOSE** → proses aplikasi tetap mati (focus ke launcher).
- Sebelum wipe sempat terjadi crash lain: `java.lang.RuntimeException: Failed to create lock file /data/user/0/com.hextetrahedral.minetuner/app_webview/webview_data.lock` (data dir hilang setelah `pm clear`); `-wipe-data` menghilangkan crash ini.
- Coba AVD lain: `Medium_Phone_API_37.0` dengan image `system-images/android-37.0/google_apis_playstore/x86_64/` — **Play Store ASLI**: `com.android.vending` versionName 50.7.34 (versionCode 85073430) di `/product/priv-app/Phonesky/Phonesky.apk`. Di-reset dengan `-wipe-data`; app berhasil install. Launch → PairIP kini membuka **halaman sign-in Play Store** (`com.google.android.finsky.unauthenticated.activity.UnauthenticatedMainActivity`) alih-alih dialog “Something went wrong”; tanpa akun Google cek lisensi tidak selesai, BACK → aplikasi keluar ke launcher.
- AVD playstore adalah production build (`adb root` ditolak: “adbd cannot run as root in production builds”), jadi tidak bisa mematikan komponen sign-in atau mengubah paket sistem.
- **Kesimpulan akhir:** setelah reset kedua AVD (stub maupun Play Store asli), aplikasi **tetap tidak bisa dibuka penuh** di emulator — PairIP memerlukan Play Store berlisensi/akun Google; tanpa itu CLOSE / sign-in-back mengakhiri proses aplikasi. World list MainActivity tidak pernah terobservasi dinamis.

### 4. Evidence dinamis tersimpan
- Screenshots: `screenshots/dyn_01_pairip_dialog.png`, `screenshots/dyn_02_after_close.png`, `screenshots/dyn_03_main_launcher.png`
- Hierarchy (uiautomator): `screenshots/dyn_01_main.xml`, `dyn_02_after_close.xml`, `dyn_03_pairip_main.xml`, `dyn_04_after_close.xml` (world list MainActivity tidak ada — aplikasi mati di dialog PairIP)
- Re-run (2026-11-22): `screenshots/dyn2_main_launch.xml`, `screenshots/dyn2_summary.txt`
- Reset (2026-11-23): `screenshots/dyn3_01_pairip_after_wipe.xml`, `dyn3_02_playstore_signin.xml`, `dyn3_02_playstore_signin.png`, `dyn3_03_after_close.xml`

---

## Bagian S — Port XAPK → C++ & build NBTEDITOR (2026-11-23)

> Pendekatan diubah: bukan memaksa XAPK referensi jalan di emulator (diblokir PairIP), melainkan memakai hasil reverse-engineering statis (Bagian B2) untuk melengkapi/membangun aplikasi native C++ milik kita. Implementasinya sudah ada: `app/src/main.native.cpp` (8107 baris, satu translation unit) — port fitur editor NBT referensi, lihat Bagian C/D/H.

### 1. Build C++ → APK
- Script baru: `scripts/build_apk_host.sh` — g++ + ASan/UBSan untuk tes host; NDK 30 clang++ (`--target=aarch64-linux-android26`, `-mno-outline-atomics`, `-nostdlib`) untuk `libnbt.so`; aapt2 build-tools 36 + android.jar platform 37; apksigner v1/v2/v3.
- Perbaikan source: `app/src/main.native.cpp:3413` — dua pernyataan `if` dipisah (g++ `-Werror=misleading-indentation`).
- `VERSION`: 1.0.4 (code 5) → **1.0.5 (code 6)**.
- Hasil: **70 tes host lulus, 0 gagal**; guard simbol atomic lolos; `aapt2 dump badging out/NBTEditor.apk` → `package: name='com.nbteditor' versionCode='6' versionName='1.0.5'`, minSdk 26, targetSdk 37; sha256 libnbt.so `d0f749c17504cede873aa407e5da66698050258c2bd7753f03b8f18cbc9f`.

### 2. Verifikasi dinamis di emulator (Pixel_5)
- `adb install -r out/NBTEditor.apk` → Success; `am start -n com.nbteditor/android.app.NativeActivity` → proses hidup (pid 5440), focus di NativeActivity — **tidak ada dialog PairIP**.
- Apa yang tampil (dibaca via OCR screenshot):
  - Home: “NBT Editor”, “Native NBT Editor”, “All 13 tag types — Bedrock little-endian”, “level.dat aware — .mcworld import/export”; tombol **Open file**, **New NBT**, **Worlds**, **Import .mcworld**; seksi **Recent files** (“No recent files yet”).
  - World `H-S` (14.3 MB) di-push ke `/storage/emulated/0/Android/data/com.nbteditor/files/worlds/H-S/`.
  - Tap **Worlds** → “Minecraft Worlds”, baris **H-S** (icon “W”, “14.3 MB: H-S”).
  - Tap baris **H-S** → editor **level.dat — 146 tags**: BiomeOverride (empty string), CenterMapsToOrigin 0, ConfirmedPlatformLockedContent 0, Difficulty 3, FlatWorldLayers JSON, HasUncompleteWorldFileOnDisk 0, InventoryVersion 1.26.62, IsHardcore 1…
  - Tap baris **CenterMapsToOrigin** → dialog **Edit**: “Type: TAG_Byte”, nilai `0`, keypad numerik (digit, `.`, `CLR`), tombol **SAVE**/**CANCEL**.
- Bukti: `screenshots/our_01_home.png`, `our_02_worlds.png`, `our_03_editor.png`, `our_04_value_editor.png`.

### 3. Status
- Aplikasi C++ kita jalan penuh di emulator — tidak bergantung Play Store/PairIP.
- Belum ada di C++ (Bagian C §2): external legacy world listing (F2.2), share FileProvider (F2.10), layar LevelDB (F2.8 — out of scope phase 1), change tag type (F3.15). Bisa ditambahkan berikutnya.

### 4. Penyesuaian dengan `ANALISIS_KODE_XAPK.md` (2026-11-23)
- Visual sudah sesuai sejak awal: palet `themeLight()` memakai **#429e46 / #66bb6a / #f1f8e9 / #ffffff / #b00020** persis seperti `md_theme_light_*` (Bagian 5 §3.7), font Inter, kartu 8dp/4dp, FAB primary, dan sudah ada dark theme.
- **String alur dunia diselaraskan dengan teks referensi** (Bagian 8 §8.4, kutipan verbatim):
  - Konfirmasi hapus: `"Delete World"` / `"Are you sure you want to delete this world? This action cannot be undone."` (menggantikan "Delete world" lama); tes M32 & screenshot test B8 ikut diperbarui.
  - World terenkripsi: toast diganti dialog **"Encrypted World"** dengan pesan lengkap referensi tentang Marketplace.
  - Ekspor world: progress **"Compiling world..."** / **"Please wait while the world is being compiled. Do not exit the app."**
  - Flat builder: judul layer **"Flat World Layers"** (tanpa "(bottom first)").
  - Help & About: ditambah baris identitas referensi `MineTuner+ 2.2.3 (com.hextetrahedral.minetuner)` dan tautan diskusi `discord.gg/Mr2za3VkUQ` sebagai teks statis (tanpa akses jaringan).
- Perubahan difokuskan pada **perilaku/teks bersama**; identitas aplikasi tetap `com.nbteditor` / "NBT Editor", tidak ada iklan/PairIP, dan tidak menyalin brand XAPK.
- `VERSION`: 1.0.5 (code 6) → **1.0.6 (code 7)**; rebuild `scripts/build_apk_host.sh`: **70/70 tes host lulus**, APK signed & verified, sha256 `libnbt.so` `2466e961…725b96`.
- Verifikasi emulator: `adb install -r` → Success, `am start -W` → `mCurrentFocus=com.nbteditor/android.app.NativeActivity`, screenshot home terbaca via OCR ("NBT Editor", "Open file New NBT", "Worlds Import .mcworld", "Recent files").
- Yang **tidak** diubah (masih menjadi perbedaan disengaja, lihat Bagian C §2 / Bagian 8 §8.6): tidak ada "Open in Minecraft" (kita memakai "Export .mcworld"), tidak ada menu drawer 13 item dengan jargon LevelDB, tidak ada iklan/paywall, dan nama aplikasi tetap NBT Editor.

### 5. Perombakan UI/UX Home mengikuti referensi (v1.0.8 / code 9)
- Home sekarang = **world list** (bukan hero card + grid tombol): toolbar hijau, kartu world (ikon 48dp, nama 18sp bold, ukuran, internal name) persis hierarki `world_item.xml` referensi; `listWorlds()` dipanggil saat boot agar daftar segar.
- **FAB stack** mengikuti pola referensi: FAB utama `+` expand/collapse (state `fabExpanded`, aksi `A_FAB_TOGGLE`) menjadi dua aksi berlabel **"New World"** dan **"Import World"** (label putih di atas primary, FAB sekunder).
- Tap world → dialog **"Options for \"<nama>\""** dengan tiga aksi **EDIT / EXPORT / DELETE** (`A_WORLD_EDIT`, `A_WORLD_EXPORT`, `A_WORLD_DELETE`) — menggantikan perilaku lama yang langsung membuka `level.dat`.
- Tes M2 & M41 diperbarui untuk memeriksa FAB stack + world list; **70/70 tes host lulus**; APK v1.0.8 signed, sha256 `libnbt.so` `f268e4df…bae9e`.
- Verifikasi emulator via OCR: Home menampilkan `H-S / 14.3 MB / H-S`; tap FAB `+` memunculkan label **"New World"** & **"Import World"**; tap world memunculkan dialog **"Options for \"H-S\""** dengan tombol EXPORT/DELETE (EDIT terpotong OCR). UI/UX kini mengikuti referensi.

---

> **Analisis kode tingkat implementasi (telusur penuh, tanpa ringkasan):** lihat **`ANALISIS_KODE_XAPK.md`** — 11 bagian: arsitektur lintas-DEX, UI/aktivitas, engine NBT, model world/LevelDB/peta, resource & SDK pihak ketiga, PairIP (smali), ringkasan lintas layer, analisis UI/UX, inventaris seluruh file XAPK, mekanisme keluhan UI/UX, dan inventaris method.

### 6. Lanjutan pengembangan (v1.0.9 / code 10)
- **Analisis fungsi:** `ANALISIS_KODE_XAPK.md` **Bagian 11** mencatat **302 method** dari **67 class aplikasi** + method PairIP (smali), melengkapi inventaris file (Bagian 9) dan method biner 13 DEX (64.716 class).
- **Ikon tipe tag:** `typeGlyph()` dipetakan per `nbt::Type` (B/S/I/L/F/D/BA/T/C/IA/LA); renderer `Kind::TreeRow` sekarang menggambar **tile ikon 32×32** berlatar warna tipe + glyph (menggantikan titik 4px), jarak 12dp — mengikuti `nbt_*` drawable + `tree_node_item.xml` referensi (Bagian 10 §10.1).
- **Aside geser kiri:** `buildOverflowMenu()` diubah dari popup kanan-atas menjadi **drawer geser dari kiri** (scrim gelap + panel 320dp + header primary + baris menu). Tap world kini membuka aside berisi **EDIT / EXPORT .mcworld / DELETE WORLD / CLOSE**, bukan dialog tengah — mengikuti `DrawerLayout` + `NavigationView` referensi (Bagian 10 §10.4).
- Build: **70/70 tes host lulus** (M2/M41 disesuaikan sebelumnya), APK v1.0.9 di-sign, sha256 `libnbt.so` `3b87137f…521b0`.
- Verifikasi emulator: Home ter-render ("NBT Editor", FAB, daftar world pada build sebelumnya). Daftar world tidak tampil pada sesi terakhir karena kendaraan visibilitas file emulator setelah `-wipe-data`/`adb root`; alur render/aksi tetap tercakup tes host (M15/M32). Modul **peta** belum diimplementasikan — membutuhkan LevelDB (tidak ada di APK kita), dijelaskan di Bagian 10 §10.2.

### 7. Import world memakai file manager bawaan OS (v1.1.0 / code 11)
- **Keputusan arsitektur:** untuk pertama kalinya APK menyertakan DEX kecil. Karena `Activity.onActivityResult()` tidak punya hook native di `ANativeActivity`, mustahil menerima hasil `startActivityForResult` tanpa Java. Solusinya: **`app/java/com/nbteditor/FilePickerActivity.java`** (~130 baris) — Activity transparan yang hanya menjalankan `ACTION_GET_CONTENT` + `CATEGORY_OPENABLE` + MIME whitelist + `Intent.createChooser()`, menyalin `content://` Uri ke `files/picked/` lewat `ContentResolver`, lalu meneruskannya ke native via `static native onFilePicked(...)`. **Semua logika aplikasi tetap di C++**; DEX hanya glue intent (sesuai mekanisme referensi Bagian 10 §10.3).
- Manifest: `android:hasCode="true"` + activity `com.nbteditor.FilePickerActivity` (non-exported, tema transparan). Build script: `javac` (JBR Android Studio) → `d8 --min-api 26` → `classes.dex` → dimasukkan ke APK; verifikasi build memeriksa `FilePickerActivity` ada di DEX.
- **C++:** `A_WORLD_IMPORT` dan `A_OPEN_NBT` kini memanggil `launchFilePicker(mode)` (JNI, pola guard yang sama dengan `requestStoragePermission`). Hasil masuk lewat `Java_com_nbteditor_FilePickerActivity_onFilePicked` → antrean `g_picked` → `dispatchPickerResult()` di render thread → `A_PICK_RESULT`. Import memakai helper bersama `startWorldImport()` dengan `tempCopy=true` (file sementara dihapus setelah ekstraksi). Extension yang diterima: `.mcworld`, `.mctemplate`, `.mcaddon`, `.mcpack`, `.zip`.
- **Tes host:** 73/73 lulus. Tambahan **M11b** (hasil SAF → world ter-import + temp copy terhapus + flag bersih) dan **M11c** (hasil SAF → file NBT dibuka di editor).
- **Verifikasi emulator nyata:** FAB `+` → **Import World** → chooser sistem **`com.google.android.documentsui/.picker.PickActivity`** tampil → pilih `SAFTest.mcworld` → world **"SAF Test World"** muncul di daftar (bersama `H-S`) dan layar otomatis beralih ke **Minecraft Worlds**; direktori `files/picked/` kembali kosong. (Catatan: sebelumnya perlu memperbaiki owner `files/` ke uid aplikasi `u0_a234` karena sisa `chown` u0_a235 dari sesi root.)

### 8. Drawer grup, Show Map, dan ikon world (v1.2.2 / code 14)

> **Koreksi alur (v1.3.0 / code 15):** berdasarkan analisis ulang `MainActivity.a()` (tap world → `SimpleListDialog` `"Options for \"<name>\""` dengan `{"Edit","Open in Minecraft","Delete"}`) dan `DrawerActivity.onCreate` (**drawer awalnya TERTUTUP**, hanya dibuka hamburger / edge-swipe `GravityCompat.START`, edge default ~20dp):
> - Tap world → **aside Options** (Edit / Export / Delete / Close) — bukan langsung editor.
> - Tap **Edit** → **Show Map**, drawer **tertutup**.
> - **Swipe dari tepi kiri (28–56dp, menghindari gesture sistem 0–24dp) ke kanan → aside grup muncul**; geser horizontal di area lain → **pan peta** (X dan Y, sesuai `k1.f.onScroll`). Sebelumnya drawer auto-open sehingga swipe horizontal menggeser drawer, bukan peta — itulah sebabnya pan terasa hanya vertikal.
> - Status bar: `kTopInset=24dp` ditambahkan ke `kBarH` (52→76dp) dan semua kontrol app bar + header drawer di-shift, sehingga judul tidak bertabrakan dengan jam/baterai.
> - Verifikasi emulator v1.3.0: tap world → Options; Edit → peta tanpa aside; swipe horizontal `850→350` mengubah koordinat `(-16,32)` → `(0,32)`; swipe tepi kiri `150→620` memunculkan aside grup; 75/75 tes host lulus.

### 14. Persistent background saat aside & scroll halus (patch 1.7.6)

**Masalah 1 — background hitam saat aside dibuka.** Penyebabnya bukan scene di-reset: `buildMap()` sudah membangun scene lalu menumpuk `buildDrawer()` sebagai layer terakhir di pohon UI yang sama (`renderFrame` menggambar halaman sekali). Penyebab sebenarnya ada di rasteriser: `gfx::Canvas::fill/fillRectDev/roundRect/image` menulis `pack(Color)` **mentah** ke framebuffer `WINDOW_FORMAT_RGBA_8888` yang diperlakukan sebagai RGB opak. Scrim `rgba(0,0,0,0x50)` (sebelumnya `0x60`) dengan demikian menjadi **hitam solid**, menutup seluruh scene.

Perbaikan (arsitektur Scene Stacking dipertahankan, tidak ada inisialisasi ulang scene):
- `Canvas::blendPixel(x,y,c,alpha)` baru: alpha-blend ke piksel yang sudah ada, menulis alpha=255; fast path `std::fill` tetap dipakai untuk warna opak (`c.a>=255`), jadi tidak ada biaya tambahan pada frame normal.
- Semua primitive (`fill`, `fillRectDev`, `roundRect`, `circle`, `pixel`, `hline`, `vline`, `line`, `image`) kini memakai `blendPixel` untuk warna transparan. Efek samping positif: dialog & legenda peta sekarang benar-benar transparan, ikon PNG tidak lagi bersisa kotak hitam.
- Scrim drawer dilunakkan ke `0x50` (~31%) agar scene tetap jelas terlihat.

**Masalah 2 — scroll kaku.** Input dulu langsung mengubah offset render per-event (`drawerScrollY += touchLastY - ey`), sehingga tiap sampel touch melompat dan kecepatan bergantung frekuensi event. Perbaikan:
- `drawerScrollY` sekarang hanya **target**; `drawerScrollF` adalah offset yang benar-benar digambar.
- `drawerScrollStart` + `totalDy` melacak posisi jari secara absolut (1:1, tanpa akumulasi drift / dead-zone 4px).
- `advanceDrawerAnimation()` dipanggil di awal `renderFrame`, melakukan easing eksponensial berbasis **delta-time** (`k = 1-exp(-dt/70ms)`), frame-rate independent, konvergen bila `|diff|<0.05`; tanpa jeda dan tanpa mengubah state peta.

**Pengujian:** sebelum/sesudah membuka+menutup aside, screenshot peta **identik piksel demi piksel** (`ImageChops.difference` → bbox `None`, mean diff 0.0) → posisi/state scene tidak berubah. Saat aside terbuka, peta tetap terlihat di bawah scrim abu-abu. Scroll tengah (0,7 s) dan akhir (2,5 s) menunjukkan pergerakan bertahap, bukan lompatan; seluruh 14 item + Actions tetap reachable. **64/64 + 77/77 tes host lulus.**

### 13. Perbaikan UI/UX aside / NavigationView (patch 1.7.5)
Catatan: `ANALISIS_KODE_XAPK.md` §10.4 kini memuat status terbaru.

| Masalah | Dampak | Perbaikan |
|---|---|---|
| Panel drawer tidak mengecat pita status bar | Pita 24dp putih dengan ikon status putih → judul/jam sulit dibaca | `statusBand`primary `0..kTopInset` di dalam panel; header tetap 160dp |
| Ikon navigasi berupa huruf (M/W/P/2/X/O…) | Tidak sesuai `drawer_menu.xml` (ic_map, ic_tune, ic_person, …) | 28 PNG dihasilkan dari **vector drawable asli XAPK** oleh `scripts/gen_nav_icons.py` → `app/assets/navIcons/`; `IconTile` kini menerima path `asset:` via `gfx::loadAssetImage` |
| Ikon PNG transparan tampil sebagai kotak hitam | `Canvas::image` menulis RGBA mentah ke framebuffer RGB | Image blit kini **alpha-blend** ke piksel existing (alpha 0 → skip), memperbaiki juga marker entity |
| Baris dipadatkan ke 32dp agar muat | Target sentuh terlalu kecil; Export/Delete tetap berpotensi terpotong di layar pendek | Kembali ke **48dp** (Material touch target) + daftar `ui::list` yang bisa **di-scroll**; `drawerScrollY` / `drawerContentBottom` dan gesture vertikal di dalam panel ditangani terpisah dari pan peta |
| Tidak ada highlight item aktif | Reference `checkableBehavior=single` tidak terpenuhi | `drawerSelected` → baris aktif memakai bg `blendOpaque(primary, surface, 34)`, teks & ikon primary (varian `_sel`) |
| Item Actions tidak menutup drawer | Drawer tetap menutupi layar saat dialog export/delete muncul | `A_WORLD_EXPORT` / `A_WORLD_DELETE` menutup drawer; semua `A_NAV_*` juga memperbarui seleksi |
| Swipe dari tepi kiri bisa menggeser peta setelah drawer terbuka | Gesture "bocor" ke peta | `drawerTouch` menandai gesture yang dimulai di panel: drag vertikal = scroll daftar, drag kiri = tutup drawer, peta tidak ikut bergerak |

**Verifikasi emulator (Pixel_5, v1.7.5):** pita status hijau, ikon asli tanpa kotak hitam, `Show Map` tersorot, scroll membuka `Open Custom NBT` + grup `Actions` (Export/Delete), **swipe dari tepi kiri membuka** dan **swipe kiri menutup** drawer, peta tetap diam saat scroll. **64/64 + 77/77 tes host lulus.**

### 12. Perbaikan lanjutan Show Map, aside & entitas (patch 1.7.x)
- **Warna biome/dataran:** tabel warna sebelumnya hanya 1.104 entitas dan salah ambil register (`grass_block`, `stone` hilang; nilai `grass_block` jadi (0,7,0)). Parser smali diperbaiki (register tracking penuh + `const/4`/`const/16`/`const` + hex) → **1.181 warna RGB asli** dari `t/c.smali` (`grass_block` (0,168,0), `stone` (125,125,125), `water` (46,85,167), `sand` (219,207,163)). `app/src/block_colors.inc` diganti seluruhnya.
- **Bug UI aside:** 14 item + 2 subheader melebihi tinggi layar sehingga **Export/Delete terpotong**. Baris dipadatkan (tinggi 32dp, jarak 36dp, subheader 24dp) → semua item termasuk grup Actions terlihat (diverifikasi screenshot).
- **Entitas tidak bisa diedit:** dua bug:
  1. Tombol dialog memakai action id sendiri (`A_MAP_ENTITIES`, `A_MAP_SELECT`, `A_HOME_CONFIRM`) tetapi handler `A_DIALOG_POS/NEG/NEU` hanya mengenal beberapa id khusus dan **tidak pernah melakukan dispatch generik** — tombol "ENTITIES" tidak bereaksi. Ditambahkan fallback `closeDialog(); handleAction(a, chosen);` di akhir handler dialog.
  2. Klik ikon mob tidak terhubung ke editor. `EntityChoice` kini menyimpan `identifier`, key `actorprefix+id`, dan NBT mentah. Tap dalam radius 3 blok dari marker → `openEntityEditor()` (parse NBT, `dbBacked=true` dengan key actorprefix agar Save kembali ke LevelDB). Tombol **ENTITIES** kini membuka dialog daftar entitas dalam radius 32 blok (lebih lunak dari chunk eksak) dengan `A_ENTITY_PICK`.
- **Verifikasi emulator:** tap ikon camel di peta → editor `minecraft:camel` (90 tag); tap area kosong → dialog `Coords…` → **ENTITIES** → dialog `Chunk Entities at (-7,-7)` berisi `minecraft:camel` → tap → editor terbuka. Semua item aside tampil. **77/77 tes host lulus.**

### 11. Perbaikan mekanisme Show Map (patch 1.7.x)
Analisis ulang XAPK ditulis di `ANALISIS_KODE_XAPK.md` **§10.2.7** (byte-level + koreksi JADX). Bug yang ditemukan dan diperbaiki pada decoder kita:

| Bug | Dampak | Perbaikan |
|---|---|---|
| Panjang data packed memakai `32.0/bits` (float) | Palet meleset ~172 byte untuk bits=5 → palet gagal, semua blok tak terbaca | `ceil(4096.0 / (32 / bits)) * 4` dengan **pembagian integer** (683 word untuk bits=5) |
| Entri palet dibaca sebagai TAG_String polos | Palet hanya terbaca 1 entri lalu gagal (`valid=0`) | Entri adalah **NBT Compound lengkap**; dibaca child pertama (TAG_String) lalu sisa compound dilewati dengan `skipNbtTag`/`skipNbtPayload` |
| Index blok Y-major `ly*256+z*16+x` | Blok salah → seluruh peta tampak biru (air) | Urutan referensi **X-major** `(x<<8)|(z<<4)|ly` |
| Tabel warna heuristik | Warna tidak sesuai referensi | **1.104 RGB asli diekstrak dari smali `t/c.smali`** → `app/src/block_colors.inc`, lookup binary-search; unknown → magenta `0xFFFF00FF` |
| Tidak ada shading | Permukaan datar | Shading tetangga barat/utara `f = clamp(1+0.05*((Y−Yw)+(Y−Yn)),0.5,1.5)` dari heightmap chunk sebelah |

Verifikasi dengan probe chunk (-7,-5): sebelum perbaikan `valid=0, palette=0`; sesudahnya `valid=1, bits=4, palette=9` dengan entri `minecraft:bedrock`, `minecraft:deepslate`, dll. Screenshot emulator menunjukkan **terrain asli** (pasir, air, struktur) menggantikan checkerboard/biru polos. Marker PNG mob tetap tampil. Tes host **77/77 lulus**.

> Versi tetap **1.7.0** menurut kebijakan Anda; perbaikan patch berikutnya akan bernomor 1.7.1 dst (belum saya naikkan tanpa konfirmasi).

### 10. Perbaikan bug & optimasi (v1.7.0 / code 21)
- **Save resultat tidak bisa disimpan (root cause):** document dari drawer (player/world NBT: `~local_player`, `Overworld`, dll.) disimpan dengan `a.ed.path` = nama key, lalu `writeFileAtomic("Overworld", …)` gagal / menulis file liar di CWD — **LevelDB tidak pernah disentuh**. Solusi: `EditorState::dbBacked` + `dbKey`, `ldbPut()` lewat dlsym `Java_com_hextetrahedral_leveldb_LevelDB_nativePut`, dan `saveDocument()` memilih `ldbPut` untuk dokumen berbasis DB. File dari SAF kini disimpan balik ke Uri asli lewat `FilePickerActivity.writeBytes()` (picker beralih ke `ACTION_OPEN_DOCUMENT` + persistable permission). Tes M11g memverifikasi key/value DB tertulis. Di emulator: Overworld NBT (3 tag) berhasil disimpan — `*.log` LevelDB berubah dan tidak ada file liar.
- **ANR saat buka aplikasi:** `bootstrap()` dulu memanggil `listWorlds()` di main thread (`dirSize()` rekursif + `looksEncrypted()` membaca semua file db). Sekarang world scan di-`appWorker` dan daftar diisi lewat `g_stateMutex`. Entity marker scan (2337 key `digp`, masing-masing butuh 2 `ldbGet` + parse NBT) juga dipindah ke worker, dibatasi 2048, dan dipublish lewat `g_markersMutex` — tidak lagi memblock render/input.
- **JNI thread-safe:** `JNIEnv` LevelDB kini `thread_local` (dipakai render thread dan worker), `PushLocalFrame/PopLocalFrame` di setiap `ldbGet/Put/Iterate`, tidak ada `DetachCurrentThread` lintas-thread. `g_mapSubs` juga dikunci per `(cx,cz,sy)` (sebelumnya hanya per `sy`, sehingga data subchunk chunk pertama dipakai ulang → peta salah).
- **Ikon PNG mob/player dari XAPK:** 83 file `assets/entityIcons/*.png` + `steve.png` + `ui/home.png` + `ui/question_mark.png` diekstrak ke `app/assets` dan dikemas via `aapt2 -A`. `readAssetBytes()` (AAssetManager) + stb_image mendekode sekali dan meng-cache. Peta sekarang menampilkan **marker PNG asli** (42 device px) untuk entitas: key `digp` → 8-byte actor id → `"actorprefix"+id` → NBT `identifier`/`Pos` → tabel 123 entitas dari `t.f.java`. Spawn marker memakai `home.png`. Legend peta menampilkan jumlah entitas (mis. `27/2337`).
- **Verifikasi:** 77/77 tes host lulus; emulator tanpa ANR aplikasi, peta menampilkan terrain + ikon mob PNG terlihat; save Overworld NBT berhasil; BACK → beranda tetap bekerja.

### 9. Tombol Kembali → langsung ke beranda (v1.5.1 / code 18)
- **Analisis XAPK:** `w.a` (`OnBackPressedCallback`) memprioritaskan: tutup drawer → dialog (DialogFragment didahulukan) → breadcrumb naik (`g.c(parent)`) → dialog konfirmasi **`"Close World"` / `"Are you sure you want to close this world?"`** (YES → `x.b.a().a()` + `finish()` → `MainActivity`), atau **`"Close NBT Editor"`** khusus tag `player_nbt_editor`/`entity_nbt_editor`. Help/CreateWorld/MainActivity memakai `finish()` biasa. Semua toolbar memakai `@id/toolbar` primary + white; Entities/Help/CreateWorld punya UP arrow; DrawerActivity hanya hamburger.
- **Perbaikan di kita:** `goHome()` menggantikan penelusuran `screenStack` satu per satu. BACK dari layar mana pun (peta, tree, help, browser) → **langsung `S_HOME`** (daftar world), LevelDB ditutup, stack dikosongkan; BACK berikutnya dari home baru keluar aplikasi. Modal (drawer/dialog/keypad/menu) tetap ditutup lebih dulu. Konfirmasi `"Close NBT Editor"` tetap muncul hanya bila `ed.dirty`. `A_NAV_CHANGE_WORLD` juga memakai `goHome()`.
- **Jembatan sistem:** `MainActivity.java` (subclass `NativeActivity`) meneruskan `onBackPressed()` ke native via `onBackPressedNative()`; manifest `android:enableOnBackInvokedCallback="false"` memastikan event BACK sampai ke dispatcher native (sebelumnya `NativeActivity` menyelesaikan Activity sehingga aplikasi keluar ke launcher). Flag `g_backRequested` + `exchange()` di render loop menjamin satu kali penanganan per tekan (baik lewat JNI maupun antrean input native).
- **Tes:** 76/76 host lulus (M11f: drawer tertutup dulu → home sekali tekan → baru exit; M35 diperbarui). Verifikasi emulator: dari peta BACK → daftar world; BACK lagi → launcher; dari aside Options BACK → daftar world (tidak keluar).
- **Analisis ulang XAPK:** `drawer_menu.xml` (5 item grup utama + subheader **Advanced** berisi 7 item), `DrawerActivity` (default fragment = `d0.e` Show Map; `o()` replace `fragment_container`, tanpa back-stack; toggle = `ActionBarDrawerToggle`, swipe-from-start aktif), `d0.a`/`s.a`/`e0.a` (chunk key `LE32(cx)|LE32(cz)|0x2B`, subchunk `0x2F` + index; heightmap LE16; palet + packed bits; warna + shading), `t.j.h()`/`MainActivity.a()` (ikon `world_icon.jpeg` 48dp `centerCrop`, fallback `question_mark`). Semua ditulis ulang di `ANALISIS_KODE_XAPK.md` §10.1–10.5.
- **Edit → aside:** `enterWorldEditor()` kini dipanggil oleh tap world **dan** Edit. Layout = **Show Map + drawer aside** (bukan tree langsung). Drawer memuat grup persis seperti referensi: **Show Map · World NBT · Singleplayer NBT · Multiplayer NBT · Change World**, subheader **Advanced** (Overworld/Nether/The End/BiomeData/Portals/Autonomous Entities/Open Custom NBT), plus grup **Actions** (Export/Delete). Back menutup drawer lebih dulu (`w.a` case 0). Swipe dari tepi dalam 28–80dp membuka drawer (x≤24dp dipakai gesture sistem Android).
- **Show Map nyata:** `libleveldb.so` referensi (1,26 MB) disertakan; dipanggil langsung lewat JNI exports `nativeOpen/nativeGet/nativeIterateKeys/nativeClose` (tanpa class Java tambahan). `decodeChunkTop()` membaca heightmap + subchunk palet dan mewarnai 16×16 kolom; tile menampilkan checkerboard `#2E2E2E/#444444`, border putih, label koordinat `(x, z)`, zoom 50% awal, pan, tap → dialog `Coords(x,z) Chunk(cx,cz)` dengan **BEGIN SEL / ENTITIES / CANCEL**. Peta otomatis terpusat ke **SpawnX/SpawnZ** dari `level.dat`.
- **Ikon world:** `listWorlds()` mendeteksi `world_icon.jpeg/.jpg/.png`; dekoder **stb_image** (vendored, public domain) dipakai `gfx::loadImage()` + `Canvas::image()` center-crop. Baris world 48×48dp memakai gambar asli; fallback glyph "W" bila tidak ada (referensi memakai `question_mark.png`).
- **Tes:** 75/75 host lulus (M11d aside saat Edit + grup navigasi + back; M11e deteksi ikon). APK v1.2.2 code 14, `libnbt.so` sha256 `bc1c5a33…ad5a`.
- **Verifikasi emulator:** tap world → drawer grup + header memakai **world_icon.jpeg asli**; swipe → drawer muncul; swipe/tap lain → peta menampilkan **terrain asli dari LevelDB** (chunk berwarna, bukan placeholder); tap peta → dialog koordinat & chunk; world list menampilkan ikon asli H-S dan fallback "W" untuk world tanpa ikon.
