# Bagian 1 — Arsitektur & alur keseluruhan

## 1.1 Peta modul lintas DEX

Aplikasi ini adalah APK multildex dengan **13 file `classes*.dex`**. Paket-paket yang membentuk aplikasi (bukan library iklan) tersebar sebagai berikut:

| Paket / file | DEX roughly | Peran |
|---|---|---|
| `com.hextetrahedral.minetuner.MainActivity`, `DrawerActivity`, `app.*` | classes9 | UI utama (world list, drawer, create world, entities, help) |
| `a0.*`, `b0.*`, `c0.*` | classes9 | util UI: FAB-stack animator, ads gate, antarmuka callback adapter |
| `d0.*` (term. `d0.e` 431 baris) | classes9 | custom map view + presenter/helper peta |
| `f0.*`, `h0.*`, `e0.*`, `i0.*`, `w.*`, `q.*`, `r.*`, `u.*` | classes9 | editor NBT (fragment, adapter tree, node state), callback iklan, back handler |
| `s.*`, `t.*`, `y.*`, `z.*` | classes9 | `s.a` chunk model/parser; `t.a/c/f` tabel biome/block/entity, `t.j` world context, `t.i` visitor ukuran; `y.*` adapter & compat; `z.*` **dialog** pemilih biome/block + slider (bukan model world) |
| `x.b` (13 baris) | classes9 | singleton world context (`AtomicReference<t.j>`) |
| `x2.a`, `y2.*` | classes6 | enum tipe NBT + 12 kelas tag + parser/serializer + cast helper |
| `d1.b` (517 baris) | classes12 | validator nilai, builder pohon editor, generator world flat, copy asset, import/export helper |
| `com.pairip.licensecheck.*` (22 class) | classes2 | license check (only smali; auto-start via ContentProvider) |
| `com.hextetrahedral.leveldb.LevelDB` | classes12 | wrapper JNI ke `libleveldb.so` |

Konsekuensi praktis: membaca satu alur sering memerlukan lintasan antar-DEX — misalnya fragment `f0.g` (classes9) memanggil enum `x2.a` (classes6) dan helper `d1.b` (classes12). JADX memberi nama konstanta berbeda antar-dex (`TAG_COMPOUND` vs `x2.a.m`); lihat Bagian 3 §2.4 untuk pemetaannya.

## 1.2 Lapisan arsitektur (dari bawah ke atas)

```
[6] Entry point / OS
      android.app.NativeActivity? TIDAK — aplikasi ini Java/Kotlin:
      com.hextetrahedral.minetuner.MainActivity (launcher) + PairIP provider
[5] UI layer
      MainActivity, DrawerActivity, app.CreateWorldActivity,
      app.EntitiesNBTActivity, app.HelpActivity,
      f0.g/a/h/b (fragments editor), h0.c/b (adapter tree),
      y.* (adapter world/player/layer), d0.e (map view)
[4] Presentation / state
      h0.d (node tree), f0.g.g (current node), DrawerActivity.k (checked item),
      Bundle extras: worldName/internalName/db_key/chunkX/chunkZ/itemPos/newFragmentId,
      onSaveInstanceState → cacheDir/rootTag.tmp + breadcrumbPath
[3] Domain / business
      x2.a + y2.l (NBT type/parse/serialize), d1.b (validation, world builder),
      t.a (biome), t.c (block table, 1205 baris), t.f (entity icons)
[2] Data / repository
      t.j (LevelDB wrapper + level.dat header I/O), x.b.a() singleton,
      d1.b.c/d (copy asset), d1.b.l/n (Uri → file, zip extract),
      z.* (dialog pilih biome/block + slider; BUKAN model world),
      t.j (world context: levelname.txt, level.dat, enkripsi, LevelDB, cache chunk)
[1] Platform / libs
      ReentrantReadWriteLock, Handler(Looper.getMainLooper()), zip4j,
      Gson/JSONObject, OkHttp, Appodeal/AdMob/MAX + ~20 SDK iklan,
      PairIP LicenseClient, Sentry, Play Billing, libleveldb.so
```

Prinsip yang terlihat: **UI tidak berbicara langsung ke LevelDB**. Semua akses world melewati singleton `x.b.a()` yang mengembalikan `t.j` (world context). Fragment editor hanya menerima root tag `y2.l` dan memanipulasinya di memori; penyimpanan terjadi lewat method `d()` yang dioverride per fragment.

## 1.3 Konteks world singleton

```java
public abstract class b {
    public static final AtomicReference a = new AtomicReference(null);
    public static j a() { return (j) a.get(); }   // t.j
}
```

- `x.b.a` di-set di `MainActivity` setelah world dipilih (dari path folder world) dan dibaca oleh `DrawerActivity`, `f0.a`, `f0.h`, `f0.b`, `EntitiesNBTActivity`.
- Jika null, aktivitas/fragmen redirect ke `MainActivity` dengan flag `FLAG_ACTIVITY_CLEAR_TOP|NEW_TASK` (`0x14000000`) — ini yang terjadi bila world belum dibuka.
- `t.j` menyimpan: path folder world (`c`), objek `LevelDB` (`a`), `ReentrantReadWriteLock` (`d`), dan menyediakan `c(key)`/`e(key)`/`e()`/`l()`/`m()`.

## 1.4 Alur startup penuh (urutan kausal)

```
Proses aplikasi dimulai
 ├─ Android meng-instantiate SEMUA provider di manifest
 │    └─ com.pairip.licensecheck.LicenseContentProvider.onCreate()
 │         └─ new LicenseClient(ctx).initializeLicenseCheck()
 │              ├─ performLocalInstallerCheck()  (installer harus Play Store)
 │              ├─ bind ILicensingService (com.android.vending)
 │              ├─ ResponseValidator: RS256 + SHA256withRSA + packageName
 │              └─ gagal → LicenseActivity(activitytype=PAYWALL|ERROR_DIALOG)
 │                          → paywall/PL sign-in ATAU "Something went wrong" → closeApp()
 └─ MainActivity.onCreate()
      ├─ setContentView(activity_main) + toolbar "MineTuner"
      ├─ FAB stack (a0.c) + listeners (import/new world)
      ├─ low-storage warning (<10 MB) → SimpleDialog
      ├─ SDK ≤ 28 ? minta READ/WRITE_EXTERNAL_STORAGE : langsung l()
      ├─ l(): scan getExternalFilesDir(null)/worlds + (legacy) Minecraft worlds
      ├─ Appodeal.initialize(...) → x.a callback → d1.b.a = true
      └─ onResume() → l() lagi (refresh world list)
```

Catatan: PairIP berjalan **sebelum** `MainActivity` sempat menggambar, karena provider di-instantiate lebih awal. Inilah sebabnya analisis dinamis di emulator (B3) selalu berakhir di dialog/paywall.

## 1.5 Model penyimpanan (ringkas; detail Bagian 4)

```
<externalFilesDir>/worlds/<nama-world>/
 ├─ level.dat                 ← NBT + 8-byte Bedrock header
 ├─ levelname.txt             ← nama tampilan
 ├─ world_icon.jpeg           ← ikon (opsional)
 ├─ db/                       ← LevelDB (keys: ~local_player, player_server_*,
 │                              Overworld, TheEnd, Nether, BiomeData,
 │                              portals, AutonomousEntities, chunk keys)
 ├─ behavior_packs/, resource_packs/, ...
 └─ world_behavior_pack_history.json, world_behavior_packs.json, ...
<externalFilesDir>/export.mcworld     ← hasil compile world (zip)
<externalFilesDir>/import.<ext>       ← file sementara hasil Uri copy
<cacheDir>/rootTag.tmp                ← state editor saat rotasi
```

Deteksi world terenkripsi (heuristik): baca dua `int32` LE dari setiap file di `<world>/db/`; jika `first == 0 && second == -1680885252` → `j()` bernilai true → dialog “Encrypted World”.

## 1.6 Threading & konkurensi

- Semua pembaruan UI dialog/progress melewati `d1.b.r(Runnable)` → `new Handler(Looper.getMainLooper()).post(...)`.
- `d1.b.q(...)` menampilkan progress dialog di main thread lalu menjalankan `Runnable` (pekerjaan berat: import/export/compile world) di worker thread tersembunyi di balik class synthetic.
- `t.j` memakai `ReentrantReadWriteLock` (`d`) untuk melindungi akses LevelDB. **Catatan bug:** `t.j.m()` (tulis `level.dat`) memakai `readLock()` padahal melakukan tulis; `f0.a.d()` sudah benar memakai `writeLock()`.
- `MainActivity.l()` memakai `ReentrantReadWriteLock` per world entry (`t.j.d`) saat menghitung ukuran folder dengan `Files.walkFileTree`.

## 1.7 Pola dialog & identitas aksi

Aplikasi memakai library `eltos.simpledialogfragment` dengan pola:

```
SimpleDialog.build()
   .title(...)
   .msg(...)
   .pos("...")/.neg("...")/.neut("...")
   .extra(Bundle)
   .show(fragment/activity, "TAG_DIALOG")
→ onResult(String tag, int result, Bundle extras)
     result == -1 → POSITIVE/OK, -2 → NEGATIVE/CANCEL
```

Daftar tag dialog seluruh aplikasi: `import_world_dialog`, `about_dialog`, `delete_world_dialog`, `edit_world_progress_dialog`, `delete_world_progress_dialog`, `export_world_progress_dialog`, `import_file_progress_dialog`, `close_world_dialog`, `close_nbt_dialog`, `enter_db_key_dialog`, `show_nbt_tag_options_dialog`, `remove_nbt_tag__dialog`, `edit_nbt_tag_dialog`, `add_new_nbt_tag_dialog`, `edit_nbt_tag_type_dialog`, `save_world_progress_dialog`, `pick_biome_dialog`, `pick_block_dialog`, `import_texture_pack_progress_dialog`.

Bundle keys yang sering dipakai: `worldPath`, `itemPos`, `worldName`, `internalName`, `destZipPath`, `error`, `db_key`, `SimpleInputDialog.text`, `NEW_TAG_NAME`, `NEW_TAG_TYPE`, `SimpleListDialog.selectedSingleLabel`, `newFragmentId`, `isPlayerNBTEditor`, `tagTypeToCheck`, `texturePackPath`, `chunkX`, `chunkZ`, `lastCheckedItemId`, `breadcrumbPath`.

## 1.8 Gaya kode & temuan lintas layer

- Nama class obfuscated, tetapi method tetap deskriptif pada dex tertentu (`x2.a.TAG_*` vs `x2.a.c/d/f/...`).
- Banyak `switch(i)` dengan dua cabang identik akibat JADX (konstanta berbeda tapi kode sama) — bukan bug runtime.
- Penanganan exception umum: `catch (Exception unused) {}` — banyak kegagalan dibungkam (parser NBT, restore editor, ikon world).
- Charset default dipakai untuk nama/string NBT; validator memakai eksplisit UTF-8 (inkonsistensi potensial, lihat Bagian 3).
- Tidak ada Room/DataStore; preferensi dan state disimpan via `SharedPreferences`, file cache, dan struktur folder.
- Monetisasi menyumbang mayoritas klass (5222 file Java hasil dekompilasi), tetapi tidak masuk jalur data NBT — hanya `d1.b.a/b/t` (flag Appodeal + throttle interstitial) yang bersentuhan dengan UI.
