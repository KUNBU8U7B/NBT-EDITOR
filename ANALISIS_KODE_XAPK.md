# Analisis Kode XAPK MineTuner+ 2.2.3 — Telusur Penuh

> Dokumen ini adalah analisis kode tingkat implementasi dari `MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk` (`com.hextetrahedral.minetuner`, versionCode 16 / versionName 2.2.3). Tujuannya: **mendokumentasikan semua yang bisa ditelusuri** dari kode hasil dekompilasi — aktivitas, fragment, adapter, listener, dialog, engine NBT, model world, LevelDB, peta, resource, manifest, native library,sampai ke dependencies pihak ketiga — tanpa mengabaikan detail kecil.
>
> Analisis bersifat **statis** (dekompilasi + resource + smali). Eksekusi dinamis di emulator dibahas di `DOKUMENTASI.md` Bagian B3/B4 (PairIP memblokir aplikasi referensi). Tidak ada kode yang dijalankan dari aplikasi referensi untuk menulis dokumen ini.

## 0. Sumber & metodologi

| Sumber | Path | Keterangan |
|---|---|---|
| APK dasar | `xapk_extract/extracted/com.hextetrahedral.minetuner.apk` | 110 MB, 13 file `classes*.dex`, `resources.arsc`, `assets/` |
| Dekompilasi jadx (sebagian besar app) | `.ref_extract/jadx_out/sources/` | Aktivitas, obfuscated packages `a0`–`z`, `com/hextetrahedral/minetuner` |
| Dekompilasi jadx (engine NBT) | `.ref_extract/jadx_c6/sources/` | `x2/a.java` (enum tipe), `y2/*.java` (kelas tag) |
| Dekompilasi jadx (helper) | `.ref_extract/jadx_c12/sources/d1/` | `d1/b.java` (helper validasi/serialization) |
| Smali apktool | `.ref_extract/apktool_out/smali_classes*/` | PairIP (22 class), alternatif verifikasi kode |
| Manifest | `aapt2 dump xmltree` / `dump badging` | Semua komponen, permission, meta-data |
| Resource | `aapt2 dump resources` / `dump strings` / `dump xmltree` | 742 string, 1208 color, 527 layout, 6 menu, dll |
| Aset | `.ref_extract/base/assets/` | `worldTemplate/`, `entityIcons/`, `texture_pack.mcpack`, dll |

**Cakupan kode aplikasi:** dari 13 DEX ditemukan **113 class** yang termasuk paket aplikasi/obfuscated (`a0`–`z`, `com/hextetrahedral/**`) + **22 class PairIP** (`com/pairip/licensecheck/**`, hanya tersedia sebagai smali). Total file Java hasil dekompilasi seluruh APK: **5.222** (mayoritas library pihak ketiga). Seluruh class aplikasi tercakup dalam 3 direktori jadx di atas; PairIP dicakup dari smali.

**Catatan obfuscation:** sebagian besar class aplikasi memakai nama pendek (`f0.g`, `d1.b`, `x.b`, `y2.l`, dll). Nama asli tidak tersedia; dokumen ini memakai nama dekompilasi apa adanya dan menjelaskan perannya.

## Daftar isi

- [Bagian 1 — Arsitektur & alur keseluruhan](#bagian-1--arsitektur--alur-keseluruhan)
- [Bagian 2 — Aktivitas, fragment, adapter & alur interaksi (UI)](#bagian-2--aktivitas-fragment-adapter--alur-interaksi-ui)
- [Bagian 3 — Engine NBT: tipe tag, parser/serializer, validasi, editor](#bagian-3--engine-nbt-tipe-tag-parserserializer-validasi-editor)
- [Bagian 4 — Model world, penyimpanan, LevelDB, peta & aset](#bagian-4--model-world-penyimpanan-leveldb-peta--aset)
- [Bagian 5 — Manifest, resource, native library & SDK pihak ketiga](#bagian-5--manifest-resource-native-library--sdk-pihak-ketiga)
- [Bagian 6 — PairIP: kode lisensi, paywall & error dialog](#bagian-6--pairip-kode-lisensi-paywall--error-dialog)
- [Bagian 7 — Ringkasan temuan lintas lapisan](#bagian-7--ringkasan-temuan-lintas-lapisan)
- [Bagian 8 — Analisis UI/UX: desain, interaksi, aksesibilitas & evaluasi](#bagian-8--analisis-uiux-desain-interaksi-aksesibilitas--evaluasi)
- [Bagian 9 — Inventarisasi EXHAUSTIF seluruh file XAPK (tanpa pengecualian)](#bagian-9--inventarisasi-exhaustif-seluruh-file-xapk-tanpa-pengecualian)
- [Bagian 10 — Mekanisme referensi untuk keluhan UI/UX (analisis mendalam)](#bagian-10--mekanisme-referensi-untuk-keluhan-uiux-analisis-mendalam)
- [Bagian 11 — Inventarisasi FUNGSI/METHOD seluruh kode aplikasi](#bagian-11--inventarisasi-fungsimethod-seluruh-kode-aplikasi)

---

---

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

---

# Bagian 2 — Aktivitas, fragment, adapter & alur interaksi (UI)

# LAPORAN ANALISIS KODE — MineTuner+ 2.2.3 (Referensi, hasil dekompilasi JADX)

Bahasa: Indonesia. Sumber: 22 file yang diminta di `.ref_extract/jadx_out/sources/`, ditambah kelas pendukung yang dipakai langsung (`d1.b` dari `jadx_c12`, `r2.a` dan `net.pubnative…a` dari `jadx_c6`, enum `t.a`/`t.c`, adapter `y.*`, fragment `f0.*`, resources `apktool_out`). Semua literal dikutip **verbatim** dari kode.

---

## 0. Peta sumber dan konvensi dekompilasi

| File | Peran ringkas |
|---|---|
| `com/hextetrahedral/minetuner/MainActivity.java` | Home: daftar world, FAB, import/export/hapus, menu Help/About |
| `…/DrawerActivity.java` | Editor world: NavigationDrawer 13 item + fragment NBT |
| `…/app/CreateWorldActivity.java` | Wizard flat world (biome + layer + LevelDB) |
| `…/app/EntitiesNBTActivity.java` | Daftar entitas per chunk |
| `…/app/HelpActivity.java` | Tutorial export/import texture pack |
| `a0/a.java`, `a0/c.java` | Klik FAB + animasi expand/collapse |
| `b0/a.java`, `b0/b.java`, `b0/c.java` | Debounce 300 ms + pembungkus Runnable + validator rentang angka |
| `c0/a.java`, `c0/b.java` | Antarmuka callback adapter + penanda fragment |
| `w/a.java` | `OnBackPressedCallback` untuk Drawer & Entities |
| `i0/a.java`, `i0/b.java` | `Function`/`Consumer` untuk hapus rekursif |
| `e0/a.java`, `e0/b.java` | Callback interstitial Appodeal + pewarnaan peta |
| `q/a.java` | `SimpleProgressTask` penangkap error ke bundle `"error"` |
| `r/a.java` | Exception marker kegagalan LevelDB |
| `u/a.java` | Vec3 (`x`,`y`,`z`) |
| `s/a.java`, `s/b.java`, `s/c.java` | Pembaca chunk/subchunk LevelDB + render peta + dekode palette |

Catatan JADX: angka desimal seperti `2131363019` adalah ID resource (`0x7f0a04cb`). Nama simbolik dipulihkan dari `res/values/public.xml`.

---

## 1. `MainActivity` — Layar utama

### 1.1 Peran, field, konstruktor
- **Peran**: `AppCompatActivity` implementing `SimpleDialog.OnDialogResultListener` dan `c0.a` (callback adapter). Mengelola daftar world, FAB stack, seluruh dialog utama, dan init iklan.
- **Field**:
  - `public static final int n = 0` — sentinel synthetic JADX.
  - `public ArrayList k` — model daftar world (`ArrayList<y.e>`).
  - `public g l` — adapter `y.g` (world list).
  - `public c m` — helper FAB `a0.c`.
- **Konstruktor**: tidak ada eksplisit; state dibuat di `onCreate`.

### 1.2 `a(ViewHolder, int)` — binding baris world (baris 60–132)
Peran: mengisi teks, ukuran, label, ikon, dan memasang `OnClickListener`.

```java
final f fVar = (f) viewHolder;
final j jVar = ((e) this.k.get(i)).a;
final String strD = jVar.d();            // levelname.txt / "Unnamed World"
File file = jVar.c;
String strG = jVar.g();                  // nama folder internal
...
Files.walkFileTree(file.toPath(), new i(atomicLong));
String fileSize = Formatter.formatFileSize(this, atomicLong.get());
textView3.setText(fileSize);
```

Logika lengkap:
1. `jVar.d()` → nama world dari `levelname.txt`; fallback `"Unnamed World"` (`t/j.java:88`).
2. Ukuran dihitung dengan `Files.walkFileTree` + `t.i` yang menjumlahkan `basicFileAttributes.size()` ke `AtomicLong`;IOException diabaikan.
3. Deteksi world eksternal: `file.getCanonicalPath().startsWith(new File(Environment.getExternalStorageDirectory(), "Android/data/com.mojang.minecraftpe/files/games/com.mojang/minecraftWorlds").getCanonicalPath())`. Jika ya → `textView2.setText("## [EXTERNAL WORLD] ##")`; jika tidak → nama internal `strG`.
4. Ikon: `jVar.i()` mengecek `world_icon.jpeg`; `jVar.h()` decode bitmap; gagal/`IOException` → `setImageResource(2131231699)`.
5. Klik baris dibungkus `b0.b.a()` (debounce). Bundle: `"itemPos"` = `getBindingAdapterPosition()`, `"worldPath"` = path absolut. Dialog: `SimpleListDialog.build().title("Options for \"" + strD + "\"").choiceMode(11).layout(LAYOUT_ACTION).extra(bundle)`, tag `"select_world_options_dialog"`.
6. Jika eksternal: `.msg("Note: This world is located outside the app's managed directory. Please create a backup if you have not already.")` + item `{"Edit","Delete"}`. Jika internal: item `{"Edit","Open in Minecraft","Delete"}`.

### 1.3 `l()` — scan & refresh daftar (135–167)
```java
ArrayList arrayList = gVar.j;
int size = arrayList.size();
arrayList.clear();
gVar.notifyItemRangeRemoved(0, size);
...
if (Build.VERSION.SDK_INT <= 28 && d1.b.k(this)) { ... minecraftWorlds ... }
File file3 = new File(getExternalFilesDir(null), "worlds");
...
for (File file4 : fileArrListFiles) arrayList2.add(new j(file4));
```
- Sumber 1 (hanya SDK ≤ 28 **dan** izin granted): `Android/data/com.mojang.minecraftpe/files/games/com.mojang/minecraftWorlds`, difilter `k.d` mode 2.
- Sumber 2 (selalu): `<externalFilesDir>/worlds`, difilter `k.d` mode 2.
- Tiap `File` dibungkus `new j(file)` (world handle) lalu `new y.e(j)`; `notifyItemInserted` dipanggil per item.

### 1.4 `onCreate(Bundle)` (254–352)
```java
setContentView(2131558434); // activity_main
setSupportActionBar(findViewById(2131363360)); // toolbar
recyclerView.setLayoutManager(new LinearLayoutManager(this));
recyclerView.setAdapter(this.l);
c cVar = new c(findViewById(2131362726), new ArrayList(Arrays.asList(
        findViewById(2131361896), findViewById(2131362147))));
this.m = cVar;
cVar.a.setOnClickListener(new a0.a(cVar, 0));
```
- `mainFab` = `2131362726`; child = `addWorldFabHolder` (`2131361896`) dan `createWorldFabHolder` (`2131362147`).
- Posisi awal child: `setVisibility(8)` (GONE), `alpha=0`, `translationY=100`.
- `addWorldFab` (`2131361895`) → `w.c` mode 0 → dialog import.
- `createWorldFab` (`2131362146`) → `w.c` mode 1 (default) → `startActivity(CreateWorldActivity)`.
- Keduanya memanggil `m.a()` (collapse) lebih dulu dan dijaga `b0.b.a()`.
- Warning: `new StatFs(Environment.getDataDirectory().getPath()).getAvailableBytes() < 10485760` → dialog `"Low Storage Warning"`.
- Scan: SDK > 28 → `l()` langsung; selain itu bila `!d1.b.k(this)` → `requestPermissions({READ_EXTERNAL_STORAGE, WRITE_EXTERNAL_STORAGE}, 100)`.
- Iklan: `if (d1.b.a) return; Appodeal.initialize(this, "79e2afef82af6ff44ec14c4cd547374550cfb9f45c77e338", 3, new x.a());`

### 1.5 `onActivityResult(int,int,Intent)` (170–251)
```java
if (i2 != -1 || intent == null || intent.getData() == null) return;
int[] iArr = new int[4];
System.arraycopy(s.b.a, 0, iArr, 0, 4);
...
if (net.pubnative...a.b(i3) == i || net.pubnative...a.c(i3) == i) break;
```
- Mencocokkan request code dengan pasangan `b(type)` (1001–1004) / `c(type)` (2001–2004).
- Tidak cocok → Toast `"No file type selected."`.
- `strA = a(type)` → `"mcpack" | "mcworld" | "mctemplate" | "mcaddon"`.
- Nama file: untuk skema `content`, query `_display_name` via `ContentResolver`; fallback `getLastPathSegment()`.
- Validasi: `str.toLowerCase(Locale.ENGLISH).endsWith("." + strA)`; gagal → Toast `"Please select a valid ." + strA + " file"`.
- Valid → progress dialog tag `"import_file_progress_dialog"` (`"Importing world..."`, `"Please wait while the world is being imported. Do not exit the app."`):
```java
File fileL = d1.b.l(context, uri, i5);   // → import.<ext>
int iC = net.pubnative...a.c(i5);
if (i == iC) d1.b.r(new androidx.activity.f(context, fileL, i5, 18)); // buka dgn app eksternal
else if (i == net.pubnative...a.b(i5)) d1.b.n(context, fileL, str2);  // ekstrak zip → worlds/<nama>
```

### 1.6 `onResult(String,int,Bundle)` (386–599) — pusat alur dialog
Struktur bersarang: `about_dialog` → `import_world_dialog` → `select_world_options_dialog` → `delete_world_dialog` → progress dialogs.

**`select_world_options_dialog`** mengambil `"SimpleListDialog.selectedSingleLabel"`, `"worldPath"`, `"itemPos"`, lalu:

- **`"Edit"`**
```java
if (!jVar.j()) {                       // j() = cek enkripsi LevelDB
    Bundle bundle2 = new Bundle();
    bundle2.putString("worldName", jVar.d());
    bundle2.putString("internalName", jVar.g());
    d1.b.q(this, "edit_world_progress_dialog", "Loading world...",
        "Please wait while the world is being loaded.", new w.b(), bundle2);
} else {
    SimpleDialog.build().title("Encrypted World").msg(
      "This world is encrypted and cannot be edited. Any worlds downloaded from Minecraft Marketplace are usually encrypted by default.").show(this);
}
```
  - Runnable `w.b` (switch dengan `i2=1`, jadi **default branch**): `jVar.k()` (buka LevelDB) lalu `x.b.a.set(jVar)` (simpan world aktif global).
  - Selesai tanpa error → `Intent(DrawerActivity)` dengan extra `"worldName"`, `"internalName"`.
  - Error → Toast `"Failed to edit world."` + `System.out.println("ERROR: " + string3)`.

- **`"Open in Minecraft"`**
```java
File file = new File(getExternalFilesDir(null), "export.mcworld");
Bundle bundle3 = new Bundle();
bundle3.putString("destZipPath", file.getAbsolutePath());
d1.b.q(this, "export_world_progress_dialog", "Compiling world...",
   "Please wait while the world is being compiled. Do not exit the app.",
   new r2.a(jVar, file, 9), bundle3);
```
  - `r2.a(...,9)` meng-zip isi folder world memakai zip4j (`addFolder`/`addFile`).
  - Setelah selesai: if `now - d1.b.b >= 10000 && Appodeal.isLoaded(3)` → `Appodeal.show(this, 3, "video_interstitial")`; lalu `d1.b.r(new androidx.activity.f(this, new File(string9), 2, 18))`.
  - Error/`destZipPath == null` → Toast `"Failed to compile world."`.

- **`"Delete"`** → dialog konfirmasi:
```java
SimpleDialog.build().title("Delete World")
 .msg("Are you sure you want to delete this world? This action cannot be undone.")
 .neg("CANCEL").pos("DELETE").extra(bundle4).show(this, "delete_world_dialog");
```
  Bundle `"itemPos"`, `"worldPath"`.

**`delete_world_dialog`**: `i == -1` → progress `"delete_world_progress_dialog"` / `"Deleting world..."` / `"Please wait while the world is being deleted."`; Runnable `w.b` dengan `i3=0` → `d1.b.h(file2)` (hapus rekursif). Selesai: `arrayList.remove(i5); notifyItemRemoved(i5); d1.b.t(this)`. Error → Toast `"Failed to delete world."`.

**`edit_world_progress_dialog`**, **`import_file_progress_dialog`**, **`export_world_progress_dialog`**, **`delete_world_progress_dialog`** semuanya membaca `"error"` dari bundle (diisi `q/a.java` dari `Exception.getMessage()`).

**`import_world_dialog`**:
- `i == -2` (neg/`MCWORLD`) → `d1.b.m(this, 2)`.
- `i == -1` (pos/`MCTEMPLATE`) → `d1.b.m(this, 3)`.
- `i == 0` (neut/`CANCEL`) → tidak melakukan apa-apa.

**`about_dialog`**: `i == -2` (neg/`DISCORD`) → `startActivity(new Intent("android.intent.action.VIEW", Uri.parse("https://www.discord.gg/Mr2za3VkUQ")))`.

### 1.7 Menu, permission, lifecycle
- `onCreateOptionsMenu` inflate `2131689476` (`main_activity_toolbar_menu.xml`: `action_help`, `action_about`).
- `onOptionsItemSelected`: `2131361865` (`action_help`) → `HelpActivity`; `2131361854` (`action_about`) → dialog `"About"` dengan `msgHtml(... .<br><br> ...)` dan `.neg("DISCORD")` tag `"about_dialog"`.
- `onRequestPermissionsResult(100)` → `l()`.
- `onResume()` → `l()` (refresh tiap kali kembali).

---

## 2. `DrawerActivity` — Editor NBT

### 2.1 Field & konstruktor
- `public int k = 2131363019` — ID menu terakhir (`nav_map`), dipakai sebagai `lastCheckedItemId`.
- `public DrawerLayout l` (`2131362214`), `public NavigationView m` (`2131363026`).
- Tanpa konstruktor eksplisit.

### 2.2 `l(int)` — mapping ID menu → fragment (29–66)
```java
if (i == 2131363019) return new e();                    // nav_map
if (i == 2131363027) return new h();                    // nav_world_nbt
if (i == 2131363024) return a.f("~local_player");       // nav_singleplayer_nbt
if (i == 2131363020) return new b();                    // nav_multiplayer_nbt
if (i == 2131363022) return a.f("Overworld");           // nav_overworld_nbt
...
if (i == 2131363015) { /* dialog enter_db_key_dialog */ }
return null;
```
Tabel lengkap (ID → nama resource → fragment/key):

| ID desimal | ID resource | Nama menu | Fragment / key |
|---|---|---|---|
| 2131363019 | 0x7f0a04cb | `nav_map` | `d0.e` (peta) — default |
| 2131363027 | 0x7f0a04d3 | `nav_world_nbt` | `f0.h` (level.dat) |
| 2131363024 | 0x7f0a04d0 | `nav_singleplayer_nbt` | `f0.a.f("~local_player")` |
| 2131363020 | 0x7f0a04cc | `nav_multiplayer_nbt` | `f0.b` (daftar player) |
| 2131363022 | 0x7f0a04ce | `nav_overworld_nbt` | `a.f("Overworld")` |
| 2131363025 | 0x7f0a04d1 | `nav_the_end_nbt` | `a.f("TheEnd")` |
| 2131363021 | 0x7f0a04cd | `nav_nether_nbt` | `a.f("Nether")` |
| 2131363013 | 0x7f0a04c5 | `nav_biomedata_nbt` | `a.f("BiomeData")` |
| 2131363023 | 0x7f0a04cf | `nav_portals_nbt` | `a.f("portals")` |
| 2131363012 | 0x7f0a04c4 | `nav_autonomous_entities_nbt` | `a.f("AutonomousEntities")` |
| 2131363015 | 0x7f0a04c7 | `nav_custom_nbt` | dialog input key |
| 2131363014 | 0x7f0a04c6 | `nav_change_world` | dialog `"Close World"` |

`a.f(String)` (`f0/a.java:16`) membuat fragment dengan argument `"db_key"` = `str.getBytes(UTF_8)`.

### 2.3 Dialog `m(int, boolean)` dan `n()`
```java
Bundle bundle = new Bundle();
bundle.putBoolean("isPlayerNBTEditor", z2);
bundle.putInt("newFragmentId", i);
SimpleDialog.build().title("Close NBT Editor")
 .msg("Are you sure you want to close this NBT editor? Any unsaved changes will be lost.")
 .neg("NO").pos("YES").extra(bundle).show(this, "close_nbt_dialog");
```
`n()` → `SimpleDialog.build().title("Close World").msg("Are you sure you want to close this world?").neg("NO").pos("YES").show(this, "close_world_dialog")`.

### 2.4 `o(int, Fragment, String)` (79–84)
`getSupportFragmentManager().beginTransaction().replace(2131362319, fragment, str).commit(); this.k = i;` — tag fragment = `String.valueOf(itemId)` atau key NBT bebas.

### 2.5 `onCreate(Bundle)` (87–133)
- Guard: `if (x.b.a() == null)` → `Intent(MainActivity)` + flags `335544320` (`NEW_TASK|CLEAR_TASK`), `finish()`.
- Extra: `"worldName"`, `"internalName"` → toolbar title + header `nav_header_title`/`nav_header_subtitle`; `nav_header_icon` dari `world_icon.jpeg`.
- `ActionBarDrawerToggle(this, l, toolbar, 2131952114, 2131952113)` + `syncState()`.
- Back dispatcher: `new w.a(this, 0)`.
- `bundle != null` → restore `lastCheckedItemId`; else default `new e()` dengan tag `"2131363019"` + `d1.b.t(this)` (interstitial statis).

### 2.6 `onNavigationItemSelected` (135–160)
- `b0.b.a()` guard; item sama dengan `k` → tutup drawer; `nav_change_world` → `n()`, kembalikan checked lama, tutup drawer.
- Jika `k` bukaneditor (yaitu `nav_map` atau `nav_world_nbt` atau `nav_multiplayer_nbt`?) — syarat: `k` **tidak termasuk** 10 ID editor → langsung `o(itemId, l(itemId), String.valueOf(itemId))`.
- Jika sedang di editor → `m(itemId,false)` + reset checked + tutup drawer.

### 2.7 `onResult` (164–202) & `onSaveInstanceState`
- `"enter_db_key_dialog"`: `"SimpleInputDialog.text"`, `"newFragmentId"`. `i == -2`/`i == 0`/kosong → reset checked; `i == -1` + teks tidak kosong → `o(i2, a.f(string), string)`.
- `"close_world_dialog"`: `i == -1` → `x.b.a().a()` (tutup LevelDB) + `finish()`.
- `"close_nbt_dialog"`: `"isPlayerNBTEditor"`, `"newFragmentId"`; `i == -1` → bila `z2` maka `popBackStack()`, else pindah fragment baru.
- `onSaveInstanceState` → `bundle.putInt("lastCheckedItemId", this.k)`.

---

## 3. `CreateWorldActivity` — Pembuat flat world

### 3.1 Field
`TextView k` (nama biome), `l` (id biome), `t.a m` (enum biome terpilih), `b n` (adapter layer `y.b`), `ArrayList o` (`ArrayList<t.h>` layer).

### 3.2 `a(ViewHolder,int)` (44–59) — binding layer
```java
aVar.m.setText(hVar.a.b);                       // nama tampilan blok
aVar.n.setText(cVar.c);                         // resource name blok
aVar.o.setText(String.format("x%d", hVar.b));   // ketebalan
cardView.setCardBackgroundColor(ColorUtils.blendARGB(-1, cVar.d, 0.3f));
itemView.setOnClickListener(new s9(this, 2, aVar, hVar));
aVar.p.setOnClickListener(new r(this, aVar, 11));
```
Warna kartu = putih dicampur warna blok (`t.c.d`) 30%.

### 3.3 `l(t.a)` (61–65)
`this.m = aVar; k.setText(aVar.b); l.setText(String.format("%s (%d)", aVar.c, aVar.d));` → contoh `"Plains (1)"` / `"minecraft:plains (1)"`.

### 3.4 `onCreate` (68–173)
- Toolbar + tombol up. `selected_biome_card` → `new e().pos("CANCEL").show(..., "pick_biome_dialog")`; `add_layer_button` → `new j().pos("CANCEL").show(..., "pick_block_dialog")`.
- Default biome `t.a.PLAINS`; state restore `"selectedBiome"`, `"layerBlocks"`, `"layerThicknesses"`.
- Default layer (jika `bundle == null`): `c.Qb` = **Grass Block**, `c.pb` = **Dirt**, `c.o7` = **Bedrock** (masing-masing 1).
- RecyclerView `layers_recycler`, `setItemAnimator(null)`, `ItemTouchHelper(new x.d(this))` → drag reorder.
- Enum `t.a` berisi ~87 biome dengan field `b` (nama user), `c` (`minecraft:...`), `d` (numeric id); `t.c` berisi 1.181 entri blok dengan `b` (nama tampilan), `c` (identifier `minecraft:*`), `d` (warna ARGB dari `Color.rgb(R,G,B)`). Tidak ada field `e` — koreksi ini diverifikasi langsung dari konstruktor `t/c.java:1199-1203` dan smali `Color.rgb(p2,p3,p4)`.

### 3.5 `onOptionsItemSelected` — validasi & submit (181–207)
```java
if (this.m == null) { Toast "Please select a biome."; return true; }
if (this.o.isEmpty()) { Toast "Please select at least one layer, air also works."; return true; }
if (this.o.stream().mapToInt(new g()).sum() > 384) {
    Toast.makeText(this, String.format("Total layer thickness exceeds the limit (%d).",
        Integer.valueOf(RendererCapabilities.MODE_SUPPORT_MASK)), 1).show(); return true;
}
d1.b.q(this, "create_flat_world_progress_dialog", "Compiling custom flat world...",
  "Please wait while the world is being created. Do not exit the app.",
  new androidx.media3.exoplayer.audio.g(14, this, ((CheckBox) findViewById(2131363129)).isChecked()), null);
```
`MODE_SUPPORT_MASK = 384` (batas tinggi world). Checkbox `2131363129` = `prevent_slimes_checkbox`.

### 3.6 `onResult` (210–246)
- `"pick_biome_dialog"` → `"BiomeSelectionDialog.SELECTED_BIOME"` → `l(t.a.values()[i2])`.
- `"pick_block_dialog"` → `"BlockSelectionDialog.SELECTED_BLOCK"` → `o.add(new h(c.values()[i3],1)); n.notifyItemInserted(...)`.
- `"set_layer_thickness_dialog"` → `"itemPos"`, `"SimpleInputDialog.text"`; `i == -1` & valid → `((h)o.get(i4)).b = Integer.parseInt(string2); notifyItemChanged(i4)`.
- `"create_flat_world_progress_dialog"` → error → `Toast(error)`; sukses → `Toast "World created successfully."` (duration 0) + `finish()`.

### 3.7 `onSaveInstanceState` & `validate`
- Simpan `"selectedBiome"` (ordinal), `"layerBlocks"` (integer ArrayList ordinal), `"layerThicknesses"`.
- `validate(String tag, String value, Bundle)` hanya untuk `"set_layer_thickness_dialog"`: harus nonempty, `Integer.parseInt > 0`, else → `"Please enter a valid integer value greater than 0."`.

### 3.8 Pembuatan world sebenarnya (`d1.b.g`) — penting untuk alur
```java
ArrayList arrayList2 = new ArrayList(arrayList);
if (z && (iSum = total - 65) < 39) arrayList2.add(new h(t.c.Mk, 39 - iSum));
if (total == 1) arrayList2.add(new h(t.c.Mk, 1));
Collections.reverse(arrayList2);
File file2 = new File(file, "MineTuner Custom Flat World");   // atau _1, _2, ...
... jVar.k(); jVar.a();
c(assets, "worldTemplate/level.dat", new File(file2,"level.dat"));
c(assets, "worldTemplate/levelname.txt", new File(file2,"levelname.txt"));
```
- `preventSlimes=true` (`z`) → tambah layer `"minecraft:air"` sampai total mencapai 65 (`39 - (total-65)`), juga menambah 1 air bila total tepat 1; lalu **urutan dibalik** (MC menulis layer dari atas ke bawah).
- `d1.b.a(t.a, ArrayList)` menghasilkan JSON:
```java
jSONObject.put("biome_id", aVar.d);
... put("block_name", hVar.a.c); put("count", hVar.b);
jSONObject.put("encoding_version", 6);
jSONObject.put("structure_options", JSONObject.NULL);
jSONObject.put("world_version", "version.post_1_18");
```
- JSON ditulis sebagai string tag `"FlatWorldLayers"` di `level.dat`; file template disalin dari assets `worldTemplate/`.
- Error spesifik: `"Could not create worlds directory."`, `"Could not create world folder."`, `"Failed to create world database."`, `"Failed to copy template files."`, `"Failed to load world level data."`, `"Failed to build flat world data."`, `"Failed to save world level data."`. Semua kegagalan memanggil `h(file2)` (hapus rekursif).

---

## 4. `EntitiesNBTActivity` — Entitas per chunk

### 4.1 Field
`RecyclerView k`, `FrameLayout l` (`fragment_container`), adapter `d m`, `ArrayList n` (byte[] key entitas), `s.a o` (chunk accessor).

### 4.2 `a(ViewHolder,int)` (41–69)
```java
byte[] bArr = (byte[]) this.n.get(i);
Optional optionalB = this.o.b(bArr);
Optional optionalM = ((l) optionalB.get()).d().m("identifier");
String str2 = ((l) optionalM.get()).g().c;
cVar.n.setText(str2);
f fVar = (f) f.d.get(str2);
File file = new File(getCacheDir(), "entityIcons/".concat(str));
itemView.setOnClickListener(new r(this, bArr, 12));
```
- Ikon: dari map statis `t.f.d` → field `c` (nama file ikon), file di `cacheDir/entityIcons/<nama>`; jika tidak ada → fallback drawable `2131231699`.
- Exception → `System.err.println("ERROR: " + e.getMessage())`.
- Klik → taip ke fragment editor NBT entitas dengan key byte[] (synthetic `r(...,12)`).

### 4.3 `l()` & `m()`
- `l()` → cek fragment di `2131362319` bertag `"entity_nbt_editor"`.
- `m()` → dialog `"Close NBT Editor"` / pesan identik dengan Drawer + `"NO"`/`"YES"`, tag `"close_nbt_dialog"`.

### 4.4 `onCreate` (84–126)
```java
if (b.a() == null) { /* redirect MainActivity, flags 335544320, finish */ }
int intExtra = getIntent().getIntExtra("chunkX", Integer.MIN_VALUE);
int intExtra2 = getIntent().getIntExtra("chunkZ", Integer.MIN_VALUE);
toolbar.setTitle(String.format("Chunk Entities at (%d, %d)", intExtra, intExtra2));
getOnBackPressedDispatcher().addCallback(this, new w.a(this, 1));
s.a aVarB = b.a().b(intExtra, intExtra2, 1);
ArrayList arrayListC = aVarB.c();
arrayList.addAll(arrayListC);
this.m.notifyItemRangeInserted(size, arrayListC.size());
```
- Gagal mengambil key → Toast `"Could not get entity keys."`.
- Jika fragment editor masih aktif (setelah rotasi), daftar disembunyikan (`k.setVisibility(8)`, `l.setVisibility(0)`).

### 4.5 `onOptionsItemSelected` & `onResult`
- Up button (`16908332`) → `m()` bila editor aktif, else `finish()`.
- `"close_nbt_dialog"` + `i == -1` → tampilkan lagi RecyclerView, sembunyikan container, `popBackStack()`.

### 4.6 Asal intent
`d0.e.onResult` (peta) untuk `"View Entities"`:
```java
int iFloorDiv = Math.floorDiv(i3, 16);   // worldX / 16
int iFloorDiv2 = Math.floorDiv(i4, 16);  // worldZ / 16
Intent intent = new Intent(context, EntitiesNBTActivity.class);
intent.putExtra("chunkX", iFloorDiv);
intent.putExtra("chunkZ", iFloorDiv2);
startActivity(intent);
```
Bersumber dari dialog `"mapview_options_dialog"` dengan bundle `"worldX"`, `"worldZ"`.

---

## 5. `HelpActivity` — Tutorial export/import texture pack

- Field: `static int k = 0`.
- `onCreate`: layout `2131558432` (`activity_help`), toolbar dengan up, tombol `button_import` (`2131362082`) → `new a(this, 28)` (yaitu `a0.a` case 28).
- `a0.a` case 28:
```java
d1.b.t(appCompatActivity);
File file = new File(appCompatActivity.getExternalFilesDir(null), "texture_pack.mcpack");
Bundle bundle = new Bundle();
bundle.putString("texturePackPath", file.getAbsolutePath());
d1.b.q(appCompatActivity, "import_texture_pack_progress_dialog", "Opening Minecraft...",
   "Please wait while the texture pack is being imported into Minecraft.",
   new r2.a(appCompatActivity, file, 10), bundle);
```
- `r2.a(...,10)` menyalin asset `"texture_pack.mcpack"` ke file tersebut.
- `onResult`: tag `"import_texture_pack_progress_dialog"`; baca `"error"`, `"texturePackPath"`; error → Toast `"Failed to import texture pack."`; sukses → `d1.b.r(new androidx.activity.f(this, new File(string2), 1, 18))` (buka dengan aplikasi lain/Minecraft).
- Konten tutorial (dari `activity_help.xml`, verbatim): "Step 1: Click the button below to import the texture pack into Minecraft.", "Step 2: Activate the texture pack in Minecraft.", "Step 3: Open a world in Minecraft and go into Game settings.", "Step 4: Scroll down to the bottom and press 'Export Template'.", "Select a file location then wait for the world to export.", "Step 5: Go into the home page of this app and press the '+' icon to import the exported world."

---

## 6. Kelas pendukung yang diminta — detail per class

### 6.1 `a0.a` — `View.OnClickListener` sintetik 29-case
- Field `int b`, `Object c`; konstruktor `(Object,int)`.
- **case 0** (FAB): bila `c` (`a0.c`) sedang expanded → `cVar.a()` (collapse); else set `c=true`, animasi FAB `rotation(45°)` 300 ms dengan `OvershootInterpolator`, tiap child `setVisibility(0)` + animasi `translationY(0)`, `alpha(1)`.
- case 1–27, default = delegasi ke implementasi SDK iklan (tidak relevan dengan UI aplikasi).
- **case 28** = tombol Import texture pack di `HelpActivity` (lihat §5).

### 6.2 `a0.c` — FAB stack
```java
public final FloatingActionButton a;
public final ArrayList b;
public boolean c = false;
public final OvershootInterpolator d = new OvershootInterpolator();
public c(FloatingActionButton fab, ArrayList views) { ... }
```
`a()` (collapse): `rotation(0)`, tiap view `translationY(100)`, `alpha(0)`, 300 ms, `withEndAction` → `setVisibility(8)` (case 0). Expand dibuka di `a0.a` case 0.

### 6.3 `b0.a` — `Runnable` sintetik; case 0 = `b.b = true` (mengembalikan status debounce setelah 300 ms).

### 6.4 `b0.b` — **anti double-tap / "PairIP-like" guard**
```java
public static final Handler a = new Handler(Looper.getMainLooper());
public static boolean b = true;
public static boolean a() {
    if (!b) return false;
    b = false;
    a.postDelayed(new a(0), 300L);
    return true;
}
```
Dipakai di semua aksi klik/klik menu sebelum eksekusi. Bukan SDK PairIP; PairIP sungguhan ada di manifest (`com.pairip.licensecheck.LicenseActivity`, `LicenseContentProvider`, otoritas `com.hextetrahedral.minetuner.com.pairip.licensecheck.LicenseContentProvider`) dan smali `LicenseClient` yang bind ke `com.android.vending.licensing.ILicensingService`, tetapi tidak dipanggil dari 5 Activity yang dianalisis.

### 6.5 `b0.c` — validator rentang angka
Field `double a,b`; `boolean c=false`. `a(String)` → `Long.parseLong` (atau `Double` jika `c`) dan cek `>= a && <= b`; null/kosong/NumberFormatException → false. Dipakai `d1.b.b(...)` untuk pesan `"Out of bounds for ..."`.

### 6.6 `c0.a` — antarmuka `void a(RecyclerView.ViewHolder, int)` (callback binding). `c0.b` — marker kosong untuk fragment.

### 6.7 `w.a` — `OnBackPressedCallback` (field `b` mode, `c` Activity; `super(true)` = enabled)
- mode 0 (DrawerActivity): tutup drawer bila terbuka; else cek fragment:
  - `f0.g` dengan breadcrumb `g.e != null` → `gVar.c(dVar2)` (kembali ke parent breadcrumb);
  - tag `"player_nbt_editor"` → `drawerActivity.m(-1, true)` (konfirmasi dengan `isPlayerNBTEditor=true` → `popBackStack`);
  - else → `drawerActivity.n()` ("Close World").
- mode 1 (EntitiesNBTActivity): tanpa fragment → `finish()`; fragment dengan breadcrumb → `g.c(...)`; else → `m()` ("Close NBT Editor").

### 6.8 `i0.a` — `Function`: case 0 `Path → File`, cases 1–4 mapping enum `t.c`/`t.f` ke field. `i0.b` — `Consumer`: `((File)obj).delete()`; dipakai `d1.b.h()` untuk urutan terbalik.

### 6.9 `e0.a` — `InterstitialCallbacks` + warna peta
- `a(t.b, int,int,int)`: `c = t.c.Ql.get(b.a)` (blok under beacon), `fMax = clamp(0.5, 1 + (dy1+dy2)*0.05, 1.5)`, `Color.rgb` dari kanal warna blok.
- `b(s.a, Canvas, x,y,w,h, Paint)`: render 16×16 kolom, memanggil `j.b(y-1, cx, cz)` (blok atas) dan `x.b.a().b(y, cx-1, cz)` (tetangga) untuk shading.
- `onInterstitialClosed()` → `d1.b.b = System.currentTimeMillis()` (cooldown 10 detik). Callback lain kosong.
- `e0.b` — holder sintetik 9 field (koordinat, `s.a`×3, `Paint`, `Canvas`) untuk garbage-free rendering.

### 6.10 `q.a` — `SimpleProgressTask`
```java
protected Object doInBackground(Object[] p) {
    try { if (this.a != null) this.a.run(); return null; }
    catch (Exception e) { return e; }
}
protected void onPostExecute(Object obj) {
    Bundle extras = ((SimpleProgressTask)this).mDialog.getExtras();
    extras.putString("error", exc.getMessage());
    ((SimpleProgressTask)this).mDialog.extra(extras);
    super.onPostExecute(exc);
}
```
Inilah yang membuat semua `onResult` bisa membaca `"error"`.

### 6.11 `r.a` — `extends Exception`, tanpa isi: penanda "LevelDB gagal dibuka" (ditangkap di `d1.b.g`).

### 6.12 `u.a` — Vec3: field `int a,b,c`; konstruktor `(int,int,int)`; `toString()` → `"Vec3{x=<a>, y=<b>, z=<c>}"`.

### 6.13 `s.a` — accessor chunk (`int a=chunkX, b=chunkZ; j c=world; int d=dimension`)
- `a(int,int,int,byte,boolean)` → susun key DB: untuk dimension 1 → 9/10 byte `[x(4)][z(4)][dbKey][(subchunk)]`; untuk dimension >1 → 13/14 byte dengan `b.b(dim)` (mapping `s.b.a`).
- `b(byte[])` → `j.c(key)` → `d1.b.o(false, ByteArrayInputStream)` → `Optional<l>` NBT entitas.
- `c()` → key `"digp"` + chunkX + chunkZ (+dim) → potong 8-byte `"actorprefix"+key` untuk setiap entitas.
- `e(int x,int z,t.b out)` → baca heightmap, subchunk versi 1/8/9, palette, dan blok; mengembalikan Y tertinggi atau `Integer.MIN_VALUE`. Error literals: `"Unknown or corrupted subchunk version: "`, `"Unexpected number of storage areas: "`, `"Unexpected chunk index value: "`, `"Unexpected non-save persistence version found."`, `"Not enough block data."`, `"Unexpected end of stream when reading palette size."`, `"Unexpectedly large number of palette entries: "`, `"Error reading entity data"`, `"Error reading subchunk data"`.
- `d(e0.b)` → render 256 kolom ke Canvas.
- Field `ConcurrentHashMap e` cache subchunk, `AtomicReference f` cache heightmap, `ThreadLocal g` scratch `t.b`.

### 6.14 `s.b` — `a[] = {1..23}`; `a(int)` memetakan 1→43, 2→44, …, 23→118 (`byte`) — konversi ID dimensi ke kunci DB; `b(int)` → `i-1` (0 → throw null).

### 6.15 `s.c` — subchunk terdekode: `String[][] a` (palette per storage), `byte[][] b` (data), `int[] c` (bits per block), `d/e/f` (chunkX, subchunkY, chunkZ), `ReentrantReadWriteLock g`. `a(int index)` → `32/bits` blok per uint32; error `"Block data too short for expected number of blocks"`.

---

## 7. ALUR 1 — Startup lengkap

1. `MainActivity.onCreate` → `setContentView(activity_main)`, toolbar, `RecyclerView(LinearLayoutManager, y.g)`.
2. `a0.c` dibuat dengan `mainFab` + dua holder; child diset GONE/alpha 0/translateY 100; klik FAB → `a0.a` case 0 (expand).
3. Storage check `< 10 MiB` → dialog `"Low Storage Warning"` (tanpa tag, tombol default).
4. `Build.VERSION.SDK_INT > 28` → `l()` langsung. SDK ≤ 28: bila `d1.b.k()` false → `requestPermissions(READ+WRITE, 100)`; `onRequestPermissionsResult(100)` → `l()`.
5. `l()` membaca dua direktori (eksternal Minecraft hanya SDK≤28 + izin; internal `<files>/worlds`) → `j` + `y.e` → adapter.
6. `d1.b.a` false → `Appodeal.initialize(..., "79e2afef82af6ff44ec14c4cd547374550cfb9f45c77e338", 3, new x.a())`. Callback `x.a`: sukses → `d1.b.a = true` + `Appodeal.setInterstitialCallbacks(new e0.a())`; gagal → `System.err.println("Appodeal initialization error: " + err.getMessage())`.
7. PairIP: terdaftar di manifest/smali sebagai `LicenseActivity` + `LicenseContentProvider` + `LicenseClient` (bind `com.android.vending.licensing.ILicensingService`), **tidak** dipanggil langsung oleh Activity di atas.
8. `onResume()` selalu memanggil `l()` → daftar segar setelahActivity lain selesai.

---

## 8. ALUR 2 — FAB stack: animasi, New World, Import World

1. `a0.a` case 0 → `a0.c`: FAB rotate 45°, child VISIBLE + animate ke `translationY=0`, `alpha=1`, `OvershootInterpolator`, 300 ms.
2. Klik child → `m.a()` (collapse: rotate 0, child turun 100/alpha 0, lalu GONE) + `b0.b.a()` guard.
3. **CreateWorldFab** → `startActivity(CreateWorldActivity)`.
4. **AddWorldFab** → dialog:
   `.title("Import World") .msg("To import a world, please provide a .mcworld or .mctemplate file.") .neut("CANCEL") .neg("MCWORLD") .pos("MCTEMPLATE")` tag `"import_world_dialog"`.
5. `onResult`: `MCWORLD(-2)` → `d1.b.m(this,2)`; `MCTEMPLATE(-1)` → `d1.b.m(this,3)`.
6. `d1.b.m`:
```java
Intent intent = new Intent("android.intent.action.GET_CONTENT");
intent.setType("*/*");
intent.putExtra("android.intent.extra.MIME_TYPES", new String[]{
  "application/octet-stream","application/x-mcpack","application/x-mcworld",
  "application/x-mctemplate","application/x-mcaddon"});
intent.addCategory("android.intent.category.OPENABLE");
startActivityForResult(Intent.createChooser(intent, "Select a ." + a(type) + " file"),
                       b(type));
```
7. `onActivityResult`: validasi akhiran → `d1.b.l()` salin ke `import.<ext>` → progress `"import_file_progress_dialog"` → `d1.b.n()` ekstrak zip ke `worlds/<nama tanpa ekstensi>` (dengan suffix `_1`, `_2` bila bentrok) → `l()` + `d1.b.t(this)`; error → `"Failed to import file: " + msg`.

---

## 9. ALUR 3 — Tap baris world → Options → Edit / Open / Delete

1. `a(ViewHolder,i)` → dialog `"Options for \"<nama>\""`, tag `"select_world_options_dialog"`, bundle `itemPos` + `worldPath`.
2. World di luar direktori kelolaan → pesan `"Note: This world is located outside the app's managed directory. Please create a backup if you have not already."`, menu hanya Edit/Delete; world internal → Edit/Open in Minecraft/Delete.
3. **Edit**: `j.j()` memeriksa 8 byte pertama tiap file di `db/`; terenkripsi bila `Integer.reverseBytes(readInt()) == 0 && Integer.reverseBytes(readInt()) == -1680885252` → dialog `"Encrypted World"`. Jika tidak → progress `"edit_world_progress_dialog"` → `j.k()` (buka LevelDB) + `x.b.a.set(j)` → `DrawerActivity(worldName, internalName)`.
4. **Open in Minecraft**: zip ke `export.mcworld` via `r2.a(...,9)` → interstitial `"video_interstitial"` bila cooldown 10 s terpenuhi → buka file.
5. **Delete**: konfirmasi `"Delete World"` → progress `"delete_world_progress_dialog"` → `d1.b.h()` hapus rekursif → `adapter.remove + notifyItemRemoved` → `d1.b.t(this)`.

---

## 10. ALUR 4 — DrawerActivity lengkap

1. Guard world aktif: `x.b.a() == null` → back ke `MainActivity` (NEW_TASK|CLEAR_TASK).
2. Toolbar title = `"worldName"`; header = judul (`worldName`), subjudul (`internalName`), ikon dari `world_icon.jpeg`.
3. Menu 13 item seperti tabel §2.2. Default `nav_map` (`d0.e`), checked disimpan di `"lastCheckedItemId"`.
4. Pindah menu: bila item == `k` → tutup drawer; `nav_change_world` → `"Close World"`; bila sedang di editor → `"Close NBT Editor"` (konfirmasi `isPlayerNBTEditor=false`); else `replace(fragment_container, fragment, tag)`.
5. `nav_custom_nbt` → dialog `"Custom NBT Key"` / `"Enter the database key to view its NBT data:"` / hint `"e.g. ~local_player"`, tag `"enter_db_key_dialog"`, bundle `"newFragmentId"`. Hasil → `f0.a.f(text)` dengan argumen `"db_key"`.
6. Back: `w.a` case 0 → breadcrumb / `"player_nbt_editor"` / `"Close World"`.
7. Close World YES → `x.b.a().a()` + `finish()`.

---

## 11. ALUR 5 — CreateWorldActivity lengkap

1. Biome card → `z.e` (`"BiomeSelectionDialog."`, hint `"Search biomes..."`) → balik `"BiomeSelectionDialog.SELECTED_BIOME"`.
2. Add layer → `z.j` (`"BlockSelectionDialog."`, hint `"Search blocks..."`) → `"BlockSelectionDialog.SELECTED_BLOCK"`, tambah `h(blok,1)`.
3. Drag (`x.d`): `makeMovementFlags(3,0)` = UP|DOWN, swipe dinonaktifkan, `onMove` memindahkan elemen `o` dan `notifyItemMoved`.
4. Klik layer → dialog ketebalan `"set_layer_thickness_dialog"` (bundle `"itemPos"`, baca `"SimpleInputDialog.text"`); validasi `validate()` → `"Please enter a valid integer value greater than 0."`.
5. Tombol delete layer (`aVar.p`) → hapus dari `o` + notify (synthetic `r(...,11)`).
6. Save: validasi biome, layer, total ≤ 384 → progress `"create_flat_world_progress_dialog"`;Runnable `androidx.media3…g(14, activity, preventSlimes)` memanggil `d1.b.g(...)`.
7. `d1.b.g`: buat folder unik `"MineTuner Custom Flat World"`, `db/`, salin `level.dat` + `levelname.txt`, tulis `"FlatWorldLayers"` JSON, `j.m(level)` (header 8 byte little-endian), gagal → hapus folder + error.
8. Sukses → Toast `"World created successfully."` + `finish()`; error → Toast pesan exception.

---

## 12. ALUR 6 — EntitiesNBTActivity lengkap

1. Dipanggil dari peta (`d0.e`) lewat `"mapview_options_dialog"` → `"View Entities"` dengan `chunkX = floorDiv(worldX,16)`, `chunkZ = floorDiv(worldZ,16)`.
2. Judul: `"Chunk Entities at (%d, %d)"`.
3. `b.a().b(chunkX,chunkZ,1)` → `s.a.c()` → key `"actorprefix"` + koordinat chunk → `ArrayList<byte[]>` → adapter `d`.
4. Tiap entitas: NBT `identifier` → nama + ikon dari cache `entityIcons`; klik → fragment editor tag `"entity_nbt_editor"`.
5. Back/up → dialog `"Close NBT Editor"` bila editor aktif; YES → `popBackStack()` + list tampil lagi.
6. Gagal baca → Toast `"Could not get entity keys."`.

---

## 13. ALUR 7 — HelpActivity

Toolbar "Exporting Worlds" → 5 langkah → tombol `"Import"` → copy asset `texture_pack.mcpack` → progress `"Opening Minecraft..."` → buka via intent → tutorial lanjut: aktifkan pack, buka world → Game settings → **Export Template** → pilih lokasi → kembali ke app tekan `+` untuk import. Error → Toast `"Failed to import texture pack."`.

---

## 14. INVENTARIS STRING LITERAL (verbatim)

### MainActivity
`"## [EXTERNAL WORLD] ##"` · `"Options for \"" + strD + "\""` · `"Note: This world is located outside the app's managed directory. Please create a backup if you have not already."` · `"Edit"` · `"Open in Minecraft"` · `"Delete"` · `"No file type selected."` · `"Import World"` · `"To import a world, please provide a .mcworld or .mctemplate file."` · `"CANCEL"` · `"MCWORLD"` · `"MCTEMPLATE"` · `"Low Storage Warning"` · `"Your device is low on storage. This may cause issues with the app's usability. Please continue with caution."` · `"About"` · `"This app is designed for NBT editing, helping you manage and tweak your Minecraft worlds.<br><br>The app is supported by ads, but only a minimal amount to avoid disrupting your experience.<br><br>Tested with Minecraft 1.21.x.<br><br>Join our Discord to report any bug reports."` · `"DISCORD"` · `"Encrypted World"` · `"This world is encrypted and cannot be edited. Any worlds downloaded from Minecraft Marketplace are usually encrypted by default."` · `"Loading world..."` · `"Please wait while the world is being loaded."` · `"Compiling world..."` · `"Please wait while the world is being compiled. Do not exit the app."` · `"Delete World"` · `"Are you sure you want to delete this world? This action cannot be undone."` · `"DELETE"` · `"Failed to edit world."` · `"ERROR: "` · `"Failed to delete world."` · `"Deleting world..."` · `"Please wait while the world is being deleted."` · `"Failed to import file: "` · `"Failed to compile world."` · `"Importing world..."` · `"Please wait while the world is being imported. Do not exit the app."` · `"Please select a valid ." + strA + " file"` · `"https://www.discord.gg/Mr2za3VkUQ"` · `"video_interstitial"` · `"static_interstitial"`.

### DrawerActivity
`"Custom NBT Key"` · `"Enter the database key to view its NBT data:"` · `"CANCEL"` · `"OK"` · `"e.g. ~local_player"` · `"Close NBT Editor"` · `"Are you sure you want to close this NBT editor? Any unsaved changes will be lost."` · `"NO"` · `"YES"` · `"Close World"` · `"Are you sure you want to close this world?"`.

### CreateWorldActivity
`"Please select a biome."` · `"Please select at least one layer, air also works."` · `"Total layer thickness exceeds the limit (%d)."` · `"Compiling custom flat world..."` · `"Please wait while the world is being created. Do not exit the app."` · `"World created successfully."` · `"Please enter a valid integer value greater than 0."` · `"CANCEL"` · `"Search biomes..."` · `"Search blocks..."`.

### EntitiesNBTActivity
`"Chunk Entities at (%d, %d)"` · `"Could not get entity keys."` · `"Close NBT Editor"` + pesan sama · `"NO"`/`"YES"` · `"entity_nbt_editor"`.

### HelpActivity
`"Failed to import texture pack."` · `"Opening Minecraft..."` · `"Please wait while the texture pack is being imported into Minecraft."` · `"Import"` + 6 kalimat tutorial (§5).

### Helper & editor (langsung terpakai)
`"Out of bounds for byte."` · `"Out of bounds for short."` · `"Out of bounds for long."` · `"Out of bounds for float."` · `"Out of bounds for double."` · `"Out of bounds for string."` · `"Out of bounds for integer."` · `"Unknown tag to edit, report to developers."` · `"Could not create worlds directory."` · `"Could not create world folder."` · `"Failed to create world database."` · `"Failed to copy template files."` · `"Failed to load world level data."` · `"Failed to build flat world data."` · `"Failed to save world level data."` · `"MineTuner Custom Flat World"` · `"MineTuner Custom Flat World_"` · `"FlatWorldLayers"` · `"version.post_1_18"` · `"biome_id"` · `"block_name"` · `"count"` · `"encoding_version"` · `"structure_options"` · `"Unnamed World"` · `"No NBT data found."` · `"No world data found."` · `"No players found."` · `"NBT data saved successfully."` · `"Failed to save NBT data."` · `"Failed to create tag, tag name is out of bounds."` · `"Select a list element type for \"" + str + "\""` · `"Tags inside a CompoundTag must have a name"` · `"Failed to create tag"` · `"Only byte tags can be added to byte arrays"` · `"List element type mismatch, it should be "` · `"A tag with this name already exists in the compound"` · `"Only int tags can be added to int arrays"` · `"Failed to add tag: "` · `"Edit \"" + str + "\""` · `"new value for the tag"` · `"Remove \"" + str + "\""` · `"Are you sure you want to remove this tag?"` · `"REMOVE"` · `"Cannot remove this tag."` · `"Failed to remove tag: "` · `"An error occurred when editing value."` · `"No selection made."` · `"Selection is too large."` · `"Set the scale (blocks per pixel) for the picture."` · `"View Entities"` · `"Take Picture"` · `"Begin Selection"` · `"Could not zip file."` · `"Appodeal initialization error: "` · `"ERROR "` · `"error"`.

---

## 15. INVENTARIS EXTRA / BUNDLE KEY / TAG DIALOG / REQUEST CODE

**Intent extras**
- `MainActivity → DrawerActivity`: `"worldName"`, `"internalName"`.
- `d0.e → EntitiesNBTActivity`: `"chunkX"`, `"chunkZ"`.
- Redirect ke `MainActivity`: flags `335544320`.

**Bundle keys**
| Key | Dipakai di |
|---|---|
| `"itemPos"` | select-world options, delete, set_layer_thickness_dialog, editor |
| `"worldPath"` | select-world options, delete |
| `"worldName"`, `"internalName"` | edit progress → DrawerActivity |
| `"destZipPath"` | export progress |
| `"error"` | semua progress dialog (diisi `q.a`) |
| `"SimpleListDialog.selectedSingleLabel"` | select-world options, mapview options, editor |
| `"SimpleInputDialog.text"` | enter_db_key, set_layer_thickness, edit tag |
| `"newFragmentId"` | enter_db_key, close_nbt_dialog |
| `"isPlayerNBTEditor"` | close_nbt_dialog |
| `"lastCheckedItemId"` | DrawerActivity saved state |
| `"texturePackPath"` | import texture pack progress |
| `"BiomeSelectionDialog.SELECTED_BIOME"` | pick_biome_dialog |
| `"BlockSelectionDialog.SELECTED_BLOCK"` | pick_block_dialog |
| `"selectedBiome"`, `"layerBlocks"`, `"layerThicknesses"` | CreateWorld saved state |
| `"db_key"` | argumen `f0.a` |
| `"worldX"`, `"worldZ"` | mapview_options_dialog |
| `"breadcrumbPath"`, `"tagTypeToCheck"`, `"NEW_TAG_TYPE"`, `"NEW_TAG_NAME"` | editor NBT |

**Dialog tags (31)**
`about_dialog`, `import_world_dialog`, `select_world_options_dialog`, `delete_world_dialog`, `edit_world_progress_dialog`, `delete_world_progress_dialog`, `import_file_progress_dialog`, `export_world_progress_dialog`, `low storage (tanpa tag)`, `enter_db_key_dialog`, `close_nbt_dialog`, `close_world_dialog`, `pick_biome_dialog`, `pick_block_dialog`, `set_layer_thickness_dialog`, `create_flat_world_progress_dialog`, `import_texture_pack_progress_dialog`, `mapview_options_dialog`, `mapview_picture_dialog`, `save_world_progress_dialog`, `edit_nbt_tag_type_dialog`, `edit_nbt_tag_dialog`, `add_new_nbt_tag_dialog`, `remove_nbt_tag__dialog`, `entity_nbt_editor` (fragment tag), `player_nbt_editor` (fragment tag), dll.

**Request / konstanta**
- Permission: `100` (READ+WRITE_EXTERNAL_STORAGE).
- File type → ekstensi: 1=`mcpack`, 2=`mcworld`, 3=`mctemplate`, 4=`mcaddon`.
- File type → request picker `b()`: `1001,1002,1003,1004`; → `c()` (ekstrak): `2001,2002,2003,2004`.
- `d(type)`: -64 (mcpack), 0 (mcworld/mctemplate).
- Ad type `3` = INTERSTITIAL; placement `"video_interstitial"`, `"static_interstitial"`; cooldown `10000 ms`.
- `Intent.createChooser` request `18` untuk ACTION_VIEW hasil (dipakai synthetic `androidx.activity.f`).
- `choiceMode(11)` = single-choice; `GravityCompat.START = 8388611`; `16908332` = android.R.id.home.

---

## 16. PERILAKU ADAPTER onClick / longClick

| Adapter | onClick | longClick |
|---|---|---|
| `y.g` (world list) | Buka `"Options for …"` (lihat §9), debounce 300 ms | Tidak ada |
| `y.b` (layer list) | Item → `s9(...,2,...)` → dialog ketebalan | Tidak ada; reorder via **long-press drag** (`x.d.isLongPressDragEnabled() == true`) |
| `y.a.p` (hapus layer) | `r(...,11)` → hapus layer | — |
| `y.c` / `y.d` (entities) | `r(activity, byte[], 12)` → editor entitas | Tidak ada |
| `z.d` (biome list) | `w0.a` → balik `SELECTED_BIOME` | — |
| `z.i` (block list) | `w0.a` → balik `SELECTED_BLOCK` | — |
| `f0.b` (player list) | Buka player NBT editor | — |
| `h0.c` (tag list) | `f(adapter)` → edit/remove tag | — |
| `f0.g` breadcrumb `TextView` | `r(this, dVar, 6)` → navigate ke level | — |

---

## 17. CATATAN TEKNIS TAMBAHAN

1. **Enkripsi world** dideteksi dari header file LevelDB (`0, -1680885252` setelah `Integer.reverseBytes`), bukan dari nama folder.
2. **Konvensi kunci DB**: `d1.b.i(int)` little-endian 4 byte; `s.b.a(int)` memetakan ID dimensi ke byte kunci; chunk key `digp`, entitas `actorprefix`.
3. **Progress dialog** memakai pola `d1.b.q(activity, tag, title, msg, Runnable, Bundle)` → `com.applovin.impl.mediation.j(...,10)` → `q.a`; semua error masuk bundle `"error"` dan ditampilkan Toast.
4. **Iklan**: `Appodeal.initialize` hanya sekali (`d1.b.a`); `d1.b.t()` (interstitial statis) dipanggil setelah hapus/import/success drawer; `video_interstitial` setelah export.
5. **PairIP** hadir sebagai proteksi lisensi Play (Activity + ContentProvider + bind service), terpisah dari debounce `b0.b`.
6. **Resource fallback drawable** `2131231699` dipakai untuk ikon world/entitas default.

---

## 18. RINGKASAN ALOUR (urutan lengkap)

`onCreate(Main)` → permission/scan → `Low Storage Warning` → `Appodeal.init` → FAB expand → **Import** (`MCWORLD/MCTEMPLATE` → GET_CONTENT → validasi ekstensi → `import.<ext>` → ekstrak zip → refresh) atau **New World** (biome + layer + drag + thickness + validasi 384 → JSON `FlatWorldLayers` → `LevelDB` → sukses) → tap world → **Options** → Edit (enkripsi check → Loading world → `DrawerActivity`) / Open in Minecraft (zip `export.mcworld` → interstitial) / Delete (konfirmasi → hapus → refresh) → Drawer 13 menu → fragment NBT/peta → peta long-press → View Entities → `EntitiesNBTActivity(chunkX,chunkZ)` → editor entitas → back dialog → Help → import `texture_pack.mcpack` → Minecraft → Export Template → kembali ke `+`.

Demikian laporan lengkap. Seluruh class yang diminta telah dibaca dan dijelaskan per field/konstruktor/method, beserta literal, key, tag, request code, dan perilaku adapter.

---

# Bagian 3 — Engine NBT: tipe tag, parser/serializer, validasi, editor

Berikut laporan teknis lengkap hasil analisis terhadap seluruh file yang diminta (plus beberapa dependensi pendukung yang diperlukan untuk memahami alur, seperti `b0.c`, `t.j`, dan `g0.a` dari smali).

---

# LAPORAN TEKNIS DETAIL: ENGINE NBT & EDITOR NBT — MineTuner+ 2.2.3

## 0. PETA SUMBER DAN RUANG LINGKUP

File yang dianalisis (semua sudah dibaca, tanpa terkecuali):

| Grup | Path | Peran |
|---|---|---|
| Enum tipe | `jadx_c6/sources/x2/a.java` | Definisi 12 tipe tag NBT + tabel lookup id→enum |
| Engine tag | `jadx_c6/sources/y2/a.java` … `y2/l.java` (12 file: a,b,c,d,e,f,g,h,i,j,k,l) | Kelas tag, parser, serializer, cast helper |
| Helper/validasi | `jadx_c12/sources/d1/a.java`, `d1/b.java` (517 baris) | Validasi nilai, pembangun pohon, loader header, world generator, utilitas |
| Editor fragment | `jadx_out/sources/f0/a.java`, `b.java`, `d.java`, `e.java`, `f.java`, `g.java` (673 baris), `h.java` | UI editor NBT, dialog, state restore |
| Adapter/state | `jadx_out/sources/h0/b.java`, `h0/c.java`, `h0/d.java` | ViewHolder, RecyclerView adapter, node state |
| Singleton/helper | `jadx_out/sources/x/a.java`, `x/b.java`, `x/d.java` | Context leveldb, callback ads, drag reorder |
| Adapter pendukung | `jadx_out/sources/y/a.java` … `y/g.java` (7 file) | Adapter daftar world/player/layer |

Dependensi tambahan yang ikut dibaca/diperiksa karena dipanggil langsung:

| File | Alasan |
|---|---|
| `jadx_out/sources/b0/c.java` | Validator rentang numerik yang dipakai `d1.b.b()` |
| `jadx_out/sources/t/j.java` (249 baris) | Wrapper LevelDB + baca/tulis `level.dat` (header 8 byte) |
| `apktool_out/smali_classes9/g0/a.smali` | Predicate pencarian nama tag (`y2.c.m`, duplikat nama) |
| `apktool_out/smali_classes9/c0/a.smali`, `c0/b.smali` | Antarmuka callback adapter |

Catatan penting tentang dekompilasi: `x2.a` dan `y2.*` berasal dari `classes6.dex` dengan nama konstanta deskriptif (`TAG_BYTE`, dll.), sedangkan `f0.*`/`h0.*` berasal dari `classes9.dex` yang merujuk enum yang sama dengan nama konstanta ter-obfuscasi (`x2.a.c`, `x2.a.d`, `x2.a.f`, `x2.a.j`, `x2.a.l`, `x2.a.m`, `x2.a.n`). Ini bukan enum berbeda — hanya perbedaan pemetaan nama oleh JADX antar-dex. Pemetaan fungsionalnya dijelaskan di §2.3.

---

## 1. FORMAT KAWH NBT (STRUKTUR BYTE)

### 1.1 Dua lapisan format yang dipakai aplikasi

Ada dua mode penyimpanan yang dibedakan oleh flag `boolean z` pada `d1.b.o(boolean, ByteArrayInputStream)`:

```
d1.b.o(z, stream)
   ├─ z == true  → level.dat (world): LEWAT 8 byte header dulu
   └─ z == false → nilai LevelDB / cache rootTag.tmp: langsung NBT mentah
```

Kode persisnya (`d1/b.java:469-481`):

```java
public static l o(boolean z, ByteArrayInputStream byteArrayInputStream) {
    if (z) {
        try {
            if (byteArrayInputStream.skip(8L) != 8) {
                throw new IllegalStateException("No header found.");
            }
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }
    return l.k(byteArrayInputStream, -1);
}
```

### 1.2 Header 8 byte Bedrock (level.dat)

Penulisnya ada di `t/j.java:235-247` (`t.j.m(l)`):

```java
byte[] bArrH = lVar.h();
byte[] bArrArray = ByteBuffer.allocate(bArrH.length + 8)
    .order(ByteOrder.LITTLE_ENDIAN)
    .putInt(8)                 // 08 00 00 00  = "header length" = 8
    .putInt(bArrH.length)      // 4 byte LE    = panjang payload root tag
    .put(bArrH)                // payload dimulai dgn 1 byte root type
    .array();
```

Jadi layout sebenarnya (meluruskan deskripsi singkat pada permintaan):

```
offset 0..3   : int32 LE = 8                     → 08 00 00 00
offset 4..7   : int32 LE = panjang payload NBT   → root type + name + data + ...
offset 8..    : payload NBT:
                  [0] 1 byte  root type   (biasanya 0x0A = TAG_COMPOUND)
                  [1] uint16 LE panjang nama root
                  [..] byte nama root (sering kosong)
                  [..] isi root
```

"Root type" bukan field header ke-3 yang terpisah, melainkan **byte pertama dari payload** yang dihasilkan `y2.l.h()`. `d1.b.o(true, …)` hanya `skip(8)` lalu menyerahkan sisanya ke parser `y2.l.k(stream, -1)`, yang membaca type byte itu sebagai langkah pertama.

### 1.3 Serialisasi satu tag — `y2.l.h()`

```java
public final byte[] h() {
    String str = this.b;
    if (str == null) {
        return l();                       // elemen list / array: TANPA tipe & nama
    }
    byte[] bytes = str.getBytes();
    byte[] bArrL = l();
    return ByteBuffer.allocate(bytes.length + bArrL.length + 3)
        .order(ByteOrder.LITTLE_ENDIAN)
        .put(j().b)                       // 1 byte type id
        .putShort((short) bytes.length)   // 2 byte panjang nama (signed short)
        .put(bytes)                       // nama
        .put(bArrL)                       // payload dari l()
        .array();
}
```

Konsekuensi:
- Tag **bernama** (anak Compound): `[type][nameLen:u16][name][payload]`.
- Tag **tanpa nama** (elemen List/ByteArray/IntArray): `[payload]` saja. Ini sebabnya `h()` mengembalikan `l()` langsung saat `b == null`.
- Root tag level.dat biasanya bernama `""` → tetap ada 3 byte overhead (type + `00 00`).
- Semua primitif numerik ditulis **LITTLE_ENDIAN** (khas Bedrock/Java NBT), kecuali Compound yang mengakhiri dengan `0x00` (TAG_End) dan List yang mendahului element-type byte.

### 1.4 Ringkasan payload per tipe (`l()`)

| Tipe | Isi `l()` | Ukuran |
|---|---|---|
| Byte | `{c}` | 1 |
| Short | `putShort LE` | 2 |
| Int | `putInt LE` | 4 |
| Long | `putLong LE` | 8 |
| Float | `putFloat LE` | 4 |
| Double | `putDouble LE` | 8 |
| String | `putShort((short)bytes.length)` + byte | 2 + n |
| ByteArray | `putInt(len)` + byte[] | 4 + n |
| IntArray | `putInt(count)` + tiap `putInt LE` | 4 + 4n |
| List | `put(typeId)` + `putInt(count)` + `child.h()` berturut | 5 + Σ |
| Compound | semua `child.h()` lalu `put((byte)0)` | Σ + 1 |

---

## 2. ENUM `x2.a` — TIPE TAG

### 2.1 Definisi lengkap (`x2/a.java`, 32 baris)

```java
public enum a {
    TAG_END((byte) 0),
    TAG_BYTE((byte) 1),
    TAG_SHORT((byte) 2),
    TAG_INT((byte) 3),
    TAG_LONG((byte) 4),
    TAG_FLOAT((byte) 5),
    TAG_DOUBLE((byte) 6),
    TAG_BYTE_ARRAY((byte) 7),
    TAG_STRING((byte) 8),
    TAG_LIST((byte) 9),
    TAG_COMPOUND((byte) 10),
    TAG_INT_ARRAY((byte) 11);

    public static final HashMap o = new HashMap();
    public final byte b;

    static {
        for (a aVar : values()) {
            o.put(Byte.valueOf(aVar.b), aVar);
        }
    }

    a(byte b) { this.b = b; }
}
```

### 2.2 Tabel enum (nama, ordinal, id, kelas y2, kegunaan)

| Ordinal (= id byte) | Nama enum (`classes6`) | `b` (id) | Kelas `y2.*` | Kegunaan khas |
|---:|---|---:|---|---|
| 0 | `TAG_END` | `0x00` | (tidak ada kelas; `null` saat parsing) | Terminator Compound; pengecualian daftar pilihan |
| 1 | `TAG_BYTE` | `0x01` | `y2.b` | Nilai byte, juga elemen ByteArray |
| 2 | `TAG_SHORT` | `0x02` | `y2.j` | Nilai short 16-bit |
| 3 | `TAG_INT` | `0x03` | `y2.g` | Nilai int, juga elemen IntArray |
| 4 | `TAG_LONG` | `0x04` | `y2.i` | Nilai long 64-bit |
| 5 | `TAG_FLOAT` | `0x05` | `y2.e` | Float 32-bit |
| 6 | `TAG_DOUBLE` | `0x06` | `y2.d` | Double 64-bit |
| 7 | `TAG_BYTE_ARRAY` | `0x07` | `y2.a` | Array byte (count int32) |
| 8 | `TAG_STRING` | `0x08` | `y2.k` | String UTF-ish (panjang u16) |
| 9 | `TAG_LIST` | `0x09` | `y2.h` | List homogen; simpan tipe elemen |
| 10 | `TAG_COMPOUND` | `0x0A` | `y2.c` | Map bernama; diakhiri `0x00` |
| 11 | `TAG_INT_ARRAY` | `0x0B` | `y2.f` | Array int32 (count int32) |

### 2.3 Field dan method — apa yang ADA dan apa yang TIDAK ADA

Yang ada:
- `public final byte b` — id kawat (0..11).
- `public static final HashMap o` — lookup `Byte → x2.a`, diisi di static initializer. Dipakai parser List: `x2.a aVar = (x2.a) x2.a.o.get(Byte.valueOf((byte) in.read()));`.
- `values()` dan `valueOf(String)` — method implisit enum. `valueOf` dipanggil editor pada `"edit_nbt_tag_type_dialog"` dan saat membuat tag dari spinner.

Yang **tidak ada** di file ini (meski disebut dalam permintaan):
- Tidak ada method `j()` di `x2.a`. `j()` ada di kelas abstrak `y2.l` dan mengembalikan `x2.a`.
- Tidak ada field `label`. Label tampilan berasal dari konstanta enum itu sendiri (`TAG_BYTE`, …) yang dipetakan ke string UI oleh fungsi synthetic `com.google.android.material.color.utilities.f(29)` (lihat `Arrays.stream(x2.a.values()).filter(...).map(new ...f(29))`).
- Tidak ada static `fromId(int)` eksplisit. "From id" dilakukan lewat `x2.a.o.get(Byte.valueOf(id))`, atau secara tidak langsung lewat `x2.a.values()[ordinal]` karena ordinal == id (dipakai `f0.g.validate()`).

### 2.4 Pemetaan konstanta ter-obfuscasi di `classes9` (penting untuk membaca `f0.g`)

Dari konteks pemakaian di `f0.g.java`, `f0.d.java`, `h0.c.java`:

| Referensi di `f0.*`/`h0.*` | Tipe sebenarnya | Bukti dari kode |
|---|---|---|
| `x2.a.c` | `TAG_END` (0) | `f0.d` memfilter `aVar != x2.a.c` → dimaksudkan "buang TAG_END" dari daftar pilihan |
| `x2.a.d` | `TAG_BYTE` (1) | `if (bVar.j() != x2.a.d) "Only byte tags can be added to byte arrays"` |
| `x2.a.f` | `TAG_INT` (3) | `if (bVar.j() != x2.a.f) "Only int tags can be added to int arrays"` |
| `x2.a.l` | `TAG_LIST` (9) | `boolean z2 = parent.j() == x2.a.l` → tidak butuh nama; `if (newType == aVar2 && aVar == null)` → minta tipe elemen list |
| `x2.a.m` | `TAG_COMPOUND` (10) | `if (parent.j() == x2.a.m && name kosong) "Tags inside a CompoundTag must have a name"` |
| `x2.a.j`, `x2.a.n` | `TAG_BYTE_ARRAY` (7) / `TAG_INT_ARRAY` (11) (urutan relatif tidak krusial) | Keduanya masuk klausa "anak array tak bernama" di baris 59 & 72 |

---

## 3. HIERARKI `y2.l` DAN 12 KELAS TAG

### 3.1 Kelas abstrak `y2.l` (`l.java`, 165 baris)

Field/konstruktor:
```java
public abstract class l {
    public final String b;            // nama tag (boleh null)
    public l(String str) { this.b = str; }
    ...
}
```

Method lengkap:

| Method | Tanda tangan | Fungsi |
|---|---|---|
| `k` | `static l k(ByteArrayInputStream, int)` | **Parser utama**. `int = -1` berarti "baca type byte dari stream"; `int >= 0` berarti "tipe sudah diketahui" (dipakai elemen List) |
| `c()` | `final a c()` | Cast aman ke `y2.a` (ByteArrayTag) via `i(a.class)` |
| `d()` | `final c d()` | Cast aman ke `y2.c` (CompoundTag) |
| `e()` | `final g e()` | Cast aman ke `y2.g` (IntTag) |
| `f()` | `final f f()` | Cast aman ke `y2.f` (IntArrayTag) |
| `g()` | `final k g()` | Cast aman ke `y2.k` (StringTag) |
| `h()` | `final byte[] h()` | Serialisasi lengkap `[type][nameLen][name][payload]` (atau `l()` jika tanpa nama) |
| `i(Class)` | `final void i(Class cls)` | Type-check: `if (cls.isAssignableFrom(getClass())) return;` else `IllegalArgumentException("Expected class " + cls.getSimpleName() + ", but is " + getClass().getSimpleName())` |
| `j()` | `abstract x2.a j()` | Mengembalikan tipe tag |
| `l()` | `abstract byte[] l()` | Payload saja (tanpa type & nama) |

Lima method `c/d/e/f/g` adalah "cast helper" yang diminta; semuanya memanggil `i(Class)` lalu cast. Ini yang menjelaskan pemakaian seperti `dVar3.e().c = Integer.parseInt(...)` (IntTag) dan `lVarL.d().m("FlatWorldLayers")` (CompoundTag).

### 3.2 Parser `y2.l.k(ByteArrayInputStream, int)` — detail per case

```
k(stream, type):
  ├─ available()==0 → throw IllegalStateException("Invalid NBT formatting.")
  ├─ name = null
  ├─ JIKA type == -1:
  │     type = stream.read()
  │     if type == 0 → return null        ← terminator TAG_End
  │     nameLen = int16 LE dari 2 byte
  │     name = new String(byte[nameLen])  ← charset default platform
  ├─ switch(type):
  │   1 → new y2.b(name, (byte) read())
  │   2 → 2 byte → getShort LE           → new y2.j
  │   3 → 4 byte → getInt LE             → new y2.g
  │   4 → 8 byte → getLong LE            → new y2.i
  │   5 → 4 byte → getFloat LE           → new y2.e
  │   6 → 8 byte → getDouble LE          → new y2.d
  │   7 → int32 count LE; baca count byte → new y2.a
  │   8 → int16 len LE; baca len byte → new String → new y2.k
  │   9 → typeElemen = x2.a.o.get(read()); count = int32 LE;
  │       loop count × k(stream, typeElemen.b) → new y2.h(name, typeElemen, list)
  │       (anak list dipanggil dgn tipe eksplisit ⇒ nama = null)
  │   10→ loop: child = k(stream, -1); until child == null → new y2.c
  │   11→ int32 count LE; loop count × (4 byte → getInt LE) → new y2.f
  │   default → throw IllegalStateException("Invalid NBT formatting.")
```

Cuplikan kunci case 9 dan 10:

```java
case 9:
    x2.a aVar = (x2.a) x2.a.o.get(Byte.valueOf((byte) byteArrayInputStream.read()));
    int i7 = ByteBuffer.wrap(new byte[]{(byte) read(), (byte) read(),
                                         (byte) read(), (byte) read()})
                .order(ByteOrder.LITTLE_ENDIAN).getInt();
    ArrayList arrayList = new ArrayList();
    while (i2 < i7) { arrayList.add(k(byteArrayInputStream, aVar.b)); i2++; }
    return new h(str, aVar, arrayList);
case 10:
    ArrayList arrayList2 = new ArrayList();
    do { lVarK = k(byteArrayInputStream, -1);
         if (lVarK != null) arrayList2.add(lVarK);
    } while (lVarK != null);
    return new c(str, arrayList2);
```

Catatan: pembacaan menggunakan `read()` per byte dan `ByteBuffer.wrap(...)` untuk menggabungkan 2/4/8 byte — bukan `DataInputStream`. Tidak ada batas eksplisit pada `count`/`len`, sehingga file berbahaya dapat memicu `NegativeArraySizeException`/`OutOfMemoryError`.

### 3.3 Detail 12 kelas `y2.*`

#### `y2.b` — ByteTag
```java
public byte c;
public b(String str, byte b) { super(str); this.c = b; }
public x2.a j() { return x2.a.TAG_BYTE; }
public byte[] l() { return new byte[]{this.c}; }
```
`equals`: `c ==` dan nama null-atau-sama. `hashCode`: `Objects.hash(Byte.valueOf(c))`.

#### `y2.j` — ShortTag
```java
public short c;
public j(String str, short s) { super(str); this.c = s; }
public x2.a j() { return x2.a.TAG_SHORT; }
public byte[] l() { return ByteBuffer.allocate(2).order(LITTLE_ENDIAN).putShort(this.c).array(); }
```

#### `y2.g` — IntTag
```java
public int c;
public g(String str, int i) { super(str); this.c = i; }
public x2.a j() { return x2.a.TAG_INT; }
public byte[] l() { return ByteBuffer.allocate(4).order(LITTLE_ENDIAN).putInt(this.c).array(); }
```

#### `y2.i` — LongTag
```java
public long c;
public i(String str, long j) { super(str); this.c = j; }
public x2.a j() { return x2.a.TAG_LONG; }
public byte[] l() { return ByteBuffer.allocate(8).order(LITTLE_ENDIAN).putLong(this.c).array(); }
```

#### `y2.e` — FloatTag
```java
public float c;
public e(String str, float f) { super(str); this.c = f; }
public x2.a j() { return x2.a.TAG_FLOAT; }
public byte[] l() { return ByteBuffer.allocate(4).order(LITTLE_ENDIAN).putFloat(this.c).array(); }
```

#### `y2.d` — DoubleTag
```java
public double c;
public d(String str, double d) { super(str); this.c = d; }
public x2.a j() { return x2.a.TAG_DOUBLE; }
public byte[] l() { return ByteBuffer.allocate(8).order(LITTLE_ENDIAN).putDouble(this.c).array(); }
```

#### `y2.k` — StringTag
```java
public String c;
public k(String str, String str2) { super(str); this.c = str2; }
public x2.a j() { return x2.a.TAG_STRING; }
public byte[] l() {
    byte[] bytes = this.c.getBytes();          // ⚠ charset default
    return ByteBuffer.allocate(bytes.length + 2).order(LITTLE_ENDIAN)
        .putShort((short) bytes.length).put(bytes).array();
}
```

#### `y2.a` — ByteArrayTag
```java
public byte[] c;
public a(String str, byte... bArr) { super(str); this.c = bArr; }   // varargs
public x2.a j() { return x2.a.TAG_BYTE_ARRAY; }
public byte[] l() {
    return ByteBuffer.allocate(this.c.length + 4).order(LITTLE_ENDIAN)
        .putInt(this.c.length).put(this.c).array();
}
```
`hashCode` memakai `Arrays.hashCode(c)` (bukan `Objects.hash`), `equals` memakai `Arrays.equals(c, other.c)`.

#### `y2.f` — IntArrayTag
```java
public int[] c;
public f(String str, int... iArr) { super(str); this.c = iArr; }   // varargs
public x2.a j() { return x2.a.TAG_INT_ARRAY; }
public byte[] l() {
    ByteBuffer b = ByteBuffer.allocate((this.c.length * 4) + 4).order(LITTLE_ENDIAN);
    b.putInt(this.c.length);
    for (int i : this.c) b.putInt(i);
    return b.array();
}
```

#### `y2.h` — ListTag
```java
public final x2.a c;       // tipe elemen (homogen)
public final ArrayList d;  // elemen bertipe y2.l, semua b == null
public h(String str, x2.a aVar, ArrayList arrayList) { super(str); this.c = aVar; this.d = arrayList; }
public x2.a j() { return x2.a.TAG_LIST; }
public byte[] l() {
    ByteBuffer b = ByteBuffer.allocate(5).order(LITTLE_ENDIAN).put(this.c.b);
    byte[] arr = b.putInt(this.d.size()).array();          // [type][count]
    for (Object o : this.d) {
        byte[] child = ((l) o).h();                        // tanpa nama
        arr = Arrays.copyOf(arr, arr.length + child.length);
        System.arraycopy(child, 0, arr, arr.length - child.length, child.length);
    }
    return arr;
}
```
`equals` membandingkan `c` (tipe elemen) **dan** `d` (isi) **dan** nama.

#### `y2.c` — CompoundTag
```java
public final ArrayList c;  // children y2.l, semua punya nama
public c(String str, ArrayList arrayList) { super(str); this.c = arrayList; }
public x2.a j() { return x2.a.TAG_COMPOUND; }
public byte[] l() {
    byte[] out = new byte[0];
    for (Object o : this.c) {
        byte[] child = ((l) o).h();
        out = Arrays.copyOf(out, out.length + child.length);
        System.arraycopy(child, 0, out, out.length - child.length, child.length);
    }
    return ByteBuffer.allocate(out.length + 1).put(out).put((byte) 0).array();  // TAG_End
}
public final Optional m(String str) {
    return this.c.stream().filter(new g0.a(str, 1)).findFirst();
}
```
`g0.a(str, 1)` (dari smali) melakukan `tag.b.equals(str)` — artinya pencarian **null-safe terhadap query**, tetapi akan NPE jika ada child dengan nama `null` di dalam Compound (kasus normal tidak terjadi karena editor melarangnya). Mode `0` (dipakai di `f0.g`) melakukan `str.equals(tag.b)`.

#### `y2.l` — base
Sudah dijelaskan di §3.1. Tidak ada kelas untuk `TAG_End`; parser mengembalikan `null` sebagai gantinya.

### 3.4 Tabel ringkas field/method yang diminta

| Kelas | Tipe `j()` | Field payload | `l()` = payload | `c()/d()/e()/f()/g()` relevan |
|---|---|---|---|---|
| `y2.a` | BYTE_ARRAY | `byte[] c` | int32 len + byte[] | `c()` |
| `y2.b` | BYTE | `byte c` | 1 byte | — |
| `y2.c` | COMPOUND | `ArrayList c` | children + `0x00` | `d()`, `m(nama)` |
| `y2.d` | DOUBLE | `double c` | 8 byte LE | — |
| `y2.e` | FLOAT | `float c` | 4 byte LE | — |
| `y2.f` | INT_ARRAY | `int[] c` | int32 count + int32[] | `f()` |
| `y2.g` | INT | `int c` | 4 byte LE | `e()` |
| `y2.h` | LIST | `x2.a c`, `ArrayList d` | type + count + children | `j()` type elemen |
| `y2.i` | LONG | `long c` | 8 byte LE | — |
| `y2.j` | SHORT | `short c` | 2 byte LE | — |
| `y2.k` | STRING | `String c` | u16 len + byte[] | `g()` |
| `y2.l` | abstrak | `String b` | `h()` membungkus `l()` | semua via `i(Class)` |

---

## 4. `d1.b` — HELPER & VALIDASI (517 BARIS)

### 4.1 State statis

```java
public static boolean a = false;   // dit Set true oleh x.a (Appodeal init sukses)
public static long b;              // timestamp terakhir interstitial ditampilkan
```

### 4.2 Daftar SELURUH method statis `a` … `u`

| Method | Tanda tangan | Penjelasan lengkap |
|---|---|---|
| `a` | `String a(t.a, ArrayList)` | Build JSON `FlatWorldLayers`: `biome_id` dari `aVar.d`; `block_layers` array of `{block_name, count}` dari `t.h.a.c` / `t.h.b`; `encoding_version = 6`; `structure_options = JSONObject.NULL`; `world_version = "version.post_1_18"` |
| `b` | `String b(x2.a, String)` | **Validator nilai tag** — detail penuh di §4.3 |
| `c` | `void c(AssetManager, String, File)` | Salin satu asset ke file (buffer `e.w`), try-with-resources manual dengan `addSuppressed` |
| `d` | `void d(AssetManager, String, File)` | Rekursif salin direktori asset; `mkdirs()` jika belum ada; `assetManager.list()` null → return |
| `e` | `h0.d e(h0.d parent, int depth)` | Buat node array-element: `new d("<unnamed>", true, null)`, `e = parent`, `f = depth + 1` |
| `f` | `h0.d f(l tag, h0.d parent, int depth)` | **Pembangun pohon editor** — detail di §4.5 |
| `g` | `void g(Context, t.a, ArrayList, boolean)` | Buat world flat baru — detail di §4.6 |
| `h` | `void h(File)` | Hapus rekursif via `Files.walk(...).sorted(reverseOrder()).forEach(delete)`; error ke `System.err.println("ERROR " + msg)` |
| `i` | `byte[] i(int)` | int → 4 byte LE: `(byte)i, (byte)(i>>8), (byte)(i>>16), (byte)(i>>24)` |
| `j` | `int j(byte,byte,byte,byte)` | 4 byte LE → int: `(b3&255) | ((b4&255)<<8) | ...` |
| `k` | `boolean k(Context)` | Cek `READ_EXTERNAL_STORAGE` dan `q.w` (WRITE) lewat `ContextCompat.checkSelfPermission == 0` |
| `l` | `File l(Context, Uri, int)` | Copy Uri ke `externalFilesDir/import.<ext>` (ext dari `net.pubnative...a.a(i)`), buffer `OpenBitSet.PAGE_SIZE` |
| `m` | `void m(MainActivity, int)` | `ACTION_GET_CONTENT` chooser, MIME: `application/octet-stream`, `x-mcpack`, `x-mcworld`, `x-mctemplate`, `x-mcaddon`; category `OPENABLE` |
| `n` | `void n(Context, File, String)` | Import `.zip` ke `worlds/<nama>` via zip4j `extractAll`; error → `r(new androidx.profileinstaller.c(ctx, 12/13/14))` |
| `o` | `l o(boolean, ByteArrayInputStream)` | **Loader NBT** — lihat §1.1 |
| `p` | `byte[] p(ByteArrayInputStream, int)` | Baca tepat `n` byte; jika `-1` → `EOFException("Reached end of stream while attempting to read " + n + " bytes")` |
| `q` | `void q(FragmentActivity, String, String, String, Runnable, Bundle)` | posting `com.applovin.impl.mediation.j(...)` ke main thread via `r()` (progress dialog) |
| `r` | `void r(Runnable)` | `new Handler(Looper.getMainLooper()).post(runnable)` |
| `s` | `int s(int, float)` | `return (int)((double)(i * f) + 0.5d)` (pembulatan) |
| `t` | `void t(Activity)` | Interstitial: jika `now - b >= h.c` dan `Appodeal.isLoaded(3)` → `show(activity, 3, "static_interstitial")` |
| `u` | `int u(int, float)` | `return (int)((double)(i / f) + 0.5d)` |

`d1.a` (12 baris) hanya penampung data: `double a,b,c,d; int e,f; boolean g;` — bukan bagian engine NBT (dipakai modul iklan/konfigurasi).

### 4.3 Validator `d1.b.b(x2.a, String)` — tabel lengkap

Kode kunci (dipotong sesuai switch):

```java
public static String b(x2.a aVar, String str) {
    int iOrdinal = aVar.ordinal();
    if (iOrdinal != 11) {
        boolean z = false;
        switch (iOrdinal) {
            case 1: case 7:  // BYTE, BYTE_ARRAY
                if (new c(-128L, 127L).a(str)) return null;
                return "Out of bounds for byte.";
            case 2:          // SHORT
                if (new c(-32768L, 32767L).a(str)) return null;
                return "Out of bounds for short.";
            case 3:          // INT → break, dicek setelah switch
                break;
            case 4:          // LONG
                if (new c(Long.MIN_VALUE, Long.MAX_VALUE).a(str)) return null;
                return "Out of bounds for long.";
            case 5:          // FLOAT
                ... cek [-3.4028234663852886E38, +3.4028234663852886E38] ...
                return "Out of bounds for float.";
            case 6:          // DOUBLE
                ... cek [-1.7976931348623157E308, Double.MAX_VALUE] ...
                return "Out of bounds for double.";
            case 8:          // STRING
                if (str != null && str.getBytes(StandardCharsets.UTF_8).length <= 65535) z = true;
                return z ? null : "Out of bounds for string.";
            default:
                return "Unknown tag to edit, report to developers.";
        }
    }
    if (new c(-2147483648L, 2147483647L).a(str)) return null;
    return "Out of bounds for integer.";
}
```

| Ordinal | Tipe | Aturan validasi | Pesan error persis (return) | Sukses |
|---:|---|---|---|---|
| 0 | TAG_END | jatuh ke `default` | `"Unknown tag to edit, report to developers."` | tidak pernah |
| 1 | TAG_BYTE | `Long.parseLong(str)` ∈ [-128, 127] | `"Out of bounds for byte."` | `null` |
| 2 | TAG_SHORT | ∈ [-32768, 32767] | `"Out of bounds for short."` | `null` |
| 3 | TAG_INT | `break` → cek int32 di bawah | `"Out of bounds for integer."` | `null` |
| 4 | TAG_LONG | ∈ [Long.MIN_VALUE, Long.MAX_VALUE] (selalu lolos jika angka valid) | `"Out of bounds for long."` | `null` |
| 5 | TAG_FLOAT | `Double.parseDouble` ∈ [-Float.MAX_VALUE, Float.MAX_VALUE] | `"Out of bounds for float."` | `null` |
| 6 | TAG_DOUBLE | `Double.parseDouble` ∈ [-Double.MAX_VALUE, Double.MAX_VALUE] | `"Out of bounds for double."` | `null` |
| 7 | TAG_BYTE_ARRAY | sama seperti BYTE (elemen byte) | `"Out of bounds for byte."` | `null` |
| 8 | TAG_STRING | `str.getBytes(UTF_8).length <= 65535`, null ditolak | `"Out of bounds for string."` | `null` |
| 9 | TAG_LIST | jatuh ke `default` | `"Unknown tag to edit, report to developers."` | tidak pernah |
| 10 | TAG_COMPOUND | jatuh ke `default` | `"Unknown tag to edit, report to developers."` | tidak pernah |
| 11 | TAG_INT_ARRAY | gate `if (ordinal != 11)` dilewati → cek int32 | `"Out of bounds for integer."` | `null` |

`null` berarti **valid** (konvensi validator Android). Semua pesan di atas dikirim ke `Toast.makeText(context, strB, Toast.LENGTH_SHORT).show()`.

Validator pembantu `b0.c` (`b0/c.java`, 38 baris):

```java
public final double a, b;
public final boolean c = false;              // mode "long only"
public c(long j, long j2) { this.a = j; this.b = j2; }
public boolean a(String str) {
    if (str == null || str.isEmpty()) return false;
    try {
        if (c) { double d3 = Double.parseDouble(str); return d3 >= a && d3 <= b; }
        else   { double d4 = Long.parseLong(str);    return d4 >= a && d4 <= b; }
    } catch (NumberFormatException unused) { return false; }
}
```

Karena `c` selalu `false`, float/double **tidak** memakai `b0.c`; mereka memakai `Double.parseDouble` manual. String kosong selalu invalid untuk numerik, tetapi editor mengizinkan string kosong lewat `allowEmpty(true)` — konsekuensinya `"Out of bounds for ..."` muncul saat mengosongkan field numerik.

### 4.4 Pesan error lain di `d1.b` (world generator)

| Situasi | Pesan persis |
|---|---|
| `skip(8)` gagal di `o()` | `"No header found."` (`IllegalStateException`) |
| parser rusak (`y2.l.k`) | `"Invalid NBT formatting."` (`IllegalStateException`) |
| gagal buat folder worlds | `"Could not create worlds directory."` |
| gagal buat folder world | `"Could not create world folder."` |
| gagal buat folder db | `"Could not create db folder."` |
| gagal simpan level data | `"Failed to save world level data."` |
| gagal build flat data | `"Failed to build flat world data."` |
| gagal load level data | `"Failed to load world level data."` |
| gagal salin template | `"Failed to copy template files."` |
| gagal buat database | `"Failed to create world database."` |
| `p()` kehabisan stream | `"Reached end of stream while attempting to read <n> bytes"` |

### 4.5 `d1.b.f()` — pembangun pohon editor (`h0.d`)

```java
public static d f(l lVar, d parent, int depth) {
    x2.a type = lVar.j();
    String name = lVar.b;
    if (name == null) name = "<unnamed>";
    if (depth == 0) name = "root";

    d node = new d(name, false, lVar);
    node.e = parent;
    node.f = depth;

    switch (type.ordinal()) {
        case 9:  // LIST
            lVar.i(y2.h.class);
            for (Object child : ((y2.h) lVar).d)
                node.d.add(f((l) child, node, depth + 1));
            break;
        case 10: // COMPOUND
            for (Object child : lVar.d().c)
                node.d.add(f((l) child, node, depth + 1));
            break;
        case 11: // INT_ARRAY → satu node per int
            for (int v : lVar.f().c) { String.valueOf(v); node.d.add(e(node, depth+1)); }
            break;
        case 7:  // BYTE_ARRAY → satu node per byte
            for (byte v : lVar.c().c) { String.valueOf((int) v); node.d.add(e(node, depth+1)); }
            break;
        default: // skalar: leaf
            lVar.toString();
            node.b = true;
            return node;
    }
    return node;
}
```

Poin penting:
- Root selalu diberi label `"root"` (`depth == 0`), menggantikan nama asli.
- Anak array memakai `e()`: tag `c = null`, label `"<unnamed>"`, `b = true` (bisa diedit sebagai primitif), `f = depth+1`.
- Node container (List/Compound/array) punya `b = false`; node skalar `b = true`. Flag ini yang menentukan apakah menu "Edit" muncul (lihat §8.3).
- `String.valueOf(...)` dipanggil tapi hasilnya **tidak disimpan** — JADX artifact; secara fungsional hanya efek samping konversi.

### 4.6 `d1.b.g()` — pembuatan world flat (ringkas tapi lengkap)

```
1. arrayList2 = copy(arrayList)
2. JIKA z: total = sum(count) - 65; jika total < 39 → tambah h(t.c.Mk, 39-total)
3. JIKA sum == 1 → tambah h(t.c.Mk, 1)
4. Collections.reverse(arrayList2)
5. folder = externalFilesDir/worlds  (mkdirs, else "Could not create worlds directory.")
6. world = "MineTuner Custom Flat World", jika ada → suffix _1, _2, …
7. world/db mkdirs (else "Could not create db folder.")
8. j db = new j(world); db.k(); db.a();
9. salin asset worldTemplate/level.dat & levelname.txt
10. root = db.l(); json = a(biome, arrayList2)
11. Optional opt = root.d().m("FlatWorldLayers")
    ├─ ada → ((l) opt.get()).g().c = json
    └─ tidak → root.d().c.add(new k("FlatWorldLayers", json))
12. db.m(root)
13. setiap gagal → h(world) + RuntimeException pesan sesuai tabel §4.4
```

Ini membuktikan pemakaian nyata `y2.c.m(nama)`, `y2.k` (StringTag), dan `t.j.l()/m()` (header 8 byte).

---

## 5. EDITOR UTAMA `f0.g` (673 BARIS)

### 5.1 Field dan deklarasi kelas

```java
public class g extends Fragment
        implements SimpleDialog.OnDialogResultListener,
                   SimpleInputDialog.InputValidator,
                   c0.b {
    public l b = null;                  // root tag yang sedang diedit
    public FloatingActionButton c;      // FAB SAVE (icon 2131231307, margin 16dp)
    public FloatingActionButton d;      // FAB ADD  (icon 2131231263, margin 80dp)
    public LinearLayout e;              // breadcrumb (id 2131362055)
    public RecyclerView f;              // daftar anak (id 2131363160)
    public h0.d g;                      // node saat ini
    public h0.c h;                      // adapter
}
```

### 5.2 `onCreateView()` — konstruksi UI (baris 293-412)

Diagram susunan view:

```
FrameLayout (root)
 ├─ LinearLayout (orientation=VERTICAL, MATCH_PARENT)
 │   ├─ HorizontalScrollView (MATCH_PARENT × WRAP_CONTENT, fillViewport=true)
 │   │   └─ LinearLayout id=0x7F…2055 (2131362055)  ← BREADCRUMB
 │   │       orientation=HORIZONTAL, padding 8dp
 │   └─ RecyclerView id=0x7F…3160 (2131363160)      ← TREE
 │       weight=1, LinearLayoutManager
 ├─ FloatingActionButton c  gravity=TOP|END (0x800035), margin 16dp ← SAVE
 └─ FloatingActionButton d  gravity=TOP|END, margin bottom 80dp    ← ADD
```

Kode kunci:

```java
int i = (int) (density * 16.0f);
linearLayout2.setId(2131362055);          // breadcrumb
recyclerView.setId(2131363160);           // tree
FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(-2, -2, 8388693);
lp.setMargins(i, i, i, i);                // FAB c
lp2.setMargins(i, i, i, (int)(density*64f) + i);  // FAB d (di atas FAB c)
```

Perilaku klik:
- FAB `c` → `switch(0)` case 0 → cek `b0.b.a()`, lalu
  `d1.b.r(new androidx.media3.exoplayer.audio.i(runnable, "Saving NBT data...", "Please wait while the NBT data is being saved.", fragment, "save_world_progress_dialog", 17))` → memanggil `d()` (setter tersimpan di `com.unity3d.services.ads.gmascar.managers.a(gVar, 14)`).
- FAB `d` → `switch(1)` → default → buka `SimpleFormDialog` "Create NBT Tag" / "Create a new tag inside the current compound or list." / pos "CREATE" / theme 2132017500 / fullscreen:
  - `Input.spinner("NEW_TAG_TYPE", strArr).hint("Tag Type").text(strArr[0]).required()`
  - `Input.plain("NEW_TAG_NAME").hint("Tag Name").required()` **hanya jika** parent bukan List/array (`z2`/`z3` false).
  - `show(this, "add_new_nbt_tag_dialog")`.
- Kedua listener berisi duplikat kode save/create (cabang yang tak terpakai karena konstanta switch berbeda) — artifact JADX.

`strArr` berasal dari `Arrays.stream(x2.a.values()).filter(new d(1)).map(f(29)).toArray(new e(1))` → semua tipe kecuali `TAG_END`, dengan nama tampilan dari enum.

### 5.3 `c(h0.d)` — breadcrumb builder (baris 195-227)

```java
public final void c(h0.d dVar) {
    this.g = dVar;
    this.e.removeAllViews();
    ArrayList list = new ArrayList();
    h0.d cur = this.g;
    while (cur != null) { list.add(0, cur); cur = cur.e; }   // kumpulkan ancestor, reverse

    for (int i = 0; i < list.size(); i++) {
        h0.d node = (h0.d) list.get(i);
        TextView tv = new TextView(getContext());
        tv.setText(node.a);                       // label node
        tv.setPadding(10, 10, 10, 10);
        tv.setOnClickListener(new r(this, node, 6)); // klik → navigasi ke node itu
        this.e.addView(tv);
        if (i < list.size() - 1) {
            TextView sep = new TextView(getContext());
            sep.setText(" > ");
            this.e.addView(sep);
        }
    }
    this.h = new h0.c(getContext(), dVar.d);      // adapter anak node saat ini
    this.f.setAdapter(this.h);
    this.h.l = new f(this);                       // callback klik
    this.h.m = new f(this);                       // callback long-click
}
```

Alur breadcrumb: ikuti pointer `e` (parent) sampai `null`, sisipkan di indeks 0 agar urutan root → … → current. Tiap label bisa diklik (`r(..., 6)`) untuk "kembali" ke level tersebut. Adapter dibuat ulang setiap navigasi.

### 5.4 `b(Context, String, String, x2.a, int, Bundle)` — membuat tag (baris 48-193)

Parameter: `name`, `typeName`, `listElementType`, `dialogResult (i)`, `extras`.

**Alur lengkap:**

```
1. if (i != -1 || context == null || typeName == null) return;   // hanya proses OK
2. newType = x2.a.valueOf(typeName)
   parentType = current.j()
   z2 = (parentType == LIST)
   z3 = (parentType == BYTE_ARRAY || parentType == INT_ARRAY)
3. if (!z2 && !z3 && (name == null || UTF8(name).length > 65535))
      Toast "Failed to create tag, tag name is out of bounds."; return
4. if (newType == LIST && listElementType == null)
      SimpleListDialog: title "Select a list element type for \"<name>\"",
      choiceMode 11, LAYOUT_ACTION, items = semua tipe kecuali TAG_END,
      extra(bundle), show "edit_nbt_tag_type_dialog"; return
5. strTrim = (parent ∈ {LIST, BYTE_ARRAY, INT_ARRAY}) ? null : name.trim()
6. if (parent == COMPOUND && (strTrim == null || strTrim.isEmpty()))
      Toast "Tags inside a CompoundTag must have a name"; return
7. switch (newType.ordinal()) buat bVar:
      1 → new y2.b(strTrim, (byte)0)
      2 → new y2.j(strTrim, (short)0)
      3 → new y2.g(strTrim, 0)
      4 → new y2.i(strTrim, 0L)
      5 → new y2.e(strTrim, 0.0f)
      6 → new y2.d(strTrim, 0.0d)
      7 → new y2.a(strTrim, new byte[0])
      8 → new y2.k(strTrim, "")
      9 → new y2.h(strTrim, listElementType, new ArrayList())
     10 → new y2.c(strTrim, new ArrayList())
     11 → new y2.f(strTrim, new int[0])
     default → null
   JIKA null → Toast "Failed to create tag"; return
8. switch (parentType.ordinal()):
      7 (BYTE_ARRAY):  if bVar.j()!=BYTE → "Only byte tags can be added to byte arrays"
                       copyOf(byteArray, len+1), isi dgn ((y2.b)bVar).c,
                       newNode = d1.b.e(parent, depth+1)
      9 (LIST):        if parent.c != bVar.j() → "List element type mismatch, it should be " + parent.c
                       else parent.d.add(bVar), newNode = d1.b.f(bVar, parent, depth+1)
     10 (COMPOUND):    if strTrim != null && ada child dgn nama sama →
                       "A tag with this name already exists in the compound"
                       else parent.d().c.add(bVar), newNode = d1.b.f(...)
     11 (INT_ARRAY):   if bVar.j()!=INT → "Only int tags can be added to int arrays"
                       copyOf(intArray, len+1), isi dgn bVar.e().c,
                       newNode = d1.b.e(parent, depth+1)
   catch (Exception e) → Toast "Failed to add tag: " + e.getMessage()
9. JIKA newNode != null → adapter.j.add(newNode); adapter.notifyItemInserted(size-1)
```

Tabel pesan Toast pada `b()`:

| Kondisi | Pesan persis | Durasi |
|---|---|---|
| Nama > 65535 byte UTF-8 (non-list/array) | `"Failed to create tag, tag name is out of bounds."` | `1` (SHORT) |
| Parent Compound, nama kosong | `"Tags inside a CompoundTag must have a name"` | `1` |
| Tipe baru di luar 1..11 | `"Failed to create tag"` | `1` |
| Elemen ByteArray bukan BYTE | `"Only byte tags can be added to byte arrays"` | `1` |
| Tipe elemen List beda | `"List element type mismatch, it should be <TIPE>"` | `1` |
| Duplikat nama di Compound | `"A tag with this name already exists in the compound"` | `1` |
| Elemen IntArray bukan INT | `"Only int tags can be added to int arrays"` | `1` |
| Exception saat insert | `"Failed to add tag: <pesan exception>"` | `1` |

Catatan: validator `d1.b.b()` **tidak** dipanggil saat create — nilai awal selalu nol/string kosong, jadi aman.

### 5.5 `e(h0.d, int)` — dialog Edit nilai (baris 232-291)

```java
Bundle bundle = new Bundle();
bundle.putInt("itemPos", i);
bundle.putInt("tagTypeToCheck",
    (dVar.c == null ? dVar.e.c.j() : dVar.c.j()).ordinal());   // array element → tipe parent
SimpleInputDialog dlg = SimpleInputDialog.build();
if (dVar.c != null) {
    switch (dVar.c.j().ordinal()) {
        case 1: inputType(4098);  text(String.valueOf((int)((y2.b)dVar.c).c)); break;
        case 2: inputType(4098);  text(String.valueOf((int)((y2.j)dVar.c).c)); break;
        case 3: inputType(4098);  text(String.valueOf(dVar.c.e().c));        break;
        case 4: inputType(4098);  text(String.valueOf(((y2.i)dVar.c).c));    break;
        case 5: inputType(12290); text(String.valueOf(((y2.e)dVar.c).c));    break;
        case 6: inputType(12290); text(String.valueOf(dVar.c.c));            break;
        case 8: inputType(131073);text(dVar.c.g().c);                        break;
    }
} else {
    int idx = dVar.e.d.indexOf(dVar);
    if (parentType == BYTE_ARRAY) { inputType(4098); text(String.valueOf((int) parent.c().c[idx])); }
    if (parentType == INT_ARRAY)  { inputType(4098); text(String.valueOf(parent.f().c[idx])); }
}
if (nama == null) nama = "<unnamed>";
dlg.title("Edit \"" + nama + "\"").neg("CANCEL").pos("OK")
   .hint("new value for the tag").allowEmpty(true)
   .extra(bundle).show(this, "edit_nbt_tag_dialog");
```

`inputType`:
- `4098 = 0x1002` → `TYPE_CLASS_NUMBER | TYPE_NUMBER_FLAG_SIGNED`
- `12290 = 0x3002` → `TYPE_CLASS_NUMBER | TYPE_NUMBER_FLAG_SIGNED | TYPE_NUMBER_FLAG_DECIMAL`
- `131073 = 0x20001` → `TYPE_CLASS_TEXT | TYPE_TEXT_FLAG_MULTI_LINE`

`tagTypeToCheck` crucial: untuk elemen array (tag `null`) dipakai tipe induknya (7 atau 11), sehingga `validate()` menerapkan batas byte/int, bukan "Unknown tag".

### 5.6 `validate()` — implementasi `SimpleInputDialog.InputValidator` (baris 666-672)

```java
public final String validate(String str, String str2, Bundle bundle) {
    if (!"edit_nbt_tag_dialog".equals(str)) return null;
    int i = bundle.getInt("tagTypeToCheck", -1);
    return i == -1 ? "Unknown tag type" : d1.b.b(x2.a.values()[i], str2);
}
```

Karena `ordinal == id`, `x2.a.values()[i]` adalah tipe yang tepat. Pesan `"Unknown tag type"` muncul hanya jika extras hilang.

### 5.7 `onResult()` — semua dialog (baris 414-599)

Tabel lengkap:

| Tag dialog | Aksi saat `i == -1` (OK/POSITIVE) | Catatan |
|---|---|---|
| `"show_nbt_tag_options_dialog"` | Baca `SimpleListDialog.selectedSingleLabel` (default `""`) dan `itemPos`. `"Remove"` → untuk parent bertipe 7/9/10/11 tampil konfirmasi `"Remove \"<nama>\" / Are you sure you want to remove this tag?"` dengan pos `REMOVE`, tag `"remove_nbt_tag__dialog"`; selain itu Toast `"Cannot remove this tag."`. `"Edit"` → `e(node, pos)`. | Menu item: `"Edit","Remove"` jika `node.b == true`, else hanya `"Remove"` (lihat `h0.c`) |
| `"remove_nbt_tag__dialog"` | Hapus dari struktur: case 7 → `System.arraycopy` buang byte pada `iIndexOf`; case 9 → `list.d.remove(lVar2)`; case 10 → `compound.m(nama).ifPresent(remove)`; case 11 → `arraycopy` buang int. Lalu `adapter.j.remove(pos)` + `notifyItemRemoved`. | catch → Toast `"Failed to remove tag: <msg>"` |
| `"edit_nbt_tag_dialog"` | Baca `SimpleInputDialog.text` dan `itemPos`. **Jika node array** (`c == null`): validasi `d1.b.b(tipeParent, text)`, lalu case 7 → `parent.c().c[idx] = Byte.parseByte(text)`, case 11 → `parent.f().c[idx] = Integer.parseInt(text)`. **Jika node skalar**: validasi `d1.b.b(tipeTag, text)`, lalu switch 1→`Byte.parseByte`, 2→`Short.parseShort`, 3→`Integer.parseInt` (via `e()`), 4→`Long.parseLong`, 5→`Float.parseFloat`, 6→`Double.parseDouble`, 8→String assignment. | Jika validasi gagal → Toast pesan dari `d1.b.b()`. Jika parsing gagal → Toast `"An error occurred when editing value."` |
| `"add_new_nbt_tag_dialog"` | `b(getContext(), extras.getString("NEW_TAG_NAME"), extras.getString("NEW_TAG_TYPE"), null, i, bundle)` | Minta tipe elemen List terjadi di dalam `b()` |
| `"edit_nbt_tag_type_dialog"` | Baca `NEW_TAG_NAME`, `NEW_TAG_TYPE`, `selectedSingleLabel`; `b(..., x2.a.valueOf(selectedSingleLabel), i, bundle)` | Dipanggil setelah user pilih tipe elemen List |
| `"save_world_progress_dialog"` | Jika `extras.getString("error") == null` → Toast `"NBT data saved successfully."` (durasi `0` = LENGTH_LONG); selain itu → Toast `"Failed to save NBT data."` (durasi `1`) | Untuk semua dialog lain yang dikenal → `return true` |

Method mengembalikan `true` untuk semua dialog yang dikenal, `false` untuk tag dialog tak dikenal.

Ringkasan seluruhToast di editor:

| Pesan | Sumber | Durasi |
|---|---|---|
| `"Failed to create tag, tag name is out of bounds."` | `b()` | 1 |
| `"Tags inside a CompoundTag must have a name"` | `b()` | 1 |
| `"Failed to create tag"` | `b()` | 1 |
| `"Only byte tags can be added to byte arrays"` | `b()` | 1 |
| `"List element type mismatch, it should be <T>"` | `b()` | 1 |
| `"A tag with this name already exists in the compound"` | `b()` | 1 |
| `"Only int tags can be added to int arrays"` | `b()` | 1 |
| `"Failed to add tag: <msg>"` | `b()` | 1 |
| `"Cannot remove this tag."` | `onResult` | 1 |
| `"Failed to remove tag: <msg>"` | `onResult` | 1 |
| `"An error occurred when editing value."` | `onResult` | 1 |
| pesan dari `d1.b.b()` | `onResult`/`validate()` | 1 |
| `"NBT data saved successfully."` | `onResult` | 0 |
| `"Failed to save NBT data."` | `onResult` | 1 |
| `"Unknown tag type"` | `validate()` | (inline validator) |

### 5.8 State restore — `rootTag.tmp` + `breadcrumbPath` (baris 601-664)

**`onSaveInstanceState(Bundle)`**
```
1. super.onSaveInstanceState(bundle)
2. JIKA rootTag != null && context != null:
      FileOutputStream(new File(context.getCacheDir(), "rootTag.tmp"))
      fileOutputStream.write(rootTag.h())        ← serialisasi NBT mentah (tanpa header 8 byte)
      close()
   (IOException ditelan)
3. JIKA currentNode != null:
      ArrayList<String> path
      cur = currentNode
      while (cur != null && cur.e != null):
          label = (cur.c == null || cur.c.b == null) ? "<unnamed>" : cur.c.b
          path.add(0, label)
          cur = cur.e
      bundle.putStringArrayList("breadcrumbPath", path)
```
Catatan: loop berhenti sebelum node root (karena `cur.e != null`), jadi path berisi label semua level di bawah root. Label berasal dari `tag.b`, **bukan** `node.a` (node root "root" tidak pernah masuk).

**`onViewStateRestored(Bundle)`**
```
1. super...; if bundle == null || context == null → return
2. File f = cacheDir/rootTag.tmp
3. JIKA f.exists():
      root = d1.b.o(false, new ByteArrayInputStream(Files.readAllBytes(f)))  ← flag false
      this.b = root
      node = d1.b.f(root, null, 0)
      c(node)
      path = bundle.getStringArrayList("breadcrumbPath")
      JIKA path != null:
         for (String label : path):
            for (h0.d child : node.d):
               if (label.equals(child.a)) { node = child; break; }
         c(node)
   catch (Exception) → ditelan
```

Diagram alur restore:
```
[Bundle breadcrumbPath] ─┐
[rootTag.tmp → o(false) → l.k(-1) → root tag] → d1.b.f(...) → tree node "root"
                                                      │
                                    telusuri path per level (cocokkan child.a)
                                                      ↓
                                             c(node) → breadcrumb + adapter
```

Kelemahan: pencocokan hanya pada anak langsung dan berdasarkan label; kalau dua child bernama sama di Compound, navigasi bisa salah. Juga `exceptions` ditelan sehingga cache korup membuat editor tampil kosong tanpa pesan.

---

## 6. EDITOR KHUSUS: `f0.a`, `f0.h`, `f0.b`

### 6.1 `f0.a` — editor nilai LevelDB (75 baris)

```java
public class a extends g {
    public byte[] i;                                   // kunci LevelDB

    public static a f(String str) {                    // FACTORY
        a aVar = new a();
        Bundle b = new Bundle();
        b.putByteArray("db_key", str.getBytes(StandardCharsets.UTF_8));
        aVar.setArguments(b);
        return aVar;
    }
    public void onCreate(Bundle b) {
        super.onCreate(b);
        if (getArguments() != null) i = getArguments().getByteArray("db_key");
    }
    public void onViewCreated(View v, Bundle b) {
        super.onViewCreated(v, b);
        if (this.b == null) {
            try {
                j db = x.b.a();
                byte[] key = this.i;
                db.d.readLock().lock();
                try {
                    l parsed = d1.b.o(false, new ByteArrayInputStream(db.c(key))); // ⚠ flag false
                    db.d.readLock().unlock();
                    this.b = parsed;
                    if (parsed != null) { this.b = parsed; c(d1.b.f(parsed, null, 0)); return; }
                } catch (Throwable t) { db.d.readLock().unlock(); throw t; }
            } catch (Exception ignored) {}
            this.c.setEnabled(false); this.d.setEnabled(false);
            this.c.setAlpha(0.5f);     this.d.setAlpha(0.5f);
            Toast.makeText(v.getContext(), "No NBT data found.", 1).show();
        }
    }
    public void d() {                                   // SAVE
        j db = x.b.a();
        byte[] key = this.i;
        l root = this.b;
        db.d.writeLock().lock();
        try { db.a.e(key, root.h()); }                  // tulis NBT mentah (tanpa header)
        finally { db.d.writeLock().unlock(); }
    }
}
```

Poin:
- Factory `f(key)` mengemas kunci sebagai UTF-8 di Bundle `"db_key"`.
- Load: `x.b.a().c(key)` → byte[] dari LevelDB → `d1.b.o(false, …)` (raw, **tanpa** skip 8) → `d1.b.f(...)` → `c(tree)`.
- Save: `writeLock` → `db.a.e(key, root.h())`.
- Jika gagal/parse null: dua FAB di-`setEnabled(false)` + alpha 0.5 + Toast `"No NBT data found."`.

### 6.2 `f0.h` — editor world `level.dat` (35 baris)

```java
public class h extends g {
    public void d() { x.b.a().m(this.b); }               // SAVE → tulis level.dat + header 8 byte

    public void onViewCreated(View v, Bundle b) {
        super.onViewCreated(v, b);
        if (this.b == null) {
            try {
                l root = x.b.a().l();                    // t.j.l() → o(true, level.dat)
                this.b = root;
                if (root != null) { this.b = root; c(d1.b.f(root, null, 0)); return; }
            } catch (Exception ignored) {}
            this.c.setEnabled(false); this.d.setEnabled(false);
            this.c.setAlpha(0.5f); this.d.setAlpha(0.5f);
            Toast.makeText(v.getContext(), "No world data found.", 1).show();
        }
    }
}
```

`x.b.a().l()` (`t/j.java:225-233`) melakukan:
```java
return d1.b.o(true, new ByteArrayInputStream(
        Files.readAllBytes(new File(this.c, "level.dat").toPath())));
```
Sedangkan `m()` sudah ditampilkan di §1.2. Jadi world editor = **dengan header**, LevelDB editor = **tanpa header**.

### 6.3 `f0.b` — daftar player (62 baris)

- `RecyclerView` dalam `FrameLayout`, adapter `y.d(daftarKey, this)`.
- `onViewCreated`: ambil `x.b.a().e()` (semua kunci LevelDB sebagai String UTF-8), tambahkan ke list, `notifyItemRangeRemoved`, toast `"No players found."` jika kosong.
- `a(ViewHolder, i)`: `cVar.n.setText(str.substring(14))` — memotong prefix kunci player (14 char, mis. `"player_<uuid>"` atau `"+o..."`), ikon `2131231750`, klik → `com.appodeal...r(this, str, 5)` → buka `f0.a.f(key)`.

### 6.4 `f0.d`, `f0.e`, `f0.f` — synthetic

- `f0.d`: `Predicate`, `case 0` dan `default` identik → `aVar != x2.a.c` (filter TAG_END). Duplikasi cabang artifact JADX.
- `f0.e`: `IntFunction`, `case 0`/`default` → `new String[i]` (array builder untuk `toArray`).
- `f0.f`: pembungkus `{ final g a; }` — dipakai `h0.c` sebagai callback klik (`l`, `m`).

---

## 7. ADAPTER & STATE TREE: `h0.b`, `h0.c`, `h0.d`

### 7.1 `h0.d` — node state (20 baris)

```java
public final class d {
    public final String a;          // label tampilan ("root", "<unnamed>", atau nama tag)
    public boolean b;               // true = node skalar (bisa diedit nilainya)
    public final l c;               // tag asli; NULL untuk elemen array
    public final ArrayList d = new ArrayList();  // anak (h0.d)
    public d e;                     // parent
    public int f;                   // kedalaman
    public d(String str, boolean z, l lVar) { a=str; b=z; c=lVar; }
}
```

Ini bukan `Parcelable` — oleh karena itu state tidak disimpan di Bundle sebagai objek, melainkan **diserialisasi ulang** ke `rootTag.tmp` dan breadcrumb cukup berupa daftar String (§5.8).

### 7.2 `h0.b` — ViewHolder (18 baris)

```java
public final TextView m;   // findViewById(2131362689)
public final ImageView n;  // findViewById(2131363045)
```

### 7.3 `h0.c` — RecyclerView adapter (120 baris)

Field: `public final List j` (children), `public final Context k`, `public f l` (klik), `public f m` (long-click).

`b(int i)` — akses aman:
```java
if (i < 0) return null;
if (i < j.size()) return (d) j.get(i);
return null;
```

`onBindViewHolder` — ikon menurut tipe (ordinal):

| Ordinal | Tipe | Resource ikon |
|---:|---|---:|
| 1 | BYTE | 2131231652 |
| 2 | SHORT | 2131231661 |
| 3 | INT | 2131231657 |
| 4 | LONG | 2131231660 |
| 5 | FLOAT | 2131231656 |
| 6 | DOUBLE | 2131231655 |
| 7 | BYTE_ARRAY | 2131231653 |
| 8 | STRING | 2131231662 |
| 9 | LIST | 2131231659 |
| 10 | COMPOUND | 2131231654 |
| 11 | INT_ARRAY | 2131231658 |
| (c == null, parent 7) | elemen ByteArray | 2131231652 |
| (c == null, parent 11) | elemen IntArray | 2131231657 |

Interaksi:
- **Long-click** → `fVar = this.m`; jika null return false; `pos = holder.getBindingAdapterPosition()`; Bundle `"itemPos" = pos`; `SimpleListDialog.build().title("Options for \"<label>\"").choiceMode(11).layout(LAYOUT_ACTION).items(node.b ? {"Edit","Remove"} : {"Remove"}).extra(bundle).show(fragment, "show_nbt_tag_options_dialog")`.
- **Click** → `new s9(this, 1, node, holder)` → masuk ke node (breadcrumb). Kelas `s9` adalah synthetic yang tidak ikut ter-dekompilasi ke `sources/`, tetapi perannya jelas dari `f0.g.c()` yang mengganti node saat ini.
- `onCreateViewHolder` → inflate layout `2131558924`.

---

## 8. HELPER `x`, `y`, DAN KONTEKS WORLD

### 8.1 `x.b` — singleton konteks world (13 baris)

```java
public abstract class b {
    public static final AtomicReference a = new AtomicReference(null);
    public static j a() { return (j) a.get(); }        // t.j (LevelDB wrapper)
}
```
Semua editor (`f0.a`, `f0.h`, `f0.b`) memakai `x.b.a()` untuk mengakses `t.j` milik world yang sedang dibuka.

### 8.2 `x.a` — callback inisialisasi iklan (23 baris)
Jika daftar error kosong → `d1.b.a = true` dan `Appodeal.setInterstitialCallbacks(new e0.a())`; else print setiap error ke stderr. Menjelaskan arti flag `d1.b.a` yang dicek tiap klik FAB (`b0.b.a()`).

### 8.3 `x.d` — ItemTouchHelper (35 baris)
`makeMovementFlags(3, 0)` (UP|DOWN, tidak ada swipe), `isLongPressDragEnabled() = true`, `onMove` memindahkan `t.h` dalam `CreateWorldActivity.o` dan `notifyItemMoved`. Terkait pembuatan world (layer), bukan engine NBT langsung.

### 8.4 `y.*` — adapter pendukung

| File | Isi |
|---|---|
| `y/a.java` | ViewHolder world/layer: `m,n,o` TextView (2131362693/2692/2694), `p` ImageButton (2131362167) |
| `y/b.java` | `RecyclerView.Adapter` untuk `CreateWorldActivity`; `onBindViewHolder` mendelegasi ke `createWorldActivity.a(holder, i)`; inflate 2131558926 |
| `y/c.java` | ViewHolder daftar player: `m` ImageView (2131363045), `n` TextView (2131362689) — dipakai `f0.b` |
| `y/d.java` | Adapter generik dengan `Object k` bertipe `c0.a`; `onBindViewHolder` memanggil `k.a(cVar, i)`; inflate 2131558924 |
| `y/e.java` | Wrapper `{ final t.j a; }` — menyimpan referensi DB world |
| `y/f.java` | ViewHolder daftar world: `m` ImageView (2131363432), `n,o,p` TextView (2131363434/3436/3433) |
| `y/g.java` | Adapter daftar world, delegasi ke `MainActivity.a(holder, i)`; inflate 2131558925 |

### 8.5 `c0.a` / `c0.b` (smali)
`c0.a` = antarmuka `void a(RecyclerView.ViewHolder, int)` (callback bind). `c0.b` = antarmuka marker yang diimplementasi `f0.g` (dipakai sebagai penanda fragment editor).

---

## 9. DIAGRAM ALUR TEKS (END-TO-END)

### 9.1 Buka editor World
```
MainActivity → x.b.a() [t.j world aktif]
   → f0.h.onViewCreated()
   → t.j.l()
   → Files.readAllBytes(level.dat)
   → d1.b.o(true, stream)        [skip 8 byte header]
   → y2.l.k(stream, -1)          [baca type byte root, name, children…]
   → root tag y2.c (Compound)
   → d1.b.f(root, null, 0)       [pohon h0.d, root label "root"]
   → f0.g.c(node)                [breadcrumb + adapter h0.c]
```

### 9.2 Buka editor nilai LevelDB player
```
f0.b (player list) → kunci "player_…" → f0.a.f(key) → Bundle "db_key"
   → f0.a.onViewCreated()
   → x.b.a().c(key) [readLock] → byte[] NBT mentah
   → d1.b.o(false, stream)      [TANPA skip]
   → y2.l.k(...) → d1.b.f(...) → f0.g.c(...)
   → gagal → FAB disabled + alpha .5 + "No NBT data found."
```

### 9.3 Edit nilai
```
long-click baris → h0.c → "show_nbt_tag_options_dialog"
   → pilih "Edit" → f0.g.e(node, pos)
   → SimpleInputDialog "edit_nbt_tag_dialog" (extra itemPos, tagTypeToCheck)
   → validate() → d1.b.b(tipe, teks) → null = lolos / pesan error
   → onResult:
       ├─ node skalar → parse Byte/Short/Int/Long/Float/Double/String → set field
       └─ node array → parse Byte/Int → tulis ke elemen array pada index
```

### 9.4 Tambah tag
```
FAB d → "add_new_nbt_tag_dialog" (spinner tipe + nama)
   → onResult → f0.g.b(..., result, extras)
   → cek nama (UTF-8 ≤ 65535; Compound wajib nama)
   → JIKA LIST & belum ada tipe elemen → "edit_nbt_tag_type_dialog"
   → buat y2.* dengan nilai nol
   → validasi kelayakan parent (byte/int array, tipe list cocok, duplikat nama)
   → mutasi struktur + d1.b.e()/f() → adapter.notifyItemInserted()
```

### 9.5 Hapus tag
```
long-click → "Remove" → "remove_nbt_tag__dialog" (konfirmasi)
   → onResult: hapus dari byte[]/int[]/List.d/Compound.c
   → adapter.notifyItemRemoved()
   → parent bukan 7/9/10/11 → "Cannot remove this tag."
```

### 9.6 Simpan
```
FAB c → cek b0.b.a() → progress "save_world_progress_dialog"
   → f0.g.d():
       ├─ f0.a.d(): writeLock → db.a.e(key, root.h())      [mentah]
       └─ f0.h.d(): x.b.a().m(root)                        [+ header 8 byte]
   → onResult sukses → "NBT data saved successfully." (LONG)
   → onResult gagal → "Failed to save NBT data." (SHORT)
```

### 9.7 Rotasi/restore
```
onSaveInstanceState → rootTag.tmp = root.h() ; breadcrumbPath = List<String>
onViewStateRestored → o(false, rootTag.tmp) → f() → c() → telusuri path → c(node)
```

---

## 10. TEMUAN, INKONSISTENSI, DAN CATATAN TEKNIS PENTING

1. **Encoder charset tidak eksplisit.** `y2.k.l()` memakai `String.getBytes()` dan parser `y2.l.k()` memakai `new String(byte[])`, keduanya memakai charset default platform. Sementara `d1.b.b()` memvalidasi panjang dengan `StandardCharsets.UTF_8`. Di Android charset default UTF-8, jadi biasanya cocok — tetapi secara teori bisa beda.

2. **Panjang nama dibaca sebagai signed `short`.** Parser: `getShort()`, sedangkan validator mengizinkan sampai 65535 byte. Nilai 32768..65535 akan menjadi negatif → `new byte[negatif]` → `NegativeArraySizeException`. Batas aman sebenarnya 32767 untuk parser, bukan 65535.

3. **Tidak ada batas `count` saat parsing** Array/List. File korup bisa menyebabkan alokasi besar; tidak ada `p()` (helper baca eksak) yang dipakai di parser `y2.l.k()` — `p()` hanya dipakai di tempat lain.

4. **`x2.a.c` tidak ada di `classes6`.** Enum dari `classes6.dex` memakai nama `TAG_*`, tetapi `f0.d`/`f0.g` dari `classes9.dex` merujuk `x2.a.c/d/f/j/l/m/n`. Ini perbedaan pemetaan nama JADX antar-dex; secara runtime sama. `f0.d` juga punya dua cabang switch identik (artifact).

5. **Kode FAB berduplikat.** Kedua `OnClickListener` berisi blok save dan create; hanya satu yang terjangkau karena konstanta switch `0` vs `1`. Tidak berbahaya, tapi menyesatkan saat membaca dekompilasi.

6. **`t.j.m()` memakai `readLock()` saat menulis `level.dat`.** Seharusnya `writeLock()`. `f0.a.d()` sudah benar memakai `writeLock()` untuk LevelDB.

7. **`validate()` dipanggil dua lapis.** `SimpleInputDialog.InputValidator.validate` menolak input sebelum dialog ditutup; `onResult` kembali memanggil `d1.b.b()` sebelum parsing. Ini pertahanan ganda yang bagus, tetapi pesan error bisa muncul dua kali dalam kondisi edge (misal extras berubah).

8. **Root label "root" vs `breadcrumbPath`.** `d1.b.f()` memberi label `"root"`, tetapi `onSaveInstanceState` menyimpan `tag.b`, bukan `node.a`. Karena loop berhenti sebelum root, tidak ada mismatch — namun nama root asli dari file hilang setelah restore (tetap ditampilkan sebagai `"root"` oleh `f()`).

9. **`Compound.m(nama)` bisa NPE** pada child tanpa nama (mode `g0.a(str,1)` = `tag.b.equals(str)`). Editor mencegahnya lewat pemeriksaan `"Tags inside a CompoundTag must have a name"`.

10. **Array element tidak punya `y2.l` Object.** `h0.d.c == null` untuk elemen ByteArray/IntArray. Semua operasi harus naik ke `dVar.e.c` (parent) dan memakai index `parent.d.indexOf(dVar)`. Ini konsisten di `e()`, `onResult`, dan `d1.b.e()`.

11. **`TAG_END` bukan kelas.** Parser mengembalikan `null` untuk type `0`; itulah terminator Compound. Karena itu daftar pilihan tipe di editor memfilter `x2.a.c` (= TAG_END).

12. **Float/Double validation memakai `Double.parseDouble`, lalu `Float.parseFloat`.** Rentang dicek dalam domain double terhadap `Float.MAX_VALUE`, sehingga nilai seperti `"3.4028235E38"` lolos; nilai `"NaN"`/`"Infinity"` ditolak karena perbandingan `>=`/`<=` bernilai false.

13. **`d1.b.b()` tidak memvalidasi nama tag.** Validasi nama dilakukan terpisah di `f0.g.b()` (UTF-8 ≤ 65535 + aturan Compound). Jadi `d1.b.b()` murni validator nilai.

14. **`d1.b.p()` (EOFException helper) tidak dipakai oleh parser NBT.** Parser memakai `read()` mentah; `p()` adalah utilitas umum yang tersedia tapi tidak terhubung ke `y2.l.k()`.

---

## 11. KESIMPULAN RINGKAS

- **Engine** terdiri dari `x2.a` (12 tipe, id 0..11, lookup map `o`) + `y2.l` (parser `k`, serializer `h`/`l`, cast helper `c/d/e/f/g/i`, `j` type) + 11 subclass konkret. Semua numerik LITTLE_ENDIAN; String memakai panjang 16-bit; ByteArray/IntArray memakai count 32-bit; List menyimpan tipe elemen + count; Compound diakhiri `0x00`.
- **Header Bedrock** adalah dua int32 LE (`8`, `panjang payload`) yang ditulis `t.j.m()` dan dilewati `d1.b.o(true, …)`; `o(false, …)` dipakai untuk data LevelDB dan cache.
- **Validasi** terpusat di `d1.b.b()` dengan 7 pesan error khas ("Out of bounds for byte/short/long/float/double/string/integer" dan "Unknown tag to edit, report to developers."), dibantu `b0.c` untuk rentang integer.
- **Editor** `f0.g` membangun breadcrumb dari rantai parent `h0.d.e`, memakai `h0.c` sebagai adapter tree, enam tag dialog untuk seluruh operasi, dan menyimpan state lewat `cacheDir/rootTag.tmp` + `breadcrumbPath` alih-alih parceling node.
- **Dua varian editor** (`f0.a` untuk kunci LevelDB, `f0.h` untuk `level.dat`) hanya berbeda di flag header dan method save (`db.a.e(key, root.h())` vs `x.b.a().m(root)`).
- **Singleton** `x.b.a()` menyediakan `t.j` (world context) ke seluruh fragmen; `y.*` adalah adapter daftar world/player/layer yang mendukung alur membuka editor.

Demikian laporan lengkap — seluruh file yang diminta telah dibaca dan didokumentasikan, termasuk tabel enum, tabel validasi, tabelToast/error, diagram alur teks, dan cuplikan kode kunci.

---

# Bagian 4 — Model world, penyimpanan, LevelDB, peta & aset

Berikut laporan teknis lengkap hasil analisis terhadap seluruh berkas yang diminta, termasuk pelacakan lintas paket untuk bagian yang tidak berada di lokasi yang diasumsikan.

---

# LAPORAN ANALISIS TEKNIS MENDALAM — MineTuner+ 2.2.3 (referensi dekompilasi)

## 0. Ringkasan dan koreksi pemetaan paket

Sebelum masuk ke detail, ada **temuan penting** yang harus diluruskan karena memengaruhiinterpretasi laporan:

| Asumsi permintaan | Realita di kode dekompilasi |
|---|---|
| `d0/e.java` = Show Map | ✅ Benar. `d0.e extends Fragment`, `SimpleDialog.OnDialogResultListener` |
| `z/a..l` = model world, WorldInfo, file tree, filter, zip import/export, encryption detection | ❌ **Tidak benar.** Paket `z` berisi **3 keluarga dialog UI**: pemilih biome (`z/a–e`), pemilih block (`z/f–j`), dan slider (`z/k–l`). Model dunianya ada di `t/j.java`, chunk di `s/a.java`, import/export di `d1/b.java` + `r2/a.java`, context di `x/b.java` |
| Deteksi terenkripsi di `z` | ❌ Ada di **`t/j.java:175–210`**, method `j()` |
| `x/b.java` = world context singleton | ✅ Benar (13 baris, `AtomicReference<t.j>`) |
| `x/d.java` = world context | ❌ `x/d.java` = `ItemTouchHelper.Callback` untuk drag-and-drop layer di `CreateWorldActivity` |
| Key `Overworld`, `TheEnd`, dll. ditangani `LevelDB.java` | ⚠️ `LevelDB.java` hanya menangani prefix `player_server_`. Key lain ditangani di `DrawerActivity.java:37–63` lewat `f0.a.f(key)` |

Semua berkas tetap saya dokumentasikan "tanpa terkecuali", dengan koreksi di tempatnya.

---

## 1. `d0/e.java` — Fragment "Show Map" (431 baris)

### 1.1 Identitas dan field utama

```java
public class e extends Fragment implements SimpleDialog.OnDialogResultListener {
    public static final int h = Math.floorDiv(536870912, 16); // = 33.554.432 = 2^25
    public static final int i = Math.floorDiv(536870912, 16); // = 33.554.432 = 2^25
    public g b;                       // TileView kustom (extends com.qozix.tileview.c)
    public i c;                       // overlay seleksi (FrameLayout)
    public final e0.a d = new e0.a(); // helper render biome
    public final e0.a e = new e0.a(); // helper render blok
    public final HashMap f = new HashMap(); // cache bitmap ikon entity "nama_42"
    public final HashSet g = new HashSet(); // chunk yang sudah dimuat (deduplikasi marker)
}
```

`536870912 = 2^29`; dibagi 16 menghasilkan `2^25 = 33.554.432`. Ini adalah **setengah dari rentang koordinat TileView** (total `2^26 = 67.108.864` unit). Dengan 1 tile = 256 px dan 1 chunk = 16 px pada scale 1.0, peta mencakup kira-kira **±1.048.576 blok (= 2^20)** sesuai konstanta `1048576.0f` yang dipakai berulang kali.

### 1.2 Konstruksi UI di `onCreateView()` (baris 135–286)

Fragment membuat `FrameLayout` lalu membangun **Qozix TileView** secara manual (JADX membongkar konstruktor library menjadi field assignments):

| Komponen | Inisialisasi | Fungsi |
|---|---|---|
| `g` (= `k1.f` extends TileView) | `f=1, g=0, h=1, k=0, n=400`, `HashSet o`, `u=k1.b.b` | View peta utama, gesture, scroller |
| `Scroller` | `gVar.p = new Scroller(ctx)` | inertial scrolling |
| `GestureDetector` | `gVar.s`, listener = `gVar` sendiri | tap, long-press, fling |
| `ScaleGestureDetector` | `gVar.r` | pinch zoom |
| `j1.a`/`c1.d` | `gVar.v` | tile renderer + cache |
| `d1.a` | `gVar.w` | transform koordinat (`a=-1,b=-1,c=2,d=2,g=true`) |
| `f1.b` | `gVar.x` | daftar tile |
| `i1.e` | `gVar.y`, child ke-0 | manajer tile + thread pool (`LinkedBlockingDeque`, `Looper.getMainLooper()`) |
| `h1.a` | `gVar.z`, child ke-1 | layer path/garis (stroke hitam 10 px, anti-alias) |
| `k1.a` | `gVar.A`, child ke-2 | layer overlay |
| `g1.d` | `gVar.B`, child ke-3 | marker penempatan selection (`c=-0.5, d=-1.0`) |
| `g1.a` | `gVar.C`, child ke-4 | marker tambahan |

Konfigurasi penting:

```java
gVar.E = false;                         // loop scale dimatikan
gVar.H = true;                          // touchable
gVar.b = h; gVar.c = i;                 // world width/height = 2^25
gVar.d = d1.b.s(i2, gVar.f);            // skala awal
gVar.e = d1.b.s(gVar.c, gVar.f);
dVar3.d = i2; dVar3.e = i3;             // renderer dimensions
aVar2.e = i2; aVar2.f = i3;             // transform extents
cVar.setBackgroundColor(-12303292);     // 0xFF444444 (abu gelap)
for (float s : new float[]{0.0625f,0.125f,0.25f,0.5f,1.0f})
    this.b.e(s, "0.0625" /* dst." */);  // daftar level zoom
this.b.setMinimumScaleMode(k1.b.c);
this.b.setScale(0.5f);
((k1.f) gVar2).g = 0.2f;                // min scale
((k1.f) gVar2).h = 1.0f;                // max scale
gVar2.setScale(gVar2.f);
this.b.setShouldLoopScale(false);
this.b.setTransitionsEnabled(false);
this.b.setShouldRenderWhilePanning(false);
this.b.setSaveEnabled(true);
this.b.setBitmapProvider(new a(this));  // ← d0/a.java
this.b.setOnLongPressListener(new a(this));
```

Catatan menarik: level zoom didaftarkan 0.0625–1.0, tetapi **min scale diubah ke 0.2** (`k1.f.g=0.2f`) sebelum `setScale(f)` dipanggil. Jadi rentang efektif 0.2–1.0.

Di akhir, overlay `i` dipasang dengan `setTileView(this.b)` dan `setSelectionView(this.c)`, visibility awal `8` (GONE), lalu state dipulihkan dari bundle (`map_save`, `selection_save`), dan `this.b.post(new d.a(this, bundle, 1))` menjadwalkan render/marker setelah layout.

### 1.3 Render chunk → bitmap: `d0/a.java:a(i1.a, Context)` (baris 22–122)

Ini adalah `BitmapProvider` TileView. Structur `i1.a` (dari `jadx_c12/sources/i1/a.java`) memuat `b,c` (ukuran tile), `e,f` (koordinat tile), `o` (state renderer dengan `b`=scale), `h` (bitmap reusable).

Alur perhitungan (dengan `f = aVar.o.b` = scale):

```java
int iFloorDiv  = Math.floorDiv(256, 16);          // 16
float f        = aVar.o.b;                        // scale TileView
int iRound     = Math.round(1.0f / f);             // inverse scale (1 blok = ? px)
int iRound2    = Math.round(iFloorDiv * f);        // 16 * scale (lebar 1 chunk dlm px)
int iRound3    = Math.round(iFloorDiv * f);        // tinggi 1 chunk
int i2 = (iRound2 * 1048576) / 256;                // = iRound2 * 4096
int i3 = (1048576 * iRound3) / 256;
int i4 = (aVar.f - i2) * iRound;                   // chunk X awal
int i5 = (aVar.e - i3) * iRound;                   // chunk Z awal
int i6 = i4 + iRound;                              // chunk X akhir (eksklusif)
int i7 = iRound + i5;                              // chunk Z akhir
int i8 = iRound2 * 16;                             // 256 * scale
int i9 = iRound3 * 16;
Canvas canvas = new Canvas(bitmapCreateBitmap);    // bitmap RGB_565 256x256
```

Bitmap dibuat sekali dengan `Bitmap.createBitmap(aVar.b, aVar.c, Bitmap.Config.RGB_565)` bila `aVar.h == null` — hemat memori (565).

**Checkerboard background**: untuk tiap chunk, canvas diisi 16×16 kotak kecil:

```java
paint.setColor(((i21 + i18) & 1) == 1 ? -13750738 : -12303292);
// -13750738 = 0xFF2E2E2E, -12303292 = 0xFF444444
canvas.drawRect(new Rect(i20, i16, i20 + iRound2, i16 + iRound3), paint);
```

**Data chunk diambil dari singleton dunia**:

```java
s.a aVarB = x.b.a().b(i12, i10, 1);   // chunkX, chunkZ, dimension=1 (Overworld)
eVar.e.b(aVarB, canvas, i28, i11, iRound2, iRound3, paint2);
// e0.a.b() menggambar 256 blok dalam chunk (lihat s/a.java:d())
```

`eVar.d.getClass();` adalah no-op yang ditinggalkan JADX (pemanggalan method yang di-inline).

**Dedup marker**: kunci chunk `long j = (((long) i12) << 32) | (((long) i27) & 4294967295L)` dimasukkan ke `eVar.g`; jika baru, dipanggil `eVar.b(aVarB)` → pasang ikon entity untuk chunk itu. Ini mencegah marker dobel setiap kali tile di-render ulang.

**Bingkai peta**: setelah semua chunk, 256 pixel di tepi atas/bawah/kiri/kanan diisi `-1` (putih) — border putih 1 px. Lalu koordinat kiri-atas ditulis dengan `e.c("(" + (i4*16) + ", " + (i31*16) + ")", canvas)` (teks putih 24 sp, `StaticLayout`).

Semua dibungkus `try/catch (Exception)`: error hanya dicetak `System.err.println("ERROR: " + e.getMessage())` dan bitmap dikembalikan apa adanya — peta tetap tampil walau sebagian data rusak.

### 1.4 Gesture, tap, dan pemetaan koordinat

`d0/g.java` (subclass TileView) meneruskan gesture ke listener `f` (= `d0/a`):

```java
public void onLongPress(MotionEvent e)            { ((a) F).b(e); }
public boolean onSingleTapConfirmed(MotionEvent e){ ((a) F).b(e); return true; }
public void onScrollChanged(...) { super...; if (selection.n) selection.requestLayout(); }
public boolean onTouchEvent(MotionEvent e) { if (!H) return false; super...; return true; }
```

`d0/a.java:b(MotionEvent)` (baris 124–140) adalah **konversi koordinat layar → blok dunia**:

```java
float scale = eVar.b.getScale();
float f = floorDiv(256,16) * scale;                       // 16 * scale
int x2 = (int)(((motionEvent.getX() + eVar.b.getScrollX()) / (16 * scale)) - 1048576.0f);
int y2 = (int)(((motionEvent.getY() + eVar.b.getScrollY()) / f) - 1048576.0f);
int iFloorDiv3 = Math.floorDiv(x2, 16);                   // chunkX
int iFloorDiv4 = Math.floorDiv(y2, 16);                   // chunkZ
Bundle bundle = new Bundle();
bundle.putInt("worldX", x2);
bundle.putInt("worldZ", y2);
```

Artinya: koordinat TileView (0..2^26) dibagi `16*scale` menjadi offset blok dari tengah, lalu dikurangi `2^20` agar menjadi koordinat blok Minecraft (bisa negatif). Chunk = `floorDiv(blok, 16)`.

Dialog yang muncul (`SimpleListDialog`, tag `"mapview_options_dialog"`):

| Mode | Judul | Item |
|---|---|---|
| `selection.n == false` | `Coords(x, z) Chunk(cx, cz)` | `Begin Selection`, `View Entities` |
| `selection.n == true` | `Selection Options` | `Take Picture`, `End Selection` |

`choiceMode(11)` = single-choice, `LAYOUT_ACTION`, `extra(bundle)` membawa worldX/worldZ.

### 1.5 Overlay seleksi `d0/i.java` (160 baris)

- Extends `FrameLayout`, 4 child (`ImageView` handle) dari layout `2131558695`.
- Field: `b` (child handle), `c..f` (koordinat), `g` = `Paint(argb(128,0,0,0))` (gelap 50%), `h` = `Rect` seleksi dalam **koordinat blok**, `j,k` = `RectF` hasil konversi ke layar, `l = density*100`, `m` = touch delegate, `n` = aktif/tidak, `i` = `WeakReference<g>` ke TileView.
- `onDraw()` menggambar `k` (rect ter-clamp ke layar) dengan `quickReject(BW)` optimization.
- `onLayout()` konversi blok → piksel:

```java
float scale = tileView.getScale() * Math.floorDiv(256, 16); // scale * 16
float f = 1048576.0f * scale;
float scrollX = tileView.getScrollX() - f;
rectF.left = (rect.left * scale) - scrollX;  // dst.
// 4 handle di-layout di sudut kiri-atas, kanan-atas, kiri-bawah, kanan-bawah
```

- `onViewAdded()`: handle dengan id tertentu mendapat `b2.b(this,7)` touch listener (drag resize).
- `onSaveInstanceState`/`onRestoreInstanceState`: menyimpan `super`, `has_sel`, `left/top/right/bottom` (konstanta `TtmlNode.LEFT/RIGHT` dari JADX = "left"/"right").
- `setSelectionChangedListener(h)` **kosong** (stub), `setTileView(g)` menyimpan WeakReference.
- `getSelection()` mengembalikan copy `Rect`.

### 1.6 Marker entity — `d0/e.b(s.a)` (baris 66–133)

1. `File entityIcons = new File(cacheDir, "entityIcons")`; kalau belum ada → `d1.b.d(assets, "entityIcons", file)` (ekstrak rekursif aset APK ke cache).
2. Untuk tiap entity key dari `aVar.c()` (yaitu `actorprefix` + 8-byte UUID, lihat `s/a.java`), ambil NBT via `aVar.b(byte[])`.
3. Baca `identifier` → petakan ke `t.f` enum: `t.f.d.get(identifier)` (map statis).
4. Baca `Pos` → `y2.h.d` list of `y2.e` float; ambil elemen `[0]` = X dan `[2]` = Z.
5. Di thread UI (`d1.b.r`):

```java
String str = fVar.c;                       // nama file ikon, mis. "zombie.png"
String strConcat = str.concat("_42");      // cache key
Bitmap bmp = (Bitmap) map.get(strConcat);
if (bmp == null) {
    File file2 = new File(cacheDir, "entityIcons/" + str);
    if (!file2.exists()) return;           // entity tanpa ikon → tidak ditampilkan
    bmp = BitmapFactory.decodeFile(file2.getAbsolutePath());
    bmp = Bitmap.createScaledBitmap(bmp, 42, 42, true);
    map.put(strConcat, bmp);
}
ImageView iv = new ImageView(context);
iv.setImageBitmap(bmp);
eVar.b.f(iv, ((double) f) / 1048576.0d, ((double) f2) / 1048576.0d);
```

**Pemetaan posisi marker**: koordinat blok entity dibagi `1048576` (`2^20`) menjadi fraksi `-1..1`, lalu `d0/b.java.accept()` menerapkannya:

```java
d3 = x / 1048576.0; d4 = z / 1048576.0;
// d1.a.g == true (mode relatif):
iS = d1.b.s(aVar2.e, (float)((d3 - aVar2.a) / aVar2.c));  // (x+1)/2 → 0..1 → * 2^25
iS3 = d1.b.s(iS, scale);                                    // scale ke TileView
gVar.scrollTo(iS3 - gVar.getHalfWidth(), ... - gVar.getHalfHeight());
```

Jadi fraksi `-1..1` dinormalisasi `(x+1)/2` menjadi `0..1`, lalu dikalikan `e=f=2^25` dan di-scale — memusatkan peta ke entity bila `Bundle == null`. Bila `Bundle != null` (kasus marker), `eVar.b.f(imageView, ...)` langsung menempatkan marker tanpa scroll.

`d0/b.java` juga mengambil drawable `2131231235`, di-scale ke 28×28, dipakai sebagai placeholder bila ikon entity tidak ada.

### 1.7 Dialog hasil (`onResult`, baris 316–406)

| Tag dialog | Aksi |
|---|---|
| `mapview_options_dialog` → `"View Entities"` | `floorDiv(worldX,16)`, `floorDiv(worldZ,16)` → `Intent(EntitiesNBTActivity)` dengan extra `chunkX`, `chunkZ` |
| → `"Take Picture"` | Hitung `w = right-left`, `h = bottom-top`; tolak bila ≤0 (`"No selection made."`) atau `2048/max(w,h) < 1` (`"Selection is too large."`). Buka `z.l` SliderDialog: `MIN=1`, `INITIAL=1`, `MAX=max(min(iMax,24)-1,1)`, title `"Take Picture"`, pesan scale blok/piksel |
| → `"Begin Selection"` | `iRound = round((min(lebar,tinggi)/4) / (scale*16))`; `h.set(wx-iRound, wz-iRound, wx+iRound, wz+iRound)`, `n=true`, VISIBLE |
| → `"End Selection"` | `n=false`, GONE, requestLayout |
| `mapview_picture_dialog` | Cek `i == -1` (OK) dan `SliderDialog.VALUE`; untuk SDK ≤28 cek izin storage; jalankan worker `"Saving Picture..."` dengan `ContentResolver` (uri dari activity result, request code 14) |
| `mapview_save_picture_dialog` | Jika `error != null` → `"Error saving picture."`, else `"Successfully saved picture."` |

### 1.8 Lifecycle

- `onSaveInstanceState`: `map_save` = `g.onSaveInstanceState()`, `selection_save` = `i.onSaveInstanceState()`.
- `onPause`: hapus pesan handler `0`, set semua `c1.c.g = null` (lepas bitmap), `setWillNotDraw(true)`.
- `onResume`: `setWillNotDraw(false)`, `h()`, `y.d(v.m)`, `g()`, `requestLayout()`.
- `onDestroy`: `((com.qozix.tileview.c) b).y.a()` = shutdown thread pool tile.
- `c(String, Canvas)` statis: menulis koordinat dengan `StaticLayout` putih 24 sp, translate (5,5).

---

## 2. Peran `d0/a, b, g, i, f, h` (ringkasan tabel)

| File | Peran | Method kunci / listener |
|---|---|---|
| `d0/e.java` | Fragment utama Show Map, orkestrasi semua komponen | `onCreateView`, `b(s.a)` (marker), `onResult`, `c(String,Canvas)` |
| `d0/a.java` | `f` + `e1.a` — **BitmapProvider** sekaligus **gesture callback** | `a(i1.a,Context)` render 256×256 bitmap 16×16 chunk; `b(MotionEvent)` tap→dialog koordinat |
| `d0/b.java` | `java.util.function.Consumer<u.a>` — penempatan marker/ikon | `accept(u.a)` pasang `ImageView` 28px / 42px; `post(Runnable)` scroll ke entity bila tidak ada Bundle |
| `d0/g.java` | Subclass TileView kustom | `F` = long-press listener (`f`), `G` = WeakReference selection view, `H` = touchable; override `onLongPress`, `onSingleTapConfirmed`, `onScrollChanged`, `onTouchEvent`; setter `setOnLongPressListener`, `setSelectionView`, `setTouchable` |
| `d0/i.java` | Overlay seleksi (FrameLayout) | `getSelection()`, `onDraw`, `onLayout`, `onSave/RestoreInstanceState`, `onViewAdded`, `setTileView`, stub `setSelectionChangedListener` |
| `d0/f.java` | Interface marker (kosong, 5 baris) | Dipakai sebagai tipe `F` di `g.java`, di-cast ke `a` |
| `d0/h.java` | Interface marker untuk selection changed (kosong, 5 baris) | Dipakai `setSelectionChangedListener(@Nullable h)` (stub) |

---

## 3. Paket `t` — tabel referensi, enum, dan model dunia

### 3.1 `t/c.java` — enum **BLOCK** (118.205 byte, 1.110 entri)

Struktur source JADX:

```java
public enum c {
    e(198, 252, "BLOCK_0_PINK_TULIP", "Pink Tulip", "minecraft:pink_tulip"),
    f(96, 186, "BLOCK_1_BUDDING_AMETHYST", "Budding Amethyst", "minecraft:budding_amethyst"),
    ...
    Pl(125, 125, "BLOCK_1180_DOUBLE_STONE_BLOCK_SLAB", "Double Stone Block Slab",
                  "minecraft:double_stone_block_slab");

    public static final Map Ql = Arrays.stream(values())
        .collect(Collectors.toMap(new i0.a(1), new i0.a(2)));  // identifier → enum
    public final String b;  // display name ("Pink Tulip")
    public final String c;  // identifier ("minecraft:pink_tulip")
    public final int d;     // warna ARGB

    c(int i10, int i11, String str, String str2, String str3) {
        this.b = str2; this.c = str3; this.d = Color.rgb(i, i10, i11);
    }
}
```

| Field | Isi | Contoh |
|---|---|---|
| `b` | Nama tampil | `"Nether Gold Ore"`, `"White Wool"` |
| `c` | Namespaced ID Minecraft | `"minecraft:nether_gold_ore"` |
| `d` | `Color.rgb(r,g,b)` | lihat catatan di bawah |
| `Ql` | Map statis `identifier → enum` | dipakai pencarian cepat |

**Artefak dekompilasi penting (terverifikasi dari smali `smali_classes9/t/c.smali`):**

```smali
.method public constructor <init>(IIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
    invoke-static {p2, p3, p4}, Landroid/graphics/Color;->rgb(III)I
```

Konstruktor aslinya punya **7 parameter**: `p1`=ordinal enum, `p2`=R, `p3`=G, `p4`=B, `p5`=nama internal (`BLOCK_n_...`), `p6`=display name, `p7`=identifier. Contoh entri `e` di smali: `p1=0, p2=0xEB(235), p3=0xC6(198), p4=0xFC(252)` → `Color.rgb(235,198,252)` = pink. **JADX menghilangkan ordinal dan komponen merah**, sehingga di source hanya tampak `(198, 252, ...)`; variabel `i` pada `Color.rgb(i, i10, i11)` adalah sisa komponen merah yang "hilang" saat dekompilasi. Jadi warna asli harus diambil dari smali, bukan dari angka yang tampak di `.java`.

 fakta menarik lain: field-field enum di-obfuskasi (`e, f, g, ..., f347q, f348r, A, B, ..., Ql`), ada ~71 "Fake field, exist only in values array" (JADX menandai entri duplikat nama), array `values()` berukuran `0x49d = 1181`.

Contoh entri terpilih (ID = angka pada `BLOCK_<ID>_`):

| ID | Field | Nama | Identifier |
|---|---|---|---|
| 0 | `e` | Pink Tulip | `minecraft:pink_tulip` |
| 426 | `o7` | Bedrock | `minecraft:bedrock` |
| 635 | `pb` | Dirt | `minecraft:dirt` |
| 662 | `Qb` | Grass Block | `minecraft:grass_block` |
| 711 | `Nc` | Netherrack | `minecraft:netherrack` |
| 1005 | `wi` | Red Nether Brick | `minecraft:red_nether_brick` |
| 1125 | `Mk` | Air | `minecraft:air` |
| 1180 | `Pl` | Double Stone Block Slab | `minecraft:double_stone_block_slab` |

`Mk` (Air, warna `Color.rgb(?,255,255)`) dipakai sebagai layer pengisi di `d1.b.g()` saat membuat world datar. `Qb/pb/o7` (grass/dirt/bedrock) adalah layer default `CreateWorldActivity.onCreate()` (baris 99–101). Beberapa konstanta numerik diganti JADX dengan konstanta ExoPlayer (`TsExtractor.TS_STREAM_TYPE_HDMV_DTS = 129` untuk `BLOCK_1142_FENCE`, dll.) — murni artefak substitusi literal, bukan makna fungsional.

**Pemakaian**: `z/i.java` (adapter block) menampilkan `cVar.b`, `cVar.c`, dan mewarnai `CardView` dengan `ColorUtils.blendARGB(-1, cVar.d, 0.3f)`. `CreateWorldActivity` melakukan hal sama untuk layer. `t/j.f()` memakai `t.c` tidak langsung.

### 3.2 `t/a.java` — enum **BIOME** (87 entri, 191 baris)

```java
public enum a {
    OCEAN("Ocean", "minecraft:ocean", 0),
    PLAINS("Plains", "minecraft:plains", 1),
    DESERT("Desert", "minecraft:desert", 2),
    EXTREME_HILLS("Windswept Hills", "minecraft:extreme_hills", 3),
    FOREST_HILLS("Forest", "minecraft:forest", 4),
    TAIGA("Taiga", "minecraft:taiga", 5),
    SWAMPLAND("Swamp", "minecraft:swampland", 6),
    RIVER("River", "minecraft:river", 7),
    HELL("Nether Wastes", "minecraft:hell", 8),
    THE_END("The End", "minecraft:the_end", 9),
    ...
    MANGROVE_SWAMP("Mangrove Swamp", "minecraft:mangrove_swamp", 191),
    PALE_GARDEN("Pale Garden", "minecraft:pale_garden", 193),
    WARPED_FOREST("Warped Forest", "minecraft:warped_forest", 180);
    public final String b, c; public final int d; // display, identifier, biome ID
}
```

- Field `b` = nama tampil (sering sudah diganti ke nama modern: `"Snowy Plains"`, `"Badlands"`, `"Old Growth Birch Forest"`).
- Field `c` = ID lama `minecraft:*`.
- Field `d` = **numeric biome ID** (0..193), bukan ordinal. Contoh: `CHERRY_GROVE` = `PsExtractor.AUDIO_STREAM` = 177; `STONY_PEAKS` = `PsExtractor.PRIVATE_STREAM_1` = 188; `SUNFLOWER_PLAINS` = `TS_STREAM_TYPE_AC3` = 129 — lagi-lagi substitusi konstanta JADX.
- Ada **nama field duplikat** (JADX "Fake field"): `DEEP_DARK` muncul dua kali (45 dan 190), `FOREST_HILLS` 4 dan 18, `LUKEWARM_OCEAN` 10 dan 42. `t.a.values()` berisi keduanya; ordinal dipakai sebagai indeks penyimpanan (`CreateWorldActivity` menyimpan `aVar.ordinal()` ke bundle `selectedBiome`, `z/d.java` memakai `aVar.d` sebagai ID).

### 3.3 `t/f.java` — enum **ENTITY ICON** (123 entri, 264 baris)

```java
ENTITY_0_Zombie_Villager_V2("minecraft:zombie_villager_v2", "zombie_villager.png"),
...
ENTITY_122_Area_Effect_Cloud("minecraft:area_effect_cloud", null);

public static final Map d = Arrays.stream(values())
    .collect(Collectors.toMap(new i0.a(3), new i0.a(4)));  // identifier → enum
public final String b;  // identifier
public final String c;  // nama file ikon (nullable)
```

Statistik:

| Kategori | Jumlah |
|---|---|
| Total entri | 123 |
| Punya ikon (`.png`) | 83 |
| Ikon `null` (proyektil, minecart, dll.) | 40 |
| File PNG di `assets/entityIcons/` | 82 (+ 1 `credits.txt`) |

Beberapa pemetaan khusus: `minecraft:zombie_pigman → zombified_piglin.png`, `minecraft:happy_ghast → ghast.png`, `minecraft:tropicalfish → tropical_fish.png`, `minecraft:silverfish → silver_fish.png`, `minecraft:villager_v2` dan `minecraft:villager` keduanya → `villager.png`, `minecraft:evocation_illager` → `null` (padahal ada `evoker.png` — tidak dipetakan). Icon dipakai di dua tempat: `d0/e.b()` (marker peta, di-scale 42×42, cache key `nama+"_42"`) dan `EntitiesNBTActivity.a()` (daftar entity chunk, ikon asli dari cache).

### 3.4 Helper `t/b, d, e, g, h, i`

| File | Peran | Detail |
|---|---|---|
| `t/b.java` | Model seleksi layer (8 baris) | `String a = ""`, `u.a b = new u.a(0,0,0)` (min), `u.a c = (0,0,0)` (max) — dipakai `s/a.java:d()` sebagai `bVar` untuk shade warna per blok (lihat `i2=i6*i3+i2`, `iE` = height tetangga, `e0.a.a(bVar, height, iE, iE2)`) |
| `t/d.java` | `LruCache` 256 chunk | `create(Object)` → `new s.a(e.a, e.b, jVar, e.c)`; `WeakReference<t.j>` agar world bisa di-GC |
| `t/e.java` | Key cache chunk | `(x, z, dimension)`; `equals`/`hashCode` memakai `s.b.b(i3)` (normifikasi dimension), menolak `c==0` |
| `t/g.java` | `ToIntFunction<t.h>` | `applyAsInt → h.b` = ketebalan layer; dipakai `mapToInt(new g()).sum()` di `CreateWorldActivity` (limit 384/512 blok) dan `d1.b.g()` |
| `t/h.java` | Pasangan `t.c` (block) + `int b` (ketebalan) | Dipakai RecyclerView layer, drag-drop via `x/d.java` |
| `t/i.java` | `SimpleFileVisitor` penghitung ukuran | `visitFile` → `AtomicLong.addAndGet(basicFileAttributes.size())`; dipanggil `MainActivity.a()` → `Files.walkFileTree(worldPath, new i(atomicLong))` → `Formatter.formatFileSize()` |

### 3.5 `t/j.java` — **model dunia / WorldInfo** (249 baris)

Ini kelas yang sebenarnya memegang data world (bukan paket `z`):

```java
public final class j {
    public LevelDB a;                              // handle DB
    public final j1.a b;                           // cache chunk (via t.d)
    public final File c;                           // folder world
    public final ReentrantReadWriteLock d = new ReentrantReadWriteLock();
    public j(File file) { this.c = file;
        j1.a aVar = new j1.a(); aVar.a = new d(this); this.b = aVar; }
}
```

| Method | Baris | Fungsi | Lock |
|---|---|---|---|
| `a()` | 38–52 | Tutup LevelDB (`levelDB.a()`), exception `r.a` diabaikan | write |
| `b(int,int,int)` | 54–64 | Ambil chunk `s.a` dari cache: `((d) b.a).get(new e(x,z,dim))` | read |
| `c(byte[])` | 66–74 | `levelDB.b(key)` → nilai NBT/byte[] mentah | read |
| `d()` | 76–89 | Nama world dari `levelname.txt`; fallback `"Unnamed World"` | read |
| `e()` | 91–107 | Daftar player: `levelDB.c()` → tiap key UTF-8 | read |
| `f(l)` | 109–142 | Spawn dari `level.dat`: `SpawnX/Y/Z`. Jika `Y > 320`, hitung tinggi permukaan `b(cx,cz,1).e(x%16,z%16,null)+1` (fallback tetap 32767 bila gagal) → `new u.a(x,y,z)` | read |
| `g()` | 144–152 | `c.getName()` (nama folder internal) | read |
| `h()` | 154–163 | Decode `world_icon.jpeg` → Bitmap | read |
| `i()` | 165–173 | Cek `world_icon.jpeg` exists | read |
| **`j()`** | **175–210** | **Deteksi world terenkripsi** | read |
| `k()` | 212–223 | Buka DB: `new LevelDB().d(new File(c,"db").getAbsolutePath())` | write |
| `l()` | 225–233 | Baca `level.dat`: `d1.b.o(true, new ByteArrayInputStream(Files.readAllBytes(...)))` → `y2.l` (NBT root) | read |
| `m(l)` | 235–248 | Tulis `level.dat`: header 8 byte little-endian + payload NBT | read (sic) |

**Penulisan `level.dat` (method `m`)** — format khas Bedrock:

```java
byte[] bArrH = lVar.h();                       // NBT root, already little-endian
ByteBuffer.allocate(bArrH.length + 8)
    .order(ByteOrder.LITTLE_ENDIAN)
    .putInt(8)                                 // header size
    .putInt(bArrH.length)                      // payload size
    .put(bArrH)
    .array();
new FileOutputStream(new File(c, "level.dat")).write(bArrArray);
```

Pembacaan (`d1.b.o(true, stream)`) melakukan `skip(8L)` dan menolak kalau kurang (`"No header found."`), lalu `l.k(stream, -1)` = parser NBT Bedrock. Ini cocok dengan hasil inspeksi template: header `(10, 2969)` untuk file 2977 byte.

---

## 4. Paket `z` — tiga keluarga dialog (12 file, semua terdokumentasi)

### 4.1 Keluarga pemilih BIOME

| File | Peran | Method kunci |
|---|---|---|
| `z/e.java` | `CustomViewDialog<e>`, `TAG="BiomeSelectionDialog."` | `onCreateContentView`: `LinearLayout` vertikal + `SearchView` ("Search biomes...") + `RecyclerView` `LinearLayoutManager`; adapter `new d(t.a.values())`; `searchView.setOnQueryTextListener(new a(this))`; `b.l = new w0.a(this)` (callback hasil→Bundle `BiomeSelectionDialog.SELECTED_BIOME`) |
| `z/a.java` | `SearchView.OnQueryTextListener` | Filter `aVar.b` (nama) **atau** `aVar.c` (identifier) dengan `toLowerCase().contains(lowerCase)`; `DiffUtil.calculateDiff(new b(...))`; `k.clear(); k.addAll(hasil); dispatchUpdatesTo(d)`; `onQueryTextSubmit` return false |
| `z/b.java` | `DiffUtil.Callback` | `areItemsTheSame` bandingkan `t.a.c` (identifier); `areContentsTheSame` selalu true; ukuran = `d.k` vs list baru |
| `z/d.java` | `RecyclerView.Adapter` | `j` = semua biome, `k` = hasil filter; `onBindViewHolder`: `m.setText(aVar.b)`, `n.setText(String.format("%s (%d)", aVar.c, aVar.d))`, item click → `new r(this, aVar, 13)`; layout `2131558606` |
| `z/c.java` | `RecyclerView.ViewHolder` | `m` = `findViewById(2131362034)`, `n` = `findViewById(2131362033)` |

### 4.2 Keluarga pemilih BLOCK

| File | Peran | Method kunci |
|---|---|---|
| `z/j.java` | `CustomViewDialog<j>`, `TAG="BlockSelectionDialog."` | Sama dengan `z/e`, tapi `SearchView` hint "Search blocks..." dan adapter `new i(t.c.values())`; hasil → `BlockSelectionDialog.SELECTED_BLOCK` |
| `z/f.java` | Search listener | Filter `t.c.c` (identifier) atau `t.c.b` (nama); `DiffUtil` via `new g(iVar, list)` |
| `z/g.java` | DiffUtil callback | Bandingkan `t.c.c` |
| `z/i.java` | Adapter | `hVar.m.setText(cVar.b)`, `hVar.n.setText(cVar.c)`; `CardView.setCardBackgroundColor(ColorUtils.blendARGB(-1, cVar.d, 0.3f))`; click → `new r(this, cVar, 14)`; layout `2131558926` |
| `z/h.java` | ViewHolder | `m`=2131362693, `n`=2131362692, `ImageButton` 2131362167 disembunyikan (GONE) |

Kedua dialog dipanggil di `CreateWorldActivity`: `new e().pos("CANCEL").show(...,"pick_biome_dialog")` dan `new j()...("pick_block_dialog")`. Hasilnya memakai `t.a.values()[ordinal]` / `t.c.values()[ordinal]`.

### 4.3 Keluarga Slider

`z/l.java` (`TAG="SliderDialog."`) — dialog seekbar horizontal dengan padding 40, gravity center:

```java
this.b.setMax(getArgs().getInt("SliderDialog.MAX", 1) - i);   // i = MIN
int i2 = getArgs().getInt("SliderDialog.INITIAL", 0);
if (bundle != null) i2 = bundle.getInt("SliderDialog.VALUE", i2);
this.b.setProgress(i2 - i);
this.c.setText(String.valueOf(i2));
this.b.setOnSeekBarChangeListener(new k(this, i));
```

`z/k.java` — `onProgressChanged` menulis `String.valueOf(i + a)` ke TextView (a = MIN), `onStartTrackingTouch`/`onStopTrackingTouch` kosong.
`onResult(i)` mengembalikan Bundle `SliderDialog.VALUE = progress + MIN`; `onSaveInstanceState` menyimpan nilai yang sama. Dipakai oleh `d0/e.java` untuk "Take Picture" (MIN=1, INITIAL=1, MAX≤24) dan oleh `CreateWorldActivity` untuk ketebalan layer (`set_layer_thickness_dialog`, divalidasi > 0 di `validate()`).

### 4.4 Koreksi: deteksi world terenkripsi ada di `t/j.j()` — penjelasan mendalam

```java
public final boolean j() {
    readLock.lock();
    try {
        File[] fileArrListFiles = new File(this.c, "db").listFiles((FilenameFilter) new s0.l(2));
        if (fileArrListFiles != null && fileArrListFiles.length != 0) {
            for (File file : fileArrListFiles) {
                DataInputStream in = new DataInputStream(
                    new BufferedInputStream(Files.newInputStream(file.toPath())));
                try {
                    if (file.length() > 8) {
                        int first  = Integer.reverseBytes(in.readInt());
                        int second = Integer.reverseBytes(in.readInt());
                        if (first == 0 && second == -1680885252) {
                            in.close();
                            readLock.unlock();
                            return true;                 // ← TERENKRIPSI
                        }
                    }
                    in.close();
                } catch (Throwable th) { ...in.close(); throw th; }
            }
        }
        return false;
    } catch (Throwable th3) { readLock.unlock(); throw th3; }
}
```

Penjelasan baris demi baris:

1. **Objek yang diperiksa**: bukan `level.dat`, melainkan **setiap file di dalam folder `db/`** (file LevelDB: `*.ldb`, `LOG`, `MANIFEST-*`, dll.). Filter `s0.l(2)` membatasi kandidat (JADX tidak membongkar isinya, namun pola ini dipakai juga di `MainActivity.l()` untuk filter folder world).
2. **`file.length() > 8`**: syarat minimal 2 int32 (8 byte) agar `readInt()` dua kali tidak EOF.
3. **`DataInputStream.readInt()` membaca BIG-ENDIAN** (format Jawa). File LevelDB/Minecraft ditulis **LITTLE-ENDIAN**. `Integer.reverseBytes(...)` melakukan swap byte secara eksplisit: `reverseBytes(0x0A000000) = 0x0000000A`. Jadi kedua nilai dibaca sebagai int32 little-endian.
4. **Magic number**: `first == 0`, `second == -1680885252`. Nilai `-1680885252` = `0x9BCFB9FC` (unsigned); **byte little-endiannya adalah `FC B9 CF 9B`**. Jadi 8 byte pertama file adalah:
   ```
   offset 0: 00 00 00 00        → int32 LE = 0
   offset 4: FC B9 CF 9B        → int32 LE = 0x9BCFB9FC = -1680885252
   ```
   Ini adalah **header/signature file database Minecraft Bedrock yang dienkripsi** (world dari Marketplace). File DB normal diawali header LevelDB lain (mis. `LOG` diawali teks, `*.ldb` diawaliblok data), bukan pasangan ini.
5. **Alasan desain**: aplikasi ini memodifikasi NBT langsung di LevelDB (chunk, entity, biome). Bila datanya terenkripsi, semua kunci dan nilai tidak bisa dibaca/ditulis dengan JNI `leveldb` biasa. Lebih baik **ditolak lebih awal** daripada menghasilkan world korup.
6. **Konsekuensi di UI** (`MainActivity.onResult`, kasus `"Edit"`, baris 399–452):

```java
if (!jVar.j()) {
    // lanjut: k() buka DB, simpan ke x.b.a, buka DrawerActivity
} else {
    SimpleDialog.build()
        .title("Encrypted World")
        .msg("This world is encrypted and cannot be edited. Any worlds downloaded "
           + "from Minecraft Marketplace are usually encrypted by default.")
        .show(this);
}
```

Catatan kecil: method ini me-unlock readLock secara manual di dalam loop (`readLock.unlock()` sebelum `return true`) dan juga di `finally`; karena `ReentrantReadWriteLock` tidak reentrant antar-unlock berlebih, `return true` terjadi tepat setelah unlock tunggal — aman, tetapi pola ini rapuh terhadap exception di `in.close()`.

---

## 5. `x/b.java`, `x/d.java` (dan `x/a.java`)

### 5.1 `x/b.java` — world context singleton (13 baris)

```java
package x;
import java.util.concurrent.atomic.AtomicReference;
import t.j;

public abstract class b {
    public static final AtomicReference a = new AtomicReference(null);  // mentah, tanpa generic
    public static j a() { return (j) a.get(); }
}
```

- Satu-satunya "world yang sedang diedit" disimpan di `static AtomicReference` (thread-safe visibility, bukan CAS khusus).
- **Penulisan** terjadi di `MainActivity.onResult` → `"Edit"` / progress dialog: `x.b.a.set(jVar)` setelah `jVar.k()` (buka DB). **Pembacaan** di:
  - `DrawerActivity.onCreate` (baris 90): kalau null → redirect ke `MainActivity` + `finish()`.
  - `DrawerActivity.onResult` → `"close_world_dialog"`: `x.b.a().a()` (tutup DB) lalu `finish()`.
  - Header navigasi: `x.b.a().i()` / `x.b.a().h()` untuk ikon world.
  - `d0/a.a()` & `EntitiesNBTActivity`: `x.b.a().b(cx,cz,1)` untuk chunk.
- Tidak ada method `set()` di kelas ini — pemanggilan `x.b.a.set(...)` langsung mengakses field publik.

### 5.2 `x/d.java` — ItemTouchHelper (bukan world context)

```java
public final class d extends ItemTouchHelper.Callback {
    public final CreateWorldActivity a;
    public int getMovementFlags(...) { return makeMovementFlags(3, 0); } // drag UP|DOWN, no swipe
    public boolean isLongPressDragEnabled() { return true; }
    public boolean onMove(RecyclerView rv, ViewHolder from, ViewHolder to) {
        int p1 = from.getBindingAdapterPosition(), p2 = to.getBindingAdapterPosition();
        CreateWorldActivity act = this.a;
        act.o.add(p2, (t.h) act.o.remove(p1));   // pindahkan t.h (block+ketebalan)
        act.n.notifyItemMoved(p1, p2);
        return true;
    }
    public void onSwiped(...) {}                 // tidak ada
}
```

Dipasang di `CreateWorldActivity.onCreate`: `new ItemTouchHelper(new d(this)).attachToRecyclerView(recyclerView)`. Ini mengatur **urutan layer flat world** (bedrock → dirt → grass), yang penting karena JSON `FlatWorldLayers` disusun setelah `Collections.reverse(arrayList2)` di `d1.b.g()`.

### 5.3 `x/a.java` (tambahan)

`ApdInitializationCallback` untuk Appodeal: bila tidak ada error → `d1.b.a = true` dan pasang `e0.a()` sebagai interstitial callback; error hanya dicetak ke stderr. Menjelaskan why `MainActivity.onCreate()` mengecek `if (d1.b.a) return;` sebelum inisialisasi iklan.

---

## 6. `LevelDB.java` — wrapper JNI (79 baris)

```java
package com.hextetrahedral.leveldb;
public class LevelDB {
    public long a;                              // native handle (pointer)
    static { System.loadLibrary("leveldb"); }   // libleveldb.so dari config.arm64_v8a.apk

    private static native void     nativeClose(long j) throws Exception;
    private static native byte[]   nativeGet(long j, byte[] key) throws Exception;
    private static native byte[][] nativeIterateKeys(long j, byte[] prefix) throws Exception;
    private static native long     nativeOpen(String path) throws Exception;
    private static native void     nativePut(long j, byte[] key, byte[] value) throws Exception;
}
```

| Method publik | Baris | Native | Fungsi | Error wrapper (`r.a`) |
|---|---|---|---|---|
| `a()` | 26–36 | `nativeClose` | Tutup DB; hanya jika `a != 0`, lalu set `a=0` | `"Error closing LevelDB"` |
| `b(byte[])` | 38–44 | `nativeGet` | Ambil nilai untuk key (bisa null) | `"Error getting value for key: " + Arrays.toString(key)` |
| `c()` | 46–58 | `nativeIterateKeys(a, "player_server_".getBytes(UTF_8))` | Iterasi **semua key dengan prefix biner `player_server_`**; null → list kosong | `"Error iterating keys with binary prefix"` |
| `d(String)` | 60–70 | `nativeOpen(path)` | Buka DB; handle 0 → error; simpan ke `this.a` | `"Failed to open LevelDB at path: " + path` / `"Error opening LevelDB at path: ... - More info: ..."` |
| `e(byte[],byte[])` | 72–78 | `nativePut` | Tulis key-value | `"Error putting key-value pair: " + Arrays.toString(key)` |

**Path DB**: selalu `<worldFolder>/db`, disusun di `t/j.k()`: `new LevelDB().d(new File(this.c, "db").getAbsolutePath())`.

**Penanganan key**:

- `player_server_<UUID>` → **khusus**, ditangani `LevelDB.c()` (method `c()` di `t/j` mengubahnya jadi String UTF-8 dan dipakai `MainActivity` sebagai daftar player di UI / `DrawerActivity` → `f0.a.f("~local_player")` untuk editor player).
- Key worldOther ditangani di **`DrawerActivity.l(int)`** lewat fragment `f0.a.f(String key)`:

| Menu ID | Key LevelDB | Isi nilai |
|---|---|---|
| 2131363024 | `~local_player` | NBT player lokal |
| 2131363022 | `Overworld` | data dimensi Overworld |
| 2131363025 | `TheEnd` | data End |
| 2131363021 | `Nether` | data Nether |
| 2131363013 | `BiomeData` | biome per chunk |
| 2131363023 | `portals` | portal |
| 2131363012 | `AutonomousEntities` | entity otonom |
| 2131363015 | *custom* (SimpleInputDialog) | key bebas, hint `"e.g. ~local_player"` |

- Key **chunk** tidak lewat `LevelDB.b()` langsung, tetapi lewat `s.a.a(...)` yang menyusun key biner:

```java
// s/a.java:a(x, z, subChunkId, y, withY)
// dimension == 1 (Overworld): key = int32LE(x) | int32LE(z) | byte(subchunkId) [| byte(y)]
// dimension != 1:             key = int32LE(x) | int32LE(z) | int32LE(dimensionMap) | byte(id) [| byte(y)]
```

`d1.b.i(int)` = encode int32 little-endian manual `(byte)i, i>>8, i>>16, i>>24`; `d1.b.j(b3,b4,b5,b6)` = decode. Contoh:

| Kunci | Prefix | Kegunaan |
|---|---|---|
| `dig` + `x` + `z` [+ dim] | `{100,105,103}` = ASCII "dig" | heightmap 256×2 byte (`s/a.java:e`) |
| `5` + `x` + `z` [+ dim] + `y` | id subchunk 5 | blok subchunk 16×16×16, versi 1/8/9 (`s/a.java:e`) |
| `"actorprefix"` + 8 byte UUID | `{97,99,116,111,114,112,114,101,102,105,120}` | daftar entity chunk (`s/a.java:c`) — nilai dibaca per 8 byte sebagai suffix key |
| `player_server_*` | string | NBT player |

Subchunk parser di `s/a.java:e()` menolak versi di luar `{1,8,9}` (`"Unknown or corrupted subchunk version"`), jumlah storage area selain 1/2, dan persistence version ganjil — bukti implementing langsung format Bedrock v1.21.x (sesuai teks About: "Tested with Minecraft 1.21.x").

---

## 7. Aset aplikasi referensi

### 7.1 Daftar `assets/worldTemplate/` (hanya 2 file)

| File | Ukuran | Isi |
|---|---|---|
| `levelname.txt` | 27 byte | `MineTuner Custom Flat World` (tanpa newline) |
| `level.dat` | 2.977 byte | Template NBT world datar (lihat 7.2) |

Keduanya disalin oleh `d1.b.g()` saat membuat world baru:

```java
c(assets, "worldTemplate/level.dat",     new File(file2, "level.dat"));
c(assets, "worldTemplate/levelname.txt", new File(file2, "levelname.txt"));
```

### 7.2 Struktur `level.dat` (hasil parse NBT dengan Python)

**Header 8 byte little-endian**:

```
0a 00 00 00   → version   = 10
99 0b 00 00   → payload   = 2969 byte
```
`8 + 2969 = 2977` = ukuran file. Sesuai `ByteBuffer.putInt(8).putInt(len)` di `t/j.m()`.

Sisanya adalah **TAG_Compound root bernama ""**, berisi **114 tag**. Penting: **NBT Bedrock menggunakan little-endian** (string length `uint16 LE`, `int32 LE`, `float LE`, dll.), beda dengan NBT Java yang big-endian — ini yang menyebabkan parser pertama saya gagal sebelum dikoreksi.

Contoh tag (tipe: 1=Byte, 3=Int, 4=Long, 5=Float, 8=String, 9=List, 10=Compound):

| Tipe | Nama | Nilai |
|---|---|---|
| 8 | `BiomeOverride` | `minecraft:minecraft:` |
| 1 | `CenterMapsToOrigin` | 0 |
| 3 | `Difficulty` | 2 |
| 8 | `FlatWorldLayers` | `{"biome_id":1,"block_layers":[...],"encoding_version":6,"preset_id":"ClassicFlat","world_version":"version.post_1_18"}` |
| 8 | `InventoryVersion` | `1.21.101` |
| 3 | `GameType` / `Generator` | 1 / 2 |
| 4 | `LastPlayed` | 1758227673 |
| 8 | `LevelName` | `MineTuner Custom Flat World` |
| 3 | `LimitedWorldOriginY` | 32767 (= `Short.MAX_VALUE`, menandakan tidak ada batas) |
| 9 | `MinimumCompatibleClientVersion` | `[1,21,100,0,0]` |
| 4 | `RandomSeed` | -6886543781138396986 |
| 3 | `SpawnX/Y/Z` | 0 / 32767 / 0 |
| 3 | `StorageVersion` | 10 |
| 9 | `lastOpenedWithVersion` | `[1,21,101,1,0]` |
| 10 | `abilities` | `attackmobs`, `build`, `flySpeed`, `walkSpeed`, ... (byte 0/1, float) |
| 10 | `experiments` | `experiments_ever_used=0`, `saved_with_toggled_experiments=0` |
| 10 | `world_policies` | `{}` (kosong) |
| 8 | `baseGameVersion` | `*` |
| 3 | `NetherScale` | 8 |

**Modifikasi oleh aplikasi** (`d1.b.g()` baris 304–327): `lVarL = jVar.l()` → `FlatWorldLayers` dicari; bila ada, `((l)optional.get()).g().c = strJSON` (field value string langsung ditimpa); bila tidak ada, ditambah `new k("FlatWorldLayers", strJSON)`. JSON dibuat `d1.b.a(aVar, arrayList2)`:

```java
{
  "biome_id": <t.a.d>,                 // ID biome numerik
  "block_layers": [{"block_name":"minecraft:bedrock","count":1}, ...],
  "encoding_version": 6,
  "structure_options": null,
  "world_version": "version.post_1_18"
}
```

Kemudian `jVar.m(lVarL)` menulis ulang seluruh file dengan header 8 byte.

### 7.3 `assets/entityIcons/`

- **83 file**: 82 PNG + `credits.txt`.
- Isi credits (tanpa newline): `https://github.com/Simplexity-Development/Entity-Icons`.
- Nama file: `zombie.png`, `creeper.png`, `pillager.png`, `ender_dragon.png`, `wither_skull.png`, `silver_fish.png`, `tropical_fish.png`, dll.
- Diekstrak ke `cacheDir/entityIcons/` oleh `d1.b.d(AssetManager, "entityIcons", File)` (rekursif: `assetManager.list(path)`, kalau bukan folder → `c()` copy stream). Dipanggil dari `d0/e.b()` (marker peta) dan tidak langsung oleh `EntitiesNBTActivity` (yang mengandalkan cache sudah ada).
- Mapping identifier ↔ file ada di `t/f.java` (123 identifier, 83 terpetakan ke PNG, 40 `null`).

### 7.4 `assets/texture_pack.mcpack` (15.546 byte)

Side-load resource pack agar tombol "Export world" muncul di Minecraft. Isi ZIP (5 entri):

```
manifest.json                                  620 byte
pack_icon.png                                16.459 byte
ui/                                              0 (folder)
ui/settings_sections/                            0 (folder)
ui/settings_sections/world_section.json         70 byte
```

`manifest.json`:

```json
{
  "format_version": 2,
  "header": {
    "name": "World Export Button",
    "description": "Enables the button for exporting worlds.",
    "uuid": "4ae8ec5a-e350-4b48-ac64-77d8140007a4",
    "version": [1,0,0], "min_engine_version": [1,16,0]
  },
  "modules": [{"type":"resources","uuid":"242abf67-0aed-4c89-992a-c75cb4920e52","version":[1,0,0]}],
  "metadata": {"authors":["MineTuner"],
               "url":"play.google.com/store/apps/details?id=com.hextetrahedral.minetuner"}
}
```

Ditulis ke file oleh `r2/a.java` case 10 (`HelpActivity` → `assets.open("texture_pack.mcpack")` → `Files.newOutputStream`).

Aset lain di root `assets/` (untuk kelengkapan): `ad.html`, `a.json`, `android_bridge.js`, `aps_*`, `audience_network.dex`, `builddatas.json`, `dic`, `dsa_page.html`, `dtb-m.js`, `favicon.ico` (0 byte), `fyb_*`, `ia_js_load_monitor.txt`, `ia_mraid_bridge.txt`, `mbridge_download_dialog_view.xml`, `mraid*.js`, `omsdk-v1.js`, `pob_*.svg`, `PubMatic_Logo.svg`, `rv_binddatas.xml`, `taurusx_mraid.js`, `ui.js` — semuanya aset iklan (Appodeal/Unity/Fyber/Moloco/dll.), bukan data Minecraft.

---

## 8. Alur import `.mcworld` / `.mctemplate`

### 8.1 Ringkasan alur end-to-end

```
MainActivity: tombol Import
   │  SimpleDialog "Import World"
   │  "To import a world, please provide a .mcworld or .mctemplate file."
   │  neg("MCWORLD") / pos("MCTEMPLATE") / neut("CANCEL")
   ▼
onResult("import_world_dialog", i):
   i == -1  → d1.b.m(this, 3)  [MCTEMPLATE]
   i == -2  → d1.b.m(this, 2)  [MCWORLD]
   ▼
d1.b.m(): Intent GET_CONTENT, type */*,
   MIME_TYPES = {application/octet-stream, application/x-mcpack,
                 application/x-mcworld, application/x-mctemplate, application/x-mcaddon},
   category OPENABLE, chooser "Select a .mcworld file",
   startActivityForResult(..., requestCode dari net.pubnative...a.b(i))
   ▼
onActivityResult():
   - baca "_display_name" dari ContentResolver (content://), fallback Uri.getLastPathSegment()
   - validasi str.toLowerCase().endsWith("." + ext) → kalau salah: Toast "Please select a valid .<ext> file"
   - d1.b.q(..., "import_file_progress_dialog", "Importing world...", Runnable)
   ▼
Runnable (v.a):
   File fileL = d1.b.l(context, uri, fileType);   // salin URI → <externalFilesDir>/import.mcworld
   if (requestCode == a.c(i)) → d1.b.r(new androidx.activity.f(ctx, fileL, i5, 18)) // share/open
   else if (requestCode == a.b(i)) → d1.b.n(context, fileL, str2);                  // IMPORT
   ▼
d1.b.n(context, File, String displayName):  ← inti import
   ...
   ▼
onResult("import_file_progress_dialog"): error? Toast gagal : l() (refresh daftar) + d1.b.t(this) (iklan)
```

### 8.2 Detail `d1.b.n()` — zip → folder tujuan, sanitasi, suffix

```java
public static void n(Context context, File file, String str) {
    File file2 = new File(context.getExternalFilesDir(null), "worlds");
    if (!file2.exists() && !file2.mkdirs()) {
        r(new androidx.profileinstaller.c(context, 12)); return;   // error → UI thread
    }
    int iLastIndexOf = str.lastIndexOf(".");
    if (iLastIndexOf != -1) str = str.substring(0, iLastIndexOf);  // ← HANYA "sanitasi": buang ekstensi

    File file3 = new File(file2, str);
    int i = 1;
    while (file3.exists()) {
        file3 = new File(file2, m.f(i, str, "_"));                  // str + "_" + i
        i++;
    }
    if (!file3.exists() && !file3.mkdirs()) {
        r(new androidx.profileinstaller.c(context, 13)); return;
    }
    try {
        ZipFile zipFile = new ZipFile(file);        // net.lingala.zip4j.ZipFile
        try {
            zipFile.extractAll(file3.getAbsolutePath());            // ← ekstrak semua entri
            zipFile.close();
        } catch (Throwable th) { zipFile.close(); throw th; }
    } catch (Exception e) {
        System.err.println("ERROR " + e.getMessage());
        h(file3);                                   // hapus rekursif folder hasil ekstrak
        r(new androidx.profileinstaller.c(context, 14));
    }
}
```

Temuan penting:

1. **Sanitasi nama sangat minimal**: hanya membuang bagian setelah titik terakhir. Tidak ada penghapusan karakter `../`, slash, atau karakter ilegal. Keamanan bergantung pada implementasi `zip4j.extractAll()` (yang secara default memblokir path traversal modern). Karakter aneh dari `_display_name` tetap terbawa ke nama folder.
2. **Suffix tabrakan**: `Nama` → `Nama_1` → `Nama_2` → ... sampai nama belum ada. (`m.f(i, str, "_")` = helper `String` concatenation JADX.) Perilaku identik dengan `d1.b.g()` untuk world baru: `"MineTuner Custom Flat World"` → `"MineTuner Custom Flat World_1"` ...
3. **Tujuan**: selalu `<externalFilesDir>/worlds/` — bukan path Minecraft asli (`Android/data/com.mojang.minecraftpe/...`). World eksternal hanya **dibaca** (lihat `MainActivity.a()`: label `"## [EXTERNAL WORLD] ##"` dan menu tanpa "Open in Minecraft").
4. **Deteksi `level.dat`**: `d1.b.n()` **tidak** memeriksa `level.dat` secara eksplisit. Deteksi terjadi kemudian:
   - `MainActivity.l()` mem-scan `worlds/` dengan filter folder `new d(2)` → `new j(file)` untuk tiap folder.
   - `t.j.d()` membaca `levelname.txt` (fallback `"Unnamed World"`).
   - `t.j.l()` membaca `level.dat` lewat `d1.b.o(true, ...)` (skip header 8 byte) saat world diedit / spawn dihitung.
   - Jadi file `.mcworld` dianggap valid jika hasil ekstrak mengandung struktur world yang bisa dibaca; kalau tidak, error muncul saat `Edit`/render, bukan saat import.
5. **Rollback**: kalau `extractAll()` gagal, `h(file3)` menghapus folder (walk reverse-order) dan progress dialog menerima error code 14.
6. **File sementara**: `d1.b.l()` menyalin Uri ke `<externalFilesDir>/import.<ext>` dengan buffer 4096 byte (`OpenBitSet.PAGE_SIZE`) — dipakai juga untuk ekspor `/export.mcworld`.

### 8.3 Alur ekspor (kebalikan, untuk konteks)

`MainActivity.onResult("select_world_options_dialog" → "Open in Minecraft")` → `export.mcworld` → `r2.a(jVar, file, 9)` (case 9 di `r2/a.java`):

```java
if (file2.exists() && (!file.exists() || file.delete())) {
    ZipFile zipFile = new ZipFile(file);
    if (file2.isDirectory())
        for (File child : file2.listFiles())
            if (child.isDirectory()) zipFile.addFolder(child); else zipFile.addFile(child);
    else zipFile.addFile(file2);
    zipFile.close();
}
```

Perhatian: file di root world (`level.dat`, `levelname.txt`, `world_icon.jpeg`) **tidak ikut ter-zip** bila `file2.isDirectory()` — hanya subfolder (terutama `db/`) yang ditambahkan. Ini kemungkinan besar disengaja karena Minecraft akan membangkitkan ulang metadata saat import, atau merupakan bug yang perlu dicatat. Setelah selesai: interstitial Appodeal bila `now - d1.b.b >= 10000`, lalu `d1.b.r(new androidx.activity.f(this, new File(string9), 2, 18))` untuk membagikan/membuka hasil.

---

## 9. Penutup — peta alur baca/tulis data Minecraft Bedrock

| Lapisan | Kelas | Operasi |
|---|---|---|
| UI world list | `MainActivity`, `y/e,f,g` | scan `minecraftWorlds/` (eksternal, SDK≤28 + izin) & `worlds/` (internal), ukuran via `t.i`, ikon via `t.j.h()` |
| Context | `x.b.a` (AtomicReference) | world aktif tunggal |
| World | `t.j` | baca `levelname.txt`, `world_icon.jpeg`, `level.dat` (skip 8 byte), deteksi enkripsi 8 byte `db/*`, buka/tutup LevelDB, cache chunk LRU 256 |
| Chunk | `s.a` | key biner LE `dig`/subchunk id 5/`actorprefix`, parse NBT entity & subchunk v1/8/9, heightmap, render 256 blok ke Canvas |
| Database | `com.hextetrahedral.leveldb.LevelDB` | JNI `libleveldb.so`: open/get/put/iterate(`player_server_`)/close |
| Peta | `d0.e` + `d0.a/b/g/i` | TileView 2^26 unit ≈ ±2^20 blok, bitmap 256px = 16×16 chunk, marker entity, seleksi, screenshot |
| Referensi data | `t.c` (1110 block), `t.a` (87 biome), `t.f` (123 entity/83 ikon) | warna `Color.rgb` (R tersembunyi di JADX), ID numerik, identifier `minecraft:*` |
| Dialog | `z.a–l` | filter `DiffUtil` + `SearchView`, block/biome selector, slider (MIN/INITIAL/MAX/VALUE) |
| Import/ekspor | `d1.b.l/m/n`, `r2.a` | salin URI → `import.<ext>`, zip4j `extractAll` → `worlds/<nama>[_N]`, zip folder → `export.mcworld` |
| Template | `assets/worldTemplate/*`, `entityIcons/*`, `texture_pack.mcpack` | world datar 1.21.101, ikon entity, resource pack tombol ekspor |

Seluruh berkas yang diminta telah dibaca dan didokumentasikan; penyimpangan dari asumsi awal (paket `z`, lokasi deteksi enkripsi, peran `x/d.java`, penanganan key DB) telah dikoreksi berdasarkan kode aktual dan verifikasi smali/NBT.

---

# Bagian 5 — Manifest, resource, native library & SDK pihak ketiga

# LAPORAN INVENTARISASI TEKNIS LENGKAP
## MineTuner+ 2.2.3 (XAPK) — Manifest, Resources, Native Libraries & Dependencies Pihak Ketiga

**Objek audit:** `/home/kunbu8u7b/NBT-EDITOR-P/MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk`
**Workspace:** `/home/kunbu8u7b/NBT-EDITOR-P/`
**Tanggal inventarisasi:** 23 November 2026
**Metode:** `aapt2 dump xmltree/badging/resources/strings`, `unzip -l`, `sha256sum`, `apksigner verify --print-certs`, inspeksi `apkbase/META-INF/*.properties`, serta verifikasi silang terhadap 5.222 sumber JADX di `.ref_extract/jadx_out/sources/` (+ `jadx_c12`, `jadx_c6`) dan `apktool_out/`.

---

## 0. RINGKASAN EKSEKUTIF

| Dimensi | Temuan utama |
|---|---|
| Identitas | `com.hextetrahedral.minetuner`, versi `2.2.3` (versionCode `16`), label **MineTuner** |
| Target platform | minSdk **26**, targetSdk **36**, compileSdk **36** (Android 16) |
| XAPK | 1 base + 3 split (`arm64_v8a`, `en`, `mdpi`), total 109.767.468 byte |
| Komponen manifest | **177** komponen: 129 activity, 19 provider, 14 service, 15 receiver |
| Aplikasi inti | Hanya **5 activity** + 1 FileProvider; sisanya **172 komponen iklan/SDK/AndroidX** |
| Resources | 527 layout, 6 menu, 954 drawable, 9 mipmap, 742 string, 1.208 color, 1.437 style |
| Tema | `Theme.Minetuner` → `Base.Theme.Minetuner` → `Theme.Material3.Light.NoActionBar`, hijau `#429E46` |
| Native | 12 file `.so` arm64-v8a; `libleveldb.so` adalah jantung editor.world Bedrock |
| DEX | **13** file `classes*.dex` + 1 `assets/audience_network.dex` |
| Monetisasi | ≥ 25 jaringan iklan/SDK terdeteksi (Appodeal/MAX sebagai orkestrator utama) |
| Licensing | **PairIP** (`com.pairip.licensecheck`) memblokir app di emulator tanpa Play Store |

---

## 1. IDENTITAS & BUILD METADATA

### 1.1 Identitas aplikasi (dari `aapt2 dump badging` + xmltree manifest)

| Field | Nilai |
|---|---|
| `package` | `com.hextetrahedral.minetuner` |
| `versionCode` | `16` |
| `versionName` | `2.2.3` |
| `platformBuildVersionName/Code` | `16` / `36` |
| `compileSdkVersion` | `36` (`16`) |
| `minSdkVersion` | `26` (Android 8.0 Oreo) |
| `targetSdkVersion` | `36` (Android 16) |
| `application-label` | `MineTuner` |
| `application-icon` | `res/mipmap-anydpi-v26/ic_launcher.xml` (semua densitas) |
| `launchable-activity` | `com.hextetrahedral.minetuner.MainActivity` |
| `requiredSplitTypes` | `base__abi,base__density` |
| `splitTypes` | kosong (base adalah aplikasi utama) |
| Densitas didukung | 160, 240, 320, 480, 640, 65534 (anydpi), 65535 (nodpi) |
| Layar | small, normal, large, xlarge; `supports-any-density=true` |
| Locale | `--_--` (default; resource bahasa Inggris ada di `config.en.apk`) |
| `extractNativeLibs` | `false` (`.so` dimuat langsung dari APK) |
| `allowBackup` | `true` |
| `hardwareAccelerated` | `true` |
| `supportsRtl` | `true` |
| `networkSecurityConfig` | `@xml/network_security_config` |
| `appComponentFactory` | `androidx.core.app.CoreComponentFactory` |

### 1.2 Hash & ukuran file

| File | SHA-256 | Ukuran (byte) |
|---|---|---|
| `MineTuner+-+NBT+Editor_2.2.3_APKPure.xapk` | `bb1a658a76de7d6d0d9d23bbf756c01c62b2499d8abaa597a6c315b9c1e5148c` | 109.767.468 |
| `com.hextetrahedral.minetuner.apk` (base) | `13c2cf98dd2421ddd8412097d983792528037ff7facd496ee41a17bd3cde649b` | 105.397.608 |
| `config.arm64_v8a.apk` | `defd011b8a421e7c93dbee24d0ed3c6d528c0fab97b658018a81e32374a2dbcd` | 4.048.134 |
| `config.en.apk` | `b3023e9a7af9d6631ceb2aa8067008099d085d2aa6a4355b189edbba1b2e94b5` | 99.054 |
| `config.mdpi.apk` | `aae9fa9de4092d521be6f25d2bcc6e2e78ea33f94754e2bd44443419a19bc544` | 220.643 |

Jumlah keempat APK = **109.765.439 byte**, identik dengan `total_size` di `manifest.json` (selisih 2.029 byte dengan ukuran container XAPK = metadata ZIP).

### 1.3 Struktur `manifest.json` (XAPK v2)

```json
{
  "xapk_version": 2,
  "package_name": "com.hextetrahedral.minetuner",
  "name": "MineTuner",
  "version_code": "16",
  "version_name": "2.2.3",
  "min_sdk_version": "26",
  "target_sdk_version": "36",
  "permissions": [18 entri, identik dengan manifest],
  "split_configs": ["config.mdpi", "config.en", "config.arm64_v8a"],
  "total_size": 109765439,
  "icon": "icon.png",
  "split_apks": [
    {"file":"com.hextetrahedral.minetuner.apk","id":"base"},
    {"file":"config.mdpi.apk","id":"config.mdpi"},
    {"file":"config.en.apk","id":"config.en"},
    {"file":"config.arm64_v8a.apk","id":"config.arm64_v8a"}
  ]
}
```

### 1.4 Tanda tangan (apksigner 36.0.0)

| Item | Nilai |
|---|---|
| Verifikasi | **Verifies**; v1 = false, **v2 = true**, **v3 = true**, v3.1 = false, v4 = false |
| SourceStamp | **true**, timestamp `1762712700` (≈ 9 Nov 2025) |
| Jumlah signer | 1 |
| DN signer | `CN=Android, OU=Android, O=Google Inc., L=Mountain View, ST=California, C=US` |
| SHA-256 sertifikat | `ed6faf5d24b74b9faebce13ebc9277a1245bb4aecefe7df827eece2fbb26818d` |
| SHA-1 sertifikat | `6bbc297f59a8a4ee384a6584efbf24afd97de58a` |
| MD5 sertifikat | `bb9888e228e39d6a6a3295c61b8768ff` |
| Kunci | RSA 4096-bit; public key SHA-256 `a85f63e3e99ad545356131e95b21f7b01756a27c646d8f36135a380ab5b3b15f` |
| SourceStamp signer DN | sama (Google Android), SHA-256 `3257d599a49d2c961a471ca9843f59d341a405884583fc087df4237b733bbd6d` |
| File META-INF | `BNDLTOOL.SF`, `BNDLTOOL.RSA`, `MANIFEST.MF` (bundletool, khas App Bundle → APK set) |

Metadata tambahan: `com.android.vending.splits.required=true`, `com.android.stamp.source=https://play.google.com/store`, `com.android.stamp.type=STAMP_TYPE_DISTRIBUTION_APK`, `com.android.vending.splits=@xml/splits0`, `com.android.vending.derived.apk.id=3`.

---

## 2. MANIFEST LENGKAP

### 2.1 `<uses-permission>` — 18 izin

| # | Izin | Kategori / pemilik |
|---|---|---|
| 1 | `android.permission.INTERNET` | Inti/iklan |
| 2 | `android.permission.ACCESS_NETWORK_STATE` | Inti/iklan |
| 3 | `android.permission.READ_EXTERNAL_STORAGE` | Inti — baca world legacy & ekspor |
| 4 | `android.permission.WRITE_EXTERNAL_STORAGE` | Inti — impor/ekspor world |
| 5 | `com.google.android.gms.permission.AD_ID` | AdMob / GMS |
| 6 | `android.permission.ACCESS_WIFI_STATE` | Iklan / konektivitas |
| 7 | `com.android.vending.BILLING` | Google Play Billing 7.1.1 |
| 8 | `android.permission.VIBRATE` | UI/feedback |
| 9 | `com.applovin.array.apphub.permission.BIND_APPHUB_SERVICE` | AppLovin AppHub |
| 10 | `android.permission.ACCESS_ADSERVICES_TOPICS` | Android Privacy Sandbox Ads |
| 11 | `android.permission.ACCESS_ADSERVICES_ATTRIBUTION` | Android Privacy Sandbox Ads |
| 12 | `android.permission.RECEIVE_BOOT_COMPLETED` | StartApp / WorkManager |
| 13 | `com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE` | Play Install Referrer |
| 14 | `android.permission.ACCESS_ADSERVICES_AD_ID` | Privacy Sandbox Ad ID |
| 15 | `android.permission.WAKE_LOCK` | Iklan / foreground work |
| 16 | `android.permission.FOREGROUND_SERVICE` | Iklan / WorkManager |
| 17 | `com.hextetrahedral.minetuner.DYNAMIC_RECEIVER_NOT_EXPORTED_PERMISSION` | AndroidX Core (protectionLevel `signature`) |
| 18 | `com.android.vending.CHECK_LICENSE` | **PairIP / Google Play Licensing** |

Tidak ada `READ_MEDIA_*`, `POST_NOTIFICATIONS`, `MANAGE_EXTERNAL_STORAGE`, `CAMERA`, atau `RECORD_AUDIO`.

### 2.2 `<queries>` — 21 entri visibilitas paket

| Bentuk | Nilai | Alasan |
|---|---|---|
| intent + scheme | `ACTION_VIEW`, `https` | browser/link iklan |
| package | `com.pubmatic.openwrapapp` | PubMatic OpenWrap |
| package | `com.android.chrome` | Chrome Custom Tabs |
| intent | `android.support.customtabs.action.CustomTabsService` | androidx.browser |
| intent | `com.android.vending.billing.InAppBillingService.BIND` | Billing |
| intent | `com.google.android.apps.play.billingtestcompanion.BillingOverrideService.BIND` | Billing sandbox |
| package | `com.facebook.katana` | Audience Network |
| intent | `com.applovin.am.intent.action.APPHUB_SERVICE` | AppLovin AppHub |
| intent | `VIEW` + scheme `http` / `https` | klik iklan |
| intent | `androidx.browser.customtabs.CustomTabsService` | Custom Tabs |
| intent | `VIEW` + `BROWSABLE` + `https` / `http` | deep link |
| intent | `VIEW` + scheme `market` | Play Store |
| intent | `com.attribution.REFERRAL_PROVIDER` | attribution |
| intent | `ACTION_MAIN` | resolver umum |
| intent | `INSERT` + mime `vnd.android.cursor.dir/event` | kalender (iklan) |
| intent | `VIEW` + scheme `sms`, `DIAL` + path `tel:` | iklan click-through |
| package | `com.google.android.webview`, `com.android.webview` | WebView |
| package | `com.android.vending` | Play |
| intent | `VIEW` | umum |
| intent | `com.digitalturbine.ignite.cl.IgniteRemoteService` | Digital Turbine / DT Exchange |

### 2.3 `<uses-feature>` — 11 entri

Wajib (implied): `android.hardware.faketouch`, `android.hardware.screen.landscape`, `android.hardware.screen.portrait`, `android.hardware.wifi`.
Opsional (`required=false`): `android.hardware.sensor.light`, `.compass`, `.gyroscope`, `.barometer`, `.proximity`.

### 2.4 Atribut `<application>`

`theme=@style/Theme.Minetuner`, `label=@string/app_name`, `icon=@mipmap/ic_launcher`, `roundIcon=@mipmap/ic_launcher_round`, `allowBackup=true`, `hardwareAccelerated=true`, `supportsRtl=true`, `extractNativeLibs=false`, `networkSecurityConfig=@xml/network_security_config`, `appComponentFactory=androidx.core.app.CoreComponentFactory`.

`uses-library` (keduanya `required=false`): `org.apache.http.legacy`, `android.ext.adservices`.

### 2.5 Komponen APLIKASI INTI (5 activity + 1 provider)

| Komponen | exported | Keterangan |
|---|---|---|
| `com.hextetrahedral.minetuner.MainActivity` | **true** | Launcher: `MAIN` + `LAUNCHER`. Daftar world, FAB New/Import, menu Help/About |
| `com.hextetrahedral.minetuner.DrawerActivity` | false | Editor NBT world: DrawerLayout + 12 navigasi |
| `com.hextetrahedral.minetuner.app.HelpActivity` | false | Tutorial ekspor texture-pack (“Exporting Worlds”) |
| `com.hextetrahedral.minetuner.app.CreateWorldActivity` | false | Pembuat flat world |
| `com.hextetrahedral.minetuner.app.EntitiesNBTActivity` | false | Daftar entitas chunk |
| `androidx.core.content.FileProvider` | false | authority `com.hextetrahedral.minetuner.provider`, `grantUriPermissions=true`, `@xml/file_paths` |

Catatan: kelima activity tersebut **tidak memiliki intent-filter selain MainActivity**. Seluruh 172 komponen lain adalah pihak ketiga/AndroidX.

### 2.6 Statistik & komponen yang `exported=true`

Total komponen = **177** (activity 129, provider 19, service 14, receiver 15).

| Komponen exported | Jenis | Pelindung |
|---|---|---|
| `MainActivity` | activity | launcher (disengaja) |
| `com.amazon.device.ads.DTBInterstitialActivity` | activity | — |
| `com.amazon.aps.ads.activity.ApsInterstitialActivity` | activity | — |
| `com.google.android.gms.auth.api.signin.RevocationBoundService` | service | `...permission.REVOCATION_NOTIFICATION` |
| `com.appodeal.ads.AppodealPackageAddedReceiver` | receiver | `PACKAGE_ADDED` |
| `io.appmetrica.analytics.internal.PreloadInfoContentProvider` | provider | authority `...appmetrica.preloadinfo.retail` |
| `com.startapp.sdk.adsbase.remoteconfig.BootCompleteListener` | receiver | `BOOT_COMPLETED` |
| `com.mbridge.msdk.foundation.same.broadcast.NetWorkChangeReceiver` | receiver | `CONNECTIVITY_CHANGE` |
| `com.ogury.core.internal.LogEnablerReceiver` | receiver | `com.ogury.sdk.intent.ENABLE_LOGS` |
| `androidx.work.impl.background.systemjob.SystemJobService` | service | `BIND_JOB_SERVICE` |
| `androidx.work.impl.diagnostics.DiagnosticsReceiver` | receiver | `android.permission.DUMP` |
| `androidx.profileinstaller.ProfileInstallReceiver` | receiver | `android.permission.DUMP` |

### 2.7 Daftar LENGKAP komponen per SDK (177 entri)

**Aplikasi (5):** MainActivity · DrawerActivity · app.HelpActivity · app.CreateWorldActivity · app.EntitiesNBTActivity

**AndroidX / Google Infrastructure (15):** androidx.core.content.FileProvider *(auth `com.hextetrahedral.minetuner.provider`)* · androidx.startup.InitializationProvider *(auth `...androidx-startup`)* · androidx.work.impl.background.systemalarm.SystemAlarmService · androidx.work.impl.background.systemjob.SystemJobService · androidx.work.impl.foreground.SystemForegroundService · androidx.room.MultiInstanceInvalidationService · androidx.work.impl.utils.ForceStopRunnable$BroadcastReceiver · androidx.work.impl.background.systemalarm.ConstraintProxy$BatteryChargingProxy · ...$BatteryNotLowProxy · ...$StorageNotLowProxy · ...$NetworkStateProxy · ...RescheduleReceiver · ...ConstraintProxyUpdateReceiver · androidx.work.impl.diagnostics.DiagnosticsReceiver · androidx.profileinstaller.ProfileInstallReceiver

**Google Mobile Ads / GMS / Play (13):** com.google.android.gms.ads.AdActivity · OutOfContextTestingActivity · NotificationHandlerActivity · GoogleApiActivity · auth.api.signin.internal.SignInHubActivity · com.google.android.gms.ads.MobileAdsInitProvider *(auth `...mobileadsinitprovider`, initOrder 100)* · com.google.android.gms.ads.AdService · auth.api.signin.RevocationBoundService · com.android.billingclient.api.ProxyBillingActivity · ProxyBillingActivityV2 · com.google.android.datatransport.runtime.backends.TransportBackendDiscovery · ...jobscheduling.JobInfoSchedulerService · ...AlarmManagerSchedulerBroadcastReceiver

**AppLovin / MAX (20):** com.applovin.adview.AppLovinFullscreenActivity · sdk.AppLovinWebViewActivity · mediation.MaxDebuggerActivity · MaxDebuggerDetailActivity · MaxDebuggerMultiAdActivity · MaxDebuggerAdUnitsListActivity · MaxDebuggerAdUnitWaterfallsListActivity · MaxDebuggerAdUnitDetailActivity · MaxDebuggerCmpNetworksListActivity · MaxDebuggerTcfConsentStatusesListActivity · MaxDebuggerTcfInfoListActivity · MaxDebuggerTcfStringActivity · MaxDebuggerTestLiveNetworkActivity · MaxDebuggerTestModeNetworkActivity · MaxDebuggerUnifiedFlowActivity · MaxDebuggerWaterfallSegmentsActivity · creative.MaxCreativeDebuggerActivity · creative.MaxCreativeDebuggerDisplayedAdActivity · sdk.AppLovinInitProvider *(auth `...applovininitprovider`, initOrder 101)* · impl.adview.activity.FullscreenAdService

**Appodeal (4):** ads.adapters.iab.unified.VideoPlayerActivity · ads.TestActivity · consent.form.ConsentActivity · ads.AppodealPackageAddedReceiver

**Chartboost (4):** view.CBImpressionActivity · internal.clickthrough.EmbeddedBrowserActivity · view.FullscreenAdActivity · internal.video.repository.exoplayer.VideoRepositoryDownloadService *(action `com.google.android.exoplayer.downloadService.action.RESTART`)*

**Fyber / Inneractive (5):** InneractiveInternalBrowserActivity · InneractiveFullscreenAdActivity · InneractiveRichMediaVideoPlayerActivityCore · InternalStoreWebpageActivity · FyberReportAdActivity

**Smaato (4):** rewarded.view.RewardedInterstitialAdActivity · interstitial.view.InterstitialAdActivity · core.browser.SmaatoSdkBrowserActivity · core.lifecycle.ProcessLifecycleOwnerInitializer *(auth `...smaato-lifecycle-process`)*

**Amazon DTB / APS (3):** device.ads.DTBInterstitialActivity *(exported)* · device.ads.DTBAdActivity *(scheme `amazonmobile://intent`)* · amazon.aps.ads.activity.ApsInterstitialActivity *(exported)*

**PubMatic / OpenWrap (3):** webrendering.mraid.POBVideoPlayerActivity · webrendering.ui.POBFullScreenActivity · common.browser.POBInternalBrowserActivity

**Yandex Mobile Ads (4):** common.AdActivity · features.debugpanel.ui.IntegrationInspectorActivity · core.initializer.MobileAdsInitializeProvider *(auth `...MobileAdsInitializeProvider`)* · features.debugpanel.data.local.DebugPanelFileProvider *(auth `...monetization.ads.inspector.fileprovider`)*

**InMobi (1):** ads.rendering.InMobiAdActivity

**Pangle / ByteDance (15):** activity.TTCeilingLandingPageActivity · TTLandingPageActivity · TTPlayableLandingPageActivity · TTVideoLandingPageLink2Activity · TTDelegateActivity · TTWebsiteActivity · TTAppOpenAdActivity · TTRewardVideoActivity · TTRewardExpressVideoActivity · TTFullScreenVideoActivity · TTFullScreenExpressVideoActivity · TTInterstitialActivity · TTInterstitialExpressActivity · TTAdActivity · multipro.aidl.BinderPoolService

**Unity Ads (5 + 1 initializer):** ads.adunit.AdUnitActivity · AdUnitTransparentActivity · AdUnitTransparentSoftwareActivity · AdUnitSoftwareActivity · ads.adplayer.FullScreenWebViewDisplay · (Startup meta-data `com.unity3d.services.core.configuration.AdsSdkInitializer`)

**ironSource / LevelPlay (6):** sdk.controller.ControllerActivity · InterstitialActivity · OpenUrlActivity · mediationsdk.testSuite.TestSuiteActivity · lifecycle.IronsourceLifecycleProvider · lifecycle.LevelPlayActivityLifecycleProvider

**Mintegral / Mbridge (4):** activity.MBCommonActivity · out.LoadingActivity · reward.player.MBRewardVideoActivity · foundation.same.broadcast.NetWorkChangeReceiver *(exported)*

**Vungle (2):** internal.ui.VungleActivity · VungleProvider *(auth `...vungle-provider`, initOrder 102)*

**BIGO (11):** ad.splash.AdSplashActivity · LandscapeAdSplashActivity · controller.form.AdFormActivity · api.AdActivity · api.PopupAdActivity · api.LandingStyleableActivity · api.LandscapeAdActivity · api.CompanionAdActivity · api.LandscapeCompanionAdActivity · core.mraid.MraidVideoActivity · controller.provider.BigoAdsProvider *(auth `...BigoAdsProvider`)*

**TaurusX (9):** ui.TaxWebViewActivity · ui.TaxVideoActivity · ui.AppOpenAdActivity · ui.AppOpenNativeAdActivity · utils.webview.TaxBrowserActivity · mraid.MraidBrowser · mraid.MraidVideoPlayerActivity · ui.TaurusxH5Activity · core.TaurusXProvider *(auth `...taurusxfileprovider`)*

**OGury (4):** interstitial.ui.InterstitialActivity · InterstitialAndroid8TransparentActivity · InterstitialAndroid8RotableActivity · core.internal.LogEnablerReceiver *(exported)*

**StartApp (6):** consent.ConsentActivity · ads.list3d.List3DActivity · ads.interstitials.OverlayActivity · cachedservice.BackgroundService · jobs.SchedulerService · remoteconfig.BootCompleteListener

**MobileFuse (5):** MobileFuseFullscreenActivity · MobileFuseFullscreenTransparentActivity · experimental.NativeInterstitialActivity · splashad.MobileFuseSplashAdActivity · MobileFuseSdkInitProvider *(auth `...mobilefusesdkinitprovider`)*

**Moloco (4):** Xenoss FullscreenWebviewActivity · VastActivity · StaticAdActivity · MraidActivity · (Startup meta-data `com.moloco.sdk.internal.android_context.StartupComponentInitialization`)

**MyTarget (2):** common.MyTargetActivity · common.MyTargetContentProvider *(auth `...mytargetcontentprovider`)*

**Facebook Audience Network (2):** ads.AudienceNetworkActivity · AudienceNetworkContentProvider *(auth `...AudienceNetworkContentProvider`)*

**BidMachine (5):** rendering.ad.fullscreen.FullScreenActivity · nativead.view.VideoPlayerActivity · iab.mraid.MraidActivity · iab.vast.activity.VastActivity · BidMachineInitProvider *(auth `...bidmachineinitprovider`, initOrder 100)*

**ExploreStack / IAB (3):** iab.mraid.MraidActivity · MraidDialogActivity · iab.vast.activity.VastActivity

**PubNative (8):** consent.UserConsentActivity · interstitial.activity.MraidInterstitialActivity · VastInterstitialActivity · rewarded.activity.MraidRewardedActivity · VastRewardedActivity · contentinfo.AdFeedbackActivity · utils.browser.BrowserActivity · receiver.VolumeChangedActionReceiver

**AppMetrica (2):** internal.AppMetricaService · internal.PreloadInfoContentProvider *(exported)*

**Picasso (1):** com.squareup.picasso.PicassoProvider *(auth `...com.squareup.picasso`)*

**PairIP (2):** com.pairip.licensecheck.LicenseActivity · LicenseContentProvider *(auth `...com.pairip.licensecheck.LicenseContentProvider`)*

**Sentry:** tidak ada komponen; aktif via meta-data + native `libsentry*.so`.

### 2.8 SELURUH meta-data (30 entri)

| # | Nama | Nilai |
|---|---|---|
| 1 | `android.support.FILE_PROVIDER_PATHS` | `@xml/file_paths` (FileProvider app) |
| 2 | `com.google.android.gms.ads.APPLICATION_ID` | `ca-app-pub-3353299071120459~1150848728` |
| 3 | `com.moloco.sdk.internal.android_context.StartupComponentInitialization` | `androidx.startup` |
| 4 | `androidx.lifecycle.ProcessLifecycleInitializer` | `androidx.startup` |
| 5 | `com.unity3d.services.core.configuration.AdsSdkInitializer` | `androidx.startup` |
| 6 | `androidx.emoji2.text.EmojiCompatInitializer` | `androidx.startup` |
| 7 | `androidx.work.WorkManagerInitializer` | `androidx.startup` |
| 8 | `androidx.profileinstaller.ProfileInstallerInitializer` | `androidx.startup` |
| 9 | `com.google.android.gms.ads.flag.OPTIMIZE_INITIALIZATION` | `true` |
| 10 | `com.google.android.gms.ads.flag.OPTIMIZE_AD_LOADING` | `true` |
| 11 | `com.yandex.mobile.ads.AUTOMATIC_SDK_INITIALIZATION` | `false` |
| 12 | `android.support.FILE_PROVIDER_PATHS` | `@xml/debug_panel_file_paths` (Yandex) |
| 13 | `com.google.android.play.billingclient.version` | `7.1.1` |
| 14 | `com.mobilefuse.sdk.disable_auto_init` | `true` |
| 15 | `com.startapp.sdk.SPLASH_ENABLED` | `false` |
| 16 | `com.google.android.gms.version` | `@integer/google_play_services_version` |
| 17 | `com.bytedance.sdk.pangle.version` | `7.3.0.5` |
| 18 | `io.sentry.dsn` | `https://19bf3a1f35004cbe8b0e90999cb10c39@sentry.appodeal.com/42` |
| 19 | `io.sentry.release` | `3.11.0` |
| 20 | `io.sentry.auto-init` | `false` |
| 21 | `io.sentry.ndk.scope-sync.enable` | `true` |
| 22 | `io.sentry.proguard-uuid` | `2fc9fc4a-a2ed-4e19-9561-5e9e83c126cb` |
| 23 | `android.support.FILE_PROVIDER_PATHS` | `@xml/nath_tax_file_path` (TaurusX) |
| 24 | `backend:com.google.android.datatransport.cct.CctBackendFactory` | `cct` |
| 25 | `android.webkit.WebView.EnableSafeBrowsing` | `true` (ironSource TestSuite) |
| 26 | `com.android.vending.splits.required` | `true` |
| 27 | `com.android.stamp.source` | `https://play.google.com/store` |
| 28 | `com.android.stamp.type` | `STAMP_TYPE_DISTRIBUTION_APK` |
| 29 | `com.android.vending.splits` | `@xml/splits0` |
| 30 | `com.android.vending.derived.apk.id` | `3` |

---

## 3. RESOURCES — INVENTARISASI LENGKAP

### 3.1 Rekap tipe resource (`aapt2 dump resources`)

| Tipe | Jumlah | Tipe | Jumlah |
|---|---:|---|---:|
| `attr` | 1.655 | `drawable` | 954 |
| `id` | 1.652 | `string` | 742 |
| `dimen` | 1.484 | `layout` | 527 |
| `style` | 1.437 | `integer` | 92 |
| `color` | 1.208 | `anim` | 68 |
| `plurals` | 4 | `mipmap` | 9 |
| `menu` | 6 | `raw` | 17 |
| `xml` | 13 | `font` | 11 |
| `bool` | 11 | `array` | 10 |
| `animator` | 40 | `interpolator` | 18 |

### 3.2 SELURUH 527 layout

```text
abc_action_bar_title_item · abc_action_bar_up_container · abc_action_menu_item_layout · abc_action_menu_layout · abc_action_mode_bar · abc_action_mode_close_item_material · abc_activity_chooser_view · abc_activity_chooser_view_list_item · abc_alert_dialog_button_bar_material · abc_alert_dialog_material · abc_alert_dialog_title_material · abc_cascading_menu_item_layout
abc_dialog_title_material · abc_expanded_menu_layout · abc_list_menu_item_checkbox · abc_list_menu_item_icon · abc_list_menu_item_layout · abc_list_menu_item_radio · abc_popup_menu_header_item_layout · abc_popup_menu_item_layout · abc_screen_content_include · abc_screen_simple · abc_screen_simple_overlay_action_mode · abc_screen_toolbar
abc_search_dropdown_item_icons_2line · abc_search_view · abc_select_dialog_material · abc_tooltip · activity_create_world · activity_debug · activity_drawer · activity_entities_nbt · activity_help · activity_internal_browser · activity_main · ad_mob_native_banner_template_basic_layout
ad_mob_native_banner_template_icon_text_layout · ad_mob_native_banner_template_rect_layout · ad_mob_native_banner_template_text_cta_layout · admob_empty_layout · apd_admob_banner · applovin_debugger_list_item_detail · applovin_native_ad_media_view · aps_interstitial_activity · bigo_ad_activity_form · bigo_ad_activity_interstitial · bigo_ad_activity_interstitial_banner · bigo_ad_activity_interstitial_bottom_card
bigo_ad_activity_interstitial_landingpage · bigo_ad_activity_interstitial_landingpage_7_8 · bigo_ad_activity_interstitial_native_center · bigo_ad_activity_interstitial_native_top · bigo_ad_activity_interstitial_percent_warning · bigo_ad_activity_interstitial_percent_warning_landscape · bigo_ad_activity_interstitial_rich_video · bigo_ad_activity_interstitial_rich_video_19_29 · bigo_ad_activity_interstitial_rich_video_2 · bigo_ad_activity_interstitial_rich_video_3 · bigo_ad_activity_interstitial_rich_video_4 · bigo_ad_activity_interstitial_rich_video_5
bigo_ad_activity_interstitial_rich_video_component_19 · bigo_ad_activity_interstitial_rich_video_component_20 · bigo_ad_activity_interstitial_rich_video_component_21 · bigo_ad_activity_interstitial_rich_video_component_22 · bigo_ad_activity_interstitial_rich_video_component_23 · bigo_ad_activity_interstitial_rich_video_component_24 · bigo_ad_activity_interstitial_rich_video_component_25 · bigo_ad_activity_interstitial_rich_video_component_26 · bigo_ad_activity_interstitial_rich_video_download_10 · bigo_ad_activity_interstitial_rich_video_download_11 · bigo_ad_activity_interstitial_rich_video_download_12 · bigo_ad_activity_interstitial_rich_video_download_6
bigo_ad_activity_interstitial_rich_video_download_7 · bigo_ad_activity_interstitial_rich_video_download_8 · bigo_ad_activity_interstitial_rich_video_download_9 · bigo_ad_activity_interstitial_rich_video_end · bigo_ad_activity_interstitial_rich_video_end_14_15 · bigo_ad_activity_interstitial_rich_video_end_16_17 · bigo_ad_activity_interstitial_rich_video_end_18 · bigo_ad_activity_interstitial_rich_video_end_19 · bigo_ad_activity_interstitial_rich_video_end_2 · bigo_ad_activity_interstitial_rich_video_end_3 · bigo_ad_activity_interstitial_rich_video_end_4 · bigo_ad_activity_interstitial_rich_video_end_6
bigo_ad_activity_interstitial_rich_video_end_7 · bigo_ad_activity_interstitial_rich_video_end_8 · bigo_ad_activity_interstitial_rich_video_end_9 · bigo_ad_activity_interstitial_rich_video_end_landscape · bigo_ad_activity_interstitial_rich_video_end_landscape_2 · bigo_ad_activity_interstitial_rich_video_land_material_31_32 · bigo_ad_activity_interstitial_rich_video_left_material_31 · bigo_ad_activity_interstitial_rich_video_multi_img_13 · bigo_ad_activity_interstitial_rich_video_multi_img_14 · bigo_ad_activity_interstitial_rich_video_multi_img_15 · bigo_ad_activity_interstitial_rich_video_multi_img_16 · bigo_ad_activity_interstitial_rich_video_multi_img_17
bigo_ad_activity_interstitial_rich_video_multi_img_item_layout · bigo_ad_activity_interstitial_rich_video_multi_img_media_layout · bigo_ad_activity_interstitial_rich_video_right_material_32 · bigo_ad_activity_interstitial_sab · bigo_ad_activity_interstitial_style_landscape_1 · bigo_ad_activity_interstitial_style_landscape_1_full_media · bigo_ad_activity_interstitial_style_landscape_2 · bigo_ad_activity_interstitial_style_landscape_2_full_media · bigo_ad_activity_interstitial_style_landscape_3 · bigo_ad_activity_interstitial_style_landscape_3_no_gp_element · bigo_ad_activity_interstitial_style_landscape_4 · bigo_ad_activity_interstitial_style_landscape_4_full_media
bigo_ad_activity_interstitial_vpaid · bigo_ad_activity_pop_up_style_1 · bigo_ad_activity_pop_up_style_2 · bigo_ad_activity_pop_up_style_3 · bigo_ad_activity_pop_up_style_4 · bigo_ad_activity_pop_up_style_5 · bigo_ad_activity_pop_up_style_6 · bigo_ad_activity_popup · bigo_ad_activity_popup_banner · bigo_ad_activity_popup_vpaid · bigo_ad_activity_reward · bigo_ad_activity_webview
bigo_ad_banner_placeholder_img · bigo_ad_default_loading_layout · bigo_ad_dialog_reward_retain · bigo_ad_download_msg · bigo_ad_download_msg_white · bigo_ad_form_content · bigo_ad_form_content_dark · bigo_ad_form_fill_question · bigo_ad_form_fill_question_dark · bigo_ad_form_layout · bigo_ad_form_layout_dark · bigo_ad_form_notice_privacy
bigo_ad_form_notice_privacy_dark · bigo_ad_form_question · bigo_ad_form_question_dark · bigo_ad_form_select_question · bigo_ad_form_select_question_dark · bigo_ad_item_inter_countdown_bg · bigo_ad_item_interaction_horizontal · bigo_ad_item_interaction_vertical · bigo_ad_item_splash_countdown_bg · bigo_ad_label_layout · bigo_ad_layout_gesture_slide · bigo_ad_layout_gesture_zoom
bigo_ad_layout_ic_item · bigo_ad_layout_interstitial_ad_mark_top_left · bigo_ad_layout_interstitial_ad_warning_bottom · bigo_ad_layout_interstitial_icon_item_cta_des_style1 · bigo_ad_layout_interstitial_icon_item_cta_des_style2 · bigo_ad_layout_interstitial_icon_item_cta_style1 · bigo_ad_layout_interstitial_icon_item_cta_style2 · bigo_ad_layout_interstitial_icon_item_style · bigo_ad_layout_interstitial_icon_view_flow · bigo_ad_layout_interstitial_mid_page_loading_view · bigo_ad_layout_interstitial_mid_page_loading_view_landscape · bigo_ad_layout_interstitial_mid_page_native_fallback_view_download_info_landscape
bigo_ad_layout_interstitial_mid_page_native_fallback_view_landscape · bigo_ad_layout_interstitial_mid_page_native_view · bigo_ad_layout_interstitial_mid_page_native_view_landscape · bigo_ad_layout_interstitial_rich_video_multi_img_bottom · bigo_ad_layout_more · bigo_ad_layout_space · bigo_ad_layout_space_black · bigo_ad_native_banner_medium · bigo_ad_native_banner_small · bigo_ad_optionview_feedback · bigo_ad_pop_up_ad_msg · bigo_ad_popup_close_btn
bigo_ad_splash_ad_label · bigo_ad_splash_endpage1 · bigo_ad_splash_endpage1_slide · bigo_ad_splash_endpage_item_click_guide · bigo_ad_splash_endpage_item_cta · bigo_ad_splash_endpage_item_flash · bigo_ad_splash_endpage_item_slide · bigo_ad_splash_item_arrow · bigo_ad_splash_item_click_guide · bigo_ad_splash_item_cta · bigo_ad_splash_item_cta_bg · bigo_ad_splash_item_flash
bigo_ad_splash_item_ripple · bigo_ad_splash_item_slide · bigo_ad_splash_style_3_item_cta · bigo_ad_splash_style_3_multi_img · bigo_ad_splash_style_4_cta_widget · bigo_ad_splash_style_5_card_widget · bigo_ad_splash_style_fullscreen · bigo_ad_splash_style_fullscreen_immersive · bigo_ad_splash_style_fullscreen_interaction · bigo_ad_splash_style_fullscreen_interaction_immersive · bigo_ad_splash_style_fullscreen_vpaid · bigo_ad_splash_style_halfscreen
bigo_ad_splash_style_halfscreen_interaction · bigo_ad_splash_style_halfscreen_vpaid · bigo_ad_splash_style_root · bigo_ad_success_submit · bigo_ad_success_submit_dark · bigo_ad_view_click_guide · bigo_ad_view_click_guide_1 · bigo_ad_view_click_guide_2 · bigo_ad_view_click_guide_3 · bigo_ad_view_click_guide_landscape_1 · bigo_ad_view_click_guide_landscape_2 · bigo_ad_view_click_guide_landscape_3
bigo_ad_view_slide_gesture · bigo_ad_webview_loading · biome_item · bm_exo_legacy_player_control_view · bm_exo_list_divider · bm_exo_player_control_ffwd_button · bm_exo_player_control_rewind_button · bm_exo_player_control_view · bm_exo_player_view · bm_exo_styled_settings_list · bm_exo_styled_settings_list_item · bm_exo_styled_sub_settings_list_item
bm_exo_track_selection_dialog · bm_privacy_sheet · browser_actions_context_menu_page · browser_actions_context_menu_row · content_info_left_aligned_layout · controls · creative_debugger_displayed_ad_detail_activity · custom_dialog · debug_panel_item_action_button · debug_panel_item_ad_unit · debug_panel_item_ad_units · debug_panel_item_divider
debug_panel_item_mediation_adapter · debug_panel_item_switch · debug_panel_toolbar · design_bottom_navigation_item · design_bottom_sheet_dialog · design_layout_snackbar · design_layout_snackbar_include · design_layout_tab_icon · design_layout_tab_text · design_menu_item_action_area · design_navigation_item · design_navigation_item_header
design_navigation_item_separator · design_navigation_item_subheader · design_navigation_menu · design_navigation_menu_item · design_text_input_end_icon · design_text_input_start_icon · deubg_panel_item_header · deubg_panel_item_key_value · dialog_fullscreen · drawer_header · dt_store_promo_layout · exo_legacy_player_control_view
exo_list_divider · exo_player_control_ffwd_button · exo_player_control_rewind_button · exo_player_control_view · exo_player_view · exo_styled_player_control_ffwd_button · exo_styled_player_control_rewind_button · exo_styled_player_control_view · exo_styled_player_view · exo_styled_settings_list · exo_styled_settings_list_item · exo_styled_sub_settings_list_item
exo_track_selection_dialog · expand_button · ia_endcard_container · ia_layout_activity_internal_store_webpage · ia_layout_buffering_overlay · ia_layout_click_overlay · ia_layout_default_video_end_card · ia_layout_fullscreen_activity · ia_layout_fyber_ad_identifier · ia_layout_fyber_ad_identifier_relative · ia_layout_rich_media_video · ia_layout_skip_rewarded_dialog
ia_layout_video_view · ia_video_view_remote_ui · image_frame · ime_base_split_test_activity · ime_secondary_split_test_activity · instream_layout_detail_pause_overlay · instream_layout_detail_play_overlay · instream_layout_detail_play_overlay_simple · instream_layout_detail_preroll · linear_player_count_down · loading_alert · m3_alert_dialog
m3_alert_dialog_actions · m3_alert_dialog_title · m3_auto_complete_simple_item · m3_navigation_menu_divider · m3_navigation_menu_subheader · m3_side_sheet_dialog · mainlayout · map_selection_view · material_chip_input_combo · material_clock_display · material_clock_display_divider · material_clock_period_toggle
material_clock_period_toggle_land · material_clockface_textview · material_clockface_view · material_radial_view_group · material_textinput_timepicker · material_time_chip · material_time_input · material_timepicker · material_timepicker_dialog · material_timepicker_textinput_display · max_native_ad_banner_icon_and_text_layout · max_native_ad_banner_view
max_native_ad_leader_view · max_native_ad_media_banner_view · max_native_ad_medium_template_1 · max_native_ad_mrec_view · max_native_ad_recycler_view_item · max_native_ad_small_template_1 · max_native_ad_vertical_banner_view · max_native_ad_vertical_leader_view · max_native_ad_vertical_media_banner_view · mbridge_activity · mbridge_alertview_layout · mbridge_bt_container
mbridge_cm_alertview · mbridge_cm_feedback_notice_layout · mbridge_cm_feedbackview · mbridge_cm_loading_layout · mbridge_interstitial_activity · mbridge_more_offer_activity · mbridge_nativex_fullbasescreen · mbridge_nativex_fullscreen_top · mbridge_nativex_mbmediaview · mbridge_nativex_playerview · mbridge_order_layout_item · mbridge_order_layout_list_landscape
mbridge_order_layout_list_portrait · mbridge_playercommon_player_view · mbridge_reward_activity_video_templete · mbridge_reward_activity_video_templete_transparent · mbridge_reward_clickable_cta · mbridge_reward_end_card_layout_landscape · mbridge_reward_end_card_layout_landscape_1302 · mbridge_reward_end_card_layout_portrait · mbridge_reward_end_card_layout_portrait_1302 · mbridge_reward_end_card_more_offer_item · mbridge_reward_endcard_h5 · mbridge_reward_endcard_native_half_landscape
mbridge_reward_endcard_native_half_portrait · mbridge_reward_endcard_native_hor · mbridge_reward_endcard_native_land · mbridge_reward_endcard_vast · mbridge_reward_layer_floor · mbridge_reward_layer_floor_302 · mbridge_reward_layer_floor_5002010 · mbridge_reward_layer_floor_802 · mbridge_reward_layer_floor_902 · mbridge_reward_layer_floor_904 · mbridge_reward_layer_floor_bottom · mbridge_reward_layer_floor_half_portrait
mbridge_reward_more_offer_view · mbridge_reward_videoend_cover · mbridge_reward_videoview_item · mbridge_reward_view_tag_item · mbridge_same_choice_one_layout_landscape · mbridge_same_choice_one_layout_portrait · mbridge_splash_landscape · mbridge_splash_portrait · mdtb_interstitial_ad · mediation_debugger_ad_unit_detail_activity · mediation_debugger_list_item_right_detail · mediation_debugger_list_section
mediation_debugger_list_section_centered · mediation_debugger_list_view · mediation_debugger_multi_ad_activity · mediation_debugger_text_view_activity · mobilefuse_ad_fullscreen · mobilefuse_mraid_fullscreen_content · mobilefuse_native_interstitial_fullscreen · mobilefuse_native_interstitial_layout · mobilefuse_splash_ad_fullscreen · mobilefuse_vast_fullscreen_content · mobilefuse_vast_player_view · monetization_ads_feed_item
monetization_ads_feed_progressbar · monetization_ads_internal_adtune_container · monetization_ads_internal_divkit · monetization_ads_internal_instream_skin_v1 · monetization_ads_internal_instream_skin_v2 · monetization_ads_internal_multibanner_controls · monetization_ads_internal_native_interstitial_close · monetization_ads_internal_native_interstitial_content · monetization_ads_internal_native_interstitial_landscape_horizontal_media · monetization_ads_internal_native_interstitial_landscape_sponsored_container · monetization_ads_internal_native_interstitial_landscape_vetrical_media · monetization_ads_internal_native_interstitial_portrait
monetization_ads_internal_native_popup_item · monetization_ads_internal_native_rewarded_controls · monetization_ads_internal_native_rewarded_landscape · monetization_ads_internal_native_rewarded_portrait · monetization_ads_internal_outstream_controls_app_install · monetization_ads_internal_outstream_controls_default · monetization_ads_internal_outstream_controls_fullscreen_content_v1 · monetization_ads_internal_rewarded_close_verification · mrec_layout · mtrl_alert_dialog · mtrl_alert_dialog_actions · mtrl_alert_dialog_title
mtrl_alert_select_dialog_item · mtrl_alert_select_dialog_multichoice · mtrl_alert_select_dialog_singlechoice · mtrl_auto_complete_simple_item · mtrl_calendar_day · mtrl_calendar_day_of_week · mtrl_calendar_days_of_week · mtrl_calendar_horizontal · mtrl_calendar_month · mtrl_calendar_month_labeled · mtrl_calendar_month_navigation · mtrl_calendar_months
mtrl_calendar_vertical · mtrl_calendar_year · mtrl_layout_snackbar · mtrl_layout_snackbar_include · mtrl_navigation_rail_item · mtrl_picker_actions · mtrl_picker_dialog · mtrl_picker_fullscreen · mtrl_picker_header_dialog · mtrl_picker_header_fullscreen · mtrl_picker_header_selection_text · mtrl_picker_header_title_text
mtrl_picker_header_toggle · mtrl_picker_text_input_date · mtrl_picker_text_input_date_range · mtrl_search_bar · mtrl_search_view · notification_action · notification_action_tombstone · notification_media_action · notification_media_cancel_action · notification_template_big_media · notification_template_big_media_custom · notification_template_big_media_narrow
notification_template_big_media_narrow_custom · notification_template_custom_big · notification_template_icon_group · notification_template_lines_media · notification_template_media · notification_template_media_custom · notification_template_part_chronometer · notification_template_part_time · offline_ads_dialog · open_url · player_count_down · pob_custom_product_layout
pob_layout_rewardedad_skip_alert · pob_medium_template · pob_monitor_view · pob_small_template · preference · preference_category · preference_category_material · preference_dialog_edittext · preference_dropdown · preference_dropdown_material · preference_information · preference_information_material
preference_list_fragment · preference_material · preference_recyclerview · preference_widget_checkbox · preference_widget_seekbar · preference_widget_seekbar_material · preference_widget_switch · preference_widget_switch_compat · progress_count_down · rab_card_large · select_dialog_item_material · select_dialog_multichoice_material
select_dialog_singlechoice_material · selection_drag_btn · sentry_dialog_user_feedback · simple_list_item · simple_list_item_action · simple_list_item_multiple_choice · simple_list_item_single_choice · simpledialogfragment_check_box · simpledialogfragment_color_item · simpledialogfragment_color_wheel · simpledialogfragment_custom_view · simpledialogfragment_form
simpledialogfragment_form_item_check · simpledialogfragment_form_item_color · simpledialogfragment_form_item_datetime · simpledialogfragment_form_item_hint · simpledialogfragment_form_item_input · simpledialogfragment_form_item_spinner · simpledialogfragment_grid · simpledialogfragment_image · simpledialogfragment_image_hor_scroll · simpledialogfragment_image_vert_scroll · simpledialogfragment_input · simpledialogfragment_list
simpledialogfragment_list_item · simpledialogfragment_pin · simpledialogfragment_progress · simpledialogfragment_progress_circle · smaato_sdk_core_activity_internal_browser · smaato_sdk_interstitial_activity · smaato_sdk_interstitial_splash_activity · smaato_sdk_rewarded_ads_activity · smaato_sdk_richmedia_layout_closable · smaato_sdk_video_player_view · smaato_sdk_video_vast_video_player_view · support_simple_spinner_dropdown_item
taurusx_activity_appopen · taurusx_activity_appopen_native · taurusx_activity_fullscreen · taurusx_activity_h5 · taurusx_activity_web_view · taurusx_dialog_adtune_layout · temple904 · timer_count_down · tree_node_item · world_item · world_layer_item
```

**Layout milik aplikasi (20):** `activity_main`, `activity_drawer`, `drawer_header`, `activity_help`, `activity_create_world`, `activity_entities_nbt`, `activity_debug`, `activity_internal_browser`, `tree_node_item`, `world_item`, `world_layer_item`, `biome_item`, `map_selection_view`, `selection_drag_btn`, `custom_dialog`, `loading_alert`, `mainlayout`, `controls`, `deubg_panel_item_header`, `deubg_panel_item_key_value` (dua terakhir untuk panel debug internal). Sisanya 507 adalah library (BIGO ~165, mbridge ~50, exo/bm_exo ~30, material/mtrl/m3/abc/design ~200, iklan lain ~60).

### 3.3 SELURUH 6 menu

| Menu | Pemilik | Isi |
|---|---|---|
| `menu/drawer_menu` | **Aplikasi** | 5 item utama + submenu Advanced (7 item) |
| `menu/main_activity_toolbar_menu` | **Aplikasi** | Help (`action_help`, always), About (`action_about`, ifRoom) |
| `menu/create_world_activity_toolbar_menu` | **Aplikasi** | Save (`action_save`, always) |
| `menu/mediation_debugger_activity_menu` | AppLovin MAX | debugger mediasi |
| `menu/creative_debugger_displayed_ad_activity_menu` | AppLovin | creative debugger |
| `menu/dialog_buttons` | library (simple dialog) | tombol dialog generik |

### 3.4 SELURUH 954 drawable

```text
$applovin_ic_mute_to_unmute__0 · $applovin_ic_mute_to_unmute__1 · $applovin_ic_mute_to_unmute__2 · $applovin_ic_mute_to_unmute__3 · $applovin_ic_mute_to_unmute__4 · $applovin_ic_unmute_to_mute__0 · $applovin_ic_unmute_to_mute__1 · $applovin_ic_unmute_to_mute__2 · $applovin_ic_unmute_to_mute__3 · $applovin_ic_unmute_to_mute__4 · $avd_hide_password__0 · $avd_hide_password__1
$avd_hide_password__2 · $avd_show_password__0 · $avd_show_password__1 · $avd_show_password__2 · $ic_launcher_foreground__0 · $ic_palette_color__0 · $m3_avd_hide_password__0 · $m3_avd_hide_password__1 · $m3_avd_hide_password__2 · $m3_avd_show_password__0 · $m3_avd_show_password__1 · $m3_avd_show_password__2
$mtrl_checkbox_button_checked_unchecked__0 · $mtrl_checkbox_button_checked_unchecked__1 · $mtrl_checkbox_button_checked_unchecked__2 · $mtrl_checkbox_button_icon_checked_indeterminate__0 · $mtrl_checkbox_button_icon_checked_unchecked__0 · $mtrl_checkbox_button_icon_checked_unchecked__1 · $mtrl_checkbox_button_icon_checked_unchecked__2 · $mtrl_checkbox_button_icon_indeterminate_checked__0 · $mtrl_checkbox_button_icon_indeterminate_unchecked__0 · $mtrl_checkbox_button_icon_indeterminate_unchecked__1 · $mtrl_checkbox_button_icon_indeterminate_unchecked__2 · $mtrl_checkbox_button_icon_unchecked_checked__0
$mtrl_checkbox_button_icon_unchecked_checked__1 · $mtrl_checkbox_button_icon_unchecked_checked__2 · $mtrl_checkbox_button_icon_unchecked_indeterminate__0 · $mtrl_checkbox_button_icon_unchecked_indeterminate__1 · $mtrl_checkbox_button_icon_unchecked_indeterminate__2 · $mtrl_checkbox_button_unchecked_checked__0 · $mtrl_checkbox_button_unchecked_checked__1 · $mtrl_checkbox_button_unchecked_checked__2 · $mtrl_switch_thumb_checked_pressed__0 · $mtrl_switch_thumb_checked_unchecked__0 · $mtrl_switch_thumb_checked_unchecked__1 · $mtrl_switch_thumb_pressed_checked__0
$mtrl_switch_thumb_pressed_unchecked__0 · $mtrl_switch_thumb_unchecked_checked__0 · $mtrl_switch_thumb_unchecked_checked__1 · $mtrl_switch_thumb_unchecked_pressed__0 · abc_action_bar_item_background_material · abc_btn_borderless_material · abc_btn_check_material · abc_btn_check_material_anim · abc_btn_colored_material · abc_btn_default_mtrl_shape · abc_btn_radio_material · abc_btn_radio_material_anim
abc_cab_background_internal_bg · abc_cab_background_top_material · abc_control_background_material · abc_dialog_material_background · abc_edit_text_material · abc_ic_ab_back_material · abc_ic_arrow_drop_right_black_24dp · abc_ic_clear_material · abc_ic_go_search_api_material · abc_ic_menu_copy_mtrl_am_alpha · abc_ic_menu_cut_mtrl_alpha · abc_ic_menu_overflow_material
abc_ic_menu_paste_mtrl_am_alpha · abc_ic_menu_selectall_mtrl_alpha · abc_ic_menu_share_mtrl_alpha · abc_ic_search_api_material · abc_ic_voice_search_api_material · abc_item_background_holo_dark · abc_item_background_holo_light · abc_list_divider_material · abc_list_selector_background_transition_holo_dark · abc_list_selector_background_transition_holo_light · abc_list_selector_holo_dark · abc_list_selector_holo_light
abc_ratingbar_indicator_material · abc_ratingbar_material · abc_ratingbar_small_material · abc_seekbar_thumb_material · abc_seekbar_tick_mark_material · abc_seekbar_track_material · abc_spinner_textfield_background_material · abc_star_black_48dp · abc_star_half_black_48dp · abc_switch_thumb_material · abc_tab_indicator_material · abc_text_cursor_material
abc_textfield_search_material · abc_vector_test · ad_mob_outline_shape · ad_mob_rounded_button_shape · ad_mob_rounded_corners_shape · admob_close_button_black_circle_white_cross · admob_close_button_white_circle_black_cross · admob_close_button_white_cross · apd_ad_choice_preview · apd_admob_cta_round_outline · apd_admob_round_outline · apd_iab_ad_choice_preview
apd_icon_preview · apd_media_preview · applovin_creative_debugger_report_ad_rounded_button · applovin_ic_mediation_placeholder · applovin_ic_mute_to_unmute · applovin_ic_privacy_icon · applovin_ic_privacy_icon_layered_list · applovin_ic_unmute_to_mute · applovin_ic_white_small · applovin_mediation_debugger_switch_thumb · applovin_mediation_debugger_switch_track · applovin_rounded_black_background
applovin_rounded_button · applovin_rounded_text_view_border · arrow_mark · avd_hide_password · avd_show_password · bg_btn_get · bigo_ad_advertiser_background · bigo_ad_arrow_right · bigo_ad_banner_advertiser_background · bigo_ad_banner_button_bg_rectangle_blue · bigo_ad_banner_clickguide · bigo_ad_bg_ad_tag_white_border
bigo_ad_bg_mask · bigo_ad_btn_background · bigo_ad_btn_background_white · bigo_ad_btn_background_white_dark · bigo_ad_button_bg_circle_grey · bigo_ad_button_bg_rectangle_blue · bigo_ad_button_bg_rectangle_blue_2 · bigo_ad_button_bg_rectangle_green · bigo_ad_button_bg_rectangle_green_radius8 · bigo_ad_button_countdown_close · bigo_ad_button_stroke_circle_white · bigo_ad_cardview_bg
bigo_ad_click_guide · bigo_ad_click_guide2 · bigo_ad_click_guide_landscape · bigo_ad_click_guide_shadow · bigo_ad_click_ripple · bigo_ad_close_btn_background · bigo_ad_default_base_image · bigo_ad_default_progressbar · bigo_ad_domain_left_drawable · bigo_ad_double_video_play · bigo_ad_double_video_small_play · bigo_ad_edit_background
bigo_ad_edit_background_dark · bigo_ad_feeback_arrow_right · bigo_ad_feedback_advertise_info_background · bigo_ad_feedback_background · bigo_ad_feedback_top_slide_bar · bigo_ad_flash · bigo_ad_form_icon_back_up · bigo_ad_gift_widget · bigo_ad_gradual_background · bigo_ad_heart_widget · bigo_ad_ic_ad_label · bigo_ad_ic_back_gray
bigo_ad_ic_close · bigo_ad_ic_close2 · bigo_ad_ic_close3 · bigo_ad_ic_close4 · bigo_ad_ic_close5 · bigo_ad_ic_close_gray · bigo_ad_ic_close_gray_light · bigo_ad_ic_download · bigo_ad_ic_download_box · bigo_ad_ic_download_box_white · bigo_ad_ic_everyone · bigo_ad_ic_everyone_ic_info
bigo_ad_ic_everyone_white · bigo_ad_ic_info · bigo_ad_ic_info_gray · bigo_ad_ic_info_popup · bigo_ad_ic_info_white · bigo_ad_ic_left_round_black · bigo_ad_ic_left_round_white · bigo_ad_ic_loading_gp · bigo_ad_ic_media_mute · bigo_ad_ic_media_pause · bigo_ad_ic_media_play · bigo_ad_ic_media_unmute
bigo_ad_ic_phone · bigo_ad_ic_phone_white · bigo_ad_ic_recommend · bigo_ad_ic_recommend_white · bigo_ad_ic_right_black · bigo_ad_ic_right_round_black · bigo_ad_ic_right_round_white · bigo_ad_ic_right_white · bigo_ad_ic_safe · bigo_ad_ic_star · bigo_ad_ic_star2 · bigo_ad_ic_star2_white
bigo_ad_ic_star_half · bigo_ad_ic_star_half_white · bigo_ad_ic_star_normal · bigo_ad_ic_star_normal_white · bigo_ad_ic_star_white · bigo_ad_ic_web · bigo_ad_ic_web_back · bigo_ad_ic_web_back_disable · bigo_ad_ic_web_copy · bigo_ad_ic_web_forward · bigo_ad_ic_web_forward_enable · bigo_ad_ic_web_open_browser
bigo_ad_ic_web_refresh · bigo_ad_ic_web_white · bigo_ad_icon_background · bigo_ad_icon_background_dark · bigo_ad_icon_default · bigo_ad_icon_default_img · bigo_ad_icon_default_only_icon · bigo_ad_icon_default_radius22 · bigo_ad_icon_default_radius6 · bigo_ad_icon_novideo_default · bigo_ad_interaction_animation_bg · bigo_ad_interaction_shake_arrow
bigo_ad_interaction_shake_landscape_phone · bigo_ad_interaction_shake_phone · bigo_ad_interaction_slide_hand · bigo_ad_interaction_slide_line · bigo_ad_interaction_twist_arrow · bigo_ad_interaction_twist_landscape_phone · bigo_ad_interaction_twist_phone · bigo_ad_interstitial_landscape_ad_info_bg · bigo_ad_interstitial_rich_video_end_landscape_mask · bigo_ad_label_advertiser_background_raduis7 · bigo_ad_label_background · bigo_ad_landing_bottom_bar_bg
bigo_ad_landing_progress_bar · bigo_ad_landing_top_bar_bg · bigo_ad_landing_top_indicator · bigo_ad_landscape_slide_view · bigo_ad_layer_gift_color · bigo_ad_layer_gift_shadow · bigo_ad_layer_heart_color · bigo_ad_layer_heart_shadow · bigo_ad_mid_page_ic_gp_right · bigo_ad_multi_img_advertiser_background · bigo_ad_multi_img_horizontal_mask · bigo_ad_multi_img_vertical_mask
bigo_ad_popup_ad_info_background · bigo_ad_progressbar_white · bigo_ad_ru_background · bigo_ad_slide · bigo_ad_slide_gesture · bigo_ad_splash_endpage_bg · bigo_ad_splash_endpage_slide_bg · bigo_ad_splash_header_bg · bigo_ad_star_icon · bigo_ad_success · bigo_ad_tag_background · bigo_ad_top_left_advertiser_background
bigo_ad_view_ripple · bigo_ad_warning_background_radius8 · bigo_ad_web_loading_progress_bar · bm_bg_privacy_button · bm_bg_privacy_divider · bm_bg_privacy_sheet · bm_bg_ripple_circle · bm_ic_close · browser_background_selector · browser_backward_selector · browser_bottom_button_layout_bg · browser_forward_selector
browser_progress_bar · browser_secure_connection · browser_top_button_layout_bg · btn_checkbox_checked_mtrl · btn_checkbox_checked_to_unchecked_mtrl_animation · btn_checkbox_unchecked_mtrl · btn_checkbox_unchecked_to_checked_mtrl_animation · btn_radio_off_mtrl · btn_radio_off_to_on_mtrl_animation · btn_radio_on_mtrl · btn_radio_on_to_off_mtrl_animation · button_background
circle_progress_background · circular_shape · close_card_button · common_google_signin_btn_icon_dark · common_google_signin_btn_icon_dark_focused · common_google_signin_btn_icon_dark_normal · common_google_signin_btn_icon_disabled · common_google_signin_btn_icon_light · common_google_signin_btn_icon_light_focused · common_google_signin_btn_icon_light_normal · common_google_signin_btn_text_dark · common_google_signin_btn_text_dark_focused
common_google_signin_btn_text_dark_normal · common_google_signin_btn_text_disabled · common_google_signin_btn_text_light · common_google_signin_btn_text_light_focused · common_google_signin_btn_text_light_normal · cursor_drawable · debug_panel_icon_arrow_back · debug_panel_icon_arrow_navigate · debug_panel_icon_button_background · debug_panel_icon_error · debug_panel_icon_share · debug_panel_icon_success
debug_panel_icon_warning · debug_panel_item_action_button_background · debug_panel_item_mediation_adapter_background · debug_panel_item_mediation_adapter_icon_shape · debug_panel_item_navigation_background · design_fab_background · design_ic_visibility · design_ic_visibility_off · design_password_eye · design_snackbar_background · div_button_background · div_gallery_item_border
error_counter_background · exo_controls_fastforward · exo_controls_fullscreen_enter · exo_controls_fullscreen_exit · exo_controls_next · exo_controls_pause · exo_controls_play · exo_controls_previous · exo_controls_repeat_all · exo_controls_repeat_off · exo_controls_repeat_one · exo_controls_rewind
exo_controls_shuffle_off · exo_controls_shuffle_on · exo_controls_vr · exo_legacy_controls_fastforward · exo_legacy_controls_fullscreen_enter · exo_legacy_controls_fullscreen_exit · exo_legacy_controls_next · exo_legacy_controls_pause · exo_legacy_controls_play · exo_legacy_controls_previous · exo_legacy_controls_repeat_all · exo_legacy_controls_repeat_off
exo_legacy_controls_repeat_one · exo_legacy_controls_rewind · exo_legacy_controls_shuffle_off · exo_legacy_controls_shuffle_on · exo_legacy_controls_vr · exo_notification_fastforward · exo_notification_next · exo_notification_pause · exo_notification_play · exo_notification_previous · exo_notification_rewind · exo_notification_small_icon
exo_notification_stop · exo_rounded_rectangle · exo_styled_controls_audiotrack · exo_styled_controls_check · exo_styled_controls_fastforward · exo_styled_controls_fullscreen_enter · exo_styled_controls_fullscreen_exit · exo_styled_controls_next · exo_styled_controls_overflow_hide · exo_styled_controls_overflow_show · exo_styled_controls_pause · exo_styled_controls_play
exo_styled_controls_previous · exo_styled_controls_repeat_all · exo_styled_controls_repeat_off · exo_styled_controls_repeat_one · exo_styled_controls_rewind · exo_styled_controls_settings · exo_styled_controls_shuffle_off · exo_styled_controls_shuffle_on · exo_styled_controls_simple_fastforward · exo_styled_controls_simple_rewind · exo_styled_controls_speed · exo_styled_controls_subtitle_off
exo_styled_controls_subtitle_on · exo_styled_controls_vr · fab_label_background · home · ia_bg_circle_overlay · ia_bg_green · ia_bg_text_overlay · ia_circle_overlay_bg · ia_close · ia_digital_turbine_logo · ia_fyber_clip_drawable · ia_fyber_info_button
ia_ib_background · ia_ib_close · ia_ib_left_arrow · ia_ib_refresh · ia_ib_right_arrow · ia_ib_unleft_arrow · ia_ib_unright_arrow · ia_progress_bar_drawable · ia_round_overlay_bg · ia_round_overlay_bg_fyber · ia_round_overlay_bg_with_close · ia_sel_expand_collapse
ia_sel_mute · ic_100tb · ic_add · ic_arrow_back_black_24 · ic_arrow_down_24dp · ic_arrow_forward · ic_arrow_forward_accent · ic_book · ic_check · ic_clear_black_24 · ic_clear_search · ic_clock_black_24dp
ic_exit · ic_expand_less_22px · ic_expand_more_22px · ic_group_people · ic_human · ic_info · ic_keyboard_black_24dp · ic_launcher_foreground · ic_layers · ic_list · ic_m3_chip_check · ic_m3_chip_checked_circle
ic_m3_chip_close · ic_map · ic_mtrl_arrow_circle · ic_mtrl_checked_circle · ic_mtrl_chip_checked_black · ic_mtrl_chip_checked_circle · ic_mtrl_chip_close_circle · ic_person · ic_round_close_24 · ic_round_replay_24 · ic_round_skip_next_24 · ic_round_volume_off_24
ic_round_volume_up_24 · ic_save · ic_search · ic_search_black_24 · ic_tune · ic_upload · info_badge · instream_skip · m3_avd_hide_password · m3_avd_show_password · m3_bottom_sheet_drag_handle · m3_password_eye
m3_popupmenu_background_overlay · m3_radiobutton_ripple · m3_selection_control_ripple · m3_split_button_chevron_avd · m3_tabs_background · m3_tabs_line_indicator · m3_tabs_rounded_line_indicator · m3_tabs_transparent_background · material_cursor_drawable · material_ic_calendar_black_24dp · material_ic_clear_black_24dp · material_ic_edit_black_24dp
material_ic_keyboard_arrow_left_black_24dp · material_ic_keyboard_arrow_next_black_24dp · material_ic_keyboard_arrow_previous_black_24dp · material_ic_keyboard_arrow_right_black_24dp · material_ic_menu_arrow_down_black_24dp · material_ic_menu_arrow_up_black_24dp · mbridge_arrow_down_white · mbridge_arrow_down_white_blackbg · mbridge_arrow_up_black · mbridge_arrow_up_white · mbridge_banner_close · mbridge_bottom_media_control
mbridge_cm_alertview_bg · mbridge_cm_alertview_cancel_bg · mbridge_cm_alertview_cancel_bg_nor · mbridge_cm_alertview_cancel_bg_pressed · mbridge_cm_alertview_confirm_bg · mbridge_cm_alertview_confirm_bg_nor · mbridge_cm_alertview_confirm_bg_pressed · mbridge_cm_backward · mbridge_cm_backward_disabled · mbridge_cm_backward_nor · mbridge_cm_backward_selected · mbridge_cm_browser
mbridge_cm_btn_shake · mbridge_cm_circle_50black · mbridge_cm_end_animation · mbridge_cm_exits · mbridge_cm_exits_nor · mbridge_cm_exits_selected · mbridge_cm_feedback_btn_bg · mbridge_cm_feedback_choice_btn_bg · mbridge_cm_feedback_choice_btn_bg_nor · mbridge_cm_feedback_choice_btn_bg_pressed · mbridge_cm_feedback_dialog_view_bg · mbridge_cm_feedback_dialog_view_btn_bg
mbridge_cm_forward · mbridge_cm_forward_disabled · mbridge_cm_forward_nor · mbridge_cm_forward_selected · mbridge_cm_head · mbridge_cm_highlight · mbridge_cm_progress · mbridge_cm_progress_drawable · mbridge_cm_progress_icon · mbridge_cm_refresh · mbridge_cm_refresh_nor · mbridge_cm_refresh_selected
mbridge_cm_tail · mbridge_demo_star_nor · mbridge_demo_star_sel · mbridge_finger_media_control · mbridge_icon_click_circle · mbridge_icon_click_hand · mbridge_icon_play_bg · mbridge_interstitial_close · mbridge_interstitial_over · mbridge_native_advanced_close_icon · mbridge_native_bg_loading_camera · mbridge_nativex_close
mbridge_nativex_cta_land_nor · mbridge_nativex_cta_land_pre · mbridge_nativex_cta_por_nor · mbridge_nativex_cta_por_pre · mbridge_nativex_full_land_close · mbridge_nativex_full_protial_close · mbridge_nativex_fullview_background · mbridge_nativex_pause · mbridge_nativex_play · mbridge_nativex_play_bg · mbridge_nativex_play_progress · mbridge_nativex_sound1
mbridge_nativex_sound2 · mbridge_nativex_sound3 · mbridge_nativex_sound4 · mbridge_nativex_sound5 · mbridge_nativex_sound6 · mbridge_nativex_sound7 · mbridge_nativex_sound8 · mbridge_nativex_sound_animation · mbridge_nativex_sound_bg · mbridge_nativex_sound_close · mbridge_nativex_sound_open · mbridge_order_layout_list_bg
mbridge_reward_activity_ad_end_land_des_rl_hot · mbridge_reward_close · mbridge_reward_close_ec · mbridge_reward_end_card_shape_bg · mbridge_reward_end_close_shape_oval · mbridge_reward_end_land_shape · mbridge_reward_end_pager_logo · mbridge_reward_end_shape_oval · mbridge_reward_flag_cn · mbridge_reward_flag_en · mbridge_reward_more_offer_default_bg · mbridge_reward_notice
mbridge_reward_popview_close · mbridge_reward_shape_choice · mbridge_reward_shape_choice_rl · mbridge_reward_shape_end_pager · mbridge_reward_shape_mf_selector · mbridge_reward_shape_mof_like_normal · mbridge_reward_shape_mof_like_pressed · mbridge_reward_shape_order · mbridge_reward_shape_order_history · mbridge_reward_shape_progress · mbridge_reward_shape_videoend_buttonbg · mbridge_reward_sound_close
mbridge_reward_sound_open · mbridge_reward_two_title_arabia_land · mbridge_reward_two_title_arabia_por · mbridge_reward_two_title_en_land · mbridge_reward_two_title_en_por · mbridge_reward_two_title_france_land · mbridge_reward_two_title_france_por · mbridge_reward_two_title_germany_land · mbridge_reward_two_title_germany_por · mbridge_reward_two_title_japan_land · mbridge_reward_two_title_japan_por · mbridge_reward_two_title_korea_land
mbridge_reward_two_title_korea_por · mbridge_reward_two_title_russian_land · mbridge_reward_two_title_russian_por · mbridge_reward_two_title_zh · mbridge_reward_two_title_zh_trad · mbridge_reward_user · mbridge_reward_vast_end_close · mbridge_reward_vast_end_ok · mbridge_reward_video_icon · mbridge_reward_video_progress_bg · mbridge_reward_video_progressbar_bg · mbridge_reward_video_time_count_num_bg
mbridge_shape_btn · mbridge_shape_corners_bg · mbridge_shape_line · mbridge_shape_splash_circle_14 · mbridge_shape_splash_corners_14 · mbridge_shape_splash_rightbottom_corners_10 · mbridge_slide_hand · mbridge_slide_rightarrow · mbridge_splash_ad_right_bottom_corner_en · mbridge_splash_ad_right_bottom_corner_zh · mbridge_splash_btn_arrow_right · mbridge_splash_btn_circle
mbridge_splash_btn_finger · mbridge_splash_btn_go · mbridge_splash_btn_light · mbridge_splash_button_bg_gray · mbridge_splash_button_bg_gray_55 · mbridge_splash_button_bg_green · mbridge_splash_close_bg · mbridge_splash_m_circle · mbridge_splash_notice · mbridge_splash_pop_ad · mbridge_splash_pop_ad_en · mbridge_splash_popview_close
mbridge_splash_popview_default · mbridge_video_common_full_star · mbridge_video_common_full_while_star · mbridge_video_common_half_star · mobilefuse_close_btn · mobilefuse_vast_endcard_close_btn · mobilefuse_vast_endcard_transparent_close_btn · moloco_close · moloco_replay · moloco_star · moloco_twotone_pause_24 · moloco_twotone_play_arrow_24
moloco_twotone_volume_off_24 · moloco_twotone_volume_up_24 · moloco_volume_off · moloco_volume_on · monetization_ads_instream_internal_advertiser · monetization_ads_instream_internal_advertiser_social · monetization_ads_instream_internal_click_button · monetization_ads_instream_internal_progress_display · monetization_ads_instream_internal_progress_display_v2 · monetization_ads_instream_internal_skip · monetization_ads_instream_internal_skip_focused · monetization_ads_instream_internal_skip_unfocused
monetization_ads_internal_adtune_control_shape_white · monetization_ads_internal_adtune_webview_container_shape_white · monetization_ads_internal_adtune_white · monetization_ads_internal_adtune_white_background · monetization_ads_internal_app_install_circle_for_dark_background · monetization_ads_internal_app_install_circle_for_light_background · monetization_ads_internal_app_install_close_grey · monetization_ads_internal_app_install_close_white · monetization_ads_internal_app_install_feedback_grey · monetization_ads_internal_app_install_feedback_white · monetization_ads_internal_call_to_action_background · monetization_ads_internal_call_to_action_background_dark
monetization_ads_internal_call_to_action_background_light · monetization_ads_internal_call_to_action_background_shape_blue · monetization_ads_internal_call_to_action_blue_solid_background · monetization_ads_internal_call_to_action_blue_stroke_background · monetization_ads_internal_call_to_action_gradient_yellow_background · monetization_ads_internal_call_to_action_green_solid_background · monetization_ads_internal_circle_progress_bar · monetization_ads_internal_circular_close_progress · monetization_ads_internal_close_light · monetization_ads_internal_controls_background · monetization_ads_internal_default_adtune_feedback_icon · monetization_ads_internal_feed_progress
monetization_ads_internal_fullscreen_content_v1_background_tile · monetization_ads_internal_fullscreen_content_v1_background_tile_dark · monetization_ads_internal_fullscreen_content_v1_close_disabled · monetization_ads_internal_fullscreen_content_v1_close_enabled · monetization_ads_internal_fullscreen_content_v1_close_selector · monetization_ads_internal_fullscreen_content_v1_icon_background · monetization_ads_internal_google_play_icon_white · monetization_ads_internal_ic_close_gray · monetization_ads_internal_ic_sound_off_app_install · monetization_ads_internal_ic_sound_off_default · monetization_ads_internal_ic_sound_off_default_focused · monetization_ads_internal_ic_sound_off_default_unfocused
monetization_ads_internal_ic_sound_on_app_install · monetization_ads_internal_ic_sound_on_default · monetization_ads_internal_ic_sound_on_default_focused · monetization_ads_internal_ic_sound_on_default_unfocused · monetization_ads_internal_icon_empty · monetization_ads_internal_icon_grey_solid_background_large_corners · monetization_ads_internal_icon_grey_solid_background_small_corners · monetization_ads_internal_instream_adtune_control_v2 · monetization_ads_internal_instream_adtune_control_v2_focused · monetization_ads_internal_instream_adtune_control_v2_unfocused · monetization_ads_internal_instream_advertiser_block_background · monetization_ads_internal_instream_advertiser_block_background_focused
monetization_ads_internal_instream_advertiser_block_background_unfocused · monetization_ads_internal_instream_call_to_action_background · monetization_ads_internal_instream_ic_sound_off_v2 · monetization_ads_internal_instream_ic_sound_on_v2 · monetization_ads_internal_instream_mute_checkable_v2 · monetization_ads_internal_instream_skip_arrow · monetization_ads_internal_landscape_close_button · monetization_ads_internal_multibanner_left_button_icon · monetization_ads_internal_multibanner_right_button_icon · monetization_ads_internal_mute_checkable_app_install · monetization_ads_internal_mute_checkable_default · monetization_ads_internal_mute_checkable_fullscreen_content_v1
monetization_ads_internal_qrcode_container_background · monetization_ads_internal_qrcode_view_background · monetization_ads_internal_rating_bar · monetization_ads_internal_reward_timer_view_shape_gray · monetization_ads_internal_rewarded_close · monetization_ads_internal_rewarded_close_shape_gray · monetization_ads_internal_rewarded_close_verification_background · monetization_ads_internal_rewarded_close_verification_button_close_background · monetization_ads_internal_rewarded_close_verification_button_dismiss_background · monetization_ads_internal_rewarded_close_with_padding · monetization_ads_internal_rewarded_feedback · monetization_ads_internal_rewarded_feedback_with_padding
monetization_ads_internal_rewarded_timer_background · monetization_ads_internal_sponsored_container_background · monetization_ads_internal_star_black · monetization_ads_internal_star_half_black · monetization_ads_mediation_api_feedback_icon · monetization_ads_progress_view_background · monetization_ads_video_progress_bar_background · mtrl_bottomsheet_drag_handle · mtrl_checkbox_button · mtrl_checkbox_button_checked_unchecked · mtrl_checkbox_button_icon · mtrl_checkbox_button_icon_checked_indeterminate
mtrl_checkbox_button_icon_checked_unchecked · mtrl_checkbox_button_icon_indeterminate_checked · mtrl_checkbox_button_icon_indeterminate_unchecked · mtrl_checkbox_button_icon_unchecked_checked · mtrl_checkbox_button_icon_unchecked_indeterminate · mtrl_checkbox_button_unchecked_checked · mtrl_dialog_background · mtrl_dropdown_arrow · mtrl_ic_arrow_drop_down · mtrl_ic_arrow_drop_up · mtrl_ic_cancel · mtrl_ic_check_mark
mtrl_ic_checkbox_checked · mtrl_ic_checkbox_unchecked · mtrl_ic_error · mtrl_ic_indeterminate · mtrl_navigation_bar_item_background · mtrl_popupmenu_background · mtrl_popupmenu_background_overlay · mtrl_switch_thumb · mtrl_switch_thumb_checked · mtrl_switch_thumb_checked_pressed · mtrl_switch_thumb_checked_unchecked · mtrl_switch_thumb_pressed
mtrl_switch_thumb_pressed_checked · mtrl_switch_thumb_pressed_unchecked · mtrl_switch_thumb_unchecked · mtrl_switch_thumb_unchecked_checked · mtrl_switch_thumb_unchecked_pressed · mtrl_switch_track · mtrl_switch_track_decoration · mtrl_tabs_default_indicator · native_animation_background · navigation_empty_icon · nbt_byte · nbt_byte_arr
nbt_compound · nbt_double · nbt_float · nbt_int · nbt_int_arr · nbt_list · nbt_long · nbt_short · nbt_string · neutral_counter_background · notification_action_background · notification_bg
notification_bg_low · notification_icon_background · notification_oversize_large_icon_bg · notification_template_icon_bg · notification_template_icon_low_bg · notification_tile_bg · offline_dialog_background · offline_dialog_default_icon_42dp · open_url_background · pn_circular_progress · pob_close_button · pob_ic_action_back
pob_ic_action_cancel · pob_ic_action_forward · pob_ic_action_refresh · pob_ic_action_web_site · pob_install_btn_drawable · pob_template_button_drawable · preference_list_divider_material · question_mark · rab_action_bg · rab_badge_bg · round_18_ec3a48 · round_7_white_bg
round_80000000_100 · round_80000000_3 · round_9_black_bg · round_b3000000 · round_b4001b · round_bottom_b2c4ff00 · round_edc900_100 · round_edc900_3 · round_ff81a700 · round_ff81a700_100 · round_gray · round_white_bg
seekbar_progress_drawable · sentry_edit_text_border · sentry_logo_dark · sentry_oval_button_ripple_background · sentry_user_feedback_button_logo_24 · smaato_sdk_core_browser_bottom_button_layout_bg · smaato_sdk_core_browser_progress_bar · smaato_sdk_core_browser_top_button_layout_bg · smaato_sdk_core_count_down_bg · smaato_sdk_core_ic_browser_background_selector · smaato_sdk_core_ic_browser_backward_selector · smaato_sdk_core_ic_browser_forward_selector
smaato_sdk_core_ic_browser_secure_connection · smaato_sdk_core_progress_bar · smaato_sdk_core_video_progress_bar · step1_tut · step2_tut · step3_tut · step4_tut · steve · table_cell_background · taurusx_close_no_bg · taurusx_close_pressed · taurusx_ic_close
taurusx_ic_more · taurusx_ic_more_no_bg · taurusx_ic_privacy · taurusx_ic_reload · taurusx_inner_btn_bg_pressed · taurusx_inner_video_mute · taurusx_inner_video_no_mute · taurusx_inner_video_skip · taurusx_progress_rotate · taurusx_skip_pressed · taurusx_tax_btn_dark_bg · taurusx_tax_ic_skip
taurusx_tax_ic_volume_off · taurusx_tax_ic_volume_on · taurusx_tax_round_36_bg · taurusx_tax_round_36_bg2 · taurusx_tax_round_white_bg · taurusx_tax_volume_bg · test_level_drawable · tile · tiles · timer_count_down_background · tooltip_frame_dark · tooltip_frame_light
tt_ad_arrow_backward · tt_ad_arrow_forward · tt_ad_arrow_right · tt_ad_bg_header_gradient · tt_ad_closed_background_300_250 · tt_ad_closed_background_320_50 · tt_ad_feedback · tt_ad_feedback_new · tt_ad_link · tt_ad_logo · tt_ad_refresh · tt_ad_threedots
tt_ad_xmark · tt_ad_zoom_down · tt_ad_zoom_up · tt_app_open_top_bg · tt_blue_hand · tt_button_back · tt_button_blue_back · tt_close_btn · tt_close_move_details_normal · tt_close_move_details_pressed · tt_dislike_icon · tt_dislike_icon2
tt_dislike_toast · tt_enlarge_video · tt_item_background_material · tt_lefterbackicon_titlebar · tt_lefterbackicon_titlebar_for_dark · tt_lefterbackicon_titlebar_press · tt_lefterbackicon_titlebar_press_for_dark · tt_mute · tt_new_pause_video · tt_new_pause_video_press · tt_new_play_video · tt_normalscreen_loading
tt_pangle_ad_close_drawable · tt_refreshing_video_textpage_normal · tt_refreshing_video_textpage_pressed · tt_report_ad_arrow · tt_reward_full_feedback · tt_reward_full_mute · tt_reward_full_unmute · tt_shadow_fullscreen_top · tt_shadow_lefterback_titlebar · tt_shadow_lefterback_titlebar_press · tt_shadow_lefterback_titlebar_press_withoutnight · tt_shadow_lefterback_titlebar_withoutnight
tt_shrink_fullscreen · tt_shrink_video · tt_skip_btn · tt_splash_hand · tt_splash_hand2 · tt_splash_rock · tt_splash_shake_hand · tt_splash_slide_right_bg · tt_splash_slide_right_circle · tt_splash_slide_up_10 · tt_splash_slide_up_arrow · tt_splash_slide_up_bg
tt_splash_slide_up_circle · tt_splash_slide_up_finger · tt_splash_twist · tt_splash_unlock_btn_bg · tt_splash_unlock_icon_empty · tt_splash_unlock_image_arrow · tt_splash_unlock_image_go · tt_star · tt_star_thick · tt_star_thick_dark · tt_suggestion_logo · tt_titlebar_close_drawable
tt_titlebar_close_for_dark · tt_titlebar_close_press · tt_titlebar_close_press_for_dark · tt_ugen_rating_star · tt_unmute · tt_up_slide · tt_user · tt_user_info · tt_video_close_drawable · tt_white_hand · tt_white_lefterbackicon_titlebar · tt_white_lefterbackicon_titlebar_press
tt_white_righterbackicon_titlebar · tt_white_slide_up · tt_wriggle_union · tt_wriggle_union_white · warning_counter_background · warning_error_counter_background
```

**Drawable milik aplikasi (penting):** `ic_add`, `ic_layers`, `ic_upload`, `ic_map`, `ic_tune`, `ic_person`, `ic_group_people`, `ic_exit`, `ic_list`, `ic_search`, `ic_book`, `ic_info`, `ic_save`, `fab_label_background`, `step1_tut`–`step4_tut`, `steve`, `nbt_byte`, `nbt_byte_arr`, `nbt_compound`, `nbt_double`, `nbt_float`, `nbt_int`, `nbt_int_arr`, `nbt_list`, `nbt_long`, `nbt_short`, `nbt_string`, `test_level_drawable`, `tile`, `tiles`, `question_mark`, `home`, `ic_100tb`.

### 3.5 Mipmap, raw, xml, font

| Mipmap (9) | `close`, `ic_launcher`, `ic_launcher_background`, `ic_launcher_foreground`, `ic_launcher_round`, `mute`, `replay`, `skip`, `unmute` |
|---|---|
| **Raw (17)** | `applovin_consent_flow_unified_cmp`, `com_android_billingclient_heterodyne_info`, `com_android_billingclient_registration_info`, `html_player_vast`, `keep_cronet_api`, `license`, `mobilefuse_omsdk_license`, `mobilefuse_omsdk_v1`, `monetization_ads_bundled_cert`, `monetization_ads_keep`, `monetization_ads_sdkinternalca`, `omid_session_client_v1_5_3`, `omsdk_v1`, `omsdk_v1_5_3`, `russian_trusted_root_ca`, `russian_trusted_sub_ca`, `shape_corner` |
| **XML (13)** | `ad_services_config`, `com_android_billingclient_phenotype`, `debug_panel_file_paths`, **`file_paths`**, `image_share_filepaths`, `m3_button_group_child_size_change`, `m3_split_button_inner_corner_size_state_list`, `m3expressive_button_shape_state_list`, `m3expressive_connected_buttons_inner_corner_size_state_list`, `nath_tax_file_path`, **`network_security_config`**, `progress_bg`, `splits0` |
| **Font (11)** | `inter` (**font aplikasi**), `roboto_medium_numbers`, `monetization_ads_internal_font_*` (8 varian: bold, light, medium, regular, text_bold, text_light, text_medium, text_regular, text_regular_italic) |

`@xml/file_paths` (dari xmltree): `<paths><external-files-path name="external_files" path="." /></paths>` — akar direktori `getExternalFilesDir()` diekspos untuk FileProvider (digunakan “Open in Minecraft” dan ekspor texture pack).

### 3.6 SELURUH 742 string

```text
CronetProviderClassName · abc_action_bar_home_description · abc_action_bar_up_description · abc_action_menu_overflow_description · abc_action_mode_done · abc_activity_chooser_view_see_all · abc_activitychooserview_choose_application · abc_capital_off · abc_capital_on · abc_menu_alt_shortcut_label · abc_menu_ctrl_shortcut_label · abc_menu_delete_shortcut_label · abc_menu_enter_shortcut_label · abc_menu_function_shortcut_label
abc_menu_meta_shortcut_label · abc_menu_shift_shortcut_label · abc_menu_space_shortcut_label · abc_menu_sym_shortcut_label · abc_prepend_shortcut_label · abc_search_hint · abc_searchview_description_clear · abc_searchview_description_query · abc_searchview_description_search · abc_searchview_description_submit · abc_searchview_description_voice · abc_shareactionprovider_share_with · abc_shareactionprovider_share_with_application · abc_toolbar_collapse_description
ad_customisation_title · ad_mob_ad_badge · ad_units · ads_sdk_version · advanced_section · advertisement_network_settings · age_restricted_user · alphanumeric_only_error · androidx_startup · apd_admob_banner_ad_text · apd_admob_banner_body_tools · apd_admob_banner_cta_tools · apd_admob_banner_headline_tools · apd_admob_banner_icon_decs
api_level · app_name · app_review_status · app_version · appbar_scrolling_view_behavior · application_info · applovin_agree_message · applovin_alt_privacy_policy_text · applovin_continue_button_text · applovin_creative_debugger_disabled_text · applovin_creative_debugger_no_ads_text · applovin_list_item_image_description · applovin_pp_and_tos_title · applovin_pp_title
applovin_privacy_policy_text · applovin_terms_of_service_text · applovin_terms_of_use_text · aps_mraid_close_button · autonomous_entities_nbt · bigo_ad_about_app_default · bigo_ad_click_to_try · bigo_ad_comment_num_text · bigo_ad_cta_default · bigo_ad_cta_download_default · bigo_ad_description_default · bigo_ad_double_video_choose_video · bigo_ad_double_video_count_down · bigo_ad_download_num_text
bigo_ad_feedback_copied · bigo_ad_feedback_copy_ad_id · bigo_ad_feedback_copy_link · bigo_ad_feedback_hide_ads · bigo_ad_feedback_link_copied · bigo_ad_feedback_rec_rule · bigo_ad_feedback_report_ads · bigo_ad_feedback_user_privacy · bigo_ad_feedback_why_this_ad · bigo_ad_form_agree · bigo_ad_form_cancel · bigo_ad_form_privacy_content · bigo_ad_form_privacy_notice · bigo_ad_form_question_hint
bigo_ad_form_submit · bigo_ad_form_warning · bigo_ad_ic_application · bigo_ad_ic_blank · bigo_ad_ic_everyone · bigo_ad_ic_recommended · bigo_ad_ic_web · bigo_ad_interaction_jump_hint · bigo_ad_interaction_shake · bigo_ad_interaction_slide · bigo_ad_interaction_twist · bigo_ad_link_copied · bigo_ad_lose_reward · bigo_ad_multi_img_more_text
bigo_ad_resume · bigo_ad_skip · bigo_ad_skip_video · bigo_ad_splash_skip · bigo_ad_splash_skip_after · bigo_ad_tag · bigo_ad_tag_back · bigo_ad_tag_close · bigo_ad_tag_forward · bigo_ad_tag_progressbar · bigo_ad_tag_title · bigo_ad_title_default · bigo_ad_webview_loading · biomedata_nbt
bottom_sheet_behavior · bottomsheet_action_collapse · bottomsheet_action_expand · bottomsheet_action_expand_halfway · bottomsheet_drag_handle_content_description · browser_hostname_content_description · btn_browser_backward_content_description · btn_browser_close_content_description · btn_browser_forward_content_description · btn_browser_open_content_description · btn_browser_refresh_content_description · call_notification_answer_action · call_notification_answer_video_action · call_notification_decline_action
call_notification_hang_up_action · call_notification_incoming_text · call_notification_ongoing_text · call_notification_screening_text · campaign.appName · campaign.iconUrl · campaign.imageUrl · change_world · character_counter_content_description · character_counter_overflowed_content_description · character_counter_pattern · clear · clear_color · clear_text_end_icon_content_description
close_button_description · close_card_votes · close_drawer · close_sheet · color_picker · com_moloco_sdk_xenoss_player_learn_more · com_moloco_sdk_xenoss_player_skip · common_google_play_services_enable_button · common_google_play_services_enable_text · common_google_play_services_enable_title · common_google_play_services_install_button · common_google_play_services_install_text · common_google_play_services_install_title · common_google_play_services_notification_channel_name
common_google_play_services_notification_ticker · common_google_play_services_unknown_issue · common_google_play_services_unsupported_text · common_google_play_services_update_button · common_google_play_services_update_text · common_google_play_services_update_title · common_google_play_services_updating_text · common_google_play_services_wear_update_text · common_open_on_phone · common_signin_button_text · common_signin_button_text_long · completed_integration · content_info_icon · copy
copy_toast_msg · custom_cta_ad_customisation_label · custom_cta_button · custom_cta_label · date · debug_panel_ad_unit_format · debug_panel_ad_unit_id · default_error_message · default_popup_window_title · defaults · disable · div_slider_range_end · div_slider_range_start · do_you_want_to_enable_logging
dropdown_menu · dyAction.getClick · dyAction.getLogicClick · dyAction.getLongClick · dyAction.getMove · dyAction.getWobble · dyEffect.getCountDown · dyEffect.getVisible · dyEffect.getVisibleParam · dyEffect.getWobble · dyStrategy.feedback · dyStrategy.getActivity · dyStrategy.getClose · dyStrategy.getDeeplink
dyStrategy.getDownload · dyStrategy.notice · dyStrategy.permissionInfo · dyStrategy.privateAddress · email_address · enable · error_a11y_label · error_icon_content_description · exo_controls_cc_disabled_description · exo_controls_cc_enabled_description · exo_controls_custom_playback_speed · exo_controls_fastforward_description · exo_controls_fullscreen_enter_description · exo_controls_fullscreen_exit_description
exo_controls_hide · exo_controls_next_description · exo_controls_overflow_hide_description · exo_controls_overflow_show_description · exo_controls_pause_description · exo_controls_play_description · exo_controls_playback_speed · exo_controls_previous_description · exo_controls_repeat_all_description · exo_controls_repeat_off_description · exo_controls_repeat_one_description · exo_controls_rewind_description · exo_controls_seek_bar_description · exo_controls_settings_description
exo_controls_show · exo_controls_shuffle_off_description · exo_controls_shuffle_on_description · exo_controls_stop_description · exo_controls_time_placeholder · exo_controls_vr_description · exo_download_completed · exo_download_description · exo_download_downloading · exo_download_failed · exo_download_notification_channel_name · exo_download_paused · exo_download_paused_for_network · exo_download_paused_for_wifi
exo_download_removing · exo_item_list · exo_track_bitrate · exo_track_mono · exo_track_resolution · exo_track_role_alternate · exo_track_role_closed_captions · exo_track_role_commentary · exo_track_role_supplementary · exo_track_selection_auto · exo_track_selection_none · exo_track_selection_title_audio · exo_track_selection_title_text · exo_track_selection_title_video
exo_track_stereo · exo_track_surround · exo_track_surround_5_point_1 · exo_track_surround_7_point_1 · exo_track_unknown · exo_track_unknown_name · expand_button_title · exposed_dropdown_menu_content_description · fab_transformation_scrim_behavior · fab_transformation_sheet_behavior · fallback_menu_item_copy_link · fallback_menu_item_open_in_browser · fallback_menu_item_share_link · features
feedback_form · has_location_consent · has_user_consent · hex · hide_bottom_view_on_scroll_behavior · ia_ad_identifier · ia_skip_rewarded_dialog_close_button · ia_skip_rewarded_dialog_keep_watching · ia_skip_rewarded_dialog_sub_title · ia_skip_rewarded_dialog_title · ia_str_video_error · ia_video_app_info_text · ia_video_before_skip_format · ia_video_install_now_text
ia_video_instant_install_text · ia_video_skip_text · icon_content_description · in_progress · indeterminate · info_icon_view_description · input_not_a_given_option · integrated · integration_errors · invalid_email_address · invalid_integration · invalid_mediation_adapter_version · item_view_role_description · learn_more
letters_only_error · loading · logging_is_disabled · logging_is_enabled · login · m3_exceed_max_badge_text_suffix · m3_loading_indicator_content_description · m3_ref_typeface_brand_medium · m3_ref_typeface_brand_regular · m3_ref_typeface_plain_medium · m3_ref_typeface_plain_regular · m3_sys_motion_easing_emphasized · m3_sys_motion_easing_emphasized_accelerate · m3_sys_motion_easing_emphasized_decelerate
m3_sys_motion_easing_emphasized_path_data · m3_sys_motion_easing_legacy · m3_sys_motion_easing_legacy_accelerate · m3_sys_motion_easing_legacy_decelerate · m3_sys_motion_easing_linear · m3_sys_motion_easing_standard · m3_sys_motion_easing_standard_accelerate · m3_sys_motion_easing_standard_decelerate · mSplashData.setAdClickText · mSplashData.setAppInfo · mSplashData.setCountDownText · mSplashData.setLogoImage · mSplashData.setLogoText · mSplashData.setNoticeImage
material_clock_display_divider · material_clock_toggle_content_description · material_hour_24h_suffix · material_hour_selection · material_hour_suffix · material_minute_selection · material_minute_suffix · material_motion_easing_accelerated · material_motion_easing_decelerated · material_motion_easing_emphasized · material_motion_easing_linear · material_motion_easing_standard · material_slider_range_end · material_slider_range_start
material_slider_value · material_timepicker_am · material_timepicker_clock_mode_description · material_timepicker_hour · material_timepicker_minute · material_timepicker_pm · material_timepicker_select_time · material_timepicker_text_input_mode_description · mbridge_cm_feedback_btn_text · mbridge_cm_feedback_dialog_close_close · mbridge_cm_feedback_dialog_close_submit · mbridge_cm_feedback_dialog_content_fraud · mbridge_cm_feedback_dialog_content_misleading · mbridge_cm_feedback_dialog_content_not_play
mbridge_cm_feedback_dialog_content_other · mbridge_cm_feedback_dialog_content_por_violence · mbridge_cm_feedback_dialog_content_sound_problems · mbridge_cm_feedback_dialog_privacy_des · mbridge_cm_feedback_dialog_submit_notice · mbridge_cm_feedback_dialog_title · mbridge_download_notify_cancel · mbridge_download_notify_continue · mbridge_download_notify_download_failed · mbridge_download_notify_download_finish_to_install · mbridge_download_notify_download_interrupt · mbridge_download_notify_downloading · mbridge_download_notify_ins · mbridge_download_notify_ins_progress_des
mbridge_download_notify_network_error · mbridge_download_notify_open · mbridge_download_notify_open_progress_des · mbridge_download_notify_pause · mbridge_download_notify_prefix_download · mbridge_download_notify_prefix_pause · mbridge_download_notify_start_download · mbridge_download_notify_update_finish_to_install · mbridge_download_notify_update_package · mbridge_reward_appdesc · mbridge_reward_apptitle · mbridge_reward_clickable_cta_btntext · mbridge_reward_endcard_ad · mbridge_reward_endcard_vast_notice
mbridge_reward_heat_count_unit · mbridge_reward_install · mbridge_reward_video_view_reward_time_complete · mbridge_reward_video_view_reward_time_left · mbridge_reward_video_view_reward_time_left_skip_time · mbridge_reward_viewed_text_str · mbridge_splash_count_time_can_skip · mbridge_splash_count_time_can_skip_not · mbridge_splash_count_time_can_skip_s · missing_integration · moloco_sponsored · monetization_ads_internal_after_timer_value · monetization_ads_internal_before_timer_value · monetization_ads_internal_go
monetization_ads_internal_install · monetization_ads_internal_install_from_google_play · monetization_ads_internal_instream_ad_position · monetization_ads_internal_instream_call_to_action · monetization_ads_internal_instream_feedback · monetization_ads_internal_instream_muted · monetization_ads_internal_instream_skip · monetization_ads_internal_instream_sponsored_default · monetization_ads_internal_instream_sponsored_social · monetization_ads_internal_instream_unmuted · monetization_ads_internal_mediation_sponsored_text · monetization_ads_internal_rewarded_close_verification_button_close · monetization_ads_internal_rewarded_close_verification_button_dismiss · monetization_ads_internal_rewarded_close_verification_text
monetization_ads_internal_rewarded_close_verification_title · monetization_ads_internal_scan_qrcode · monetization_ads_internal_text_divider · monetization_ads_internal_text_divider_with_space · monetization_ads_internal_tools_age_text · monetization_ads_internal_tools_app_install_call_to_action_text · monetization_ads_internal_tools_app_install_call_to_action_text_large · monetization_ads_internal_tools_body_text · monetization_ads_internal_tools_call_to_action_text · monetization_ads_internal_tools_domain_text · monetization_ads_internal_tools_price_text · monetization_ads_internal_tools_rating_text · monetization_ads_internal_tools_review_count_text · monetization_ads_internal_tools_sponsored_text
monetization_ads_internal_tools_title_text · monetization_ads_internal_tools_warning_text · monetization_ads_mediation_api_sponsored_text · mtrl_badge_numberless_content_description · mtrl_button_collapsed_content_description · mtrl_button_expanded_content_description · mtrl_checkbox_button_icon_path_checked · mtrl_checkbox_button_icon_path_group_name · mtrl_checkbox_button_icon_path_indeterminate · mtrl_checkbox_button_icon_path_name · mtrl_checkbox_button_path_checked · mtrl_checkbox_button_path_group_name · mtrl_checkbox_button_path_name · mtrl_checkbox_button_path_unchecked
mtrl_checkbox_state_description_checked · mtrl_checkbox_state_description_indeterminate · mtrl_checkbox_state_description_unchecked · mtrl_chip_close_icon_content_description · mtrl_exceed_max_badge_number_content_description · mtrl_exceed_max_badge_number_suffix · mtrl_picker_a11y_next_month · mtrl_picker_a11y_prev_month · mtrl_picker_announce_current_range_selection · mtrl_picker_announce_current_selection · mtrl_picker_announce_current_selection_none · mtrl_picker_cancel · mtrl_picker_confirm · mtrl_picker_date_header_selected
mtrl_picker_date_header_title · mtrl_picker_date_header_unselected · mtrl_picker_day_of_week_column_header · mtrl_picker_end_date_description · mtrl_picker_invalid_format · mtrl_picker_invalid_format_example · mtrl_picker_invalid_format_use · mtrl_picker_invalid_range · mtrl_picker_navigate_to_current_year_description · mtrl_picker_navigate_to_current_year_description · mtrl_picker_out_of_range · mtrl_picker_range_header_only_end_selected · mtrl_picker_range_header_only_start_selected · mtrl_picker_range_header_selected
mtrl_picker_range_header_title · mtrl_picker_range_header_unselected · mtrl_picker_save · mtrl_picker_start_date_description · mtrl_picker_text_input_date_hint · mtrl_picker_text_input_date_range_end_hint · mtrl_picker_text_input_date_range_start_hint · mtrl_picker_text_input_day_abbr · mtrl_picker_text_input_month_abbr · mtrl_picker_text_input_year_abbr · mtrl_picker_today_description · mtrl_picker_toggle_to_calendar_input_mode · mtrl_picker_toggle_to_day_selection · mtrl_picker_toggle_to_text_input_mode
mtrl_picker_toggle_to_year_selection · mtrl_picker_toggled_to_day_selection · mtrl_picker_toggled_to_year_selection · mtrl_switch_thumb_group_name · mtrl_switch_thumb_path_checked · mtrl_switch_thumb_path_morphing · mtrl_switch_thumb_path_name · mtrl_switch_thumb_path_pressed · mtrl_switch_thumb_path_unchecked · mtrl_switch_track_decoration_path · mtrl_switch_track_path · mtrl_timepicker_cancel · mtrl_timepicker_confirm · multiplayer_nbt
mute_button · name · native_body · native_headline · native_media_view · nav_rail_collapsed_a11y_label · nav_rail_expanded_a11y_label · navigation_drawer_close · navigation_drawer_open · navigation_menu · nether_nbt · no · no_external_browser_found · no_value_set
not_integrated · not_selected · not_set · notifications_permission_confirm · notifications_permission_decline · notifications_permission_title · off · offline_dialog_image_description · offline_dialog_text · offline_notification_title · offline_notification_title_with_advertiser · offline_opt_in_confirm · offline_opt_in_decline · offline_opt_in_message
offline_opt_in_title · ok · on · open_app_button_description · open_app_custom_button_description · open_custom_nbt · openwrap_skip_dialog_close_btn · openwrap_skip_dialog_message · openwrap_skip_dialog_resume_btn · openwrap_skip_dialog_title · overworld_nbt · password · password_toggle_content_description · path_password_eye
path_password_eye_mask_strike_through · path_password_eye_mask_visible · path_password_strike_through · persistent_cta_description · phone_number · pin · pob_openwrap_install_button_title · portals_nbt · preference_copied · provided · range_end · range_start · required · reward_timer_complete
reward_timer_running · rrggbb · s1 · s2 · s3 · s4 · s5 · s6 · s7 · sdk_integration · sdk_integration_status · sdk_undefined · search_menu_title · searchbar_scrolling_view_behavior
searchview_clear_text_content_description · searchview_navigation_content_description · selected · show_map · side_sheet_accessibility_pane_title · side_sheet_behavior · singleplayer_nbt · skip_ad · skip_button_description · smaato_sdk_core_browser_hostname_content_description · smaato_sdk_core_btn_browser_backward_content_description · smaato_sdk_core_btn_browser_close_content_description · smaato_sdk_core_btn_browser_forward_content_description · smaato_sdk_core_btn_browser_open_content_description
smaato_sdk_core_btn_browser_refresh_content_description · smaato_sdk_core_fullscreen_dimension · smaato_sdk_core_no_external_browser_found · smaato_sdk_richmedia_collapse_mraid_ad · smaato_sdk_video_close_button_text · smaato_sdk_video_mute_button_text · smaato_sdk_video_skip_button_text · sponsored_text · status_bar_notification_info_overflow · strong_pw_requirements · summary_collapsed_preference_list · system · tab · taurusx_ad
taurusx_ads_know_more · taurusx_ads_skip · taurusx_ads_sponsored · tcf_consent · template_cta_max_lines · template_desc_max_lines · template_percent · template_title_max_lines · the_end_nbt · time · timer_notification_icon_description · tracer_mapping_uuid · transparency · tt_00_00
tt_ad_close_text · tt_ad_data_error · tt_ad_is_closed · tt_ad_logo_txt · tt_add_bad_reason · tt_adslot_empty · tt_adslot_id_error · tt_adslot_size_empty · tt_app_empty · tt_bad · tt_banner_ad_load_image_error · tt_choose_language · tt_comment_num · tt_content_type
tt_count_down_view · tt_dislike_header_tv_back · tt_dislike_header_tv_title · tt_display_error · tt_done · tt_error_access_method_pass · tt_error_ad_able_false_msg · tt_error_ad_type · tt_error_adtype_differ · tt_error_apk_sign_check_error · tt_error_bidding_type · tt_error_code_adcount_error · tt_error_code_click_event_error · tt_error_device_ip
tt_error_empty_content · tt_error_image_size · tt_error_interstitial_version · tt_error_media_id · tt_error_media_type · tt_error_new_register_limit · tt_error_origin_ad_error · tt_error_package_name · tt_error_redirect · tt_error_request_invalid · tt_error_slot_id_app_id_differ · tt_error_splash_ad_type · tt_error_union_os_error · tt_error_union_sdk_too_old
tt_error_unknow · tt_error_update_version · tt_error_verify_reward · tt_feedback_experience_text · tt_feedback_submit_text · tt_feedback_thank_text · tt_feel_hint · tt_frequent_call_erroe · tt_full_screen_skip_tx · tt_good · tt_init_setting_config_not_complete · tt_insert_ad_load_image_error · tt_lack_android_manifest_configuration · tt_like_this_ad
tt_list_end_tip · tt_load_creative_icon_error · tt_load_creative_icon_response_error · tt_loading_language · tt_loading_more · tt_logo_cn · tt_logo_en · tt_msgPlayable · tt_multiple_ad_indicator · tt_multiple_playable_next_ad_tips · tt_multiple_playable_wait_tips · tt_negtiveBtnBtnText · tt_negtive_txt · tt_net_error
tt_no_ad · tt_no_ad_parse · tt_no_network · tt_not_bad · tt_other_reason · tt_parse_fail · tt_postiveBtnText · tt_postiveBtnTextPlayable · tt_postive_txt · tt_privacy_title · tt_reder_ad_load_timeout · tt_render_diff_template_invalid · tt_render_fail_meta_invalid · tt_render_fail_template_parse_error
tt_render_fail_timeout · tt_render_fail_unknown · tt_render_main_template_invalid · tt_render_render_parse_error · tt_report_this_ad · tt_request_body_error · tt_request_pb_error · tt_reward_feedback · tt_reward_full_skip · tt_reward_msg · tt_reward_screen_skip_tx · tt_ror_code_show_event_error · tt_select_reason · tt_skip_ad_time_text
tt_slide_up_3d · tt_splash_ad_load_image_error · tt_splash_brush_mask_hint · tt_splash_brush_mask_title · tt_splash_cache_expired_error · tt_splash_cache_parse_error · tt_splash_default_click_shake · tt_splash_not_have_cache_error · tt_splash_rock_text · tt_splash_rock_top_text · tt_splash_wriggle_text · tt_splash_wriggle_top_text · tt_splash_wriggle_top_text_style_17 · tt_suggestion_commit
tt_sys_error · tt_template_load_fail · tt_text_privacy_app_version · tt_text_privacy_development · tt_try_now · tt_video_bytesize · tt_video_dial_phone · tt_video_download_apk · tt_video_mobile_go_detail · tt_video_retry_des_txt · tt_video_without_wifi_tips · tt_wap_empty · tt_web_title_default · unmute_button
user · user_privacy · v7_preference_off · v7_preference_on · volume_off_description · volume_on_description · watermark_label_prefix · world_nbt · wrong_pin · yandex_mobileads_age_restricted_user · yandex_mobileads_appmetrica_easy_integration_enabled · yandex_mobileads_automatic_sdk_initialization · yandex_mobileads_enable_logging · yes
```

#### String-string milik aplikasi beserta nilainya (terverifikasi)

| Nama | Nilai | ID |
|---|---|---|
| `app_name` | `MineTuner` | `0x7f13002b` |
| `show_map` | `Show Map` | `0x7f130233` |
| `world_nbt` | `World NBT` | `0x7f1302df` |
| `singleplayer_nbt` | `Singleplayer NBT` | `0x7f130236` |
| `multiplayer_nbt` | `Multiplayer NBT` | `0x7f1301e9` |
| `change_world` | `Change World` | `0x7f130085` |
| `advanced_section` | `Advanced` | `0x7f130020` |
| `open_custom_nbt` | `Open Custom NBT` | `0x7f13020b` |
| `overworld_nbt` | `Overworld NBT` | `0x7f130210` |
| `the_end_nbt` | `The End NBT` | `0x7f130254` |
| `nether_nbt` | `Nether NBT` | `0x7f1301f4` |
| `biomedata_nbt` | `BiomeData NBT` | `0x7f13006f` |
| `portals_nbt` | `Portals NBT` | `0x7f13021b` |
| `autonomous_entities_nbt` | `Autonomous Entities NBT` | `0x7f13003c` |
| `close_drawer` | `Close navigation menu` | `0x7f13008e` |
| `navigation_drawer_open/close` | `Open` / `Close` | `0x7f1301f2` / `0x7f1301f1` |
| `not_selected` / `selected` | `Not selected` / `Selected` | `0x7f1301f9` / `0x7f130232` |
| `default_error_message` | `Invalid input` | `0x7f1300af` |
| `alphanumeric_only_error` | `May only contain alphanumerical chars` | `0x7f130023` |
| `letters_only_error` | `May only contain letters (A-Z)` | `0x7f130126` |
| `name` / `date` / `time` / `hex` | `Name` / `Date` / `Time` / `HEX:` | — |
| `appbar_scrolling_view_behavior` | `com.google.android.material.appbar.AppBarLayout$ScrollingViewBehavior` | `0x7f13002e` |
| `tracer_mapping_uuid` | `360b25e0-5e45-11f0-bb63-3e3c84784f26` | `0x7f13024b-ish` (Appodeal) |

String hardcoded di layout (bukan resource): toolbar **“Exporting Worlds”**, **“Create Flat World”**, **“Chunk Entities”**, **“New World”**, **“Import World”**, **“Help”**, **“About”**, **“Save”**, **“Selected Biome”**, **“Prevent Slimes from Spawning”**, **“Flat World Layers”**, langkah 1–5 tutorial texture pack.

### 3.7 SELURUH 1.208 color

```text
POBTemplateBackground · POBTemplateButtonColor · POBTemplateTextColor · abc_background_cache_hint_selector_material_dark · abc_background_cache_hint_selector_material_light · abc_btn_colored_borderless_text_material · abc_btn_colored_text_material · abc_color_highlight_material · abc_decor_view_status_guard · abc_decor_view_status_guard_light · abc_hint_foreground_material_dark · abc_hint_foreground_material_light · abc_primary_text_disable_only_material_dark · abc_primary_text_disable_only_material_light
abc_primary_text_material_dark · abc_primary_text_material_light · abc_search_url_text · abc_search_url_text_normal · abc_search_url_text_pressed · abc_search_url_text_selected · abc_secondary_text_material_dark · abc_secondary_text_material_light · abc_tint_btn_checkable · abc_tint_default · abc_tint_edittext · abc_tint_seek_thumb · abc_tint_spinner · abc_tint_switch_track
accent_material_dark · accent_material_light · ad_mob_ad_badge_green · ad_mob_button_blue · ad_mob_button_light_blue · ad_mob_outline · ad_mob_white · androidx_core_ripple_material_light · androidx_core_secondary_text_default_material_light · apd_admob_banner_ad_background · apd_admob_banner_ad_text_color · apd_admob_banner_background · apd_admob_banner_cta_color · apd_admob_banner_cta_text_color
apd_admob_banner_headline_text_color · apd_admob_banner_icon_tools · applovin_sdk_adBadgeTextColor · applovin_sdk_adControlbutton_brightBlueColor · applovin_sdk_brand_color · applovin_sdk_brand_color_dark · applovin_sdk_checkmarkColor · applovin_sdk_ctaButtonColor · applovin_sdk_ctaButtonPressedColor · applovin_sdk_disclosureButtonColor · applovin_sdk_grayColor · applovin_sdk_greenColor · applovin_sdk_highlightListItemColor · applovin_sdk_highlightTextColor
applovin_sdk_listViewBackground · applovin_sdk_listViewSectionTextColor · applovin_sdk_orangeColor · applovin_sdk_starColor · applovin_sdk_switchThumbColor · applovin_sdk_switchTrackDisabledColor · applovin_sdk_textColorPrimary · applovin_sdk_warningColor · applovin_sdk_xmarkColor · background_floating_material_dark · background_floating_material_light · background_material_dark · background_material_light · black
black_transparent · bm_privacy_button_bg_color · bm_privacy_button_icon_color · bm_privacy_button_ripple_color · bm_privacy_button_text_color · bm_privacy_icon_ripple_color · bm_privacy_sheet_bg_color · bm_privacy_subtitle_color · bm_privacy_title_color · bright_foreground_disabled_material_dark · bright_foreground_disabled_material_light · bright_foreground_inverse_material_dark · bright_foreground_inverse_material_light · bright_foreground_material_dark
bright_foreground_material_light · browser_actions_bg_grey · browser_actions_divider_color · browser_actions_text_color · browser_actions_title_color · button_material_dark · button_material_light · call_notification_answer_color · call_notification_decline_color · cardview_dark_background · cardview_light_background · cardview_shadow_end_color · cardview_shadow_start_color · common_google_signin_btn_text_dark
common_google_signin_btn_text_dark_default · common_google_signin_btn_text_dark_disabled · common_google_signin_btn_text_dark_focused · common_google_signin_btn_text_dark_pressed · common_google_signin_btn_text_light · common_google_signin_btn_text_light_default · common_google_signin_btn_text_light_disabled · common_google_signin_btn_text_light_focused · common_google_signin_btn_text_light_pressed · common_google_signin_btn_tint · countdown_bg · debug_panel_black · debug_panel_dark_gray · debug_panel_gray
debug_panel_green · debug_panel_light_gray · debug_panel_orange · debug_panel_red · debug_panel_white · design_bottom_navigation_shadow_color · design_box_stroke_color · design_dark_default_color_background · design_dark_default_color_error · design_dark_default_color_on_background · design_dark_default_color_on_error · design_dark_default_color_on_primary · design_dark_default_color_on_secondary · design_dark_default_color_on_surface
design_dark_default_color_primary · design_dark_default_color_primary_dark · design_dark_default_color_primary_variant · design_dark_default_color_secondary · design_dark_default_color_secondary_variant · design_dark_default_color_surface · design_default_color_background · design_default_color_error · design_default_color_on_background · design_default_color_on_error · design_default_color_on_primary · design_default_color_on_secondary · design_default_color_on_surface · design_default_color_primary
design_default_color_primary_dark · design_default_color_primary_variant · design_default_color_secondary · design_default_color_secondary_variant · design_default_color_surface · design_error · design_fab_shadow_end_color · design_fab_shadow_mid_color · design_fab_shadow_start_color · design_fab_stroke_end_inner_color · design_fab_stroke_end_outer_color · design_fab_stroke_top_inner_color · design_fab_stroke_top_outer_color · design_icon_tint
design_snackbar_background_color · dim_foreground_disabled_material_dark · dim_foreground_disabled_material_light · dim_foreground_material_dark · dim_foreground_material_light · div_accent_color · div_button_color · div_indicator_default_color · div_indicator_selected_color · div_separator_color · div_tab_indicator · div_text_dark_disabled_40 · div_text_dark_disabled_50 · div_text_dark_disabled_80
div_title_menu_color · div_traffic_item_stroke_color · dtx_store_promo_bg_fade · error_color_material_dark · error_color_material_light · exo_black_opacity_60 · exo_black_opacity_70 · exo_bottom_bar_background · exo_edit_mode_background_color · exo_error_message_background_color · exo_styled_error_message_background · exo_white · exo_white_opacity_70 · foreground_material_dark
foreground_material_light · highlighted_text_material_dark · highlighted_text_material_light · ia_blank_background · ia_close_rewarded_button_background_color · ia_close_rewarded_button_color · ia_close_rewarded_dialog_background · ia_close_rewarded_sub_title_color · ia_close_rewarded_title_color · ia_endcard_background · ia_fullscreen_background · ia_fyber_accent · ia_fyber_identifier_bg_color · ia_fyber_white
ia_mraid_expanded_dimmed_bk · ia_overlay_bg_color · ia_overlay_stroke_color · ia_transparent_background · ia_video_background_color · ia_video_overlay_stroke · ia_video_overlay_text · ia_video_overlay_text_background · ia_video_overlay_text_background_pressed · ia_video_overlay_text_shadow · ia_video_progressbar · ia_video_progressbar_background · ia_video_progressbar_green · ia_video_transparent_overlay
m3_appbar_overlay_color · m3_assist_chip_icon_tint_color · m3_assist_chip_stroke_color · m3_bottom_sheet_drag_handle_color · m3_button_background_color_selector · m3_button_foreground_color_selector · m3_button_outline_color_selector · m3_button_ripple_color · m3_button_ripple_color_selector · m3_calendar_item_disabled_text · m3_calendar_item_stroke_color · m3_card_foreground_color · m3_card_ripple_color · m3_card_stroke_color
m3_checkbox_button_icon_tint · m3_checkbox_button_tint · m3_chip_assist_text_color · m3_chip_background_color · m3_chip_ripple_color · m3_chip_stroke_color · m3_chip_text_color · m3_dark_default_color_primary_text · m3_dark_default_color_secondary_text · m3_dark_highlighted_text · m3_dark_hint_foreground · m3_dark_primary_text_disable_only · m3_default_color_primary_text · m3_default_color_secondary_text
m3_dynamic_dark_default_color_primary_text · m3_dynamic_dark_default_color_secondary_text · m3_dynamic_dark_highlighted_text · m3_dynamic_dark_hint_foreground · m3_dynamic_dark_primary_text_disable_only · m3_dynamic_default_color_primary_text · m3_dynamic_default_color_secondary_text · m3_dynamic_highlighted_text · m3_dynamic_hint_foreground · m3_dynamic_primary_text_disable_only · m3_efab_ripple_color_selector · m3_elevated_chip_background_color · m3_fab_efab_background_color_selector · m3_fab_efab_foreground_color_selector
m3_fab_ripple_color_selector · m3_filled_icon_button_container_color_selector · m3_floating_toolbar_vibrant_icon_button_container_color_selector · m3_floating_toolbar_vibrant_icon_button_text_color_selector · m3_highlighted_text · m3_hint_foreground · m3_icon_button_icon_color_selector · m3_navigation_bar_item_with_indicator_icon_tint · m3_navigation_bar_item_with_indicator_label_tint · m3_navigation_bar_ripple_color_selector · m3_navigation_item_background_color · m3_navigation_item_icon_tint · m3_navigation_item_ripple_color · m3_navigation_item_text_color
m3_navigation_rail_item_with_indicator_icon_tint · m3_navigation_rail_item_with_indicator_label_tint · m3_navigation_rail_ripple_color_selector · m3_popupmenu_overlay_color · m3_primary_text_disable_only · m3_radiobutton_button_tint · m3_radiobutton_ripple_tint · m3_ref_palette_black · m3_ref_palette_blue0 · m3_ref_palette_blue10 · m3_ref_palette_blue100 · m3_ref_palette_blue20 · m3_ref_palette_blue30 · m3_ref_palette_blue40
m3_ref_palette_blue50 · m3_ref_palette_blue60 · m3_ref_palette_blue70 · m3_ref_palette_blue80 · m3_ref_palette_blue90 · m3_ref_palette_blue95 · m3_ref_palette_blue98 · m3_ref_palette_blue_variant0 · m3_ref_palette_blue_variant10 · m3_ref_palette_blue_variant100 · m3_ref_palette_blue_variant20 · m3_ref_palette_blue_variant30 · m3_ref_palette_blue_variant40 · m3_ref_palette_blue_variant50
m3_ref_palette_blue_variant60 · m3_ref_palette_blue_variant70 · m3_ref_palette_blue_variant80 · m3_ref_palette_blue_variant90 · m3_ref_palette_blue_variant95 · m3_ref_palette_blue_variant98 · m3_ref_palette_cyan0 · m3_ref_palette_cyan10 · m3_ref_palette_cyan100 · m3_ref_palette_cyan20 · m3_ref_palette_cyan30 · m3_ref_palette_cyan40 · m3_ref_palette_cyan50 · m3_ref_palette_cyan60
m3_ref_palette_cyan70 · m3_ref_palette_cyan80 · m3_ref_palette_cyan90 · m3_ref_palette_cyan95 · m3_ref_palette_cyan98 · m3_ref_palette_dynamic_neutral0 · m3_ref_palette_dynamic_neutral10 · m3_ref_palette_dynamic_neutral100 · m3_ref_palette_dynamic_neutral12 · m3_ref_palette_dynamic_neutral17 · m3_ref_palette_dynamic_neutral20 · m3_ref_palette_dynamic_neutral22 · m3_ref_palette_dynamic_neutral24 · m3_ref_palette_dynamic_neutral30
m3_ref_palette_dynamic_neutral4 · m3_ref_palette_dynamic_neutral40 · m3_ref_palette_dynamic_neutral50 · m3_ref_palette_dynamic_neutral6 · m3_ref_palette_dynamic_neutral60 · m3_ref_palette_dynamic_neutral70 · m3_ref_palette_dynamic_neutral80 · m3_ref_palette_dynamic_neutral87 · m3_ref_palette_dynamic_neutral90 · m3_ref_palette_dynamic_neutral92 · m3_ref_palette_dynamic_neutral94 · m3_ref_palette_dynamic_neutral95 · m3_ref_palette_dynamic_neutral96 · m3_ref_palette_dynamic_neutral98
m3_ref_palette_dynamic_neutral99 · m3_ref_palette_dynamic_neutral_variant0 · m3_ref_palette_dynamic_neutral_variant10 · m3_ref_palette_dynamic_neutral_variant100 · m3_ref_palette_dynamic_neutral_variant12 · m3_ref_palette_dynamic_neutral_variant17 · m3_ref_palette_dynamic_neutral_variant20 · m3_ref_palette_dynamic_neutral_variant22 · m3_ref_palette_dynamic_neutral_variant24 · m3_ref_palette_dynamic_neutral_variant30 · m3_ref_palette_dynamic_neutral_variant4 · m3_ref_palette_dynamic_neutral_variant40 · m3_ref_palette_dynamic_neutral_variant50 · m3_ref_palette_dynamic_neutral_variant6
m3_ref_palette_dynamic_neutral_variant60 · m3_ref_palette_dynamic_neutral_variant70 · m3_ref_palette_dynamic_neutral_variant80 · m3_ref_palette_dynamic_neutral_variant87 · m3_ref_palette_dynamic_neutral_variant90 · m3_ref_palette_dynamic_neutral_variant92 · m3_ref_palette_dynamic_neutral_variant94 · m3_ref_palette_dynamic_neutral_variant95 · m3_ref_palette_dynamic_neutral_variant96 · m3_ref_palette_dynamic_neutral_variant98 · m3_ref_palette_dynamic_neutral_variant99 · m3_ref_palette_dynamic_primary0 · m3_ref_palette_dynamic_primary10 · m3_ref_palette_dynamic_primary100
m3_ref_palette_dynamic_primary20 · m3_ref_palette_dynamic_primary30 · m3_ref_palette_dynamic_primary40 · m3_ref_palette_dynamic_primary50 · m3_ref_palette_dynamic_primary60 · m3_ref_palette_dynamic_primary70 · m3_ref_palette_dynamic_primary80 · m3_ref_palette_dynamic_primary90 · m3_ref_palette_dynamic_primary95 · m3_ref_palette_dynamic_primary98 · m3_ref_palette_dynamic_primary99 · m3_ref_palette_dynamic_secondary0 · m3_ref_palette_dynamic_secondary10 · m3_ref_palette_dynamic_secondary100
m3_ref_palette_dynamic_secondary20 · m3_ref_palette_dynamic_secondary30 · m3_ref_palette_dynamic_secondary40 · m3_ref_palette_dynamic_secondary50 · m3_ref_palette_dynamic_secondary60 · m3_ref_palette_dynamic_secondary70 · m3_ref_palette_dynamic_secondary80 · m3_ref_palette_dynamic_secondary90 · m3_ref_palette_dynamic_secondary95 · m3_ref_palette_dynamic_secondary98 · m3_ref_palette_dynamic_secondary99 · m3_ref_palette_dynamic_tertiary0 · m3_ref_palette_dynamic_tertiary10 · m3_ref_palette_dynamic_tertiary100
m3_ref_palette_dynamic_tertiary20 · m3_ref_palette_dynamic_tertiary30 · m3_ref_palette_dynamic_tertiary40 · m3_ref_palette_dynamic_tertiary50 · m3_ref_palette_dynamic_tertiary60 · m3_ref_palette_dynamic_tertiary70 · m3_ref_palette_dynamic_tertiary80 · m3_ref_palette_dynamic_tertiary90 · m3_ref_palette_dynamic_tertiary95 · m3_ref_palette_dynamic_tertiary98 · m3_ref_palette_dynamic_tertiary99 · m3_ref_palette_error0 · m3_ref_palette_error10 · m3_ref_palette_error100
m3_ref_palette_error20 · m3_ref_palette_error30 · m3_ref_palette_error40 · m3_ref_palette_error50 · m3_ref_palette_error60 · m3_ref_palette_error70 · m3_ref_palette_error80 · m3_ref_palette_error90 · m3_ref_palette_error95 · m3_ref_palette_error98 · m3_ref_palette_error99 · m3_ref_palette_green0 · m3_ref_palette_green10 · m3_ref_palette_green100
m3_ref_palette_green20 · m3_ref_palette_green30 · m3_ref_palette_green40 · m3_ref_palette_green50 · m3_ref_palette_green60 · m3_ref_palette_green70 · m3_ref_palette_green80 · m3_ref_palette_green90 · m3_ref_palette_green95 · m3_ref_palette_green98 · m3_ref_palette_grey0 · m3_ref_palette_grey10 · m3_ref_palette_grey100 · m3_ref_palette_grey20
m3_ref_palette_grey30 · m3_ref_palette_grey40 · m3_ref_palette_grey50 · m3_ref_palette_grey60 · m3_ref_palette_grey70 · m3_ref_palette_grey80 · m3_ref_palette_grey90 · m3_ref_palette_grey95 · m3_ref_palette_grey98 · m3_ref_palette_grey_variant0 · m3_ref_palette_grey_variant10 · m3_ref_palette_grey_variant100 · m3_ref_palette_grey_variant20 · m3_ref_palette_grey_variant30
m3_ref_palette_grey_variant40 · m3_ref_palette_grey_variant50 · m3_ref_palette_grey_variant60 · m3_ref_palette_grey_variant70 · m3_ref_palette_grey_variant80 · m3_ref_palette_grey_variant90 · m3_ref_palette_grey_variant95 · m3_ref_palette_grey_variant98 · m3_ref_palette_neutral0 · m3_ref_palette_neutral10 · m3_ref_palette_neutral100 · m3_ref_palette_neutral12 · m3_ref_palette_neutral17 · m3_ref_palette_neutral20
m3_ref_palette_neutral22 · m3_ref_palette_neutral24 · m3_ref_palette_neutral30 · m3_ref_palette_neutral4 · m3_ref_palette_neutral40 · m3_ref_palette_neutral50 · m3_ref_palette_neutral6 · m3_ref_palette_neutral60 · m3_ref_palette_neutral70 · m3_ref_palette_neutral80 · m3_ref_palette_neutral87 · m3_ref_palette_neutral90 · m3_ref_palette_neutral92 · m3_ref_palette_neutral94
m3_ref_palette_neutral95 · m3_ref_palette_neutral96 · m3_ref_palette_neutral98 · m3_ref_palette_neutral99 · m3_ref_palette_neutral_variant0 · m3_ref_palette_neutral_variant10 · m3_ref_palette_neutral_variant100 · m3_ref_palette_neutral_variant20 · m3_ref_palette_neutral_variant30 · m3_ref_palette_neutral_variant40 · m3_ref_palette_neutral_variant50 · m3_ref_palette_neutral_variant60 · m3_ref_palette_neutral_variant70 · m3_ref_palette_neutral_variant80
m3_ref_palette_neutral_variant90 · m3_ref_palette_neutral_variant95 · m3_ref_palette_neutral_variant98 · m3_ref_palette_neutral_variant99 · m3_ref_palette_orange0 · m3_ref_palette_orange10 · m3_ref_palette_orange100 · m3_ref_palette_orange20 · m3_ref_palette_orange30 · m3_ref_palette_orange40 · m3_ref_palette_orange50 · m3_ref_palette_orange60 · m3_ref_palette_orange70 · m3_ref_palette_orange80
m3_ref_palette_orange90 · m3_ref_palette_orange95 · m3_ref_palette_orange98 · m3_ref_palette_pink0 · m3_ref_palette_pink10 · m3_ref_palette_pink100 · m3_ref_palette_pink20 · m3_ref_palette_pink30 · m3_ref_palette_pink40 · m3_ref_palette_pink50 · m3_ref_palette_pink60 · m3_ref_palette_pink70 · m3_ref_palette_pink80 · m3_ref_palette_pink90
m3_ref_palette_pink95 · m3_ref_palette_pink98 · m3_ref_palette_primary0 · m3_ref_palette_primary10 · m3_ref_palette_primary100 · m3_ref_palette_primary20 · m3_ref_palette_primary30 · m3_ref_palette_primary40 · m3_ref_palette_primary50 · m3_ref_palette_primary60 · m3_ref_palette_primary70 · m3_ref_palette_primary80 · m3_ref_palette_primary90 · m3_ref_palette_primary95
m3_ref_palette_primary98 · m3_ref_palette_primary99 · m3_ref_palette_purple0 · m3_ref_palette_purple10 · m3_ref_palette_purple100 · m3_ref_palette_purple20 · m3_ref_palette_purple30 · m3_ref_palette_purple40 · m3_ref_palette_purple50 · m3_ref_palette_purple60 · m3_ref_palette_purple70 · m3_ref_palette_purple80 · m3_ref_palette_purple90 · m3_ref_palette_purple95
m3_ref_palette_purple98 · m3_ref_palette_red0 · m3_ref_palette_red10 · m3_ref_palette_red100 · m3_ref_palette_red20 · m3_ref_palette_red30 · m3_ref_palette_red40 · m3_ref_palette_red50 · m3_ref_palette_red60 · m3_ref_palette_red70 · m3_ref_palette_red80 · m3_ref_palette_red90 · m3_ref_palette_red95 · m3_ref_palette_red98
m3_ref_palette_secondary0 · m3_ref_palette_secondary10 · m3_ref_palette_secondary100 · m3_ref_palette_secondary20 · m3_ref_palette_secondary30 · m3_ref_palette_secondary40 · m3_ref_palette_secondary50 · m3_ref_palette_secondary60 · m3_ref_palette_secondary70 · m3_ref_palette_secondary80 · m3_ref_palette_secondary90 · m3_ref_palette_secondary95 · m3_ref_palette_secondary98 · m3_ref_palette_secondary99
m3_ref_palette_tertiary0 · m3_ref_palette_tertiary10 · m3_ref_palette_tertiary100 · m3_ref_palette_tertiary20 · m3_ref_palette_tertiary30 · m3_ref_palette_tertiary40 · m3_ref_palette_tertiary50 · m3_ref_palette_tertiary60 · m3_ref_palette_tertiary70 · m3_ref_palette_tertiary80 · m3_ref_palette_tertiary90 · m3_ref_palette_tertiary95 · m3_ref_palette_tertiary98 · m3_ref_palette_tertiary99
m3_ref_palette_white · m3_ref_palette_yellow0 · m3_ref_palette_yellow10 · m3_ref_palette_yellow100 · m3_ref_palette_yellow20 · m3_ref_palette_yellow30 · m3_ref_palette_yellow40 · m3_ref_palette_yellow50 · m3_ref_palette_yellow60 · m3_ref_palette_yellow70 · m3_ref_palette_yellow80 · m3_ref_palette_yellow90 · m3_ref_palette_yellow95 · m3_ref_palette_yellow98
m3_selection_control_ripple_color_selector · m3_simple_item_ripple_color · m3_slider_active_tick_marks_color · m3_slider_active_track_color · m3_slider_active_track_color_legacy · m3_slider_halo_color_legacy · m3_slider_inactive_tick_marks_color · m3_slider_inactive_track_color · m3_slider_inactive_track_color_legacy · m3_slider_thumb_color · m3_slider_thumb_color_legacy · m3_standard_toolbar_button_text_color_selector · m3_standard_toolbar_icon_button_container_color_selector · m3_standard_toolbar_icon_button_icon_color_selector
m3_standard_toolbar_icon_button_ripple_color_selector · m3_switch_thumb_tint · m3_switch_track_tint · m3_sys_color_dark_background · m3_sys_color_dark_error · m3_sys_color_dark_error_container · m3_sys_color_dark_inverse_on_surface · m3_sys_color_dark_inverse_primary · m3_sys_color_dark_inverse_surface · m3_sys_color_dark_on_background · m3_sys_color_dark_on_error · m3_sys_color_dark_on_error_container · m3_sys_color_dark_on_primary · m3_sys_color_dark_on_primary_container
m3_sys_color_dark_on_secondary · m3_sys_color_dark_on_secondary_container · m3_sys_color_dark_on_surface · m3_sys_color_dark_on_surface_variant · m3_sys_color_dark_on_tertiary · m3_sys_color_dark_on_tertiary_container · m3_sys_color_dark_outline · m3_sys_color_dark_outline_variant · m3_sys_color_dark_primary · m3_sys_color_dark_primary_container · m3_sys_color_dark_secondary · m3_sys_color_dark_secondary_container · m3_sys_color_dark_surface · m3_sys_color_dark_surface_bright
m3_sys_color_dark_surface_container · m3_sys_color_dark_surface_container_high · m3_sys_color_dark_surface_container_highest · m3_sys_color_dark_surface_container_low · m3_sys_color_dark_surface_container_lowest · m3_sys_color_dark_surface_dim · m3_sys_color_dark_surface_variant · m3_sys_color_dark_tertiary · m3_sys_color_dark_tertiary_container · m3_sys_color_dynamic_dark_background · m3_sys_color_dynamic_dark_error · m3_sys_color_dynamic_dark_error_container · m3_sys_color_dynamic_dark_inverse_on_surface · m3_sys_color_dynamic_dark_inverse_primary
m3_sys_color_dynamic_dark_inverse_surface · m3_sys_color_dynamic_dark_on_background · m3_sys_color_dynamic_dark_on_error · m3_sys_color_dynamic_dark_on_error_container · m3_sys_color_dynamic_dark_on_primary · m3_sys_color_dynamic_dark_on_primary_container · m3_sys_color_dynamic_dark_on_secondary · m3_sys_color_dynamic_dark_on_secondary_container · m3_sys_color_dynamic_dark_on_surface · m3_sys_color_dynamic_dark_on_surface_variant · m3_sys_color_dynamic_dark_on_tertiary · m3_sys_color_dynamic_dark_on_tertiary_container · m3_sys_color_dynamic_dark_outline · m3_sys_color_dynamic_dark_outline_variant
m3_sys_color_dynamic_dark_primary · m3_sys_color_dynamic_dark_primary_container · m3_sys_color_dynamic_dark_secondary · m3_sys_color_dynamic_dark_secondary_container · m3_sys_color_dynamic_dark_surface · m3_sys_color_dynamic_dark_surface_bright · m3_sys_color_dynamic_dark_surface_container · m3_sys_color_dynamic_dark_surface_container_high · m3_sys_color_dynamic_dark_surface_container_highest · m3_sys_color_dynamic_dark_surface_container_low · m3_sys_color_dynamic_dark_surface_container_lowest · m3_sys_color_dynamic_dark_surface_dim · m3_sys_color_dynamic_dark_surface_variant · m3_sys_color_dynamic_dark_tertiary
m3_sys_color_dynamic_dark_tertiary_container · m3_sys_color_dynamic_light_background · m3_sys_color_dynamic_light_error · m3_sys_color_dynamic_light_error_container · m3_sys_color_dynamic_light_inverse_on_surface · m3_sys_color_dynamic_light_inverse_primary · m3_sys_color_dynamic_light_inverse_surface · m3_sys_color_dynamic_light_on_background · m3_sys_color_dynamic_light_on_error · m3_sys_color_dynamic_light_on_error_container · m3_sys_color_dynamic_light_on_primary · m3_sys_color_dynamic_light_on_primary_container · m3_sys_color_dynamic_light_on_secondary · m3_sys_color_dynamic_light_on_secondary_container
m3_sys_color_dynamic_light_on_surface · m3_sys_color_dynamic_light_on_surface_variant · m3_sys_color_dynamic_light_on_tertiary · m3_sys_color_dynamic_light_on_tertiary_container · m3_sys_color_dynamic_light_outline · m3_sys_color_dynamic_light_outline_variant · m3_sys_color_dynamic_light_primary · m3_sys_color_dynamic_light_primary_container · m3_sys_color_dynamic_light_secondary · m3_sys_color_dynamic_light_secondary_container · m3_sys_color_dynamic_light_surface · m3_sys_color_dynamic_light_surface_bright · m3_sys_color_dynamic_light_surface_container · m3_sys_color_dynamic_light_surface_container_high
m3_sys_color_dynamic_light_surface_container_highest · m3_sys_color_dynamic_light_surface_container_low · m3_sys_color_dynamic_light_surface_container_lowest · m3_sys_color_dynamic_light_surface_dim · m3_sys_color_dynamic_light_surface_variant · m3_sys_color_dynamic_light_tertiary · m3_sys_color_dynamic_light_tertiary_container · m3_sys_color_dynamic_on_primary_fixed · m3_sys_color_dynamic_on_primary_fixed_variant · m3_sys_color_dynamic_on_secondary_fixed · m3_sys_color_dynamic_on_secondary_fixed_variant · m3_sys_color_dynamic_on_tertiary_fixed · m3_sys_color_dynamic_on_tertiary_fixed_variant · m3_sys_color_dynamic_primary_fixed
m3_sys_color_dynamic_primary_fixed_dim · m3_sys_color_dynamic_secondary_fixed · m3_sys_color_dynamic_secondary_fixed_dim · m3_sys_color_dynamic_tertiary_fixed · m3_sys_color_dynamic_tertiary_fixed_dim · m3_sys_color_light_background · m3_sys_color_light_error · m3_sys_color_light_error_container · m3_sys_color_light_inverse_on_surface · m3_sys_color_light_inverse_primary · m3_sys_color_light_inverse_surface · m3_sys_color_light_on_background · m3_sys_color_light_on_error · m3_sys_color_light_on_error_container
m3_sys_color_light_on_primary · m3_sys_color_light_on_primary_container · m3_sys_color_light_on_secondary · m3_sys_color_light_on_secondary_container · m3_sys_color_light_on_surface · m3_sys_color_light_on_surface_variant · m3_sys_color_light_on_tertiary · m3_sys_color_light_on_tertiary_container · m3_sys_color_light_outline · m3_sys_color_light_outline_variant · m3_sys_color_light_primary · m3_sys_color_light_primary_container · m3_sys_color_light_secondary · m3_sys_color_light_secondary_container
m3_sys_color_light_surface · m3_sys_color_light_surface_bright · m3_sys_color_light_surface_container · m3_sys_color_light_surface_container_high · m3_sys_color_light_surface_container_highest · m3_sys_color_light_surface_container_low · m3_sys_color_light_surface_container_lowest · m3_sys_color_light_surface_dim · m3_sys_color_light_surface_variant · m3_sys_color_light_tertiary · m3_sys_color_light_tertiary_container · m3_sys_color_on_primary_fixed · m3_sys_color_on_primary_fixed_variant · m3_sys_color_on_secondary_fixed
m3_sys_color_on_secondary_fixed_variant · m3_sys_color_on_tertiary_fixed · m3_sys_color_on_tertiary_fixed_variant · m3_sys_color_primary_fixed · m3_sys_color_primary_fixed_dim · m3_sys_color_secondary_fixed · m3_sys_color_secondary_fixed_dim · m3_sys_color_tertiary_fixed · m3_sys_color_tertiary_fixed_dim · m3_tabs_icon_color · m3_tabs_icon_color_secondary · m3_tabs_ripple_color · m3_tabs_ripple_color_secondary · m3_tabs_text_color
m3_tabs_text_color_secondary · m3_text_button_background_color_selector · m3_text_button_foreground_color_selector · m3_text_button_ripple_color_selector · m3_textfield_filled_background_color · m3_textfield_indicator_text_color · m3_textfield_input_text_color · m3_textfield_label_color · m3_textfield_stroke_color · m3_timepicker_button_background_color · m3_timepicker_button_ripple_color · m3_timepicker_button_text_color · m3_timepicker_clock_text_color · m3_timepicker_display_background_color
m3_timepicker_display_ripple_color · m3_timepicker_display_text_color · m3_timepicker_secondary_text_button_ripple_color · m3_timepicker_secondary_text_button_text_color · m3_timepicker_time_input_stroke_color · m3_tonal_button_ripple_color_selector · m3_vibrant_toolbar_button_text_color_selector · m3_vibrant_toolbar_icon_button_container_color_selector · m3_vibrant_toolbar_icon_button_icon_color_selector · m3_vibrant_toolbar_icon_button_ripple_color_selector · m3expressive_bottom_nav_item_icon_tint · m3expressive_bottom_nav_item_label_tint · m3expressive_bottom_nav_item_ripple_tint · m3expressive_button_outline_color_selector
m3expressive_button_outlined_background_color_selector · m3expressive_nav_rail_item_icon_tint · m3expressive_nav_rail_item_label_tint · m3expressive_nav_rail_item_ripple_tint · material_blue_grey_800 · material_blue_grey_900 · material_blue_grey_950 · material_cursor_color · material_deep_teal_200 · material_deep_teal_500 · material_divider_color · material_dynamic_color_dark_error · material_dynamic_color_dark_error_container · material_dynamic_color_dark_on_error
material_dynamic_color_dark_on_error_container · material_dynamic_color_light_error · material_dynamic_color_light_error_container · material_dynamic_color_light_on_error · material_dynamic_color_light_on_error_container · material_dynamic_neutral0 · material_dynamic_neutral10 · material_dynamic_neutral100 · material_dynamic_neutral20 · material_dynamic_neutral30 · material_dynamic_neutral40 · material_dynamic_neutral50 · material_dynamic_neutral60 · material_dynamic_neutral70
material_dynamic_neutral80 · material_dynamic_neutral90 · material_dynamic_neutral95 · material_dynamic_neutral99 · material_dynamic_neutral_variant0 · material_dynamic_neutral_variant10 · material_dynamic_neutral_variant100 · material_dynamic_neutral_variant20 · material_dynamic_neutral_variant30 · material_dynamic_neutral_variant40 · material_dynamic_neutral_variant50 · material_dynamic_neutral_variant60 · material_dynamic_neutral_variant70 · material_dynamic_neutral_variant80
material_dynamic_neutral_variant90 · material_dynamic_neutral_variant95 · material_dynamic_neutral_variant99 · material_dynamic_primary0 · material_dynamic_primary10 · material_dynamic_primary100 · material_dynamic_primary20 · material_dynamic_primary30 · material_dynamic_primary40 · material_dynamic_primary50 · material_dynamic_primary60 · material_dynamic_primary70 · material_dynamic_primary80 · material_dynamic_primary90
material_dynamic_primary95 · material_dynamic_primary99 · material_dynamic_secondary0 · material_dynamic_secondary10 · material_dynamic_secondary100 · material_dynamic_secondary20 · material_dynamic_secondary30 · material_dynamic_secondary40 · material_dynamic_secondary50 · material_dynamic_secondary60 · material_dynamic_secondary70 · material_dynamic_secondary80 · material_dynamic_secondary90 · material_dynamic_secondary95
material_dynamic_secondary99 · material_dynamic_tertiary0 · material_dynamic_tertiary10 · material_dynamic_tertiary100 · material_dynamic_tertiary20 · material_dynamic_tertiary30 · material_dynamic_tertiary40 · material_dynamic_tertiary50 · material_dynamic_tertiary60 · material_dynamic_tertiary70 · material_dynamic_tertiary80 · material_dynamic_tertiary90 · material_dynamic_tertiary95 · material_dynamic_tertiary99
material_grey_100 · material_grey_300 · material_grey_50 · material_grey_600 · material_grey_800 · material_grey_850 · material_grey_900 · material_harmonized_color_error · material_harmonized_color_error_container · material_harmonized_color_on_error · material_harmonized_color_on_error_container · material_on_background_disabled · material_on_background_emphasis_high_type · material_on_background_emphasis_medium
material_on_primary_disabled · material_on_primary_emphasis_high_type · material_on_primary_emphasis_medium · material_on_surface_disabled · material_on_surface_emphasis_high_type · material_on_surface_emphasis_medium · material_on_surface_stroke · material_personalized__highlighted_text · material_personalized__highlighted_text_inverse · material_personalized_color_background · material_personalized_color_control_activated · material_personalized_color_control_highlight · material_personalized_color_control_normal · material_personalized_color_error
material_personalized_color_error_container · material_personalized_color_on_background · material_personalized_color_on_error · material_personalized_color_on_error_container · material_personalized_color_on_primary · material_personalized_color_on_primary_container · material_personalized_color_on_secondary · material_personalized_color_on_secondary_container · material_personalized_color_on_surface · material_personalized_color_on_surface_inverse · material_personalized_color_on_surface_variant · material_personalized_color_on_tertiary · material_personalized_color_on_tertiary_container · material_personalized_color_outline
material_personalized_color_outline_variant · material_personalized_color_primary · material_personalized_color_primary_container · material_personalized_color_primary_inverse · material_personalized_color_primary_text · material_personalized_color_primary_text_inverse · material_personalized_color_secondary · material_personalized_color_secondary_container · material_personalized_color_secondary_text · material_personalized_color_secondary_text_inverse · material_personalized_color_surface · material_personalized_color_surface_bright · material_personalized_color_surface_container · material_personalized_color_surface_container_high
material_personalized_color_surface_container_highest · material_personalized_color_surface_container_low · material_personalized_color_surface_container_lowest · material_personalized_color_surface_dim · material_personalized_color_surface_inverse · material_personalized_color_surface_variant · material_personalized_color_tertiary · material_personalized_color_tertiary_container · material_personalized_color_text_hint_foreground_inverse · material_personalized_color_text_primary_inverse · material_personalized_color_text_primary_inverse_disable_only · material_personalized_color_text_secondary_and_tertiary_inverse · material_personalized_color_text_secondary_and_tertiary_inverse_disabled · material_personalized_hint_foreground
material_personalized_hint_foreground_inverse · material_personalized_primary_inverse_text_disable_only · material_personalized_primary_text_disable_only · material_slider_active_tick_marks_color · material_slider_active_track_color · material_slider_halo_color · material_slider_inactive_tick_marks_color · material_slider_inactive_track_color · material_slider_thumb_color · material_timepicker_button_background · material_timepicker_button_stroke · material_timepicker_clock_text_color · material_timepicker_clockface · material_timepicker_modebutton_tint
mbridge_black · mbridge_black_66 · mbridge_black_alpha_50 · mbridge_cm_feedback_dialog_chice_bg_pressed · mbridge_cm_feedback_rb_text_color_color_list · mbridge_color_999999 · mbridge_color_cc000000 · mbridge_color_cc0000001 · mbridge_common_white · mbridge_cpb_blue · mbridge_cpb_blue_dark · mbridge_cpb_green · mbridge_cpb_green_dark · mbridge_cpb_grey
mbridge_cpb_red · mbridge_cpb_red_dark · mbridge_cpb_white · mbridge_dd_grey · mbridge_ee_grey · mbridge_interstitial_black · mbridge_interstitial_white · mbridge_more_offer_list_bg · mbridge_nativex_cta_txt_nor · mbridge_nativex_cta_txt_pre · mbridge_nativex_land_cta_bg_nor · mbridge_nativex_por_cta_bg_nor · mbridge_nativex_por_cta_bg_pre · mbridge_nativex_sound_bg
mbridge_purple_200 · mbridge_purple_500 · mbridge_purple_700 · mbridge_reward_black · mbridge_reward_cta_bg · mbridge_reward_desc_textcolor · mbridge_reward_endcard_hor_bg · mbridge_reward_endcard_land_bg · mbridge_reward_endcard_line_bg · mbridge_reward_endcard_vast_bg · mbridge_reward_kiloo_background · mbridge_reward_layer_text_bg · mbridge_reward_minicard_bg · mbridge_reward_six_black_transparent
mbridge_reward_six_black_transparent1 · mbridge_reward_six_black_transparent2 · mbridge_reward_title_textcolor · mbridge_reward_white · mbridge_splash_count_time_skip_text_color · mbridge_teal_200 · mbridge_teal_700 · mbridge_video_common_alertview_bg · mbridge_video_common_alertview_cancel_button_bg_default · mbridge_video_common_alertview_cancel_button_bg_pressed · mbridge_video_common_alertview_cancel_button_textcolor · mbridge_video_common_alertview_confirm_button_bg_default · mbridge_video_common_alertview_confirm_button_bg_pressed · mbridge_video_common_alertview_confirm_button_textcolor
mbridge_video_common_alertview_content_textcolor · mbridge_video_common_alertview_feedback_rb_bg · mbridge_video_common_alertview_title_textcolor · mbridge_white · md_theme_light_background · md_theme_light_error · md_theme_light_onPrimary · md_theme_light_primary · md_theme_light_secondary · md_theme_light_surface · mobilefuse_black · mobilefuse_default_banner_bg · mobilefuse_transparent · mobilefuse_white
monetization_ads_internal_age_text_color · monetization_ads_internal_app_install_call_to_action_green · monetization_ads_internal_app_install_media_view_foreground · monetization_ads_internal_app_install_timer_text_color_dark · monetization_ads_internal_app_install_timer_text_color_light · monetization_ads_internal_call_to_action_background_color · monetization_ads_internal_call_to_action_background_color_blue · monetization_ads_internal_call_to_action_background_color_yellow · monetization_ads_internal_call_to_action_blue · monetization_ads_internal_call_to_action_ripple_highlight_dark_color · monetization_ads_internal_call_to_action_ripple_highlight_light_color · monetization_ads_internal_call_to_action_text_color · monetization_ads_internal_call_to_action_text_color_black · monetization_ads_internal_domain_grey_text_color
monetization_ads_internal_grey_text_color · monetization_ads_internal_header_background_end · monetization_ads_internal_header_background_start · monetization_ads_internal_instream_advertiser_block_color_focused · monetization_ads_internal_instream_advertiser_block_color_unfocused · monetization_ads_internal_instream_background · monetization_ads_internal_instream_call_to_action_block_color · monetization_ads_internal_instream_progress_bar_color · monetization_ads_internal_price_text_color · monetization_ads_internal_review_count_text_color · monetization_ads_internal_reward_controls_background_gray · monetization_ads_internal_rewarded_close_verification_text_color · monetization_ads_internal_sponsored_text_color · monetization_ads_internal_text_color_white
monetization_ads_internal_warning_text_color · mtrl_btn_bg_color_selector · mtrl_btn_ripple_color · mtrl_btn_stroke_color_selector · mtrl_btn_text_btn_bg_color_selector · mtrl_btn_text_btn_ripple_color · mtrl_btn_text_color_disabled · mtrl_btn_text_color_selector · mtrl_btn_transparent_bg_color · mtrl_calendar_item_stroke_color · mtrl_calendar_selected_range · mtrl_card_view_foreground · mtrl_card_view_ripple · mtrl_chip_background_color
mtrl_chip_close_icon_tint · mtrl_chip_surface_color · mtrl_chip_text_color · mtrl_choice_chip_background_color · mtrl_choice_chip_ripple_color · mtrl_choice_chip_text_color · mtrl_error · mtrl_fab_bg_color_selector · mtrl_fab_icon_text_color_selector · mtrl_fab_ripple_color · mtrl_filled_background_color · mtrl_filled_icon_tint · mtrl_filled_stroke_color · mtrl_indicator_text_color
mtrl_navigation_bar_colored_item_tint · mtrl_navigation_bar_colored_ripple_color · mtrl_navigation_bar_item_tint · mtrl_navigation_bar_ripple_color · mtrl_navigation_item_background_color · mtrl_navigation_item_icon_tint · mtrl_navigation_item_text_color · mtrl_on_primary_text_btn_text_color_selector · mtrl_on_surface_ripple_color · mtrl_outlined_icon_tint · mtrl_outlined_stroke_color · mtrl_popupmenu_overlay_color · mtrl_scrim_color · mtrl_switch_thumb_icon_tint
mtrl_switch_thumb_tint · mtrl_switch_track_decoration_tint · mtrl_switch_track_tint · mtrl_tabs_colored_ripple_color · mtrl_tabs_icon_color_selector · mtrl_tabs_icon_color_selector_colored · mtrl_tabs_legacy_text_color_selector · mtrl_tabs_ripple_color · mtrl_text_btn_text_color_selector · mtrl_textinput_default_box_stroke_color · mtrl_textinput_disabled_color · mtrl_textinput_filled_box_default_background_color · mtrl_textinput_focused_box_stroke_color · mtrl_textinput_hovered_box_stroke_color
notification_action_color_filter · notification_icon_bg_color · notification_material_background_media_default_color · pin_normal · pink · pob_controls_background_color · pob_controls_stroke_color · pob_custom_page_background_color · pob_install_button_background_color · pob_monitor_background · pob_monitor_close_background · pob_skip_button_background_color · preference_fallback_accent_color · primary_color_state
primary_dark_material_dark · primary_dark_material_light · primary_material_dark · primary_material_light · primary_text_default_material_dark · primary_text_default_material_light · primary_text_disabled_material_dark · primary_text_disabled_material_light · progressbar_bg · rab_colorBtnBlue · rab_colorBtnBluePressed · red · ripple_material_dark · ripple_material_light
secondary_text_default_material_dark · secondary_text_default_material_light · secondary_text_disabled_material_dark · secondary_text_disabled_material_light · seek_bar_complete · seek_bar_incomplete · simpledialogfragment_error_color · smaato_sdk_core_countdown_bg · smaato_sdk_core_progressbar_bg · smaato_sdk_core_ui_ctrl_almost_white · smaato_sdk_core_ui_ctrl_black · smaato_sdk_core_ui_ctrl_grey · smaato_sdk_core_ui_semitransparent · smaato_sdk_richmedia_ui_semitransparent
smaato_sdk_video_grey · smaato_sdk_video_white · switch_thumb_disabled_material_dark · switch_thumb_disabled_material_light · switch_thumb_material_dark · switch_thumb_material_light · switch_thumb_normal_material_dark · switch_thumb_normal_material_light · tooltip_background_dark · tooltip_background_light · tt_000000 · tt_00000000 · tt_1e000000 · tt_2a90d7
tt_4a90e2 · tt_72000000 · tt_7f2a90d7 · tt_88000000 · tt_b2000000 · tt_e0e0e0 · tt_e4ffffff · tt_fde6e6e6 · tt_fdffffff · tt_ff1a1a1a · tt_ffffff · ui_ctrl_almost_white · ui_ctrl_black · ui_ctrl_grey
ui_semitransparent · vector_tint_color · vector_tint_theme_color · white
```

#### Warna tema aplikasi (`md_theme_light_*` — lengkap, 6 entri)

| Resource | Nilai ARGB | Peran |
|---|---|---|
| `md_theme_light_primary` | `#ff429e46` | Hijau utama (toolbar, FAB, drawer header) |
| `md_theme_light_secondary` | `#ff66bb6a` | Hijau sekunder (ripple FAB) |
| `md_theme_light_surface` | `#ffffffff` | Kartu putih |
| `md_theme_light_background` | `#fff1f8e9` | Latar hijau sangat pucat |
| `md_theme_light_onPrimary` | `#ffffffff` | Teks/icon di atas primary |
| `md_theme_light_error` | `#ffb00020` | Error |

Distribusi 1.208 warna: `m3_*` 598, `material_*` 164, `mbridge_*` 60, `mtrl_*` 55, `design_*` 38, `monetization_ads_*` 29, `abc_*` 25, `ia_*` 25, `applovin_*` 21, `tt_*` 15, sisanya <12 per prefiks. **Tidak ada varian `md_theme_dark_*`** — aplikasi secara efektif light-only.

#### Style tema & FAB

| Style | Parent | Atribut |
|---|---|---|
| `style/Base.Theme.Minetuner` (`0x7f140079`) | `Theme.Material3.Light.NoActionBar` | `colorError=@color/md_theme_light_error`, `colorOnBackground=@color/md_theme_light_background`, `colorOnPrimary=@color/md_theme_light_onPrimary`, `colorPrimary=@color/md_theme_light_primary`, `colorSecondary=@color/md_theme_light_secondary`, `colorSurface=@color/md_theme_light_surface`, `floatingActionButtonStyle=@style/Widget.Minetuner.FAB` |
| `style/Theme.Minetuner` (`0x7f14033e`) | `Base.Theme.Minetuner` | tanpa override (size=0); dipakai di `<application android:theme>` |
| `style/Widget.Minetuner.FAB` (`0x7f140555`) | `Widget.Material3.FloatingActionButton.Primary` | `backgroundTint=@color/md_theme_light_primary`, `rippleColor=@color/md_theme_light_secondary`, `shapeAppearanceOverlay=@style/ShapeAppearance.Minetuner.FAB`, `tint=@color/md_theme_light_onPrimary` |
| `style/ShapeAppearance.Minetuner.FAB` (`0x7f14021b`) | `ShapeAppearance.Material3.SmallComponent` | `cornerFamily=0` (rounded), `cornerSize=0.5%` |
| `style/ThemeOverlay.AppCompat.Dark.ActionBar` (`0x7f140342`) | — | dipakai pada semua `AppBarLayout` aplikasi → ikon/toolbar kontras terang di atas hijau |

---

## 4. DECODE XMLTREE — STRUKTUR VIEW APLIKASI

Semua ID numerik diterjemahkan ke nama melalui peta resource `0x7f0aXXXX → id/*`, `0x7f13XXXX → string/*`.

### 4.1 `res/layout/activity_main.xml` (79 baris xmltree)

```text
androidx.coordinatorlayout.widget.CoordinatorLayout (clipChildren=false, match_parent)
├── com.google.android.material.appbar.AppBarLayout (theme=ThemeOverlay.AppCompat.Dark.ActionBar, wrap_content)
│   └── androidx.appcompat.widget.Toolbar
│       id=@id/toolbar | background=?attr/colorPrimary | height=?attr/actionBarSize
│       popupTheme=@style/ThemeOverlay.AppCompat.Light
│       title=@string/app_name ("MineTuner") | titleTextColor=@android:color/white
└── androidx.recyclerview.widget.RecyclerView
    id=@id/worldRecyclerView | match_parent | layout_behavior=AppBarLayout$ScrollingViewBehavior
└── LinearLayout id=@id/fabMenu
    gravity=bottom|end (0x00800055), orientation=vertical, marginBottom=16dp, marginEnd=16dp, clipChildren=false
    ├── LinearLayout id=@id/createWorldFabHolder (visibility=gone, margin=4dp, gravity=center)
    │   ├── TextView text="New World", textColor=@android:color/white, background=@drawable/fab_label_background
    │   └── FloatingActionButton id=@id/createWorldFab, src=@drawable/ic_layers, fabCustomSize=40dp
    ├── LinearLayout id=@id/addWorldFabHolder (visibility=gone, margin=4dp, gravity=center)
    │   ├── TextView text="Import World", textColor=@android:color/white, background=@drawable/fab_label_background
    │   └── FloatingActionButton id=@id/addWorldFab, src=@drawable/ic_upload, fabCustomSize=40dp
    └── FloatingActionButton id=@id/mainFab, src=@drawable/ic_add, marginTop=4dp
```

Catatan: label FAB berada di **kiri** ikon (urutan TextView → FAB), warna teks putih di atas `fab_label_background`.

### 4.2 `res/layout/activity_drawer.xml` (32 baris)

```text
androidx.drawerlayout.widget.DrawerLayout id=@id/drawer_layout (match_parent)
├── LinearLayout orientation=vertical
│   ├── AppBarLayout theme=ThemeOverlay.AppCompat.Dark.ActionBar
│   │   └── Toolbar id=@id/toolbar, background=?colorPrimary, height=?actionBarSize,
│   │       popupTheme=ThemeOverlay.AppCompat.Light, titleTextColor=white
│   └── FrameLayout id=@id/fragment_container (match_parent)
└── com.google.android.material.navigation.NavigationView
    id=@id/nav_view | width=wrap_content | height=match_parent | gravity=start
    app:headerLayout=@layout/drawer_header
    app:menu=@menu/drawer_menu
```

### 4.3 `res/layout/drawer_header.xml` (29 baris)

```text
LinearLayout (match_parent × 160dp) background=?attr/colorPrimary, padding=16dp, gravity=center_vertical
├── ImageView id=@id/nav_header_icon — 64dp × 64dp, scaleType=6 (CENTER_CROP)
├── TextView id=@id/nav_header_title — 20sp, bold(default), white, maxLines=1, ellipsize=end, paddingTop=8dp
└── TextView id=@id/nav_header_subtitle — white, maxLines=1, ellipsize=end
```

### 4.4 `res/layout/activity_help.xml` (85 baris)

Toolbar `id=@id/toolbar`, background `?colorPrimary`, title hardcoded **“Exporting Worlds”**, titleTextColor white.
Di bawahnya `ScrollView` (paddingHorizontal=8dp, `layout_behavior=AppBarLayout$ScrollingViewBehavior`) → `LinearLayout` vertikal berisi 5 pasangan teks/gambar:

| Teks (16sp) | Gambar (match_parent, wrap_content, scaleType=3 FIT_CENTER, adjustViewBounds=true) |
|---|---|
| “Step 1: Click the button below to import the texture pack into Minecraft.” | `@drawable/step1_tut` |
| **Button** `id=@id/button_import`, text **“Import”**, match_parent | — |
| “Step 2: Activate the texture pack in Minecraft.” | `@drawable/step2_tut` |
| “Step 3: Open a world in Minecraft and go into Game settings.” | `@drawable/step3_tut` |
| “Step 4: Scroll down to the bottom and press 'Export Template'.” | `@drawable/step4_tut` |
| “Select a file location then wait for the world to export.” | — |

Tidak ada ID untuk TextView langkah; hanya `button_import` yang interaktif.

### 4.5 `res/layout/activity_create_world.xml` (74 baris)

```text
CoordinatorLayout
├── AppBarLayout → Toolbar id=@id/toolbar, title="Create Flat World", background=?colorPrimary, white title
└── ScrollView paddingTop=16dp, paddingHorizontal=16dp
    └── LinearLayout vertical
        ├── TextView "Selected Biome" — 16sp, bold, marginBottom=8dp
        ├── <include id=@id/selected_biome_card layout=@layout/biome_item>
        ├── LinearLayout horizontal gravity=center
        │   └── CheckBox id=@id/prevent_slimes_checkbox
        │       text="Prevent Slimes from Spawning", 16sp, textColor=?android:textColorSecondary
        ├── LinearLayout horizontal gravity=center
        │   ├── TextView "Flat World Layers" — 16sp, bold, width=0dp weight=1
        │   └── ImageButton id=@id/add_layer_button — 48×48dp, src=@drawable/ic_add,
        │       background=?attr/selectableItemBackgroundBorderless, scaleType=7, tint=?android:textColorSecondary
        └── RecyclerView id=@id/layers_recycler — match_parent × wrap_content, nestedScrollingEnabled=true
```

### 4.6 `res/layout/activity_entities_nbt.xml` (27 baris)

```text
CoordinatorLayout
├── AppBarLayout → Toolbar id=@id/toolbar, title="Chunk Entities", background=?colorPrimary
├── RecyclerView id=@id/entitiesRecyclerView (match_parent)
└── FrameLayout id=@id/fragment_container (visibility=gone) — tempat fragment editor NBT muncul saat entitas dipilih
```

### 4.7 `res/layout/tree_node_item.xml` (28 baris)

```text
LinearLayout horizontal id=@id/node_container
background=?attr/selectableItemBackground | focusable=true | clickable=true
minHeight=48dp | paddingHorizontal=12dp | paddingVertical=6dp | gravity=center_vertical
├── ImageView id=@id/node_icon — 32×32dp, contentDescription="icon", marginEnd=12dp
└── TextView id=@id/key_text — 18sp, bold, white(?android:textColorPrimary tergantung theme),
    text="Key" (placeholder), maxLines=1, ellipsize=end
```

### 4.8 `res/layout/world_item.xml` (54 baris)

```text
androidx.cardview.widget.CardView (clickable=true)
cardCornerRadius=8dp | cardElevation=4dp | cardUseCompatPadding=true | cardPreventCornerOverlap=false
foreground=?attr/selectableItemBackground
└── LinearLayout horizontal, padding=12dp, gravity=center_vertical
    ├── ImageView id=@id/worldIcon — 48×48dp, scaleType=CENTER_CROP, marginEnd=12dp
    └── LinearLayout vertical (width=0dp, weight=1)
        ├── TextView id=@id/worldName — 18sp, bold, textColor=?android:textColorPrimary, maxLines=1, ellipsize=end
        ├── TextView id=@id/worldSize — 14sp, textColor=?android:textColorTertiary(0x01060000), maxLines=1
        └── TextView id=@id/worldInternalName — 14sp, italic, textColor=?android:textColorTertiary, maxLines=1
```

### 4.9 `res/layout/map_selection_view.xml` (14 baris)

```text
<merge>
├── <include id=@id/topLeft     layout=@layout/selection_drag_btn>
├── <include id=@id/topRight    layout=@layout/selection_drag_btn>
├── <include id=@id/bottomLeft  layout=@layout/selection_drag_btn>
└── <include id=@id/bottomRight layout=@layout/selection_drag_btn>
```
`selection_drag_btn` = `AppCompatButton` 24×24dp, background `?selectableItemBackground`, foreground `?colorPrimary`, stateListAnimator=`@null`.

### 4.10 `res/menu/drawer_menu.xml` (57 baris)

Grup `checkableBehavior=2` (single):

| ID | Ikon | Judul | String ID |
|---|---|---|---|
| `nav_map` | `ic_map` | Show Map | `0x7f130233` |
| `nav_world_nbt` | `ic_tune` | World NBT | `0x7f1302df` |
| `nav_singleplayer_nbt` | `ic_person` | Singleplayer NBT | `0x7f130236` |
| `nav_multiplayer_nbt` | `ic_group_people` | Multiplayer NBT | `0x7f1301e9` |
| `nav_change_world` | `ic_exit` | Change World | `0x7f130085` |

Submenu **Advanced** (`@string/advanced_section`) — grup checkable kedua:
`nav_overworld_nbt` (ic_list), `nav_the_end_nbt` (ic_list), `nav_nether_nbt` (ic_list), `nav_biomedata_nbt` (ic_list), `nav_portals_nbt` (ic_list), `nav_autonomous_entities_nbt` (ic_list), `nav_custom_nbt` (ic_search / “Open Custom NBT”).

### 4.11 `res/menu/main_activity_toolbar_menu.xml` (13 baris)

| ID | Ikon | Judul | showAsAction |
|---|---|---|---|
| `action_help` | `ic_book` | Help | `2` = **always** |
| `action_about` | `ic_info` | About | `1` = ifRoom |

### 4.12 `res/menu/create_world_activity_toolbar_menu.xml` (8 baris)

| ID | Ikon | Judul | showAsAction |
|---|---|---|---|
| `action_save` | `ic_save` | Save | `2` = always |

### 4.13 Bonus: layout pendukung

`biome_item.xml` = CardView (8dp radius, 4dp elevasi, padding 12dp) + ikon 40dp + TextView 16sp bold (`id 0x7f0a00f2`) + TextView 14sp abu (`0x7f0a00f1`).
`world_layer_item.xml` = CardView serupa + dua TextView (16sp bold `0x7f0a0385`, 14sp `0x7f0a0384`) + TextView count (`0x7f0a0386`, marginEnd 8dp) + ImageButton hapus.
`res/xml/file_paths.xml` = `<paths><external-files-path name="external_files" path="."/></paths>`.

---

## 5. NATIVE LIBRARIES & STRUKTUR APK

### 5.1 `config.arm64_v8a.apk` — 12 file `.so` (3.863.628 byte terkompresi)

| `.so` | Ukuran | Peran (verifikasi) |
|---|---:|---|
| `libleveldb.so` | 1.261.744 | **JNI LevelDB milik aplikasi** — `System.loadLibrary("leveldb")` dipanggil `com/hextetrahedral/leveldb/LevelDB.java`. Menyimpan/membaca kunci Bedrock (`~local_player`, `player_server_*`, `Overworld`, `TheEnd`, `Nether`, `BiomeData`, `Portals`, `AutonomousEntities`). |
| `libsentry.so` | 681.784 | Sentry Native (NDK crash handler); terhubung ke DSN `sentry.appodeal.com/42`, `io.sentry.ndk.scope-sync.enable=true` |
| `libapplovin-native-crash-reporter.so` | 860.200 | AppLovin MAX native crash reporter |
| `libtt_ugen_layout.so` | 440.672 | Pangle/ByteDance (Toutiao `tt_`) — rendering layout iklan UGEN |
| `libnms.so` | 297.832 | Pangle/ByteDance-native helper (non-max-suppression / layout engine) |
| `libapminsightb.so` | 124.352 | AppMetrica / Appodeal APM insight (modul B) |
| `libapminsighta.so` | 102.256 | AppMetrica / Appodeal APM insight (modul A) |
| `libpglarmor.so` | 68.600 | Pangle/ByteDance — proteksi/enkripsi resource (`armor`) |
| `libsentry-android.so` | 16.832 | JNI bridge Sentry Android |
| `libbuffer_pgl.so` | 9.000 | Pangle — buffer helper (`pgl`) |
| `libtobEmbedPagEncrypt.so` | 8.304 | Pangle — enkripsi embed PAG (`tobEmbedPagEncrypt`) |
| `libfile_lock_pgl.so` | 6.312 | Pangle — file locking (`pgl`) |

Isi lain split ini: `AndroidManifest.xml` (1.052 B), `stamp-cert-sha256` (32 B), `META-INF/BNDLTOOL.SF|.RSA|MANIFEST.MF`. **Tidak ada** `armeabi-v7a`, `x86`, atau `x86_64`.

### 5.2 Split lain

- **`config.en.apk`** (9 file): `AndroidManifest.xml`, 3 drawable `res/drawable-en/monetization_ads_instream_internal_*`, `resources.arsc` (76.392 B), stamp, 3 META-INF.
- **`config.mdpi.apk`**: `AndroidManifest.xml`, resource drawable densitas-mdpi untuk AppLovin/ExoPlayer/iklan, `resources.arsc`, stamp, META-INF.
- Semua split bertanda `1981-01-01 01:01` (ZIP epoch normalisasi bundletool) dan ditandatangani sertifikat Google yang sama.

### 5.3 Base APK — struktur

| Item | Jumlah / detail |
|---|---|
| Total entri ZIP | **2.360 file**, 110.289.674 byte uncompressed |
| `AndroidManifest.xml` | 70.564 byte (biner) |
| `resources.arsc` | ada (tabel resource, package id `0x7f`) |
| `classes*.dex` | **13**: `classes.dex` (10.918.168), `classes2` (27.304), `classes3` (11.048.188), `classes4` (8.334.172), `classes5` (9.808.836), `classes6` (5.156.832), `classes7` (9.353.420), `classes8` (7.435.364), `classes9` (9.589.124), `classes10` (8.335.756), `classes11` (10.467.440), `classes12` (8.778.944) |
| DEX tambahan | `assets/audience_network.dex` (5.017.908) — Meta Audience Network dimuat dinamis |
| Lainnya | `DebugProbesKt.bin`, `androidsupportmultidexversion.txt`, `stamp-cert-sha256`, `META-INF/*` (banyak `*.version`, `*.kotlin_module`, lisensi) |

**Assets (136 file)** — milik aplikasi:
- `assets/worldTemplate/level.dat` dan `levelname.txt` (template world flat)
- `assets/texture_pack.mcpack` (texture pack untuk editor peta)
- `assets/entityIcons/*.png` — **82 ikon entitas + `credits.txt`** (allay, armadillo, axolotl, bat, bee, blaze, bogged, breeze, camel, cat, cave_spider, chicken, cod, cow, creeper, dolphin, donkey, drowned, elder_guardian, ender_dragon, enderman, endermite, evoker, fox, frog, ghast, glow_squid, goat, guardian, hoglin, horse, husk, illusioner, iron_golem, llama, magma_cube, mooshroom, mule, ocelot, panda, parrot, phantom, pig, piglin, piglin_brute, pillager, polar_bear, pufferfish, rabbit, ravager, salmon, sheep, shulker, silver_fish, skeleton, skeleton_horse, slime, sniffer, snow_golem, spider, squid, stray, strider, tadpole, trader_llama, tropical_fish, turtle, vex, villager, vindicator, wandering_trader, warden, witch, wither, wither_skeleton, wither_skull, wolf, zoglin, zombie, zombie_horse, zombie_villager, zombified_piglin)

**Assets pihak ketiga (penting):** `assets/mraid*.js`, `assets/omsdk-v1.js`, `assets/ad-viewer/omid-session-client-v1.js`, `assets/dtb-m.js`, `assets/aps-mraid.js`, `assets/pob_mraid.js`, `assets/taurusx_mraid.js`, `assets/com.moloco.sdk.xenoss.sdkdevkit.mraid.js`, `assets/mobilefuse/{mraid,vast,ad_template}*`, `assets/bm_networks/*.bmnetwork` (8 file: amazon, gam, gam_dynamic, meta_audience, mintegral, my_target, pangle_sdk, vungle), `assets/fyb_*_endcard_tmpl.html`, `assets/builddatas.json`, `assets/rv_binddatas.xml`, `assets/a.json`, `assets/dsa_page.html`, `assets/dexopt/baseline.prof|profm` (baseline profile), `assets/PubMatic_Logo.svg`, `assets/pob_*.svg`.

### 5.4 Versi dependensi dari `META-INF/*.properties`

| Pustaka | Versi | Pustaka | Versi |
|---|---|---|---|
| `play-services-ads`, `play-services-ads-api` | 24.7.0 | `billing` | 7.1.1 |
| `play-services-ads-identifier` | 18.2.0 | `play-services-auth` | 20.7.0 |
| `play-services-appset` | 16.1.0 | `play-services-auth-api-phone` | 18.0.1 |
| `play-services-base` | 18.5.0 | `play-services-basement` | 18.4.0 |
| `play-services-cronet` | 18.0.1 | `play-services-fido` | 20.0.1 |
| `play-services-location` | 21.0.1 | `play-services-measurement-base/-sdk-api` | 20.1.2 |
| `play-services-tasks` | 18.2.0 | `transport-api` | 3.0.0 |
| `transport-runtime`, `transport-backend-cct` | 3.1.8 | `user-messaging-platform` | 3.2.0 |
| `firebase-encoders` | 17.0.0 | `firebase-encoders-json` | 18.0.0 |
| `firebase-encoders-proto` | 16.0.0 | — | — |

Kotlin modules mencakup `applovin_max_productionRelease`, `bidmachine-android-sdk_*_3_4_0`, `ChartboostMonetization-9.9.1_productionRelease`, `DTBAndroidSDK_release`, `bigoads_productionRelease`, `unityads_productionRelease`, `vungle-ads_release`, `mintegral_productionRelease`, `mobilefuse-*`, `moloco-*`, `sentry-android-replay_release`, `okhttp`, `okio`, `ktor-*`, `coil-*`, `room-*`, `work-runtime-*`, dll.

---

## 6. PIHAK KETIGA / SDK / IKLAN — INVENTARISASI

### 6.1 Orkestrasi iklan utama

Bukti langsung dari JADX `MainActivity.java`:
```java
import com.appodeal.ads.Appodeal;
...
Appodeal.initialize(this, "79e2afef82af6ff44ec14c4cd547374550cfb9f45c77e338", 3, new x.a());
...
if (System.currentTimeMillis() - d1.b.b >= 10000 && Appodeal.isLoaded(3)) {
    Appodeal.show(this, 3, "video_interstitial");
}
```
**Appodeal** adalah SDK iklan yang dipanggil langsung aplikasi (app key `79e2af…e338`, tipe ad `3` = interstitial, cooldown 10 detik, placement `video_interstitial`). Appodeal kemudian memuat **AppLovin MAX** sebagai mediator (20 komponen debugger) dan puluhan adapter di bawahnya.

String About: *“The app is supported by ads, but only a minimal amount to avoid disrupting your experience.”* + Discord `https://www.discord.gg/Mr2za3VkUQ`.

### 6.2 Matriks SDK terdeteksi

| SDK / jaringan | Bukti utama | Komponen | Catatan |
|---|---|---:|---|
| **Appodeal** | import + `Appodeal.initialize` di MainActivity | 4 | orkestrator; ConsentActivity, TestActivity, PackageAddedReceiver |
| **AppLovin / MAX** | 20 komponen, `applovin_*` drawable/anim/raw, kotlin module, native crash reporter | 20 | MaxDebugger* (15 activity), AppLovinInitProvider, FullscreenAdService |
| **Google Mobile Ads (AdMob)** | APPLICATION_ID `ca-app-pub-335…8728`, `play-services-ads 24.7.0` | 7 | AdActivity, AdService, MobileAdsInitProvider, flag OPTIMIZE_* |
| **Google Play Billing** | `BILLING`, metadata versi 7.1.1 | 2 | ProxyBillingActivity + V2 |
| **Google Play Auth / GMS** | SignInHubActivity, RevocationBoundService, play-services-auth | 2 | akun Google |
| **Google Datatransport / Firebase** | 3 komponen + firebase-encoders + `CronetProviderClassName` | 3 | telemetri/CrONet |
| **Chartboost** | 4 komponen, module `ChartboostMonetization-9.9.1`, `VideoRepositoryDownloadService` | 4 | ExoPlayer untuk video |
| **Fyber / Inneractive** | 1.688 file JADX, `ia_*` layout/drawable/color, `fyb_*` HTML | 5 | report/fullscreen/browser |
| **Smaato** | 4 komponen, `smaato_sdk_*` resources | 4 | auto-init=false |
| **Amazon DTB / APS** | `DTBAndroidSDK_release`, `amazon_productionRelease`, `dtb-m.js` | 3 | DTBInterstitial(exported), DTBAdActivity, ApsInterstitial |
| **PubMatic / OpenWrap** | 3 komponen, `pob_*`, `PubMatic_Logo.svg`, query `com.pubmatic.openwrapapp` | 3 | MRAID/video/browser |
| **Yandex Mobile Ads** | 4 komponen, metadata auto-init=false, drawable mediator | 4 | AppMetrica integration |
| **AppMetrica** | 2 komponen, `libapminsight*.so`, `io.appmetrica.*` JADX | 2 | analytics (+,Yandex) |
| **InMobi** | `InMobiAdActivity`, `inmobi_productionRelease` | 1 | — |
| **Pangle / ByteDance** | 15 komponen `TT*`, pangle.version `7.3.0.5`, 6 `.so`, `libtt_*` drawables/strings | 15 + service | heavy native |
| **Unity Ads** | 5 komponen + `unity-ads_release`, `unityads_productionRelease`, `loadLibrary("unity")` | 5+1 | Startup initializer |
| **ironSource / LevelPlay** | 6 komponen, `level_play_productionRelease`, `mediationsdk_release` | 6 | 2 lifecycle provider |
| **Mintegral / Mbridge** | 4 komponen, ~50 layout `mbridge_*`, ~100 drawable | 4 | warna `mbridge_*` 60 |
| **Vungle** | 2 komponen, `vungle-ads_release`, `vungle_productionRelease` | 2 | initOrder 102 |
| **BIGO Ads** | 11 komponen, ~165 layout `bigo_ad_*`, ratusan drawable/string | 11 | splash/interstitial/reward/form |
| **TaurusX** | 9 komponen, `taurusx_*` resources + MRAID JS | 9 | file provider `nath_tax_file_path` |
| **OGury / Presage** | 4 komponen, `ogury*` JADX (589 file) | 4 | LogEnablerReceiver exported |
| **StartApp** | 6 komponen, `startio_productionRelease`, SPLASH_ENABLED=false, BOOT_COMPLETED | 6 | background + scheduler service |
| **MobileFuse** | 5 komponen, module `mobilefuse-*`, auto-init=false, OMID license | 5 | vast/mraid assets |
| **Moloco** | 4 activity + Startup initializer + `moloco-*` modul | 4+1 | Xenoss renderer |
| **MyTarget** | 2 komponen, JADX `com.my.tracker.obfuscated` | 2 | content provider |
| **Facebook Audience Network** | 2 komponen + `audience_network.dex` (5 MB) | 2 | dimuat dinamis |
| **BidMachine** | 5 komponen, 3 modul `bidmachine-android-sdk_*_3_4_0`, 8 `.bmnetwork` | 5 | version 3.4.0 |
| **ExploreStack IAB** | 3 komponen MRAID/VAST | 3 | — |
| **PubNative Lite** | 8 komponen, JADX 927 file | 8 | consent/browser/volume receiver |
| **Digital Turbine / DT Exchange** | query `IgniteRemoteService`, `dtx_store_promo_layout`, `dt_store_promo_layout` | 0 | tanpa komponen eksplisit |
| **Sentry** | DSN + 5 meta-data + `.so` + `sentry_dialog_user_feedback` | 0 | crash reporting Appodeal |
| **OkHttp / Okio / Ktor / Coil / Gson** | kotlin modules + JADX 171/79/32/93 file | 0 | networking, JSON, gambar |
| **ExoPlayer / Media3** | `exo_*` layout/drawable/string/color, Chartboost download service | 1 | pemutar video iklan |
| **Picasso** | `PicassoProvider` | 1 | gambar (adapter iklan) |
| **PairIP** | 2 komponen + `CHECK_LICENSE` | 2 | lihat §7 |

Jumlah aktivitas iklan/SDK saja ≈ **124** dari 129 activity (termasuk MAX debugger 15, TT 14, bigo 10, taurusx 8, mbridge 4, pubnative 7, dll.).

---

## 7. PAIRIP — LISENSI & DIALOG “Something went wrong”

| Aspek | Temuan |
|---|---|
| Komponen | `com.pairip.licensecheck.LicenseActivity` (`exported=false`) + `com.pairip.licensecheck.LicenseContentProvider` (`exported=false`, authority `com.hextetrahedral.minetuner.com.pairip.licensecheck.LicenseContentProvider`) |
| Izin | `com.android.vending.CHECK_LICENSE` (satu-satunya entri di akhir manifest) |
| Peran | Google Play **App Integrity / licensing check** (PairIP). Dijalankan saat cold-start sebelum `MainActivity` agar APK terlindungi dari modifikasi/piracy. Activity tidak diekspor → hanya dipanggil internal. |
| Observasi runtime (dari artefak emulator di workspace) | `am start -W -n com.hextetrahedral.minetuner/.MainActivity` → topActivity `com.pairip.licensecheck.LicenseActivity`; logcat `pid sent binder code 2 with flags 2 and got error -74` (tanpa Play Store fungsional). |
| Isi dialog (uiautomator `ui_back.xml` / `dyn_01_main.xml`) | Judul: **“Something went wrong”**; pesan: **“Check that Google Play is enabled on your device and that you're using an up-to-date version before opening the app. If the problem persists try reinstalling the app.”**; tombol **CLOSE** (`android:id/button1`). |
| Tombol BACK | Tidak menutup dialog; CLOSE mengakhiri proses (focus ke launcher). |
| AVD dengan Play Store asli | PairIP membuka halaman sign-in `com.google.android.finsky.unauthenticated.activity.UnauthenticatedMainActivity`; tanpa akun Google, BACK → aplikasi keluar. |
| Kesimpulan | Dialog **bukan fitur MineTuner**, melainkan efek lingkungan (emulator tanpa lisensi Play). World list tidak pernah tercapai secara dinamis; analisis UI statis tetap valid. |

---

## 8. KESIMPULAN & CATATAN PEMBEDA

1. **Aplikasi inti sangat kecil dibanding bungkusnya:** 5 activity + 1 provider vs 172 komponen pihak ketiga; 20 layout aplikasi vs 507 layout iklan/library; 13 DEX sebagian besar adalah kode iklan/analytics.
2. **Desain Visual:** Material3 Light, hijau `#429E46`, latar `#F1F8E9`, kartu putih radius 8dp/elevasi 4dp, FAB primary dengan ripple `#66BB6A`, font `Inter`. Tidak ada dark theme.
3. **Fitur editor:** Berbasis Android View (bukan Compose) + RecyclerView; navigasi 12 entri (5 utama + 7 Advanced); `libleveldb.so` memberi akses langsung ke database Bedrock.
4. **Monetisasi berlapis:** Appodeal → AppLovin MAX → ≥20 adapter; ditambah Sentry (via Appodeal), AppMetrica, Billing, PairIP. Appodeal app key dan AdMob app ID bersifat publik (non-rahasia) namun tetap dicatat.
5. **Build:** dihasilkan dari Android App Bundle via bundletool (v2/v3 signing, SourceStamp, `BNDLTOOL.*`, `splitTypes`), diunggah ke APKPure sebagai XAPK v2.
6. **Tidak ditemukan:** permission kontak/kamera/mikrofon/lokasi presisi; komponen exported selain 12 yang tercantum; library native untuk arsitektur selain arm64-v8a.

— **Akhir laporan.** Seluruh angka di atas berasal dari dump langsung (`aapt2`, `unzip`, `apksigner`, `sha256sum`) dan diverifikasi silang dengan dekompilasi JADX serta artefak dinamis yang tersedia di workspace.

---

# Bagian 6 — PairIP: kode lisensi, paywall & error dialog

> Bagian ini ditulis dari **smali** (`classes2.dex`) karena 22 class PairIP tidak ikut terdekompilasi dalam tiga direktori jadx. Ini adalah penjelasan code-level mengapa aplikasi referensi menampilkan dialog “Something went wrong” / halaman sign-in Play Store di emulator (lihat `DOKUMENTASI.md` Bagian B3).

### 6.1 Daftar class (22, `com.pairip/licensecheck/`)

`ILicenseV2ResultListener`, `ILicenseV2ResultListener$Stub`, `LicenseActivity`, `LicenseActivity$ActivityType`, `LicenseActivity$$ExternalSyntheticLambda0..2`, `LicenseCheckException`, `LicenseClient`, `LicenseClient$ImmediateTaskExecutor`, `LicenseClient$DelayedTaskExecutor`, `LicenseClient$LicenseCheckState`, `LicenseClient-IA`, `LicenseClient$$ExternalSyntheticLambda0..7`, `LicenseContentProvider`, `ResponseValidator`.

### 6.2 Pemicu otomatis saat proses aplikasi dimulai

`LicenseContentProvider` dideklarasikan di manifest dengan `exported=false` dan authority `com.hextetrahedral.minetuner.com.pairip.licensecheck.LicenseContentProvider`. Android meng-instantiate semua provider yang dideklarasikan saat proses aplikasi dimulai, sehingga `onCreate()`-nya langsung berjalan:

```smali
.method public onCreate()Z
    new-instance v0, Lcom/pairip/licensecheck/LicenseClient;
    invoke-virtual {p0}, ...->getContext()Landroid/content/Context;
    invoke-direct {v0, v1}, LicenseClient;-><init>(Landroid/content/Context;)V
    invoke-virtual {v0}, LicenseClient;->initializeLicenseCheck()V
    const/4 v0, 0x1
    return v0
.end method
```

Ini menjelaskan mengapa dialog muncul **sebelum** MainActivity terlihat — tidak ada pemanggilan eksplisit dari kode aplikasi yang terdekompilasi; pemicunya adalah provider PairIP itu sendiri.

### 6.3 `LicenseClient` — alur pengecekan

- Konstruktor menyimpan `Context`, membuat `DelayedTaskExecutor` (delay awal 0), `retryNum = 0`.
- `connectToLicensingService()`: bind ke **`com.android.vending.licensing.ILicensingService`**; log: “Connecting to the licensing service…”, “Connected to the licensing service.”, “Could not bind with the licensing service.”, “Not allowed to bind with the licensing service.”, “Licensing service unexpectedly disconnected.”, “Licensing service process died.”
- `checkLicenseInternal(IBinder)` mengirim request AIDL V2 dengan listener `ILicenseV2ResultListener`; log: “Request to licensing service sent.”, “Sending request to licensing service…”, “Licensing service could not process request.”, “Error when calling licensing service.”
- `populateInputDataForLicenseCheckV2(...)` / `populateInputDataForReportAutoVerifiedLicense(...)` menyiapkan `Parcel` berisi data + binder callback.
- `processResponse(int, Bundle)` — state yang terdeteksi dari string: `CHECK_REQUIRED`, `FULL_CHECK_OK`, `LOCAL_CHECK_OK`, `LOCAL_CHECK_REPORTED`. Log sukses: “License check succeeded.”
- `performLocalInstallerCheck()` memeriksa install source: “Local install check passed due to system app.” / “Local install check failed due to wrong installer.” / “Local install check bypassed due to old SDK version / package manager not found / app package info not found / install source info not found.” — **inilah sebab instalasi via `adb install` (installer bukan Play Store) selalu memicu pengecekan penuh**.
- Retry: “Retry #%d. License check failed with error '%s'. Next try in %ds…”, “Retry limit reached for: ”.
- Reporting: “Sending request to license reporting service…”, “Error when reporting license check: ”.
- Metode lain: `handleError(LicenseCheckException)`, `createCloseAppIntentOrExitIfAppInBackground()`, `isForeground()`, `getLicensePubKey()`, `lambda$initializeLicenseCheck$0/1`.

### 6.4 `ResponseValidator` — validasi respons lisensi

- `validateResponse(Bundle, String)`: mengambil **`LICENSE_DATA`**, base64 → JSON (`base64ToJson`), memeriksa `alg` harus **`RS256`**, verifikasi `SHA256withRSA` dengan kunci publik RSA, dan mencocokkan field **`packageName`** (error: “Package name doesn't match.”).
- Error lain: “Invalid response”, “Could not decode json”, “Could not base64 decode returned signature”, “Could not parse returned signature.”, “Signature verification failed.”, “Could not create key specification from the public key”, “Could not decode public key”, “%s algorithm not found on device”, “Response must be signed with RS256 algorithm.”

### 6.5 `LicenseActivity` — dua mode tampilan

`onCreate` tidak melakukan apa pun selain `super.onCreate`; semua kerja di `onStart()`:

```smali
invoke-virtual {p0}, ...->getIntent()Landroid/content/Intent;
const-string v1, "activitytype"
invoke-virtual {v0, v1}, Intent->getSerializableExtra(...)  # ActivityType
invoke-virtual {v0}, ActivityType->ordinal()I
if-eqz v0, :cond_1      # 0 = PAYWALL
const/4 v1, 0x1
if-eq v0, v1, :cond_0   # 1 = ERROR_DIALOG
:cond_0  -> showErrorDialog()
:cond_1  -> showPaywallAndCloseApp()
```

- **`ActivityType.PAYWALL` (ordinal 0)** → `showPaywallAndCloseApp()`: mengambil extra **`paywallintent`** / **`PAYWALL_INTENT`** berupa `PendingIntent`; log “Paywall intent is not provided.”, “Paywall intent unexpectedly cancelled.”; setelah selesai → `closeApp()`. Secara empiris (B3), ini yang muncul sebagai halaman **sign-in Play Store** (`UnauthenticatedMainActivity`) pada AVD dengan Play Store asli namun tanpa akun Google.
- **`ActivityType.ERROR_DIALOG` (ordinal 1)** → `showErrorDialog()`: `AlertDialog` dengan judul **“Something went wrong”**, pesan **“Check that Google Play is enabled on your device and that you're using an up-to-date version before opening the app. If the problem persists try reinstalling the app.”**, satu tombol **“Close”** (`lambda$showErrorDialog$1` → `closeApp()`). Gagal memunculkan dialog → log “Couldn't show the error dialog. ”. Secara empiris (B3), inilah dialog yang tampil pada AVD dengan stub `LicenseChecker.apk`.

### 6.6 Kenapa aktivitas ini tidak terlihat di dekompilasi jadx utama

Seluruh 22 class berada di `classes2.dex` dan_only_ tersedia sebagai smali di `.ref_extract/apktool_out/smali_classes2/...`. Tidak ada class aplikasi lain yang mereferensikannya secara langsung — satu-satunya jalur masuk adalah provider yang di-instantiate sistem (§6.2). Manifest mendeklarasikan `LicenseActivity` dengan `exported=false`, jadi hanya proses aplikasi sendiri yang bisa membukanya.


---

# Bagian 7 — Ringkasan temuan lintas lapisan

## 7.1 Peta end-to-end (versi padat)

```
[a] PairIP provider (auto-start, class2.dex)
      → LicenseClient → ILicensingService → LicenseActivity(PAYWALL|ERROR_DIALOG)
      → tanpa lisensi valid: aplikasi ditutup

[b] MainActivity (class9)
      → scan getExternalFilesDir()/worlds  (+ legacy minecraftWorlds s.d.≤28)
      → t.j (world context) per folder: levelname.txt, world_icon.jpeg, encrypted?
      → adapter y.g → kartu world → onResult "Options for <nama>"
          ├─ Edit      → j() cek enkripsi → progress → x.b.a.set(j) → DrawerActivity
          ├─ Open in Minecraft → r2.a case 9 → export.mcworld → share
          └─ Delete    → konfirmasi → d1.b.h() recursive delete → adapter.notifyItemRemoved

[c] DrawerActivity
      → 13 item menu → f0.h (level.dat) | f0.a.f(key) (LevelDB) | f0.b (players) | d0.e (map)
      → f0.g engine editor: breadcrumb h0.d, adapter h0.c, dialog-tag, rootTag.tmp restore

[d] Engine NBT (class6 + class12)
      x2.a (12 tipe) + y2.l (parse/serialize) + d1.b (validasi, builder tree, world builder)
      level.dat: [int32 LE 8][int32 LE len][NBT LE]      LevelDB value: NBT mentah

[e] Data Bedrock
      t.j → libleveldb.so: player_server_*, ~local_player, Overworld, TheEnd, Nether,
             BiomeData, portals, AutonomousEntities, chunk dig/5/actorprefix
      s.a → parse subchunk v1/8/9, heightmap, entity NBT
      t.c (1181 block) + t.a (~87 biome) + t.f (123 entity, 83 ikon)
```

## 7.2 Tabel ringkas "apa yang muncul saat menekan"

| Tempat | Aksi | Hasil (judul/teks kunci) |
|---|---|---|
| App start | (otomatis) | PairIP: “Something went wrong” / paywall Play Store |
| Home | FAB + | expand stack: “New World”, “Import World” |
| Home | Import World | “Import World” → MCWORLD/MCTEMPLATE/CANCEL → picker → “Importing world...” |
| Home | tap world | `Options for "<nama>"` → Edit / Open in Minecraft / Delete |
| Home | Edit (terenkripsi) | `Encrypted World` + penjelasan Marketplace |
| Home | Delete | `Delete World` + “Are you sure ... cannot be undone.” |
| Drawer | item NBT | `Close NBT Editor` + “Any unsaved changes will be lost.” |
| Drawer | Change World | `Close World` + “Are you sure ...” |
| Drawer | Open Custom NBT | `Custom NBT Key`, hint `e.g. ~local_player` |
| Editor | long-press baris | `Options for "<key>"` → Edit / Remove |
| Editor | Edit | `Edit "<key>"`, hint `new value for the tag`, inputType per tipe |
| Editor | Remove (container) | `Remove "<key>"` + “Are you sure you want to remove this tag?” |
| Editor | Add FAB | `Create NBT Tag` (spinner tipe + nama; List → pilih tipe elemen) |
| Editor | Save FAB | “Saving NBT data...” → “NBT data saved successfully.” / “Failed to save NBT data.” |
| Create World | Biome/Block | SearchView “Search biomes...” / “Search blocks...” + DiffUtil |
| Create World | ketebalan layer | `SliderDialog` (MIN/INITIAL/MAX/VALUE), batas 384 |
| Map | tap | `Coords(x,z) Chunk(cx,cz)` → Begin Selection / View Entities |
| Map | seleksi aktif | `Selection Options` → Take Picture / End Selection |
| Help | Import texture pack | `import_texture_pack_progress_dialog` → “Failed to import texture pack.” |

## 7.3 Konstanta & angka penting

| Nilai | Makna |
|---|---|
| `8`, `int32 LE` | header size pada `level.dat` |
| `-1680885252` (`0x9BCFB9FC`, byte LE `FC B9 CF 9B`) | signature world terenkripsi di 8 byte pertama file `db/*` |
| `65535` | batas panjang nama tag / nilai String (UTF-8) menurut validator |
| `32767` | batas aman aktual parser nama (signed `short`), lebih rendah dari validator |
| `384` | batas total ketebalan layer flat world (CreateWorldActivity) |
| `1181` | jumlah entri enum block `t.c` |
| `~87` | jumlah biome `t.a` |
| `123 / 83` | jumlah entity `t.f` / yang punya file ikon |
| `256` | ukuran bitmap tile peta (16 chunk × 16 blok) |
| `2^25`, `2^20` | koordinat TileView / koordinat blok Minecraft (±1.048.576) |
| `0.2 .. 1.0` | rentang skala peta efektif |
| `256` | kapasitas LRU cache chunk `t.d` |
| `2048` | batas maksimal dimensionalitas screenshot peta |
| `10000` ms | throttle interstitial Appodeal antar aksi |

## 7.4 Daftar pesan error/validasi (kutipan persis, lintas modul)

```
"No header found."
"Invalid NBT formatting."
"Reached end of stream while attempting to read <n> bytes"
"Unknown tag to edit, report to developers."
"Out of bounds for byte."
"Out of bounds for short."
"Out of bounds for integer."
"Out of bounds for long."
"Out of bounds for float."
"Out of bounds for double."
"Out of bounds for string."
"Failed to create tag, tag name is out of bounds."
"Failed to create tag"
"Only byte tags can be added to byte arrays"
"Only int tags can be added to int arrays"
"List element type mismatch, it should be <TIPE>"
"A tag with this name already exists in the compound"
"Tags inside a CompoundTag must have a name"
"Failed to add tag: <msg>"
"Cannot remove this tag."
"Failed to remove tag: <msg>"
"An error occurred when editing value."
"Unknown tag type"
"NBT data saved successfully."
"Failed to save NBT data."
"No NBT data found."
"No world data found."
"No players found."
"Could not get entity keys."
"Could not create worlds directory."
"Could not create world folder."
"Could not create db folder."
"Failed to save world level data."
"Failed to build flat world data."
"Failed to load world level data."
"Failed to copy template files."
"Failed to create world database."
"Encrypted World" / "This world is encrypted and cannot be edited..."
"Please select a valid .<ext> file"
"No selection made." / "Selection is too large." / "Error saving picture." / "Successfully saved picture."
"No file type selected."
"Low Storage Warning" / "Your device is low on storage..."
"Failed to import texture pack."
"Something went wrong" (PairIP)
"Check that Google Play is enabled on your device and that you're using an up-to-date version before opening the app. If the problem persists try reinstalling the app."
```

## 7.5 Inkonsistensi / potensi bug yang tercatat

1. **Parser nama NBT signed-short** vs validator 65535 byte → nama > 32767 bisa memicu `NegativeArraySizeException`.
2. **Tidak ada batas count/len** di parser `y2.l.k()` → file korup bisa memicu alokasi besar / OOM.
3. **Charset default** untuk nama & string NBT; validator eksplisit UTF-8.
4. **`t.j.m()` memakai `readLock()` saat menulis `level.dat`** (harusnya writeLock).
5. **Zip export** `r2.a` case 9 hanya menambahkan subfolder bila root world berupa direktori — file `level.dat`/`levelname.txt` di root berisiko tidak ikut (kemungkinan bug; Minecraft membangkitkan ulang metadata).
6. **Sanitasi nama import** hanya membuang ekstensi; karakter/path aneh tidak difilter (bergantung pada perlindungan zip4j).
7. **`z.l` SliderDialog `setSelectionChangedListener` stub** — tidak ada callback perubahan seleksi peta.
8. **Exception sering ditelan** (`catch (Exception unused) {}`) pada restore editor, ikon world, parsing NBT — kegagalan tampil sebagai UI kosong tanpa pesan.
9. **PairIP provider auto-start** mengunci aplikasi penuh tanpa opsi lanjut offline; `LicenseActivity` diekspor=false sehingga hanya internal yang bisa membuka.
10. **JADX artifact**: konstanta diganti nama library lain (`TsExtractor.AUDIO_STREAM` untuk biome ID 177 dll.), field "Fake field" pada enum, switch case duplikat — bukan perilaku runtime.
11. **`t.c` warna block kehilangan komponen merah & ordinal di dekompilasi Java** — nilai asli harus dibaca dari smali `Color.rgb(p2,p3,p4)`.

## 7.6 Kesimpulan

Kode aplikasi referensi terdiri dari tiga lapisan inti yang rapat:

- **UI Java (classes9)** — lima aktivitas + fragment editor + adapter + peta TileView + tiga keluarga dialog (`z.*`), semuanya berkomunikasi lewat dialog-tag/Bundle dan singleton `x.b.a()`.
- **Engine NBT (classes6 + classes12)** — `x2.a`/`y2.*`/`d1.b` dengan format Bedrock little-endian, header 8 byte, dan 7+ pesan validasi eksplisit.
- **Data Bedrock (classes9 + `libleveldb.so`)** — `t.j` + `s.a` + JNI LevelDB untuk world, player, chunk, entity, biome, portal; tabel referensi `t.a/c/f`.

Monetisasi (~5.200 file Java hasil dekompilasi) tidak mengubah jalur data NBT, kecuali flag `d1.b.a` dan throttle interstitial `d1.b.b/t`. Penguncian PairIP via ContentProvider adalah satu-satunya gerbang yang menghalangi aplikasi referensi berjalan di emulator, dan sudah dijelaskan tuntas di Bagian 6.

---

# Bagian 8 — Analisis UI/UX: desain, interaksi, aksesibilitas & evaluasi

> Sudut pandang bagian ini berbeda dari Bagian 2 (pelacakan kode) dan Bagian 5 (inventaris resource): di sini resource dan perilaku dibaca sebagai **keputusan desain** — bagaimana aplikasi terlihat, bagaimana pengguna bergerak, apa umpan baliknya, dan seberapa baik itu bekerja. Sumber: dekompilasi XML layout/menu/style/color/string, kode listener, dan artefak hierarchy emulator. **Keterbatasan:** aplikasi referensi tidak bisa dijalankan di emulator (PairIP, Bagian 6), sehingga penilaian visual bersifat statis dari definisi resource; perilaku interaksi diverifikasi dari kode, bukan dari rekaman layar langsung aplikasi ini.

## 8.1 Ringkasan eksekutif

| Dimensi | Penilaian ringkas |
|---|---|
| Paradigma | Android **View** + Material Components 3 (bukan Jetpack Compose), satu `AppCompatActivity` per layar + `Fragment` untuk editor |
| Identitas visual | Hijau `#429E46` sebagai primary, latar hijau pucat `#F1F8E9`, kartu putih, FAB primary; font **Inter** |
| Navigasi | Linear: Home → World list → Options → Drawer 13 item → editor/peta; breadcrumb horizontal di dalam editor |
| Interaksi khas | FAB stack expand/collapse, dialog konfirmasi, progress dialog + toast, long-press untuk opsi tag, drag-reorder layer, pinch-zoom peta |
| Umpan balik | Kuat (dialog + toast + progress + validasi inline), tetapi sebagian besar bersifat **modal** |
| Aksesibilitas | Dasar terpenuhi (minHeight 48dp, contentDescription ikon, ellipsis), namun tidak ada dark theme dan beberapa target kecil |
| Kekuatan | Informasi hierarkis jelas, aksi destruktif selalu dikonfirmasi, pesan error spesifik, pattern Material3 familier |
| Kelemahan | jargon LevelDB tanpa penjelasan, "Open in Minecraft" sebenarnya ekspor file, iklan interstisial, dialog "Something went wrong" dari PairIP, tanpa undo/redo |

## 8.2 Sistem visual (visual design system)

### 8.2.1 Warna (`md_theme_light_*`, terverifikasi dari `resources.arsc`)

| Peran | Nilai | Pemakaian nyata |
|---|---|---|
| Primary | `#FF429E46` | `colorPrimary` → toolbar (`?attr/colorPrimary`), drawer header (160dp), FAB backgroundTint, ripple, selection handle foreground |
| Secondary | `#FF66BB6A` | ripple FAB, aksen hijau muda |
| OnPrimary | `#FFFFFFFF` | judul toolbar (white), teks drawer header, ikon FAB (`tint`) |
| Background | `#FFF1F8E9` | latar hijau sangat pucat |
| Surface | `#FFFFFFFF` | kartu world (`world_item`), kartu layer, card selector |
| Error | `#FFB00020` | `colorError` tema |
| Toolbar overlay | `ThemeOverlay.AppCompat.Dark.ActionBar` | ikon/toolbar kontras di atas hijau |
| Peta | `#FF444444` latar, `#FF2E2E2E` checkerboard, `#FFFFFFFF` border & teks koordinat 24sp, seleksi `#80000000` | `d0/e`, `d0/a`, `d0/i` |

Tidak ada `md_theme_dark_*` sama sekali → **aplikasi light-only**. Satu-satunya variasi gelap adalah ThemeOverlay pada AppBar dan peta.

### 8.2.2 Tipografi

- Font aplikasi: **Inter** (`res/font/inter`) — satu-satunya font milik aplikasi; sisanya Roboto/roboto_medium_numbers dan font SDK iklan.
- Skala yang dipakai: **20sp bold** judul drawer (`nav_header_title`), **18sp bold** nama world (`world_item`) dan kunci tag (`tree_node_item`), **16sp** teks tubuh/tutorial/label/kotak layer, **14sp** ukuran world (abu) dan nama folder italic, **24sp** koordinat peta.
- Semua teks panjang memakai `maxLines=1` + `ellipsize=end` — menjaga baris tetap satu tinggi, tetapi menyembunyikan nama world/tag panjang.

### 8.2.3 Spasi, bentuk, dan ukuran

| Elemen | Spesifikasi |
|---|---|
| Kartu world / layer / biome | `CardView` radius **8dp**, elevasi **4dp**, padding **12dp**, `cardUseCompatPadding=true` |
| Baris world | ikon **48×48dp**, jarak 12dp, teks 18sp + 14sp + 14sp |
| Baris tree NBT | `minHeight` **48dp**, padding 12dp horizontal / 6dp vertikal, ikon **32×32dp**, jarak 12dp, key 18sp |
| FAB | `fabCustomSize=40dp`, label di kiri ikon, latar `fab_label_background`, margin 16dp |
| Drawer header | **160dp** tinggi, padding 16dp, ikon 64dp, judul 20sp |
| Toolbar | `?attr/actionBarSize`, `?colorPrimary`, titleTextColor white |
| Peta | tile **256px** = 16 chunk; handle seleksi 24×24dp; marker entity 42px; placeholder 28px |

Pola 8/12/16dp konsisten, memberi densitas sedang dan hierarki yang mudah dipindai.

### 8.2.4 Ikonografi

Ikon aplikasi (`ic_*`): `ic_add` (FAB utama), `ic_layers` (New World), `ic_upload` (Import World), `ic_map`, `ic_tune` (World NBT), `ic_person` (Singleplayer), `ic_group_people` (Multiplayer), `ic_exit` (Change World), `ic_list` (dimensi), `ic_search` (custom key), `ic_book` (Help), `ic_info` (About), `ic_save` (Create World). Ikon tipe NBT: `nbt_byte`, `nbt_short`, `nbt_int`, `nbt_long`, `nbt_float`, `nbt_double`, `nbt_string`, `nbt_list`, `nbt_compound`, `nbt_byte_arr`, `nbt_int_arr` — memberi pengenalan tipe visual di tree. Tutorial memakai `step1_tut`–`step4_tut` + `steve`. Entity memakai 82 PNG dari komunitas (kredit di `credits.txt`).

## 8.3 Arsitektur navigasi & information architecture

```
Splash/PairIP gate (di luar kendali UI aplikasi)
        │
        ▼
[Home] Toolbar "MineTuner" + RecyclerView world + FAB stack
        │  tap world
        ▼
[Options for "<nama>"] ─┬─ Edit ──► [Loading world...] ──► [DrawerActivity]
        │                ├─ Open in Minecraft ──► [Compiling world...] ──► share
        │                └─ Delete ──► [Delete World] ──► [Deleting world...]
        ▼
[Drawer] header world + 13 navigasi
   ├─ Show Map ──► [d0.e] tap ──► [Coords/Selection] ──► View Entities ──► [Entities]
   ├─ World NBT ──► [f0.h] editor level.dat
   ├─ Singleplayer / Multiplayer / 7 Advanced key ──► [f0.a / f0.b] editor LevelDB
   └─ Change World ──► [Close World]
        ▼
[Editor] breadcrumb A > B > C + RecyclerView tree + FAB Save/Add
   ├─ tap crumb ──► naik ke level
   ├─ tap baris container ──► masuk level
   ├─ long-press baris ──► [Options "<key>"] ──► Edit / Remove
   └─ back ──► breadcrumb parent / [Close NBT Editor] / [Close World]
```

**Penilaian:** kedalaman maksimal 3–4 layar dari home ke editor leaf, yang wajar untuk editor data hierarkis. Drawer 13 item dengan submenu **Advanced** adalah progressive disclosure yang baik, tetapi 7 item di dalamnya memakai jargon tingkat rendah (`BiomeData NBT`, `AutonomousEntities`, `~local_player`) tanpa penjelasan — hanya cocok untuk pengguna yang sudah paham struktur Bedrock. Tombol back punya **sedikitnya 4** perilaku berbeda (tutup drawer / naik breadcrumb / konfirmasi tutup editor / konfirmasi tutup world) — konsisten secara kontekstual, namun dapat mengejutkan jika pengguna tidak membaca dialog.

## 8.4 Interaksi & umpan balik per layar

| Layar | Elemen | Aksi | Umpan balik |
|---|---|---|---|
| Home | FAB utama `ic_add` | Tap | Rotasi 45°, dua child muncul (`OvershootInterpolator`, 300ms), label putih; guard anti-double-tap **300ms** |
| Home | New World / Import World | Tap | Collapse FAB → Activity/dialog "Import World" (MCWORLD/MCTEMPLATE/CANCEL) |
| Home | Baris world | Tap | Dialog `Options for "<nama>"`; world eksternal ditandai `## [EXTERNAL WORLD] ##` + catatan backup |
| Home | Edit world | Tap | Cek enkripsi → `Encrypted World` ATAU progress "Loading world..." + "Please wait while the world is being loaded." |
| Home | Open in Minecraft | Tap | Progress "Compiling world..." + "Do not exit the app." → interstitial (cooldown 10s) → share file |
| Home | Delete | Tap | Konfirmasi "cannot be undone." → progress "Deleting world..." → toast sukses/gagal |
| Drawer | Item menu | Tap | Jika editor aktif → `Close NBT Editor` ("unsaved changes will be lost."); YES → ganti fragment |
| Drawer | Open Custom NBT | Tap | Input dialog dengan hint `e.g. ~local_player` — memberi contoh konkret |
| Editor | Baris leaf | Tap | Dialog `Edit "<key>"`, keyboard numerik/desimal/teks sesuai tipe, hint `new value for the tag` |
| Editor | Baris apa saja | Long-press | `Options for "<key>"` → Edit / Remove (Remove hanya untuk container) |
| Editor | FAB Add | Tap | Fullscreen form: spinner tipe + nama; List → dialog pilih tipe elemen |
| Editor | FAB Save | Tap | Progress "Saving NBT data..." → toast "NBT data saved successfully." / "Failed to save NBT data." |
| Create World | Biome / +Layer | Tap | Dialog searchable dengan `SearchView` + `DiffUtil` (filter nama/identifier) |
| Create World | Layer | Drag | `ItemTouchHelper` UP/DOWN (swipe dimatikan), `notifyItemMoved` |
| Create World | Save | Tap | Validasi: biome, ≥1 layer, total ≤384; error spesifik per kondisi |
| Peta | Tap / long-press / pinch | Gesture | Dialog koordinat; seleksi kotak; zoom 0.2–1.0; marker entity; screenshot dengan slider skala |
| Help | Import | Tap | Copy `texture_pack.mcpack` → progress "Opening Minecraft..." → buka aplikasi lain |

**Kekuatan umpan balik:** hampir setiap aksi destruktif atau lama punya konfirmasi/progress, dan pesan error menyebut aturan yang dilanggar (mis. "Out of bounds for byte.", "Only byte tags can be added to byte arrays"). Validasi terjadi **di dialog sebelum ditutup** (`InputValidator`), sehingga pengguna tidak kehilangan input saat salah.

**Kelemahan umpan balik:** semua validasi dan kegagalan memakai **Toast/dialog modal**, tidak ada inline error di field; exception sering ditelan (`catch (Exception unused) {}`) sehingga kegagalan tampil sebagai daftar kosong tanpa penjelasan; progress dialog berulang dengan teks panjang "Do not exit the app" terasa mengganggu.

## 8.5 Aksesibilitas & inklusi

**Yang sudah ada (dari resource):**
- Baris tree `minHeight=48dp` dan `focusable=true`/`clickable=true` → target sentuh memadai.
- Ikon tipe punya `contentDescription="icon"`.
- SearchView punya hint ("Search biomes...", "Search blocks...") dan placeholder teks jelas.
- Kontras: teks putih di atas hijau `#429E46` (rasio kontras baik); teks sekunder abu di kartu putih; toolbar memakai dark overlay agar ikon tetap terlihat.
- `maxLines=1` + `ellipsize` mencegah teks bertabrakan dengan elemen lain.
- Semua tombol dialog punya label teks (CANCEL/OK/YES/NO/DELETE/REMOVE/CLOSE).

**Yang kurang / berisiko:**
- Tidak ada **dark theme** (`md_theme_dark_*` tidak ada) — tidak nyaman di gelap.
- FAB utama/child tidak punya `contentDescription` eksplisit; label "New World"/"Import World" hanya muncul saat expanded (mengandalkan teks visual).
- Handle seleksi peta **24×24dp** — di bawah rekomendasi 48dp.
- Ikon-only `ImageButton` (add layer, hapus layer) bergantung pada konteks visual.
- Nama world/tag panjang dipotong dengan elipsis tanpa tooltip/cara melihat penuh.
- Dialog "Low Storage Warning" tidak bertag/sulit diuji dan tidak bisa diabaikan secara eksplisit di kode.
- Tidak ada indikasi bahwa teks mendukung font scale sistem (ukuran sp sudah benar, tetapi tidak ada uji dynamic type).

## 8.6 Evaluasi UX — kekuatan & kelemahan

### Kekuatan
1. **Hierarki informasi jelas:** toolbar → kartu world → drawer → tree; pengguna selalu tahu "di mana" berada berkat judul/toolbar dan breadcrumb.
2. **Primary action tunggal:** FAB `+` dengan dua opsi (New/Import) mengikuti pola Material yang familier; animasi memberi petunjuk bahwa ada opsi tersembunyi.
3. **Progressive disclosure:** menu Advanced menyembunyikan 7 entri LevelDB tingkat lanjut dari pengguna baru.
4. **Confirmations yang baik:** delete world, remove tag, close editor, close world semuanya punya dialog dengan konsekuensi tertulis.
5. **Pesan error yang actionable:** bukan "Invalid input", melainkan aturan spesifik + batas numerik.
6. **Breadcrumb interaktif:** navigasi naik tanpa harus menekan back berulang.
7. **Pencarian dengan filter langsung** (DiffUtil) untuk 1.181 blok dan ~87 biome.
8. **Drag-reorder layer** memberi kontrol urutan yang intuitif untuk flat world.

### Kelemahan
1. **Jargon tanpa edukasi:** `BiomeData NBT`, `Portals NBT`, `AutonomousEntities`, `e.g. ~local_player` mengasumsikan pengetahuan Bedrock.
2. **Label menyesatkan:** "Open in Minecraft" sebenarnya mengekspor `.mcworld` lalu membuka file (bukan langsung meluncurkan Minecraft).
3. **Tanpa undo/redo** di editor referensi — kesalahan edit hanya bisa dibatalkan lewat dialog close yang membuang semua perubahan.
4. **Long-press sebagai satu-satunya jalan ke Edit/Remove** — tidak terlihat (no affordance) bagi pengguna baru.
5. **Iklan interstisial** muncul setelah export/delete/import (cooldown 10 detik), mengganggu alur kerja.
6. **PairIP gate** menghasilkan "Something went wrong" generik tanpa opsi lanjut — ini bukan desain aplikasi, tetapi pengalaman startup yang buruk di emulator/perangkat tanpa Play Store.
7. **Light-only** dan tidak ada pengaturan tema.
8. **Modal overload:** progress dialog + toast + konfirmasi beruntun pada alur import/export/delete.
9. **Teks dipotong** (`maxLines=1`) tanpa cara melihat nilai penuh di kartu world.
10. **State restore editor memakai file cache** (`rootTag.tmp`) dan mencocokkan breadcrumb berdasarkan **label**, sehingga dua tag dengan nama sama bisa salah navigasi setelah rotasi.

## 8.7 Perbandingan dengan praktik Material 3 & rekomendasi

| Aspek | Referensi | Praktik Material 3 | Rekomendasi (prioritas) |
|---|---|---|---|
| Tema | Light-only + hijau kustom | Dynamic color + light/dark | P1: tambah dark theme & "Use device theme" |
| Toolbar | `?colorPrimary` + overlay gelap | Surface + onSurface, `surfaceTint` | P2: pertimbangkan toolbar surface dengan primary tersembunyi |
| FAB | 40dp kustom, label kiri | 56dp default, extended FAB dengan ikon+teks | P2: seragamkan 56dp / Extended FAB agar target sentuh & label jelas |
| Dialog | SimpleDialog generik, teks panjang | Dialog dengan headline + supporting text + tombol hierarkis | P2: ringkas copy progress, gunakan linear progress inline |
| Validasi | Toast after submit | Inline error di field | P1: pindahkan pesan "Out of bounds ..." ke field helper text |
| Navigasi | Drawer 13 item + jargon | NavigationRail/Drawer dengan group label | P1: kelompokkan Advanced + berikan subtitle penjelas |
| Breadcrumb | TextView 10dp padding, scroll horizontal | Breadcrumb dengan separator & icon | P2: tambah contentDescription & indikator scroll |
| Undo | Tidak ada | Snackbar "Undo" | P0 (untuk editor): snackbar undo untuk add/edit/delete tag |
| Aksesibilitas | contentDescription hanya ikon tree | Semua ikon-only control berlabel | P1: contentDescription untuk FAB & ImageButton |
| Error handling | Exception ditelan | Surface error + retry | P1: tampilkan pesan gagal saat restore/parse/error DB |

## 8.8 Bukti & keterbatasan

- Penilaian visual bersumber dari `aapt2 dump xmltree` pada 10 layout/menu milik aplikasi, `dump resources` (warna, string, style), dan kode listener (`MainActivity`, `DrawerActivity`, `f0.g`, `d0.e`, `z.*`).
- Perilaku interaksi (animasi 300ms, debounce 300ms, thumb slider, drag UP/DOWN, pinch zoom, cooldown iklan 10s) diverifikasi dari kode, bukan dari rekaman UI — karena aplikasi referensi diblokir PairIP di emulator (Bagian 6).
- Tidak tersedia screenshot resmi aplikasi referensi; oleh karena itu penilaian kontras/kspacing bersifat **inferensi dari definisi resource**, bukan pengukuran pixel aktual.
- Rekomendasi di §8.7 ditujukan untuk implementasi turunan (termasuk aplikasi C++ `com.nbteditor` di workspace), bukan perubahan pada XAPK.

### 8.9 Status penyesuaian `out/NBTEditor.apk`
- **Sudah sesuai:** palet warna referensi (light + dark), Inter, kartu 8dp/4dp, FAB primary, breadcrumb, 13 tipe NBT, pesan validasi referensi.
- **Diselaraskan pada v1.0.6–1.0.8 (code 7–9):** teks konfirmasi `"Delete World" / "Are you sure you want to delete this world? This action cannot be undone."`, dialog `"Encrypted World"` + pesan Marketplace, progress `"Compiling world..." / "Please wait while the world is being compiled. Do not exit the app."`, judul `"Flat World Layers"`, identitas referensi + tautan diskusi di Help; **UI Home dirombak** mengikuti referensi: toolbar + daftar world berkartu (ikon 48dp · nama bold 18sp · ukuran · nama folder) dan **FAB stack** yang expands ke label **"New World" / "Import World"**; tap world membuka dialog **"Options for \"<nama>\""** dengan aksi **EDIT / EXPORT / DELETE**.
- **Masih berbeda secara disengaja:** nama paket/brand (`com.nbteditor` / "NBT Editor"), tanpa iklan/PairIP, tanpa drawer jargon LevelDB, dan ekspor disebut "Export .mcworld" (bukan "Open in Minecraft").

---

# Bagian 9 — Inventarisasi EXHAUSTIF seluruh file XAPK (tanpa pengecualian)

Diambil langsung dari 4 entri ZIP di dalam XAPK. Setiap file tercantum dengan kategori dan ukuran *uncompressed*. Total **2.591 file** (base 2.360 + arm64 17 + en 9 + mdpi 205).

## 9.1 Ringkasan per APK & kategori

| APK | Kategori | Jumlah file | Ukuran (byte) |
|---|---|---:|---:|
| base | META-INF | 253 | 78981 |
| base | assets | 135 | 640661 |
| base | dex | 13 | 104271456 |
| base | manifest | 1 | 70564 |
| base | other | 67 | 278145 |
| base | res | 1890 | 3887839 |
| base | resource-table | 1 | 1062028 |
| **base** | **TOTAL** | **2360** | **110289674** |
| config.arm64_v8a | META-INF | 3 | 4656 |
| config.arm64_v8a | manifest | 1 | 1052 |
| config.arm64_v8a | native-libs | 12 | 3877888 |
| config.arm64_v8a | other | 1 | 32 |
| **config.arm64_v8a** | **TOTAL** | **17** | **3883628** |
| config.en | META-INF | 3 | 3490 |
| config.en | manifest | 1 | 1020 |
| config.en | other | 1 | 32 |
| config.en | res | 3 | 14308 |
| config.en | resource-table | 1 | 76392 |
| **config.en** | **TOTAL** | **9** | **95242** |
| config.mdpi | META-INF | 3 | 44720 |
| config.mdpi | manifest | 1 | 1052 |
| config.mdpi | other | 1 | 32 |
| config.mdpi | res | 199 | 194819 |
| config.mdpi | resource-table | 1 | 48052 |
| **config.mdpi** | **TOTAL** | **205** | **288675** |

## 9.2 Daftar LENGKAP file per APK


### base (2360 file)

| # | File | Kategori | Ukuran |
|---:|---|---|---:|
| 1 | `AndroidManifest.xml` | manifest | 70564 |
| 2 | `DebugProbesKt.bin` | other | 1728 |
| 3 | `META-INF/ChartboostMonetization-9.9.1_productionRelease.kotlin_module` | META-INF | 88 |
| 4 | `META-INF/DTBAndroidSDK_release.kotlin_module` | META-INF | 66 |
| 5 | `META-INF/activity-compose_release.kotlin_module` | META-INF | 176 |
| 6 | `META-INF/activity_release.kotlin_module` | META-INF | 389 |
| 7 | `META-INF/admob_release.kotlin_module` | META-INF | 668 |
| 8 | `META-INF/ads-adservices-java_release.kotlin_module` | META-INF | 96 |
| 9 | `META-INF/amazon_productionRelease.kotlin_module` | META-INF | 201 |
| 10 | `META-INF/androidx.activity_activity-compose.version` | META-INF | 6 |
| 11 | `META-INF/androidx.activity_activity-ktx.version` | META-INF | 6 |
| 12 | `META-INF/androidx.activity_activity.version` | META-INF | 6 |
| 13 | `META-INF/androidx.annotation_annotation-experimental.version` | META-INF | 6 |
| 14 | `META-INF/androidx.appcompat_appcompat-resources.version` | META-INF | 6 |
| 15 | `META-INF/androidx.appcompat_appcompat.version` | META-INF | 6 |
| 16 | `META-INF/androidx.arch.core_core-runtime.version` | META-INF | 67 |
| 17 | `META-INF/androidx.autofill_autofill.version` | META-INF | 6 |
| 18 | `META-INF/androidx.browser_browser.version` | META-INF | 6 |
| 19 | `META-INF/androidx.cardview_cardview.version` | META-INF | 6 |
| 20 | `META-INF/androidx.compose.animation_animation-core.version` | META-INF | 6 |
| 21 | `META-INF/androidx.compose.animation_animation.version` | META-INF | 6 |
| 22 | `META-INF/androidx.compose.foundation_foundation-layout.version` | META-INF | 6 |
| 23 | `META-INF/androidx.compose.foundation_foundation.version` | META-INF | 6 |
| 24 | `META-INF/androidx.compose.material_material-icons-core.version` | META-INF | 6 |
| 25 | `META-INF/androidx.compose.material_material-ripple.version` | META-INF | 6 |
| 26 | `META-INF/androidx.compose.material_material.version` | META-INF | 6 |
| 27 | `META-INF/androidx.compose.runtime_runtime-saveable.version` | META-INF | 6 |
| 28 | `META-INF/androidx.compose.runtime_runtime.version` | META-INF | 6 |
| 29 | `META-INF/androidx.compose.ui_ui-geometry.version` | META-INF | 6 |
| 30 | `META-INF/androidx.compose.ui_ui-graphics.version` | META-INF | 6 |
| 31 | `META-INF/androidx.compose.ui_ui-text.version` | META-INF | 6 |
| 32 | `META-INF/androidx.compose.ui_ui-unit.version` | META-INF | 6 |
| 33 | `META-INF/androidx.compose.ui_ui-util.version` | META-INF | 6 |
| 34 | `META-INF/androidx.compose.ui_ui.version` | META-INF | 6 |
| 35 | `META-INF/androidx.constraintlayout_constraintlayout.version` | META-INF | 6 |
| 36 | `META-INF/androidx.coordinatorlayout_coordinatorlayout.version` | META-INF | 6 |
| 37 | `META-INF/androidx.core_core-ktx.version` | META-INF | 7 |
| 38 | `META-INF/androidx.core_core.version` | META-INF | 7 |
| 39 | `META-INF/androidx.cursoradapter_cursoradapter.version` | META-INF | 6 |
| 40 | `META-INF/androidx.customview_customview-poolingcontainer.version` | META-INF | 6 |
| 41 | `META-INF/androidx.customview_customview.version` | META-INF | 6 |
| 42 | `META-INF/androidx.datastore_datastore.version` | META-INF | 6 |
| 43 | `META-INF/androidx.drawerlayout_drawerlayout.version` | META-INF | 6 |
| 44 | `META-INF/androidx.dynamicanimation_dynamicanimation.version` | META-INF | 6 |
| 45 | `META-INF/androidx.emoji2_emoji2-views-helper.version` | META-INF | 6 |
| 46 | `META-INF/androidx.emoji2_emoji2.version` | META-INF | 6 |
| 47 | `META-INF/androidx.exifinterface_exifinterface.version` | META-INF | 6 |
| 48 | `META-INF/androidx.fragment_fragment.version` | META-INF | 6 |
| 49 | `META-INF/androidx.graphics_graphics-shapes.version` | META-INF | 6 |
| 50 | `META-INF/androidx.interpolator_interpolator.version` | META-INF | 6 |
| 51 | `META-INF/androidx.lifecycle_lifecycle-extensions.version` | META-INF | 6 |
| 52 | `META-INF/androidx.lifecycle_lifecycle-livedata-core.version` | META-INF | 78 |
| 53 | `META-INF/androidx.lifecycle_lifecycle-livedata.version` | META-INF | 73 |
| 54 | `META-INF/androidx.lifecycle_lifecycle-process.version` | META-INF | 72 |
| 55 | `META-INF/androidx.lifecycle_lifecycle-runtime-compose.version` | META-INF | 80 |
| 56 | `META-INF/androidx.lifecycle_lifecycle-runtime-ktx.version` | META-INF | 76 |
| 57 | `META-INF/androidx.lifecycle_lifecycle-runtime.version` | META-INF | 72 |
| 58 | `META-INF/androidx.lifecycle_lifecycle-service.version` | META-INF | 72 |
| 59 | `META-INF/androidx.lifecycle_lifecycle-viewmodel-ktx.version` | META-INF | 78 |
| 60 | `META-INF/androidx.lifecycle_lifecycle-viewmodel-savedstate.version` | META-INF | 85 |
| 61 | `META-INF/androidx.lifecycle_lifecycle-viewmodel.version` | META-INF | 74 |
| 62 | `META-INF/androidx.loader_loader.version` | META-INF | 6 |
| 63 | `META-INF/androidx.localbroadcastmanager_localbroadcastmanager.version` | META-INF | 6 |
| 64 | `META-INF/androidx.media_media.version` | META-INF | 6 |
| 65 | `META-INF/androidx.preference_preference.version` | META-INF | 6 |
| 66 | `META-INF/androidx.privacysandbox.ads_ads-adservices-java.version` | META-INF | 13 |
| 67 | `META-INF/androidx.privacysandbox.ads_ads-adservices.version` | META-INF | 13 |
| 68 | `META-INF/androidx.profileinstaller_profileinstaller.version` | META-INF | 6 |
| 69 | `META-INF/androidx.recyclerview_recyclerview.version` | META-INF | 6 |
| 70 | `META-INF/androidx.room_room-ktx.version` | META-INF | 6 |
| 71 | `META-INF/androidx.room_room-runtime.version` | META-INF | 6 |
| 72 | `META-INF/androidx.savedstate_savedstate-ktx.version` | META-INF | 6 |
| 73 | `META-INF/androidx.savedstate_savedstate.version` | META-INF | 6 |
| 74 | `META-INF/androidx.sqlite_sqlite-framework.version` | META-INF | 6 |
| 75 | `META-INF/androidx.sqlite_sqlite.version` | META-INF | 6 |
| 76 | `META-INF/androidx.startup_startup-runtime.version` | META-INF | 6 |
| 77 | `META-INF/androidx.tracing_tracing.version` | META-INF | 6 |
| 78 | `META-INF/androidx.transition_transition.version` | META-INF | 6 |
| 79 | `META-INF/androidx.vectordrawable_vectordrawable-animated.version` | META-INF | 6 |
| 80 | `META-INF/androidx.vectordrawable_vectordrawable.version` | META-INF | 6 |
| 81 | `META-INF/androidx.versionedparcelable_versionedparcelable.version` | META-INF | 6 |
| 82 | `META-INF/androidx.viewpager2_viewpager2.version` | META-INF | 6 |
| 83 | `META-INF/androidx.viewpager_viewpager.version` | META-INF | 6 |
| 84 | `META-INF/androidx.webkit_webkit.version` | META-INF | 7 |
| 85 | `META-INF/androidx.work_work-runtime-ktx.version` | META-INF | 6 |
| 86 | `META-INF/androidx.work_work-runtime.version` | META-INF | 6 |
| 87 | `META-INF/androidx/constraintlayout/constraintlayout-core/LICENSE.txt` | META-INF | 10175 |
| 88 | `META-INF/animation-core_release.kotlin_module` | META-INF | 458 |
| 89 | `META-INF/animation_release.kotlin_module` | META-INF | 346 |
| 90 | `META-INF/applovin_max_productionRelease.kotlin_module` | META-INF | 429 |
| 91 | `META-INF/apsadapter_release.kotlin_module` | META-INF | 65 |
| 92 | `META-INF/beacon_release.kotlin_module` | META-INF | 67 |
| 93 | `META-INF/bidmachine-android-sdk_bh_3_4_0.kotlin_module` | META-INF | 191 |
| 94 | `META-INF/bidmachine-android-sdk_bi_3_4_0.kotlin_module` | META-INF | 243 |
| 95 | `META-INF/bidmachine-android-sdk_ca_3_4_0.kotlin_module` | META-INF | 262 |
| 96 | `META-INF/bidmachine_productionRelease.kotlin_module` | META-INF | 227 |
| 97 | `META-INF/bidmachine_release.kotlin_module` | META-INF | 52 |
| 98 | `META-INF/bidon_productionRelease.kotlin_module` | META-INF | 2481 |
| 99 | `META-INF/bigoads_productionRelease.kotlin_module` | META-INF | 190 |
| 100 | `META-INF/chartboost_productionRelease.kotlin_module` | META-INF | 239 |
| 101 | `META-INF/coil-base_release.kotlin_module` | META-INF | 314 |
| 102 | `META-INF/coil-compose-base_release.kotlin_module` | META-INF | 120 |
| 103 | `META-INF/coil-compose-singleton_release.kotlin_module` | META-INF | 168 |
| 104 | `META-INF/coil-singleton_release.kotlin_module` | META-INF | 50 |
| 105 | `META-INF/collection.kotlin_module` | META-INF | 602 |
| 106 | `META-INF/com.google.android.material_material.version` | META-INF | 7 |
| 107 | `META-INF/com/android/build/gradle/app-metadata.properties` | META-INF | 57 |
| 108 | `META-INF/com/applovin/mediation/amazon-tam-adapter/verification.properties` | META-INF | 150 |
| 109 | `META-INF/com/applovin/mediation/bidmachine-adapter/verification.properties` | META-INF | 150 |
| 110 | `META-INF/com/applovin/mediation/bigoads-adapter/verification.properties` | META-INF | 147 |
| 111 | `META-INF/com/applovin/mediation/bytedance-adapter/verification.properties` | META-INF | 149 |
| 112 | `META-INF/com/applovin/mediation/facebook-adapter/verification.properties` | META-INF | 148 |
| 113 | `META-INF/com/applovin/mediation/fyber-adapter/verification.properties` | META-INF | 145 |
| 114 | `META-INF/com/applovin/mediation/google-ad-manager-adapter/verification.properties` | META-INF | 157 |
| 115 | `META-INF/com/applovin/mediation/google-adapter/verification.properties` | META-INF | 146 |
| 116 | `META-INF/com/applovin/mediation/inmobi-adapter/verification.properties` | META-INF | 146 |
| 117 | `META-INF/com/applovin/mediation/ironsource-adapter/verification.properties` | META-INF | 150 |
| 118 | `META-INF/com/applovin/mediation/mintegral-adapter/verification.properties` | META-INF | 149 |
| 119 | `META-INF/com/applovin/mediation/mobilefuse-adapter/verification.properties` | META-INF | 150 |
| 120 | `META-INF/com/applovin/mediation/moloco-adapter/verification.properties` | META-INF | 146 |
| 121 | `META-INF/com/applovin/mediation/mytarget-adapter/verification.properties` | META-INF | 148 |
| 122 | `META-INF/com/applovin/mediation/ogury-presage-adapter/verification.properties` | META-INF | 153 |
| 123 | `META-INF/com/applovin/mediation/pubmatic-adapter/verification.properties` | META-INF | 148 |
| 124 | `META-INF/com/applovin/mediation/smaato-adapter/verification.properties` | META-INF | 146 |
| 125 | `META-INF/com/applovin/mediation/unityads-adapter/verification.properties` | META-INF | 148 |
| 126 | `META-INF/com/applovin/mediation/verve-adapter/verification.properties` | META-INF | 145 |
| 127 | `META-INF/com/applovin/mediation/vungle-adapter/verification.properties` | META-INF | 146 |
| 128 | `META-INF/com/applovin/mediation/yandex-adapter/verification.properties` | META-INF | 146 |
| 129 | `META-INF/com/ironsource/adapters/apsadapter/verification.properties` | META-INF | 143 |
| 130 | `META-INF/com/ironsource/adapters/fyberadapter/verification.properties` | META-INF | 145 |
| 131 | `META-INF/com/ironsource/adapters/mytargetadapter/verification.properties` | META-INF | 118 |
| 132 | `META-INF/com/ironsource/adapters/smaatoadapter/verification.properties` | META-INF | 146 |
| 133 | `META-INF/com/ironsource/adapters/verveadapter/verification.properties` | META-INF | 145 |
| 134 | `META-INF/com/pangle/global/pag-sdk/verification.properties` | META-INF | 134 |
| 135 | `META-INF/com/taurusx/tax/ads/verification.properties` | META-INF | 128 |
| 136 | `META-INF/com/unity3d/ads-mediation/admob-adapter/verification.properties` | META-INF | 148 |
| 137 | `META-INF/com/unity3d/ads-mediation/adquality-sdk/verification.properties` | META-INF | 148 |
| 138 | `META-INF/com/unity3d/ads-mediation/mediation-sdk/verification.properties` | META-INF | 148 |
| 139 | `META-INF/com/unity3d/ads-mediation/mintegral-adapter/verification.properties` | META-INF | 152 |
| 140 | `META-INF/common.kotlin_module` | META-INF | 108 |
| 141 | `META-INF/common_release.kotlin_module` | META-INF | 72 |
| 142 | `META-INF/core-ktx_release.kotlin_module` | META-INF | 1146 |
| 143 | `META-INF/core-utils_release.kotlin_module` | META-INF | 232 |
| 144 | `META-INF/customview-poolingcontainer_release.kotlin_module` | META-INF | 78 |
| 145 | `META-INF/datastore_release.kotlin_module` | META-INF | 141 |
| 146 | `META-INF/div-core_release.kotlin_module` | META-INF | 66 |
| 147 | `META-INF/div-data_release.kotlin_module` | META-INF | 606 |
| 148 | `META-INF/div-evaluable.kotlin_module` | META-INF | 359 |
| 149 | `META-INF/div-histogram_release.kotlin_module` | META-INF | 167 |
| 150 | `META-INF/div-storage_release.kotlin_module` | META-INF | 386 |
| 151 | `META-INF/div_release.kotlin_module` | META-INF | 3255 |
| 152 | `META-INF/drawablepainter_release.kotlin_module` | META-INF | 81 |
| 153 | `META-INF/dtexchange_productionRelease.kotlin_module` | META-INF | 266 |
| 154 | `META-INF/ext_release.kotlin_module` | META-INF | 139 |
| 155 | `META-INF/foundation-layout_release.kotlin_module` | META-INF | 354 |
| 156 | `META-INF/foundation_release.kotlin_module` | META-INF | 3084 |
| 157 | `META-INF/graphics-shapes_release.kotlin_module` | META-INF | 195 |
| 158 | `META-INF/inmobi_productionRelease.kotlin_module` | META-INF | 181 |
| 159 | `META-INF/internal_release.kotlin_module` | META-INF | 95 |
| 160 | `META-INF/io/appmetrica/analytics/analytics/verification.properties` | META-INF | 142 |
| 161 | `META-INF/io/bidmachine/rendering/verification.properties` | META-INF | 132 |
| 162 | `META-INF/io/sentry/sentry-android-replay/verification.properties` | META-INF | 140 |
| 163 | `META-INF/java_kotlin-bytestring_lib.kotlin_module` | META-INF | 65 |
| 164 | `META-INF/java_kotlin-lite-lite_extensions.kotlin_module` | META-INF | 85 |
| 165 | `META-INF/java_kotlin-lite-well_known_protos_kotlin_lite.kotlin_module` | META-INF | 402 |
| 166 | `META-INF/kotlin-reflection.kotlin_module` | META-INF | 461 |
| 167 | `META-INF/kotlin-stdlib-jdk7.kotlin_module` | META-INF | 152 |
| 168 | `META-INF/kotlin-stdlib-jdk8.kotlin_module` | META-INF | 220 |
| 169 | `META-INF/kotlin-stdlib.kotlin_module` | META-INF | 4439 |
| 170 | `META-INF/kotlinx-coroutines-android.kotlin_module` | META-INF | 71 |
| 171 | `META-INF/kotlinx-coroutines-core.kotlin_module` | META-INF | 2779 |
| 172 | `META-INF/kotlinx-coroutines-slf4j.kotlin_module` | META-INF | 62 |
| 173 | `META-INF/kotlinx-serialization-core.kotlin_module` | META-INF | 840 |
| 174 | `META-INF/kotlinx-serialization-json.kotlin_module` | META-INF | 503 |
| 175 | `META-INF/kotlinx_coroutines_android.version` | META-INF | 5 |
| 176 | `META-INF/kotlinx_coroutines_core.version` | META-INF | 5 |
| 177 | `META-INF/kotlinx_coroutines_jdk8.version` | META-INF | 5 |
| 178 | `META-INF/kotlinx_coroutines_slf4j.version` | META-INF | 5 |
| 179 | `META-INF/ktor-client-android.kotlin_module` | META-INF | 105 |
| 180 | `META-INF/ktor-client-core.kotlin_module` | META-INF | 1686 |
| 181 | `META-INF/ktor-events.kotlin_module` | META-INF | 48 |
| 182 | `META-INF/ktor-http.kotlin_module` | META-INF | 918 |
| 183 | `META-INF/ktor-io.kotlin_module` | META-INF | 1895 |
| 184 | `META-INF/ktor-serialization.kotlin_module` | META-INF | 94 |
| 185 | `META-INF/ktor-utils.kotlin_module` | META-INF | 1140 |
| 186 | `META-INF/ktor-websocket-serialization.kotlin_module` | META-INF | 88 |
| 187 | `META-INF/ktor-websockets.kotlin_module` | META-INF | 355 |
| 188 | `META-INF/level_play_productionRelease.kotlin_module` | META-INF | 304 |
| 189 | `META-INF/lifecycle-common.kotlin_module` | META-INF | 94 |
| 190 | `META-INF/lifecycle-livedata_release.kotlin_module` | META-INF | 59 |
| 191 | `META-INF/lifecycle-runtime-compose_release.kotlin_module` | META-INF | 61 |
| 192 | `META-INF/lifecycle-runtime-ktx_release.kotlin_module` | META-INF | 104 |
| 193 | `META-INF/lifecycle-runtime_release.kotlin_module` | META-INF | 66 |
| 194 | `META-INF/lifecycle-viewmodel-ktx_release.kotlin_module` | META-INF | 55 |
| 195 | `META-INF/lifecycle-viewmodel-savedstate_release.kotlin_module` | META-INF | 97 |
| 196 | `META-INF/lifecycle-viewmodel_release.kotlin_module` | META-INF | 179 |
| 197 | `META-INF/material-icons-core_release.kotlin_module` | META-INF | 3197 |
| 198 | `META-INF/material-ripple_release.kotlin_module` | META-INF | 100 |
| 199 | `META-INF/material_release.kotlin_module` | META-INF | 920 |
| 200 | `META-INF/mediationsdk_release.kotlin_module` | META-INF | 289 |
| 201 | `META-INF/meta_productionRelease.kotlin_module` | META-INF | 177 |
| 202 | `META-INF/mintegral_productionRelease.kotlin_module` | META-INF | 202 |
| 203 | `META-INF/mobilefuse-sdk-common_release.kotlin_module` | META-INF | 1257 |
| 204 | `META-INF/mobilefuse-sdk-core_release.kotlin_module` | META-INF | 1396 |
| 205 | `META-INF/mobilefuse-sdk-mraid_release.kotlin_module` | META-INF | 86 |
| 206 | `META-INF/mobilefuse-sdk-telemetry_release.kotlin_module` | META-INF | 394 |
| 207 | `META-INF/mobilefuse-video-player_release.kotlin_module` | META-INF | 392 |
| 208 | `META-INF/mobilefuse_productionRelease.kotlin_module` | META-INF | 246 |
| 209 | `META-INF/moloco-sdk_release.kotlin_module` | META-INF | 283 |
| 210 | `META-INF/moloco_productionRelease.kotlin_module` | META-INF | 181 |
| 211 | `META-INF/native-image/io.sentry/sentry/native-image.properties` | META-INF | 36 |
| 212 | `META-INF/network_release.kotlin_module` | META-INF | 61 |
| 213 | `META-INF/okhttp.kotlin_module` | META-INF | 273 |
| 214 | `META-INF/okio.kotlin_module` | META-INF | 382 |
| 215 | `META-INF/openwrapcore_release.kotlin_module` | META-INF | 79 |
| 216 | `META-INF/parcelize-runtime.kotlin_module` | META-INF | 62 |
| 217 | `META-INF/public.kotlin_module` | META-INF | 59 |
| 218 | `META-INF/public_release.kotlin_module` | META-INF | 61 |
| 219 | `META-INF/room-ktx_release.kotlin_module` | META-INF | 111 |
| 220 | `META-INF/room-runtime_release.kotlin_module` | META-INF | 150 |
| 221 | `META-INF/runtime-saveable_release.kotlin_module` | META-INF | 160 |
| 222 | `META-INF/runtime_release.kotlin_module` | META-INF | 1474 |
| 223 | `META-INF/savedstate-ktx_release.kotlin_module` | META-INF | 51 |
| 224 | `META-INF/savedstate_release.kotlin_module` | META-INF | 76 |
| 225 | `META-INF/sdk-core_prodRelease.kotlin_module` | META-INF | 366 |
| 226 | `META-INF/sentry-android-replay_release.kotlin_module` | META-INF | 266 |
| 227 | `META-INF/services/com.smaato.sdk.core.framework.SdkInitialisationObserver` | META-INF | 151 |
| 228 | `META-INF/services/com.smaato.sdk.core.framework.SimpleModuleInterface` | META-INF | 557 |
| 229 | `META-INF/services/com.smaato.sdk.core.openmeasurement.ViewabilityPlugin` | META-INF | 57 |
| 230 | `META-INF/services/io.ktor.client.HttpClientEngineContainer` | META-INF | 54 |
| 231 | `META-INF/services/kotlin.reflect.jvm.internal.impl.builtins.BuiltInsLoader` | META-INF | 92 |
| 232 | `META-INF/services/kotlin.reflect.jvm.internal.impl.resolve.ExternalOverridabilityCondition` | META-INF | 239 |
| 233 | `META-INF/services/kotlinx.coroutines.CoroutineExceptionHandler` | META-INF | 55 |
| 234 | `META-INF/services/kotlinx.coroutines.internal.MainDispatcherFactory` | META-INF | 53 |
| 235 | `META-INF/services/ru.ok.tracer.manifest.TracerLiteManifest` | META-INF | 18 |
| 236 | `META-INF/startio_productionRelease.kotlin_module` | META-INF | 156 |
| 237 | `META-INF/taurusx_productionRelease.kotlin_module` | META-INF | 188 |
| 238 | `META-INF/ui-geometry_release.kotlin_module` | META-INF | 139 |
| 239 | `META-INF/ui-graphics_release.kotlin_module` | META-INF | 864 |
| 240 | `META-INF/ui-text_release.kotlin_module` | META-INF | 1881 |
| 241 | `META-INF/ui-unit_release.kotlin_module` | META-INF | 166 |
| 242 | `META-INF/ui-util_release.kotlin_module` | META-INF | 121 |
| 243 | `META-INF/ui_release.kotlin_module` | META-INF | 3449 |
| 244 | `META-INF/unity-ads_release.kotlin_module` | META-INF | 2621 |
| 245 | `META-INF/unityads_productionRelease.kotlin_module` | META-INF | 241 |
| 246 | `META-INF/utils_release.kotlin_module` | META-INF | 96 |
| 247 | `META-INF/version-control-info.textproto` | META-INF | 120 |
| 248 | `META-INF/versions/9/OSGI-INF/MANIFEST.MF` | META-INF | 145 |
| 249 | `META-INF/vkads_productionRelease.kotlin_module` | META-INF | 176 |
| 250 | `META-INF/vungle-ads_release.kotlin_module` | META-INF | 66 |
| 251 | `META-INF/vungle_productionRelease.kotlin_module` | META-INF | 181 |
| 252 | `META-INF/webrendering_release.kotlin_module` | META-INF | 86 |
| 253 | `META-INF/work-runtime-ktx_release.kotlin_module` | META-INF | 78 |
| 254 | `META-INF/work-runtime_release.kotlin_module` | META-INF | 740 |
| 255 | `META-INF/yandex_productionRelease.kotlin_module` | META-INF | 197 |
| 256 | `androidsupportmultidexversion.txt` | other | 53 |
| 257 | `assets/PubMatic_Logo.svg` | assets | 3515 |
| 258 | `assets/a.json` | assets | 1357 |
| 259 | `assets/ad-viewer/omid-session-client-v1.js` | assets | 68100 |
| 260 | `assets/ad-viewer/omsdk-v1.js` | assets | 43924 |
| 261 | `assets/ad.html` | assets | 16164 |
| 262 | `assets/android_bridge.js` | assets | 10422 |
| 263 | `assets/aps-mraid.js` | assets | 11529 |
| 264 | `assets/aps_mobile_client_config.json` | assets | 391 |
| 265 | `assets/audience_network.dex` | dex | 5017908 |
| 266 | `assets/bm_networks/amazon.bmnetwork` | assets | 122 |
| 267 | `assets/bm_networks/gam.bmnetwork` | assets | 108 |
| 268 | `assets/bm_networks/gam_dynamic.bmnetwork` | assets | 124 |
| 269 | `assets/bm_networks/meta_audience.bmnetwork` | assets | 142 |
| 270 | `assets/bm_networks/mintegral.bmnetwork` | assets | 132 |
| 271 | `assets/bm_networks/my_target.bmnetwork` | assets | 130 |
| 272 | `assets/bm_networks/pangle_sdk.bmnetwork` | assets | 127 |
| 273 | `assets/bm_networks/vungle.bmnetwork` | assets | 120 |
| 274 | `assets/builddatas.json` | assets | 450 |
| 275 | `assets/com.moloco.sdk.xenoss.sdkdevkit.mraid.js` | assets | 17240 |
| 276 | `assets/dexopt/baseline.prof` | assets | 10241 |
| 277 | `assets/dexopt/baseline.profm` | assets | 944 |
| 278 | `assets/dic` | assets | 4016 |
| 279 | `assets/dsa_page.html` | assets | 13200 |
| 280 | `assets/dtb-m.js` | assets | 67737 |
| 281 | `assets/entityIcons/allay.png` | assets | 151 |
| 282 | `assets/entityIcons/armadillo.png` | assets | 270 |
| 283 | `assets/entityIcons/axolotl.png` | assets | 158 |
| 284 | `assets/entityIcons/bat.png` | assets | 204 |
| 285 | `assets/entityIcons/bee.png` | assets | 213 |
| 286 | `assets/entityIcons/blaze.png` | assets | 333 |
| 287 | `assets/entityIcons/bogged.png` | assets | 292 |
| 288 | `assets/entityIcons/breeze.png` | assets | 220 |
| 289 | `assets/entityIcons/camel.png` | assets | 273 |
| 290 | `assets/entityIcons/cat.png` | assets | 151 |
| 291 | `assets/entityIcons/cave_spider.png` | assets | 265 |
| 292 | `assets/entityIcons/chicken.png` | assets | 164 |
| 293 | `assets/entityIcons/cod.png` | assets | 200 |
| 294 | `assets/entityIcons/cow.png` | assets | 204 |
| 295 | `assets/entityIcons/credits.txt` | assets | 54 |
| 296 | `assets/entityIcons/creeper.png` | assets | 265 |
| 297 | `assets/entityIcons/dolphin.png` | assets | 209 |
| 298 | `assets/entityIcons/donkey.png` | assets | 278 |
| 299 | `assets/entityIcons/drowned.png` | assets | 221 |
| 300 | `assets/entityIcons/elder_guardian.png` | assets | 529 |
| 301 | `assets/entityIcons/ender_dragon.png` | assets | 258 |
| 302 | `assets/entityIcons/enderman.png` | assets | 191 |
| 303 | `assets/entityIcons/endermite.png` | assets | 143 |
| 304 | `assets/entityIcons/evoker.png` | assets | 240 |
| 305 | `assets/entityIcons/fox.png` | assets | 193 |
| 306 | `assets/entityIcons/frog.png` | assets | 220 |
| 307 | `assets/entityIcons/ghast.png` | assets | 206 |
| 308 | `assets/entityIcons/glow_squid.png` | assets | 366 |
| 309 | `assets/entityIcons/goat.png` | assets | 310 |
| 310 | `assets/entityIcons/guardian.png` | assets | 533 |
| 311 | `assets/entityIcons/hoglin.png` | assets | 274 |
| 312 | `assets/entityIcons/horse.png` | assets | 287 |
| 313 | `assets/entityIcons/husk.png` | assets | 258 |
| 314 | `assets/entityIcons/illusioner.png` | assets | 258 |
| 315 | `assets/entityIcons/iron_golem.png` | assets | 268 |
| 316 | `assets/entityIcons/llama.png` | assets | 204 |
| 317 | `assets/entityIcons/magma_cube.png` | assets | 254 |
| 318 | `assets/entityIcons/mooshroom.png` | assets | 208 |
| 319 | `assets/entityIcons/mule.png` | assets | 301 |
| 320 | `assets/entityIcons/ocelot.png` | assets | 171 |
| 321 | `assets/entityIcons/panda.png` | assets | 257 |
| 322 | `assets/entityIcons/parrot.png` | assets | 170 |
| 323 | `assets/entityIcons/phantom.png` | assets | 149 |
| 324 | `assets/entityIcons/pig.png` | assets | 188 |
| 325 | `assets/entityIcons/piglin.png` | assets | 256 |
| 326 | `assets/entityIcons/piglin_brute.png` | assets | 267 |
| 327 | `assets/entityIcons/pillager.png` | assets | 196 |
| 328 | `assets/entityIcons/polar_bear.png` | assets | 170 |
| 329 | `assets/entityIcons/pufferfish.png` | assets | 213 |
| 330 | `assets/entityIcons/rabbit.png` | assets | 213 |
| 331 | `assets/entityIcons/ravager.png` | assets | 446 |
| 332 | `assets/entityIcons/salmon.png` | assets | 232 |
| 333 | `assets/entityIcons/sheep.png` | assets | 243 |
| 334 | `assets/entityIcons/shulker.png` | assets | 231 |
| 335 | `assets/entityIcons/silver_fish.png` | assets | 190 |
| 336 | `assets/entityIcons/skeleton.png` | assets | 235 |
| 337 | `assets/entityIcons/skeleton_horse.png` | assets | 226 |
| 338 | `assets/entityIcons/slime.png` | assets | 337 |
| 339 | `assets/entityIcons/sniffer.png` | assets | 356 |
| 340 | `assets/entityIcons/snow_golem.png` | assets | 195 |
| 341 | `assets/entityIcons/spider.png` | assets | 268 |
| 342 | `assets/entityIcons/squid.png` | assets | 194 |
| 343 | `assets/entityIcons/stray.png` | assets | 253 |
| 344 | `assets/entityIcons/strider.png` | assets | 292 |
| 345 | `assets/entityIcons/tadpole.png` | assets | 166 |
| 346 | `assets/entityIcons/trader_llama.png` | assets | 286 |
| 347 | `assets/entityIcons/tropical_fish.png` | assets | 210 |
| 348 | `assets/entityIcons/turtle.png` | assets | 201 |
| 349 | `assets/entityIcons/vex.png` | assets | 159 |
| 350 | `assets/entityIcons/villager.png` | assets | 163 |
| 351 | `assets/entityIcons/vindicator.png` | assets | 242 |
| 352 | `assets/entityIcons/wandering_trader.png` | assets | 237 |
| 353 | `assets/entityIcons/warden.png` | assets | 342 |
| 354 | `assets/entityIcons/witch.png` | assets | 160 |
| 355 | `assets/entityIcons/wither.png` | assets | 231 |
| 356 | `assets/entityIcons/wither_skeleton.png` | assets | 250 |
| 357 | `assets/entityIcons/wither_skull.png` | assets | 255 |
| 358 | `assets/entityIcons/wolf.png` | assets | 223 |
| 359 | `assets/entityIcons/zoglin.png` | assets | 409 |
| 360 | `assets/entityIcons/zombie.png` | assets | 226 |
| 361 | `assets/entityIcons/zombie_horse.png` | assets | 280 |
| 362 | `assets/entityIcons/zombie_villager.png` | assets | 161 |
| 363 | `assets/entityIcons/zombified_piglin.png` | assets | 271 |
| 364 | `assets/favicon.ico` | assets | 0 |
| 365 | `assets/fyb_iframe_endcard_tmpl.html` | assets | 520 |
| 366 | `assets/fyb_static_endcard_tmpl.html` | assets | 3350 |
| 367 | `assets/ia_js_load_monitor.txt` | assets | 968 |
| 368 | `assets/ia_mraid_bridge.txt` | assets | 28630 |
| 369 | `assets/mbridge_download_dialog_view.xml` | assets | 25224 |
| 370 | `assets/mobilefuse/ad_template.html` | assets | 510 |
| 371 | `assets/mobilefuse/mraid.js` | assets | 28612 |
| 372 | `assets/mobilefuse/mraid.src.js` | assets | 21954 |
| 373 | `assets/mobilefuse/mraid_close_controls.js` | assets | 5228 |
| 374 | `assets/mobilefuse/mraid_close_controls.src.js` | assets | 3416 |
| 375 | `assets/mobilefuse/mraid_controls.html` | assets | 3168 |
| 376 | `assets/mobilefuse/vast.js` | assets | 17265 |
| 377 | `assets/mobilefuse/vast.src.js` | assets | 11524 |
| 378 | `assets/mobilefuse/vast_controls.html` | assets | 10453 |
| 379 | `assets/mobilefuse/vast_static_resource.html` | assets | 832 |
| 380 | `assets/mraid-bridge.js` | assets | 7969 |
| 381 | `assets/mraid.js` | assets | 14400 |
| 382 | `assets/mraid3.js` | assets | 34522 |
| 383 | `assets/omsdk-v1.js` | assets | 45649 |
| 384 | `assets/pob_info_button.svg` | assets | 334 |
| 385 | `assets/pob_mraid.js` | assets | 25937 |
| 386 | `assets/pob_person_button.svg` | assets | 643 |
| 387 | `assets/rv_binddatas.xml` | assets | 2072 |
| 388 | `assets/taurusx_mraid.js` | assets | 34577 |
| 389 | `assets/texture_pack.mcpack` | assets | 15546 |
| 390 | `assets/ui.js` | assets | 4073 |
| 391 | `assets/worldTemplate/level.dat` | assets | 2977 |
| 392 | `assets/worldTemplate/levelname.txt` | assets | 27 |
| 393 | `billing.properties` | other | 50 |
| 394 | `classes.dex` | dex | 10918168 |
| 395 | `classes10.dex` | dex | 8335756 |
| 396 | `classes11.dex` | dex | 10467440 |
| 397 | `classes12.dex` | dex | 8778944 |
| 398 | `classes2.dex` | dex | 27304 |
| 399 | `classes3.dex` | dex | 11048188 |
| 400 | `classes4.dex` | dex | 8334172 |
| 401 | `classes5.dex` | dex | 9808836 |
| 402 | `classes6.dex` | dex | 5156832 |
| 403 | `classes7.dex` | dex | 9353420 |
| 404 | `classes8.dex` | dex | 7435364 |
| 405 | `classes9.dex` | dex | 9589124 |
| 406 | `explorestack/protobuf/any.proto` | other | 5916 |
| 407 | `explorestack/protobuf/api.proto` | other | 7734 |
| 408 | `explorestack/protobuf/compiler/plugin.proto` | other | 8754 |
| 409 | `explorestack/protobuf/descriptor.proto` | other | 38043 |
| 410 | `explorestack/protobuf/duration.proto` | other | 4895 |
| 411 | `explorestack/protobuf/empty.proto` | other | 2429 |
| 412 | `explorestack/protobuf/field_mask.proto` | other | 8185 |
| 413 | `explorestack/protobuf/source_context.proto` | other | 2341 |
| 414 | `explorestack/protobuf/struct.proto` | other | 3778 |
| 415 | `explorestack/protobuf/timestamp.proto` | other | 6459 |
| 416 | `explorestack/protobuf/type.proto` | other | 6126 |
| 417 | `explorestack/protobuf/wrappers.proto` | other | 4042 |
| 418 | `firebase-encoders-json.properties` | other | 82 |
| 419 | `firebase-encoders-proto.properties` | other | 84 |
| 420 | `firebase-encoders.properties` | other | 72 |
| 421 | `google/protobuf/any.proto` | other | 6154 |
| 422 | `google/protobuf/api.proto` | other | 7728 |
| 423 | `google/protobuf/duration.proto` | other | 4892 |
| 424 | `google/protobuf/empty.proto` | other | 2363 |
| 425 | `google/protobuf/field_mask.proto` | other | 8185 |
| 426 | `google/protobuf/source_context.proto` | other | 2341 |
| 427 | `google/protobuf/struct.proto` | other | 3778 |
| 428 | `google/protobuf/timestamp.proto` | other | 6449 |
| 429 | `google/protobuf/type.proto` | other | 6367 |
| 430 | `google/protobuf/wrappers.proto` | other | 4044 |
| 431 | `kotlin/annotation/annotation.kotlin_builtins` | other | 865 |
| 432 | `kotlin/collections/collections.kotlin_builtins` | other | 3635 |
| 433 | `kotlin/coroutines/coroutines.kotlin_builtins` | other | 137 |
| 434 | `kotlin/internal/internal.kotlin_builtins` | other | 583 |
| 435 | `kotlin/kotlin.kotlin_builtins` | other | 18653 |
| 436 | `kotlin/ranges/ranges.kotlin_builtins` | other | 3349 |
| 437 | `kotlin/reflect/reflect.kotlin_builtins` | other | 2370 |
| 438 | `okhttp3/internal/publicsuffix/NOTICE` | other | 218 |
| 439 | `okhttp3/internal/publicsuffix/publicsuffixes.gz` | other | 41394 |
| 440 | `play-services-ads-api.properties` | other | 80 |
| 441 | `play-services-ads-identifier.properties` | other | 94 |
| 442 | `play-services-ads.properties` | other | 72 |
| 443 | `play-services-appset.properties` | other | 78 |
| 444 | `play-services-auth-api-phone.properties` | other | 94 |
| 445 | `play-services-auth-base.properties` | other | 84 |
| 446 | `play-services-auth.properties` | other | 74 |
| 447 | `play-services-base.properties` | other | 74 |
| 448 | `play-services-basement.properties` | other | 82 |
| 449 | `play-services-cronet.properties` | other | 78 |
| 450 | `play-services-fido.properties` | other | 74 |
| 451 | `play-services-location.properties` | other | 82 |
| 452 | `play-services-measurement-base.properties` | other | 98 |
| 453 | `play-services-measurement-sdk-api.properties` | other | 104 |
| 454 | `play-services-tasks.properties` | other | 76 |
| 455 | `res/anim-v21/fragment_fast_out_extra_slow_in.xml` | res | 364 |
| 456 | `res/anim/abc_fade_in.xml` | res | 388 |
| 457 | `res/anim/abc_fade_out.xml` | res | 388 |
| 458 | `res/anim/abc_grow_fade_in_from_bottom.xml` | res | 852 |
| 459 | `res/anim/abc_popup_enter.xml` | res | 508 |
| 460 | `res/anim/abc_popup_exit.xml` | res | 508 |
| 461 | `res/anim/abc_shrink_fade_out_from_bottom.xml` | res | 852 |
| 462 | `res/anim/abc_slide_in_bottom.xml` | res | 396 |
| 463 | `res/anim/abc_slide_in_top.xml` | res | 396 |
| 464 | `res/anim/abc_slide_out_bottom.xml` | res | 396 |
| 465 | `res/anim/abc_slide_out_top.xml` | res | 396 |
| 466 | `res/anim/abc_tooltip_enter.xml` | res | 388 |
| 467 | `res/anim/abc_tooltip_exit.xml` | res | 388 |
| 468 | `res/anim/applovin_slide_down_animation.xml` | res | 464 |
| 469 | `res/anim/applovin_slide_up_animation.xml` | res | 464 |
| 470 | `res/anim/bm_bottom_sheet_slide_in.xml` | res | 616 |
| 471 | `res/anim/bm_bottom_sheet_slide_out.xml` | res | 616 |
| 472 | `res/anim/btn_checkbox_to_checked_box_inner_merged_animation.xml` | res | 2124 |
| 473 | `res/anim/btn_checkbox_to_checked_box_outer_merged_animation.xml` | res | 2780 |
| 474 | `res/anim/btn_checkbox_to_checked_icon_null_animation.xml` | res | 1196 |
| 475 | `res/anim/btn_checkbox_to_unchecked_box_inner_merged_animation.xml` | res | 2360 |
| 476 | `res/anim/btn_checkbox_to_unchecked_check_path_merged_animation.xml` | res | 2520 |
| 477 | `res/anim/btn_checkbox_to_unchecked_icon_null_animation.xml` | res | 1196 |
| 478 | `res/anim/btn_radio_to_off_mtrl_dot_group_animation.xml` | res | 1656 |
| 479 | `res/anim/btn_radio_to_off_mtrl_ring_outer_animation.xml` | res | 1656 |
| 480 | `res/anim/btn_radio_to_off_mtrl_ring_outer_path_animation.xml` | res | 1028 |
| 481 | `res/anim/btn_radio_to_on_mtrl_dot_group_animation.xml` | res | 1656 |
| 482 | `res/anim/btn_radio_to_on_mtrl_ring_outer_animation.xml` | res | 1656 |
| 483 | `res/anim/btn_radio_to_on_mtrl_ring_outer_path_animation.xml` | res | 1028 |
| 484 | `res/anim/design_bottom_sheet_slide_in.xml` | res | 616 |
| 485 | `res/anim/design_bottom_sheet_slide_out.xml` | res | 616 |
| 486 | `res/anim/design_snackbar_in.xml` | res | 312 |
| 487 | `res/anim/design_snackbar_out.xml` | res | 312 |
| 488 | `res/anim/linear_indeterminate_line1_head_interpolator.xml` | res | 400 |
| 489 | `res/anim/linear_indeterminate_line1_tail_interpolator.xml` | res | 400 |
| 490 | `res/anim/linear_indeterminate_line2_head_interpolator.xml` | res | 400 |
| 491 | `res/anim/linear_indeterminate_line2_tail_interpolator.xml` | res | 400 |
| 492 | `res/anim/m3_bottom_sheet_slide_in.xml` | res | 616 |
| 493 | `res/anim/m3_bottom_sheet_slide_out.xml` | res | 656 |
| 494 | `res/anim/m3_motion_fade_enter.xml` | res | 812 |
| 495 | `res/anim/m3_motion_fade_exit.xml` | res | 508 |
| 496 | `res/anim/m3_side_sheet_enter_from_left.xml` | res | 464 |
| 497 | `res/anim/m3_side_sheet_enter_from_right.xml` | res | 464 |
| 498 | `res/anim/m3_side_sheet_exit_to_left.xml` | res | 464 |
| 499 | `res/anim/m3_side_sheet_exit_to_right.xml` | res | 464 |
| 500 | `res/anim/mbridge_reward_activity_open.xml` | res | 420 |
| 501 | `res/anim/mbridge_reward_activity_stay.xml` | res | 416 |
| 502 | `res/anim/mobilefuse_bounce_edge_bottom.xml` | res | 632 |
| 503 | `res/anim/mobilefuse_bounce_edge_left.xml` | res | 632 |
| 504 | `res/anim/mobilefuse_bounce_edge_right.xml` | res | 632 |
| 505 | `res/anim/mobilefuse_bounce_edge_top.xml` | res | 632 |
| 506 | `res/anim/mobilefuse_close_pulltab.xml` | res | 844 |
| 507 | `res/anim/monetization_ads_internal_bottom_adtune_dialog_animation_in.xml` | res | 500 |
| 508 | `res/anim/monetization_ads_internal_bottom_adtune_dialog_animation_out.xml` | res | 500 |
| 509 | `res/anim/mtrl_bottom_sheet_slide_in.xml` | res | 616 |
| 510 | `res/anim/mtrl_bottom_sheet_slide_out.xml` | res | 616 |
| 511 | `res/anim/mtrl_card_lowers_interpolator.xml` | res | 400 |
| 512 | `res/anim/store_promo_appear_anim.xml` | res | 352 |
| 513 | `res/anim/store_promo_disappear_anim.xml` | res | 392 |
| 514 | `res/anim/tt_dislike_animation_dismiss.xml` | res | 352 |
| 515 | `res/anim/tt_dislike_animation_show.xml` | res | 396 |
| 516 | `res/anim/tt_fade_out.xml` | res | 344 |
| 517 | `res/anim/tt_loading_two_icon_scale.xml` | res | 696 |
| 518 | `res/anim/tt_text_animation_x_in.xml` | res | 684 |
| 519 | `res/anim/tt_text_animation_y_in.xml` | res | 420 |
| 520 | `res/anim/tt_text_animation_y_out.xml` | res | 420 |
| 521 | `res/anim/zoom_out.xml` | res | 760 |
| 522 | `res/anim/zoom_show.xml` | res | 760 |
| 523 | `res/animator/chevron_checked_unchecked.xml` | res | 444 |
| 524 | `res/animator/chevron_unchecked_checked.xml` | res | 444 |
| 525 | `res/animator/design_appbar_state_list_animator.xml` | res | 1216 |
| 526 | `res/animator/design_fab_hide_motion_spec.xml` | res | 796 |
| 527 | `res/animator/design_fab_show_motion_spec.xml` | res | 796 |
| 528 | `res/animator/fragment_close_enter.xml` | res | 1128 |
| 529 | `res/animator/fragment_close_exit.xml` | res | 1128 |
| 530 | `res/animator/fragment_fade_enter.xml` | res | 452 |
| 531 | `res/animator/fragment_fade_exit.xml` | res | 452 |
| 532 | `res/animator/fragment_open_enter.xml` | res | 1128 |
| 533 | `res/animator/fragment_open_exit.xml` | res | 1128 |
| 534 | `res/animator/m3_appbar_state_list_animator.xml` | res | 1216 |
| 535 | `res/animator/m3_btn_elevated_btn_state_list_anim.xml` | res | 1796 |
| 536 | `res/animator/m3_btn_state_list_anim.xml` | res | 1796 |
| 537 | `res/animator/m3_card_elevated_state_list_anim.xml` | res | 1632 |
| 538 | `res/animator/m3_card_state_list_anim.xml` | res | 1584 |
| 539 | `res/animator/m3_chip_state_list_anim.xml` | res | 1180 |
| 540 | `res/animator/m3_elevated_chip_state_list_anim.xml` | res | 1072 |
| 541 | `res/animator/m3_extended_fab_change_size_collapse_motion_spec.xml` | res | 1116 |
| 542 | `res/animator/m3_extended_fab_change_size_expand_motion_spec.xml` | res | 1116 |
| 543 | `res/animator/m3_extended_fab_hide_motion_spec.xml` | res | 820 |
| 544 | `res/animator/m3_extended_fab_show_motion_spec.xml` | res | 820 |
| 545 | `res/animator/m3_extended_fab_state_list_animator.xml` | res | 2724 |
| 546 | `res/animator/m3_fab_state_list_animator.xml` | res | 2724 |
| 547 | `res/animator/m3_split_button_chevron_overshoot_interpolator.xml` | res | 284 |
| 548 | `res/animator/m3_split_button_chevron_reverse_rotation.xml` | res | 604 |
| 549 | `res/animator/m3_split_button_chevron_rotation.xml` | res | 604 |
| 550 | `res/animator/mtrl_btn_state_list_anim.xml` | res | 2664 |
| 551 | `res/animator/mtrl_btn_unelevated_state_list_anim.xml` | res | 120 |
| 552 | `res/animator/mtrl_card_state_list_anim.xml` | res | 1208 |
| 553 | `res/animator/mtrl_chip_state_list_anim.xml` | res | 1072 |
| 554 | `res/animator/mtrl_extended_fab_change_size_collapse_motion_spec.xml` | res | 1116 |
| 555 | `res/animator/mtrl_extended_fab_change_size_expand_motion_spec.xml` | res | 1116 |
| 556 | `res/animator/mtrl_extended_fab_hide_motion_spec.xml` | res | 608 |
| 557 | `res/animator/mtrl_extended_fab_show_motion_spec.xml` | res | 820 |
| 558 | `res/animator/mtrl_extended_fab_state_list_animator.xml` | res | 2724 |
| 559 | `res/animator/mtrl_fab_hide_motion_spec.xml` | res | 796 |
| 560 | `res/animator/mtrl_fab_show_motion_spec.xml` | res | 796 |
| 561 | `res/animator/mtrl_fab_transformation_sheet_collapse_spec.xml` | res | 1888 |
| 562 | `res/animator/mtrl_fab_transformation_sheet_expand_spec.xml` | res | 1888 |
| 563 | `res/color-night-v8/material_timepicker_button_stroke.xml` | res | 376 |
| 564 | `res/color-night-v8/material_timepicker_clockface.xml` | res | 376 |
| 565 | `res/color-night-v8/material_timepicker_modebutton_tint.xml` | res | 340 |
| 566 | `res/color-v23/abc_btn_colored_borderless_text_material.xml` | res | 500 |
| 567 | `res/color-v23/abc_btn_colored_text_material.xml` | res | 500 |
| 568 | `res/color-v23/abc_color_highlight_material.xml` | res | 544 |
| 569 | `res/color-v23/abc_tint_btn_checkable.xml` | res | 624 |
| 570 | `res/color-v23/abc_tint_default.xml` | res | 1120 |
| 571 | `res/color-v23/abc_tint_edittext.xml` | res | 668 |
| 572 | `res/color-v23/abc_tint_seek_thumb.xml` | res | 500 |
| 573 | `res/color-v23/abc_tint_spinner.xml` | res | 668 |
| 574 | `res/color-v23/abc_tint_switch_track.xml` | res | 664 |
| 575 | `res/color-v31/m3_dynamic_dark_default_color_primary_text.xml` | res | 500 |
| 576 | `res/color-v31/m3_dynamic_dark_default_color_secondary_text.xml` | res | 500 |
| 577 | `res/color-v31/m3_dynamic_dark_highlighted_text.xml` | res | 376 |
| 578 | `res/color-v31/m3_dynamic_dark_hint_foreground.xml` | res | 564 |
| 579 | `res/color-v31/m3_dynamic_dark_primary_text_disable_only.xml` | res | 500 |
| 580 | `res/color-v31/m3_dynamic_default_color_primary_text.xml` | res | 500 |
| 581 | `res/color-v31/m3_dynamic_default_color_secondary_text.xml` | res | 500 |
| 582 | `res/color-v31/m3_dynamic_highlighted_text.xml` | res | 376 |
| 583 | `res/color-v31/m3_dynamic_hint_foreground.xml` | res | 564 |
| 584 | `res/color-v31/m3_dynamic_primary_text_disable_only.xml` | res | 500 |
| 585 | `res/color-v31/m3_ref_palette_dynamic_neutral12.xml` | res | 376 |
| 586 | `res/color-v31/m3_ref_palette_dynamic_neutral17.xml` | res | 376 |
| 587 | `res/color-v31/m3_ref_palette_dynamic_neutral22.xml` | res | 376 |
| 588 | `res/color-v31/m3_ref_palette_dynamic_neutral24.xml` | res | 376 |
| 589 | `res/color-v31/m3_ref_palette_dynamic_neutral4.xml` | res | 376 |
| 590 | `res/color-v31/m3_ref_palette_dynamic_neutral6.xml` | res | 376 |
| 591 | `res/color-v31/m3_ref_palette_dynamic_neutral87.xml` | res | 376 |
| 592 | `res/color-v31/m3_ref_palette_dynamic_neutral92.xml` | res | 376 |
| 593 | `res/color-v31/m3_ref_palette_dynamic_neutral94.xml` | res | 376 |
| 594 | `res/color-v31/m3_ref_palette_dynamic_neutral96.xml` | res | 376 |
| 595 | `res/color-v31/m3_ref_palette_dynamic_neutral98.xml` | res | 376 |
| 596 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant12.xml` | res | 376 |
| 597 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant17.xml` | res | 376 |
| 598 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant22.xml` | res | 376 |
| 599 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant24.xml` | res | 376 |
| 600 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant4.xml` | res | 376 |
| 601 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant6.xml` | res | 376 |
| 602 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant87.xml` | res | 376 |
| 603 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant92.xml` | res | 376 |
| 604 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant94.xml` | res | 376 |
| 605 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant96.xml` | res | 376 |
| 606 | `res/color-v31/m3_ref_palette_dynamic_neutral_variant98.xml` | res | 376 |
| 607 | `res/color-v31/m3_ref_palette_dynamic_primary98.xml` | res | 376 |
| 608 | `res/color-v31/m3_ref_palette_dynamic_secondary98.xml` | res | 376 |
| 609 | `res/color-v31/m3_ref_palette_dynamic_tertiary98.xml` | res | 376 |
| 610 | `res/color/abc_background_cache_hint_selector_material_dark.xml` | res | 468 |
| 611 | `res/color/abc_background_cache_hint_selector_material_light.xml` | res | 468 |
| 612 | `res/color/abc_hint_foreground_material_dark.xml` | res | 564 |
| 613 | `res/color/abc_hint_foreground_material_light.xml` | res | 564 |
| 614 | `res/color/abc_primary_text_disable_only_material_dark.xml` | res | 464 |
| 615 | `res/color/abc_primary_text_disable_only_material_light.xml` | res | 464 |
| 616 | `res/color/abc_primary_text_material_dark.xml` | res | 464 |
| 617 | `res/color/abc_primary_text_material_light.xml` | res | 464 |
| 618 | `res/color/abc_search_url_text.xml` | res | 588 |
| 619 | `res/color/abc_secondary_text_material_dark.xml` | res | 464 |
| 620 | `res/color/abc_secondary_text_material_light.xml` | res | 464 |
| 621 | `res/color/common_google_signin_btn_text_dark.xml` | res | 712 |
| 622 | `res/color/common_google_signin_btn_text_light.xml` | res | 712 |
| 623 | `res/color/common_google_signin_btn_tint.xml` | res | 464 |
| 624 | `res/color/design_box_stroke_color.xml` | res | 712 |
| 625 | `res/color/design_error.xml` | res | 464 |
| 626 | `res/color/design_icon_tint.xml` | res | 376 |
| 627 | `res/color/m3_appbar_overlay_color.xml` | res | 376 |
| 628 | `res/color/m3_assist_chip_icon_tint_color.xml` | res | 500 |
| 629 | `res/color/m3_assist_chip_stroke_color.xml` | res | 872 |
| 630 | `res/color/m3_bottom_sheet_drag_handle_color.xml` | res | 340 |
| 631 | `res/color/m3_button_background_color_selector.xml` | res | 748 |
| 632 | `res/color/m3_button_foreground_color_selector.xml` | res | 768 |
| 633 | `res/color/m3_button_outline_color_selector.xml` | res | 500 |
| 634 | `res/color/m3_button_ripple_color.xml` | res | 808 |
| 635 | `res/color/m3_button_ripple_color_selector.xml` | res | 1936 |
| 636 | `res/color/m3_calendar_item_disabled_text.xml` | res | 376 |
| 637 | `res/color/m3_calendar_item_stroke_color.xml` | res | 712 |
| 638 | `res/color/m3_card_foreground_color.xml` | res | 768 |
| 639 | `res/color/m3_card_ripple_color.xml` | res | 1556 |
| 640 | `res/color/m3_card_stroke_color.xml` | res | 1224 |
| 641 | `res/color/m3_checkbox_button_icon_tint.xml` | res | 964 |
| 642 | `res/color/m3_checkbox_button_tint.xml` | res | 1000 |
| 643 | `res/color/m3_chip_assist_text_color.xml` | res | 500 |
| 644 | `res/color/m3_chip_background_color.xml` | res | 712 |
| 645 | `res/color/m3_chip_ripple_color.xml` | res | 1556 |
| 646 | `res/color/m3_chip_stroke_color.xml` | res | 872 |
| 647 | `res/color/m3_chip_text_color.xml` | res | 624 |
| 648 | `res/color/m3_dark_default_color_primary_text.xml` | res | 500 |
| 649 | `res/color/m3_dark_default_color_secondary_text.xml` | res | 500 |
| 650 | `res/color/m3_dark_highlighted_text.xml` | res | 376 |
| 651 | `res/color/m3_dark_hint_foreground.xml` | res | 564 |
| 652 | `res/color/m3_dark_primary_text_disable_only.xml` | res | 500 |
| 653 | `res/color/m3_default_color_primary_text.xml` | res | 500 |
| 654 | `res/color/m3_default_color_secondary_text.xml` | res | 500 |
| 655 | `res/color/m3_efab_ripple_color_selector.xml` | res | 808 |
| 656 | `res/color/m3_elevated_chip_background_color.xml` | res | 748 |
| 657 | `res/color/m3_fab_efab_background_color_selector.xml` | res | 500 |
| 658 | `res/color/m3_fab_efab_foreground_color_selector.xml` | res | 500 |
| 659 | `res/color/m3_fab_ripple_color_selector.xml` | res | 808 |
| 660 | `res/color/m3_filled_icon_button_container_color_selector.xml` | res | 668 |
| 661 | `res/color/m3_floating_toolbar_vibrant_icon_button_container_color_selector.xml` | res | 668 |
| 662 | `res/color/m3_floating_toolbar_vibrant_icon_button_text_color_selector.xml` | res | 668 |
| 663 | `res/color/m3_highlighted_text.xml` | res | 376 |
| 664 | `res/color/m3_hint_foreground.xml` | res | 564 |
| 665 | `res/color/m3_icon_button_icon_color_selector.xml` | res | 668 |
| 666 | `res/color/m3_navigation_bar_item_with_indicator_icon_tint.xml` | res | 1196 |
| 667 | `res/color/m3_navigation_bar_item_with_indicator_label_tint.xml` | res | 1196 |
| 668 | `res/color/m3_navigation_bar_ripple_color_selector.xml` | res | 1232 |
| 669 | `res/color/m3_navigation_item_background_color.xml` | res | 588 |
| 670 | `res/color/m3_navigation_item_icon_tint.xml` | res | 1356 |
| 671 | `res/color/m3_navigation_item_ripple_color.xml` | res | 1372 |
| 672 | `res/color/m3_navigation_item_text_color.xml` | res | 1356 |
| 673 | `res/color/m3_navigation_rail_item_with_indicator_icon_tint.xml` | res | 464 |
| 674 | `res/color/m3_navigation_rail_item_with_indicator_label_tint.xml` | res | 464 |
| 675 | `res/color/m3_navigation_rail_ripple_color_selector.xml` | res | 1232 |
| 676 | `res/color/m3_popupmenu_overlay_color.xml` | res | 376 |
| 677 | `res/color/m3_primary_text_disable_only.xml` | res | 500 |
| 678 | `res/color/m3_radiobutton_button_tint.xml` | res | 1576 |
| 679 | `res/color/m3_radiobutton_ripple_tint.xml` | res | 1372 |
| 680 | `res/color/m3_selection_control_ripple_color_selector.xml` | res | 1372 |
| 681 | `res/color/m3_simple_item_ripple_color.xml` | res | 748 |
| 682 | `res/color/m3_slider_active_tick_marks_color.xml` | res | 500 |
| 683 | `res/color/m3_slider_active_track_color.xml` | res | 500 |
| 684 | `res/color/m3_slider_active_track_color_legacy.xml` | res | 500 |
| 685 | `res/color/m3_slider_halo_color_legacy.xml` | res | 500 |
| 686 | `res/color/m3_slider_inactive_tick_marks_color.xml` | res | 500 |
| 687 | `res/color/m3_slider_inactive_track_color.xml` | res | 500 |
| 688 | `res/color/m3_slider_inactive_track_color_legacy.xml` | res | 500 |
| 689 | `res/color/m3_slider_thumb_color.xml` | res | 500 |
| 690 | `res/color/m3_slider_thumb_color_legacy.xml` | res | 500 |
| 691 | `res/color/m3_standard_toolbar_button_text_color_selector.xml` | res | 1460 |
| 692 | `res/color/m3_standard_toolbar_icon_button_container_color_selector.xml` | res | 632 |
| 693 | `res/color/m3_standard_toolbar_icon_button_icon_color_selector.xml` | res | 1460 |
| 694 | `res/color/m3_standard_toolbar_icon_button_ripple_color_selector.xml` | res | 1336 |
| 695 | `res/color/m3_switch_thumb_tint.xml` | res | 588 |
| 696 | `res/color/m3_switch_track_tint.xml` | res | 624 |
| 697 | `res/color/m3_tabs_icon_color.xml` | res | 624 |
| 698 | `res/color/m3_tabs_icon_color_secondary.xml` | res | 624 |
| 699 | `res/color/m3_tabs_ripple_color.xml` | res | 1372 |
| 700 | `res/color/m3_tabs_ripple_color_secondary.xml` | res | 1372 |
| 701 | `res/color/m3_tabs_text_color.xml` | res | 624 |
| 702 | `res/color/m3_tabs_text_color_secondary.xml` | res | 624 |
| 703 | `res/color/m3_text_button_background_color_selector.xml` | res | 508 |
| 704 | `res/color/m3_text_button_foreground_color_selector.xml` | res | 748 |
| 705 | `res/color/m3_text_button_ripple_color_selector.xml` | res | 1936 |
| 706 | `res/color/m3_textfield_filled_background_color.xml` | res | 500 |
| 707 | `res/color/m3_textfield_indicator_text_color.xml` | res | 872 |
| 708 | `res/color/m3_textfield_input_text_color.xml` | res | 748 |
| 709 | `res/color/m3_textfield_label_color.xml` | res | 624 |
| 710 | `res/color/m3_textfield_stroke_color.xml` | res | 748 |
| 711 | `res/color/m3_timepicker_button_background_color.xml` | res | 528 |
| 712 | `res/color/m3_timepicker_button_ripple_color.xml` | res | 1536 |
| 713 | `res/color/m3_timepicker_button_text_color.xml` | res | 768 |
| 714 | `res/color/m3_timepicker_clock_text_color.xml` | res | 464 |
| 715 | `res/color/m3_timepicker_display_background_color.xml` | res | 464 |
| 716 | `res/color/m3_timepicker_display_ripple_color.xml` | res | 1372 |
| 717 | `res/color/m3_timepicker_display_text_color.xml` | res | 624 |
| 718 | `res/color/m3_timepicker_secondary_text_button_ripple_color.xml` | res | 808 |
| 719 | `res/color/m3_timepicker_secondary_text_button_text_color.xml` | res | 500 |
| 720 | `res/color/m3_timepicker_time_input_stroke_color.xml` | res | 464 |
| 721 | `res/color/m3_tonal_button_ripple_color_selector.xml` | res | 808 |
| 722 | `res/color/m3_vibrant_toolbar_button_text_color_selector.xml` | res | 1460 |
| 723 | `res/color/m3_vibrant_toolbar_icon_button_container_color_selector.xml` | res | 632 |
| 724 | `res/color/m3_vibrant_toolbar_icon_button_icon_color_selector.xml` | res | 1460 |
| 725 | `res/color/m3_vibrant_toolbar_icon_button_ripple_color_selector.xml` | res | 1336 |
| 726 | `res/color/m3expressive_bottom_nav_item_icon_tint.xml` | res | 464 |
| 727 | `res/color/m3expressive_bottom_nav_item_label_tint.xml` | res | 464 |
| 728 | `res/color/m3expressive_bottom_nav_item_ripple_tint.xml` | res | 1232 |
| 729 | `res/color/m3expressive_button_outline_color_selector.xml` | res | 464 |
| 730 | `res/color/m3expressive_button_outlined_background_color_selector.xml` | res | 768 |
| 731 | `res/color/m3expressive_nav_rail_item_icon_tint.xml` | res | 464 |
| 732 | `res/color/m3expressive_nav_rail_item_label_tint.xml` | res | 464 |
| 733 | `res/color/m3expressive_nav_rail_item_ripple_tint.xml` | res | 1232 |
| 734 | `res/color/material_cursor_color.xml` | res | 340 |
| 735 | `res/color/material_divider_color.xml` | res | 376 |
| 736 | `res/color/material_on_background_disabled.xml` | res | 376 |
| 737 | `res/color/material_on_background_emphasis_high_type.xml` | res | 376 |
| 738 | `res/color/material_on_background_emphasis_medium.xml` | res | 376 |
| 739 | `res/color/material_on_primary_disabled.xml` | res | 376 |
| 740 | `res/color/material_on_primary_emphasis_high_type.xml` | res | 376 |
| 741 | `res/color/material_on_primary_emphasis_medium.xml` | res | 376 |
| 742 | `res/color/material_on_surface_disabled.xml` | res | 376 |
| 743 | `res/color/material_on_surface_emphasis_high_type.xml` | res | 376 |
| 744 | `res/color/material_on_surface_emphasis_medium.xml` | res | 376 |
| 745 | `res/color/material_on_surface_stroke.xml` | res | 376 |
| 746 | `res/color/material_personalized__highlighted_text.xml` | res | 376 |
| 747 | `res/color/material_personalized__highlighted_text_inverse.xml` | res | 376 |
| 748 | `res/color/material_personalized_color_primary_text.xml` | res | 500 |
| 749 | `res/color/material_personalized_color_primary_text_inverse.xml` | res | 500 |
| 750 | `res/color/material_personalized_color_secondary_text.xml` | res | 500 |
| 751 | `res/color/material_personalized_color_secondary_text_inverse.xml` | res | 500 |
| 752 | `res/color/material_personalized_hint_foreground.xml` | res | 564 |
| 753 | `res/color/material_personalized_hint_foreground_inverse.xml` | res | 564 |
| 754 | `res/color/material_personalized_primary_inverse_text_disable_only.xml` | res | 500 |
| 755 | `res/color/material_personalized_primary_text_disable_only.xml` | res | 500 |
| 756 | `res/color/material_slider_active_tick_marks_color.xml` | res | 520 |
| 757 | `res/color/material_slider_active_track_color.xml` | res | 500 |
| 758 | `res/color/material_slider_halo_color.xml` | res | 500 |
| 759 | `res/color/material_slider_inactive_tick_marks_color.xml` | res | 520 |
| 760 | `res/color/material_slider_inactive_track_color.xml` | res | 520 |
| 761 | `res/color/material_slider_thumb_color.xml` | res | 500 |
| 762 | `res/color/material_timepicker_button_background.xml` | res | 500 |
| 763 | `res/color/material_timepicker_button_stroke.xml` | res | 376 |
| 764 | `res/color/material_timepicker_clock_text_color.xml` | res | 464 |
| 765 | `res/color/material_timepicker_clockface.xml` | res | 376 |
| 766 | `res/color/material_timepicker_modebutton_tint.xml` | res | 376 |
| 767 | `res/color/mbridge_cm_feedback_rb_text_color_color_list.xml` | res | 564 |
| 768 | `res/color/mtrl_btn_bg_color_selector.xml` | res | 500 |
| 769 | `res/color/mtrl_btn_ripple_color.xml` | res | 948 |
| 770 | `res/color/mtrl_btn_stroke_color_selector.xml` | res | 520 |
| 771 | `res/color/mtrl_btn_text_btn_bg_color_selector.xml` | res | 520 |
| 772 | `res/color/mtrl_btn_text_btn_ripple_color.xml` | res | 948 |
| 773 | `res/color/mtrl_btn_text_color_selector.xml` | res | 500 |
| 774 | `res/color/mtrl_calendar_item_stroke_color.xml` | res | 808 |
| 775 | `res/color/mtrl_calendar_selected_range.xml` | res | 376 |
| 776 | `res/color/mtrl_card_view_foreground.xml` | res | 788 |
| 777 | `res/color/mtrl_card_view_ripple.xml` | res | 768 |
| 778 | `res/color/mtrl_chip_background_color.xml` | res | 848 |
| 779 | `res/color/mtrl_chip_close_icon_tint.xml` | res | 1092 |
| 780 | `res/color/mtrl_chip_surface_color.xml` | res | 340 |
| 781 | `res/color/mtrl_chip_text_color.xml` | res | 520 |
| 782 | `res/color/mtrl_choice_chip_background_color.xml` | res | 848 |
| 783 | `res/color/mtrl_choice_chip_ripple_color.xml` | res | 948 |
| 784 | `res/color/mtrl_choice_chip_text_color.xml` | res | 808 |
| 785 | `res/color/mtrl_error.xml` | res | 464 |
| 786 | `res/color/mtrl_fab_bg_color_selector.xml` | res | 500 |
| 787 | `res/color/mtrl_fab_icon_text_color_selector.xml` | res | 500 |
| 788 | `res/color/mtrl_fab_ripple_color.xml` | res | 948 |
| 789 | `res/color/mtrl_filled_background_color.xml` | res | 808 |
| 790 | `res/color/mtrl_filled_icon_tint.xml` | res | 644 |
| 791 | `res/color/mtrl_filled_stroke_color.xml` | res | 788 |
| 792 | `res/color/mtrl_indicator_text_color.xml` | res | 520 |
| 793 | `res/color/mtrl_navigation_bar_colored_item_tint.xml` | res | 520 |
| 794 | `res/color/mtrl_navigation_bar_colored_ripple_color.xml` | res | 948 |
| 795 | `res/color/mtrl_navigation_bar_item_tint.xml` | res | 520 |
| 796 | `res/color/mtrl_navigation_bar_ripple_color.xml` | res | 1672 |
| 797 | `res/color/mtrl_navigation_item_background_color.xml` | res | 644 |
| 798 | `res/color/mtrl_navigation_item_icon_tint.xml` | res | 624 |
| 799 | `res/color/mtrl_navigation_item_text_color.xml` | res | 624 |
| 800 | `res/color/mtrl_on_primary_text_btn_text_color_selector.xml` | res | 500 |
| 801 | `res/color/mtrl_on_surface_ripple_color.xml` | res | 808 |
| 802 | `res/color/mtrl_outlined_icon_tint.xml` | res | 644 |
| 803 | `res/color/mtrl_outlined_stroke_color.xml` | res | 788 |
| 804 | `res/color/mtrl_popupmenu_overlay_color.xml` | res | 376 |
| 805 | `res/color/mtrl_switch_thumb_icon_tint.xml` | res | 1236 |
| 806 | `res/color/mtrl_switch_thumb_tint.xml` | res | 1028 |
| 807 | `res/color/mtrl_switch_track_decoration_tint.xml` | res | 624 |
| 808 | `res/color/mtrl_switch_track_tint.xml` | res | 1236 |
| 809 | `res/color/mtrl_tabs_colored_ripple_color.xml` | res | 948 |
| 810 | `res/color/mtrl_tabs_icon_color_selector.xml` | res | 500 |
| 811 | `res/color/mtrl_tabs_icon_color_selector_colored.xml` | res | 500 |
| 812 | `res/color/mtrl_tabs_legacy_text_color_selector.xml` | res | 464 |
| 813 | `res/color/mtrl_tabs_ripple_color.xml` | res | 1672 |
| 814 | `res/color/mtrl_text_btn_text_color_selector.xml` | res | 888 |
| 815 | `res/color/primary_color_state.xml` | res | 464 |
| 816 | `res/color/switch_thumb_material_dark.xml` | res | 464 |
| 817 | `res/color/switch_thumb_material_light.xml` | res | 464 |
| 818 | `res/color/vector_tint_color.xml` | res | 340 |
| 819 | `res/color/vector_tint_theme_color.xml` | res | 340 |
| 820 | `res/drawable-anydpi-v21/ic_arrow_forward.xml` | res | 640 |
| 821 | `res/drawable-anydpi-v21/ic_arrow_forward_accent.xml` | res | 640 |
| 822 | `res/drawable-anydpi-v21/ic_check.xml` | res | 632 |
| 823 | `res/drawable-anydpi-v21/ic_clear_search.xml` | res | 684 |
| 824 | `res/drawable-anydpi-v21/monetization_ads_internal_fullscreen_content_v1_background_tile.xml` | res | 4524 |
| 825 | `res/drawable-anydpi-v24/$ic_palette_color__0.xml` | res | 1088 |
| 826 | `res/drawable-hdpi-v4/mbridge_banner_close.png` | res | 405 |
| 827 | `res/drawable-hdpi-v4/mbridge_bottom_media_control.png` | res | 10242 |
| 828 | `res/drawable-hdpi-v4/mbridge_cm_backward_disabled.png` | res | 308 |
| 829 | `res/drawable-hdpi-v4/mbridge_cm_backward_nor.png` | res | 304 |
| 830 | `res/drawable-hdpi-v4/mbridge_cm_backward_selected.png` | res | 359 |
| 831 | `res/drawable-hdpi-v4/mbridge_cm_browser.png` | res | 1013 |
| 832 | `res/drawable-hdpi-v4/mbridge_cm_btn_shake.png` | res | 1755 |
| 833 | `res/drawable-hdpi-v4/mbridge_cm_end_animation.png` | res | 240 |
| 834 | `res/drawable-hdpi-v4/mbridge_cm_exits_nor.png` | res | 1016 |
| 835 | `res/drawable-hdpi-v4/mbridge_cm_exits_selected.png` | res | 1230 |
| 836 | `res/drawable-hdpi-v4/mbridge_cm_forward_disabled.png` | res | 333 |
| 837 | `res/drawable-hdpi-v4/mbridge_cm_forward_nor.png` | res | 333 |
| 838 | `res/drawable-hdpi-v4/mbridge_cm_forward_selected.png` | res | 401 |
| 839 | `res/drawable-hdpi-v4/mbridge_cm_head.png` | res | 333 |
| 840 | `res/drawable-hdpi-v4/mbridge_cm_highlight.png` | res | 186 |
| 841 | `res/drawable-hdpi-v4/mbridge_cm_progress.png` | res | 333 |
| 842 | `res/drawable-hdpi-v4/mbridge_cm_progress_icon.png` | res | 835 |
| 843 | `res/drawable-hdpi-v4/mbridge_cm_refresh_nor.png` | res | 1040 |
| 844 | `res/drawable-hdpi-v4/mbridge_cm_refresh_selected.png` | res | 1319 |
| 845 | `res/drawable-hdpi-v4/mbridge_cm_tail.png` | res | 97 |
| 846 | `res/drawable-hdpi-v4/mbridge_demo_star_nor.png` | res | 428 |
| 847 | `res/drawable-hdpi-v4/mbridge_demo_star_sel.png` | res | 455 |
| 848 | `res/drawable-hdpi-v4/mbridge_finger_media_control.png` | res | 11466 |
| 849 | `res/drawable-hdpi-v4/mbridge_icon_click_circle.png` | res | 1058 |
| 850 | `res/drawable-hdpi-v4/mbridge_icon_click_hand.png` | res | 7472 |
| 851 | `res/drawable-hdpi-v4/mbridge_icon_play_bg.png` | res | 1485 |
| 852 | `res/drawable-hdpi-v4/mbridge_interstitial_close.png` | res | 3739 |
| 853 | `res/drawable-hdpi-v4/mbridge_nativex_close.png` | res | 2030 |
| 854 | `res/drawable-hdpi-v4/mbridge_nativex_cta_land_nor.9.png` | res | 802 |
| 855 | `res/drawable-hdpi-v4/mbridge_nativex_cta_land_pre.9.png` | res | 1053 |
| 856 | `res/drawable-hdpi-v4/mbridge_nativex_pause.png` | res | 4662 |
| 857 | `res/drawable-hdpi-v4/mbridge_nativex_play.png` | res | 6539 |
| 858 | `res/drawable-hdpi-v4/mbridge_nativex_sound1.png` | res | 123 |
| 859 | `res/drawable-hdpi-v4/mbridge_nativex_sound2.png` | res | 123 |
| 860 | `res/drawable-hdpi-v4/mbridge_nativex_sound3.png` | res | 124 |
| 861 | `res/drawable-hdpi-v4/mbridge_nativex_sound4.png` | res | 121 |
| 862 | `res/drawable-hdpi-v4/mbridge_nativex_sound5.png` | res | 124 |
| 863 | `res/drawable-hdpi-v4/mbridge_nativex_sound6.png` | res | 123 |
| 864 | `res/drawable-hdpi-v4/mbridge_nativex_sound7.png` | res | 124 |
| 865 | `res/drawable-hdpi-v4/mbridge_nativex_sound8.png` | res | 122 |
| 866 | `res/drawable-hdpi-v4/mbridge_nativex_sound_close.png` | res | 1124 |
| 867 | `res/drawable-hdpi-v4/mbridge_nativex_sound_open.png` | res | 1472 |
| 868 | `res/drawable-hdpi-v4/mbridge_reward_activity_ad_end_land_des_rl_hot.png` | res | 1125 |
| 869 | `res/drawable-hdpi-v4/mbridge_reward_close.png` | res | 3739 |
| 870 | `res/drawable-hdpi-v4/mbridge_reward_close_ec.png` | res | 1321 |
| 871 | `res/drawable-hdpi-v4/mbridge_reward_end_land_shape.9.png` | res | 320 |
| 872 | `res/drawable-hdpi-v4/mbridge_reward_end_pager_logo.png` | res | 1706 |
| 873 | `res/drawable-hdpi-v4/mbridge_reward_flag_cn.png` | res | 787 |
| 874 | `res/drawable-hdpi-v4/mbridge_reward_flag_en.png` | res | 868 |
| 875 | `res/drawable-hdpi-v4/mbridge_reward_notice.png` | res | 1029 |
| 876 | `res/drawable-hdpi-v4/mbridge_reward_popview_close.png` | res | 279 |
| 877 | `res/drawable-hdpi-v4/mbridge_reward_sound_close.png` | res | 3028 |
| 878 | `res/drawable-hdpi-v4/mbridge_reward_sound_open.png` | res | 3111 |
| 879 | `res/drawable-hdpi-v4/mbridge_reward_two_title_arabia_land.png` | res | 10916 |
| 880 | `res/drawable-hdpi-v4/mbridge_reward_two_title_arabia_por.png` | res | 10981 |
| 881 | `res/drawable-hdpi-v4/mbridge_reward_two_title_en_land.png` | res | 17264 |
| 882 | `res/drawable-hdpi-v4/mbridge_reward_two_title_en_por.png` | res | 19060 |
| 883 | `res/drawable-hdpi-v4/mbridge_reward_two_title_france_land.png` | res | 9802 |
| 884 | `res/drawable-hdpi-v4/mbridge_reward_two_title_france_por.png` | res | 10998 |
| 885 | `res/drawable-hdpi-v4/mbridge_reward_two_title_germany_land.png` | res | 9874 |
| 886 | `res/drawable-hdpi-v4/mbridge_reward_two_title_germany_por.png` | res | 11062 |
| 887 | `res/drawable-hdpi-v4/mbridge_reward_two_title_japan_land.png` | res | 14144 |
| 888 | `res/drawable-hdpi-v4/mbridge_reward_two_title_japan_por.png` | res | 13375 |
| 889 | `res/drawable-hdpi-v4/mbridge_reward_two_title_korea_land.png` | res | 5069 |
| 890 | `res/drawable-hdpi-v4/mbridge_reward_two_title_korea_por.png` | res | 5348 |
| 891 | `res/drawable-hdpi-v4/mbridge_reward_two_title_russian_land.png` | res | 8503 |
| 892 | `res/drawable-hdpi-v4/mbridge_reward_two_title_russian_por.png` | res | 10089 |
| 893 | `res/drawable-hdpi-v4/mbridge_reward_two_title_zh.png` | res | 24597 |
| 894 | `res/drawable-hdpi-v4/mbridge_reward_two_title_zh_trad.png` | res | 14016 |
| 895 | `res/drawable-hdpi-v4/mbridge_reward_user.png` | res | 312 |
| 896 | `res/drawable-hdpi-v4/mbridge_reward_vast_end_close.png` | res | 2145 |
| 897 | `res/drawable-hdpi-v4/mbridge_reward_vast_end_ok.png` | res | 2312 |
| 898 | `res/drawable-hdpi-v4/mbridge_reward_video_icon.png` | res | 217 |
| 899 | `res/drawable-hdpi-v4/mbridge_slide_hand.png` | res | 1934 |
| 900 | `res/drawable-hdpi-v4/mbridge_slide_rightarrow.png` | res | 1687 |
| 901 | `res/drawable-hdpi-v4/mbridge_splash_ad_right_bottom_corner_en.png` | res | 1918 |
| 902 | `res/drawable-hdpi-v4/mbridge_splash_ad_right_bottom_corner_zh.png` | res | 1640 |
| 903 | `res/drawable-hdpi-v4/mbridge_splash_btn_arrow_right.png` | res | 205 |
| 904 | `res/drawable-hdpi-v4/mbridge_splash_btn_circle.png` | res | 982 |
| 905 | `res/drawable-hdpi-v4/mbridge_splash_btn_finger.png` | res | 828 |
| 906 | `res/drawable-hdpi-v4/mbridge_splash_btn_go.png` | res | 1383 |
| 907 | `res/drawable-hdpi-v4/mbridge_splash_btn_light.png` | res | 6808 |
| 908 | `res/drawable-hdpi-v4/mbridge_splash_m_circle.png` | res | 370 |
| 909 | `res/drawable-hdpi-v4/mbridge_splash_notice.png` | res | 1029 |
| 910 | `res/drawable-hdpi-v4/mbridge_splash_pop_ad.png` | res | 787 |
| 911 | `res/drawable-hdpi-v4/mbridge_splash_pop_ad_en.png` | res | 868 |
| 912 | `res/drawable-hdpi-v4/mbridge_splash_popview_close.png` | res | 865 |
| 913 | `res/drawable-hdpi-v4/mbridge_splash_popview_default.png` | res | 4581 |
| 914 | `res/drawable-hdpi-v4/mbridge_video_common_full_star.png` | res | 337 |
| 915 | `res/drawable-hdpi-v4/mbridge_video_common_full_while_star.png` | res | 296 |
| 916 | `res/drawable-hdpi-v4/mbridge_video_common_half_star.png` | res | 395 |
| 917 | `res/drawable-hdpi-v4/mobilefuse_close_btn.png` | res | 1078 |
| 918 | `res/drawable-hdpi-v4/mobilefuse_vast_endcard_close_btn.png` | res | 1078 |
| 919 | `res/drawable-hdpi-v4/mobilefuse_vast_endcard_transparent_close_btn.png` | res | 73 |
| 920 | `res/drawable-hdpi-v4/notification_oversize_large_icon_bg.png` | res | 909 |
| 921 | `res/drawable-nodpi-v4/ia_ib_background.png` | res | 118 |
| 922 | `res/drawable-nodpi-v4/ia_ib_close.png` | res | 662 |
| 923 | `res/drawable-nodpi-v4/ia_ib_left_arrow.png` | res | 570 |
| 924 | `res/drawable-nodpi-v4/ia_ib_refresh.png` | res | 726 |
| 925 | `res/drawable-nodpi-v4/ia_ib_right_arrow.png` | res | 487 |
| 926 | `res/drawable-nodpi-v4/ia_ib_unleft_arrow.png` | res | 570 |
| 927 | `res/drawable-nodpi-v4/ia_ib_unright_arrow.png` | res | 487 |
| 928 | `res/drawable-sw600dp-v13/ia_close.png` | res | 360 |
| 929 | `res/drawable-v21/abc_action_bar_item_background_material.xml` | res | 264 |
| 930 | `res/drawable-v21/abc_btn_colored_material.xml` | res | 1716 |
| 931 | `res/drawable-v21/abc_dialog_material_background.xml` | res | 716 |
| 932 | `res/drawable-v21/abc_edit_text_material.xml` | res | 1172 |
| 933 | `res/drawable-v21/abc_list_divider_material.xml` | res | 516 |
| 934 | `res/drawable-v21/ic_arrow_down_24dp.xml` | res | 644 |
| 935 | `res/drawable-v21/notification_action_background.xml` | res | 1180 |
| 936 | `res/drawable-v21/preference_list_divider_material.xml` | res | 516 |
| 937 | `res/drawable-v21/tt_item_background_material.xml` | res | 536 |
| 938 | `res/drawable-v23/abc_control_background_material.xml` | res | 304 |
| 939 | `res/drawable-v23/m3_radiobutton_ripple.xml` | res | 304 |
| 940 | `res/drawable-v23/m3_selection_control_ripple.xml` | res | 304 |
| 941 | `res/drawable-v23/m3_tabs_background.xml` | res | 868 |
| 942 | `res/drawable-v23/m3_tabs_transparent_background.xml` | res | 632 |
| 943 | `res/drawable-v23/mtrl_popupmenu_background_overlay.xml` | res | 1228 |
| 944 | `res/drawable-watch-v20/abc_dialog_material_background.xml` | res | 372 |
| 945 | `res/drawable-watch-v20/common_google_signin_btn_text_dark_normal.xml` | res | 908 |
| 946 | `res/drawable-watch-v20/common_google_signin_btn_text_disabled.xml` | res | 1040 |
| 947 | `res/drawable-watch-v20/common_google_signin_btn_text_light_normal.xml` | res | 872 |
| 948 | `res/drawable-xhdpi-v4/ia_digital_turbine_logo.png` | res | 1032 |
| 949 | `res/drawable-xhdpi-v4/ia_fyber_info_button.png` | res | 604 |
| 950 | `res/drawable-xhdpi-v4/tt_ad_arrow_backward.png` | res | 175 |
| 951 | `res/drawable-xhdpi-v4/tt_ad_arrow_forward.png` | res | 167 |
| 952 | `res/drawable-xhdpi-v4/tt_ad_arrow_right.png` | res | 135 |
| 953 | `res/drawable-xhdpi-v4/tt_ad_bg_header_gradient.png` | res | 3356 |
| 954 | `res/drawable-xhdpi-v4/tt_ad_closed_background_300_250.png` | res | 3533 |
| 955 | `res/drawable-xhdpi-v4/tt_ad_closed_background_320_50.png` | res | 817 |
| 956 | `res/drawable-xhdpi-v4/tt_ad_feedback.png` | res | 581 |
| 957 | `res/drawable-xhdpi-v4/tt_ad_feedback_new.png` | res | 385 |
| 958 | `res/drawable-xhdpi-v4/tt_ad_link.png` | res | 793 |
| 959 | `res/drawable-xhdpi-v4/tt_ad_logo.png` | res | 323 |
| 960 | `res/drawable-xhdpi-v4/tt_ad_refresh.png` | res | 595 |
| 961 | `res/drawable-xhdpi-v4/tt_ad_threedots.png` | res | 315 |
| 962 | `res/drawable-xhdpi-v4/tt_ad_xmark.png` | res | 247 |
| 963 | `res/drawable-xhdpi-v4/tt_ad_zoom_down.png` | res | 218 |
| 964 | `res/drawable-xhdpi-v4/tt_ad_zoom_up.png` | res | 195 |
| 965 | `res/drawable-xhdpi-v4/tt_blue_hand.png` | res | 1626 |
| 966 | `res/drawable-xhdpi-v4/tt_close_btn.png` | res | 186 |
| 967 | `res/drawable-xhdpi-v4/tt_close_move_details_normal.png` | res | 249 |
| 968 | `res/drawable-xhdpi-v4/tt_close_move_details_pressed.png` | res | 240 |
| 969 | `res/drawable-xhdpi-v4/tt_dislike_icon.png` | res | 530 |
| 970 | `res/drawable-xhdpi-v4/tt_dislike_icon2.png` | res | 429 |
| 971 | `res/drawable-xhdpi-v4/tt_dislike_toast.png` | res | 281 |
| 972 | `res/drawable-xhdpi-v4/tt_enlarge_video.png` | res | 159 |
| 973 | `res/drawable-xhdpi-v4/tt_lefterbackicon_titlebar.png` | res | 154 |
| 974 | `res/drawable-xhdpi-v4/tt_lefterbackicon_titlebar_for_dark.png` | res | 169 |
| 975 | `res/drawable-xhdpi-v4/tt_lefterbackicon_titlebar_press.png` | res | 168 |
| 976 | `res/drawable-xhdpi-v4/tt_lefterbackicon_titlebar_press_for_dark.png` | res | 167 |
| 977 | `res/drawable-xhdpi-v4/tt_mute.png` | res | 352 |
| 978 | `res/drawable-xhdpi-v4/tt_new_pause_video.png` | res | 633 |
| 979 | `res/drawable-xhdpi-v4/tt_new_pause_video_press.png` | res | 642 |
| 980 | `res/drawable-xhdpi-v4/tt_new_play_video.png` | res | 778 |
| 981 | `res/drawable-xhdpi-v4/tt_normalscreen_loading.png` | res | 868 |
| 982 | `res/drawable-xhdpi-v4/tt_pangle_ad_close_drawable.png` | res | 177 |
| 983 | `res/drawable-xhdpi-v4/tt_refreshing_video_textpage_normal.png` | res | 436 |
| 984 | `res/drawable-xhdpi-v4/tt_refreshing_video_textpage_pressed.png` | res | 395 |
| 985 | `res/drawable-xhdpi-v4/tt_report_ad_arrow.png` | res | 106 |
| 986 | `res/drawable-xhdpi-v4/tt_reward_full_feedback.png` | res | 293 |
| 987 | `res/drawable-xhdpi-v4/tt_reward_full_mute.png` | res | 464 |
| 988 | `res/drawable-xhdpi-v4/tt_reward_full_unmute.png` | res | 417 |
| 989 | `res/drawable-xhdpi-v4/tt_shadow_fullscreen_top.9.png` | res | 387 |
| 990 | `res/drawable-xhdpi-v4/tt_shadow_lefterback_titlebar.png` | res | 340 |
| 991 | `res/drawable-xhdpi-v4/tt_shadow_lefterback_titlebar_press.png` | res | 258 |
| 992 | `res/drawable-xhdpi-v4/tt_shadow_lefterback_titlebar_press_withoutnight.png` | res | 258 |
| 993 | `res/drawable-xhdpi-v4/tt_shadow_lefterback_titlebar_withoutnight.png` | res | 340 |
| 994 | `res/drawable-xhdpi-v4/tt_shrink_fullscreen.png` | res | 300 |
| 995 | `res/drawable-xhdpi-v4/tt_shrink_video.png` | res | 138 |
| 996 | `res/drawable-xhdpi-v4/tt_skip_btn.png` | res | 161 |
| 997 | `res/drawable-xhdpi-v4/tt_splash_hand.png` | res | 2801 |
| 998 | `res/drawable-xhdpi-v4/tt_splash_hand2.png` | res | 2851 |
| 999 | `res/drawable-xhdpi-v4/tt_splash_rock.png` | res | 1871 |
| 1000 | `res/drawable-xhdpi-v4/tt_splash_shake_hand.png` | res | 623 |
| 1001 | `res/drawable-xhdpi-v4/tt_splash_slide_right_bg.png` | res | 559 |
| 1002 | `res/drawable-xhdpi-v4/tt_splash_slide_right_circle.png` | res | 1308 |
| 1003 | `res/drawable-xhdpi-v4/tt_splash_slide_up_10.png` | res | 379 |
| 1004 | `res/drawable-xhdpi-v4/tt_splash_slide_up_arrow.png` | res | 432 |
| 1005 | `res/drawable-xhdpi-v4/tt_splash_slide_up_bg.png` | res | 920 |
| 1006 | `res/drawable-xhdpi-v4/tt_splash_slide_up_circle.png` | res | 1400 |
| 1007 | `res/drawable-xhdpi-v4/tt_splash_slide_up_finger.png` | res | 2347 |
| 1008 | `res/drawable-xhdpi-v4/tt_splash_twist.png` | res | 455 |
| 1009 | `res/drawable-xhdpi-v4/tt_splash_unlock_btn_bg.png` | res | 1345 |
| 1010 | `res/drawable-xhdpi-v4/tt_splash_unlock_icon_empty.png` | res | 771 |
| 1011 | `res/drawable-xhdpi-v4/tt_splash_unlock_image_arrow.png` | res | 410 |
| 1012 | `res/drawable-xhdpi-v4/tt_splash_unlock_image_go.png` | res | 4561 |
| 1013 | `res/drawable-xhdpi-v4/tt_star.png` | res | 278 |
| 1014 | `res/drawable-xhdpi-v4/tt_star_thick.png` | res | 174 |
| 1015 | `res/drawable-xhdpi-v4/tt_star_thick_dark.png` | res | 366 |
| 1016 | `res/drawable-xhdpi-v4/tt_suggestion_logo.png` | res | 257 |
| 1017 | `res/drawable-xhdpi-v4/tt_titlebar_close_drawable.png` | res | 253 |
| 1018 | `res/drawable-xhdpi-v4/tt_titlebar_close_for_dark.png` | res | 266 |
| 1019 | `res/drawable-xhdpi-v4/tt_titlebar_close_press.png` | res | 261 |
| 1020 | `res/drawable-xhdpi-v4/tt_titlebar_close_press_for_dark.png` | res | 260 |
| 1021 | `res/drawable-xhdpi-v4/tt_unmute.png` | res | 425 |
| 1022 | `res/drawable-xhdpi-v4/tt_up_slide.png` | res | 367 |
| 1023 | `res/drawable-xhdpi-v4/tt_user.png` | res | 300 |
| 1024 | `res/drawable-xhdpi-v4/tt_video_close_drawable.png` | res | 177 |
| 1025 | `res/drawable-xhdpi-v4/tt_white_hand.png` | res | 982 |
| 1026 | `res/drawable-xhdpi-v4/tt_white_lefterbackicon_titlebar.png` | res | 377 |
| 1027 | `res/drawable-xhdpi-v4/tt_white_lefterbackicon_titlebar_press.png` | res | 363 |
| 1028 | `res/drawable-xhdpi-v4/tt_white_righterbackicon_titlebar.png` | res | 1065 |
| 1029 | `res/drawable-xhdpi-v4/tt_white_slide_up.png` | res | 214 |
| 1030 | `res/drawable-xhdpi-v4/tt_wriggle_union.png` | res | 1007 |
| 1031 | `res/drawable-xhdpi-v4/tt_wriggle_union_white.png` | res | 922 |
| 1032 | `res/drawable-xxhdpi-v4/instream_skip.png` | res | 286 |
| 1033 | `res/drawable-xxhdpi-v4/taurusx_close_no_bg.png` | res | 388 |
| 1034 | `res/drawable-xxhdpi-v4/taurusx_close_pressed.png` | res | 1019 |
| 1035 | `res/drawable-xxhdpi-v4/taurusx_ic_close.png` | res | 1156 |
| 1036 | `res/drawable-xxhdpi-v4/taurusx_ic_more.png` | res | 873 |
| 1037 | `res/drawable-xxhdpi-v4/taurusx_ic_more_no_bg.png` | res | 161 |
| 1038 | `res/drawable-xxhdpi-v4/taurusx_ic_reload.png` | res | 1081 |
| 1039 | `res/drawable-xxhdpi-v4/taurusx_inner_btn_bg_pressed.png` | res | 719 |
| 1040 | `res/drawable-xxhdpi-v4/taurusx_inner_video_skip.png` | res | 1288 |
| 1041 | `res/drawable-xxhdpi-v4/taurusx_skip_pressed.png` | res | 1140 |
| 1042 | `res/drawable-xxhdpi-v4/taurusx_tax_ic_skip.png` | res | 1578 |
| 1043 | `res/drawable-xxhdpi-v4/taurusx_tax_ic_volume_off.png` | res | 1348 |
| 1044 | `res/drawable-xxhdpi-v4/taurusx_tax_ic_volume_on.png` | res | 1646 |
| 1045 | `res/drawable/$applovin_ic_mute_to_unmute__0.xml` | res | 1904 |
| 1046 | `res/drawable/$applovin_ic_mute_to_unmute__1.xml` | res | 684 |
| 1047 | `res/drawable/$applovin_ic_mute_to_unmute__2.xml` | res | 768 |
| 1048 | `res/drawable/$applovin_ic_mute_to_unmute__3.xml` | res | 736 |
| 1049 | `res/drawable/$applovin_ic_mute_to_unmute__4.xml` | res | 748 |
| 1050 | `res/drawable/$applovin_ic_unmute_to_mute__0.xml` | res | 1856 |
| 1051 | `res/drawable/$applovin_ic_unmute_to_mute__1.xml` | res | 684 |
| 1052 | `res/drawable/$applovin_ic_unmute_to_mute__2.xml` | res | 768 |
| 1053 | `res/drawable/$applovin_ic_unmute_to_mute__3.xml` | res | 736 |
| 1054 | `res/drawable/$applovin_ic_unmute_to_mute__4.xml` | res | 748 |
| 1055 | `res/drawable/$avd_hide_password__0.xml` | res | 1176 |
| 1056 | `res/drawable/$avd_hide_password__1.xml` | res | 592 |
| 1057 | `res/drawable/$avd_hide_password__2.xml` | res | 556 |
| 1058 | `res/drawable/$avd_show_password__0.xml` | res | 1136 |
| 1059 | `res/drawable/$avd_show_password__1.xml` | res | 592 |
| 1060 | `res/drawable/$avd_show_password__2.xml` | res | 556 |
| 1061 | `res/drawable/$ic_launcher_foreground__0.xml` | res | 788 |
| 1062 | `res/drawable/$m3_avd_hide_password__0.xml` | res | 1176 |
| 1063 | `res/drawable/$m3_avd_hide_password__1.xml` | res | 592 |
| 1064 | `res/drawable/$m3_avd_hide_password__2.xml` | res | 556 |
| 1065 | `res/drawable/$m3_avd_show_password__0.xml` | res | 1136 |
| 1066 | `res/drawable/$m3_avd_show_password__1.xml` | res | 592 |
| 1067 | `res/drawable/$m3_avd_show_password__2.xml` | res | 556 |
| 1068 | `res/drawable/$mtrl_checkbox_button_checked_unchecked__0.xml` | res | 592 |
| 1069 | `res/drawable/$mtrl_checkbox_button_checked_unchecked__1.xml` | res | 592 |
| 1070 | `res/drawable/$mtrl_checkbox_button_checked_unchecked__2.xml` | res | 592 |
| 1071 | `res/drawable/$mtrl_checkbox_button_icon_checked_indeterminate__0.xml` | res | 592 |
| 1072 | `res/drawable/$mtrl_checkbox_button_icon_checked_unchecked__0.xml` | res | 592 |
| 1073 | `res/drawable/$mtrl_checkbox_button_icon_checked_unchecked__1.xml` | res | 592 |
| 1074 | `res/drawable/$mtrl_checkbox_button_icon_checked_unchecked__2.xml` | res | 592 |
| 1075 | `res/drawable/$mtrl_checkbox_button_icon_indeterminate_checked__0.xml` | res | 592 |
| 1076 | `res/drawable/$mtrl_checkbox_button_icon_indeterminate_unchecked__0.xml` | res | 592 |
| 1077 | `res/drawable/$mtrl_checkbox_button_icon_indeterminate_unchecked__1.xml` | res | 592 |
| 1078 | `res/drawable/$mtrl_checkbox_button_icon_indeterminate_unchecked__2.xml` | res | 592 |
| 1079 | `res/drawable/$mtrl_checkbox_button_icon_unchecked_checked__0.xml` | res | 592 |
| 1080 | `res/drawable/$mtrl_checkbox_button_icon_unchecked_checked__1.xml` | res | 592 |
| 1081 | `res/drawable/$mtrl_checkbox_button_icon_unchecked_checked__2.xml` | res | 596 |
| 1082 | `res/drawable/$mtrl_checkbox_button_icon_unchecked_indeterminate__0.xml` | res | 592 |
| 1083 | `res/drawable/$mtrl_checkbox_button_icon_unchecked_indeterminate__1.xml` | res | 592 |
| 1084 | `res/drawable/$mtrl_checkbox_button_icon_unchecked_indeterminate__2.xml` | res | 592 |
| 1085 | `res/drawable/$mtrl_checkbox_button_unchecked_checked__0.xml` | res | 592 |
| 1086 | `res/drawable/$mtrl_checkbox_button_unchecked_checked__1.xml` | res | 592 |
| 1087 | `res/drawable/$mtrl_checkbox_button_unchecked_checked__2.xml` | res | 592 |
| 1088 | `res/drawable/$mtrl_switch_thumb_checked_pressed__0.xml` | res | 592 |
| 1089 | `res/drawable/$mtrl_switch_thumb_checked_unchecked__0.xml` | res | 548 |
| 1090 | `res/drawable/$mtrl_switch_thumb_checked_unchecked__1.xml` | res | 592 |
| 1091 | `res/drawable/$mtrl_switch_thumb_pressed_checked__0.xml` | res | 592 |
| 1092 | `res/drawable/$mtrl_switch_thumb_pressed_unchecked__0.xml` | res | 592 |
| 1093 | `res/drawable/$mtrl_switch_thumb_unchecked_checked__0.xml` | res | 548 |
| 1094 | `res/drawable/$mtrl_switch_thumb_unchecked_checked__1.xml` | res | 592 |
| 1095 | `res/drawable/$mtrl_switch_thumb_unchecked_pressed__0.xml` | res | 592 |
| 1096 | `res/drawable/abc_btn_borderless_material.xml` | res | 588 |
| 1097 | `res/drawable/abc_btn_check_material.xml` | res | 464 |
| 1098 | `res/drawable/abc_btn_check_material_anim.xml` | res | 816 |
| 1099 | `res/drawable/abc_btn_default_mtrl_shape.xml` | res | 932 |
| 1100 | `res/drawable/abc_btn_radio_material.xml` | res | 464 |
| 1101 | `res/drawable/abc_btn_radio_material_anim.xml` | res | 816 |
| 1102 | `res/drawable/abc_cab_background_internal_bg.xml` | res | 372 |
| 1103 | `res/drawable/abc_cab_background_top_material.xml` | res | 336 |
| 1104 | `res/drawable/abc_ic_ab_back_material.xml` | res | 692 |
| 1105 | `res/drawable/abc_ic_arrow_drop_right_black_24dp.xml` | res | 1000 |
| 1106 | `res/drawable/abc_ic_clear_material.xml` | res | 684 |
| 1107 | `res/drawable/abc_ic_go_search_api_material.xml` | res | 640 |
| 1108 | `res/drawable/abc_ic_menu_copy_mtrl_am_alpha.xml` | res | 756 |
| 1109 | `res/drawable/abc_ic_menu_cut_mtrl_alpha.xml` | res | 1096 |
| 1110 | `res/drawable/abc_ic_menu_overflow_material.xml` | res | 792 |
| 1111 | `res/drawable/abc_ic_menu_paste_mtrl_am_alpha.xml` | res | 796 |
| 1112 | `res/drawable/abc_ic_menu_selectall_mtrl_alpha.xml` | res | 920 |
| 1113 | `res/drawable/abc_ic_menu_share_mtrl_alpha.xml` | res | 980 |
| 1114 | `res/drawable/abc_ic_search_api_material.xml` | res | 812 |
| 1115 | `res/drawable/abc_ic_voice_search_api_material.xml` | res | 828 |
| 1116 | `res/drawable/abc_item_background_holo_dark.xml` | res | 1012 |
| 1117 | `res/drawable/abc_item_background_holo_light.xml` | res | 1012 |
| 1118 | `res/drawable/abc_list_selector_background_transition_holo_dark.xml` | res | 424 |
| 1119 | `res/drawable/abc_list_selector_background_transition_holo_light.xml` | res | 424 |
| 1120 | `res/drawable/abc_list_selector_holo_dark.xml` | res | 1064 |
| 1121 | `res/drawable/abc_list_selector_holo_light.xml` | res | 1064 |
| 1122 | `res/drawable/abc_ratingbar_indicator_material.xml` | res | 124 |
| 1123 | `res/drawable/abc_ratingbar_material.xml` | res | 124 |
| 1124 | `res/drawable/abc_ratingbar_small_material.xml` | res | 124 |
| 1125 | `res/drawable/abc_seekbar_thumb_material.xml` | res | 1100 |
| 1126 | `res/drawable/abc_seekbar_tick_mark_material.xml` | res | 516 |
| 1127 | `res/drawable/abc_seekbar_track_material.xml` | res | 1408 |
| 1128 | `res/drawable/abc_spinner_textfield_background_material.xml` | res | 1160 |
| 1129 | `res/drawable/abc_star_black_48dp.xml` | res | 640 |
| 1130 | `res/drawable/abc_star_half_black_48dp.xml` | res | 600 |
| 1131 | `res/drawable/abc_switch_thumb_material.xml` | res | 464 |
| 1132 | `res/drawable/abc_tab_indicator_material.xml` | res | 468 |
| 1133 | `res/drawable/abc_text_cursor_material.xml` | res | 516 |
| 1134 | `res/drawable/abc_textfield_search_material.xml` | res | 756 |
| 1135 | `res/drawable/abc_vector_test.xml` | res | 612 |
| 1136 | `res/drawable/ad_mob_outline_shape.xml` | res | 500 |
| 1137 | `res/drawable/ad_mob_rounded_button_shape.xml` | res | 448 |
| 1138 | `res/drawable/ad_mob_rounded_corners_shape.xml` | res | 940 |
| 1139 | `res/drawable/admob_close_button_black_circle_white_cross.xml` | res | 988 |
| 1140 | `res/drawable/admob_close_button_white_circle_black_cross.xml` | res | 988 |
| 1141 | `res/drawable/admob_close_button_white_cross.xml` | res | 704 |
| 1142 | `res/drawable/apd_ad_choice_preview.xml` | res | 1288 |
| 1143 | `res/drawable/apd_admob_cta_round_outline.xml` | res | 448 |
| 1144 | `res/drawable/apd_admob_round_outline.xml` | res | 340 |
| 1145 | `res/drawable/apd_iab_ad_choice_preview.xml` | res | 1288 |
| 1146 | `res/drawable/apd_icon_preview.xml` | res | 1600 |
| 1147 | `res/drawable/apd_media_preview.xml` | res | 10288 |
| 1148 | `res/drawable/applovin_creative_debugger_report_ad_rounded_button.xml` | res | 1316 |
| 1149 | `res/drawable/applovin_ic_mediation_placeholder.xml` | res | 628 |
| 1150 | `res/drawable/applovin_ic_mute_to_unmute.xml` | res | 916 |
| 1151 | `res/drawable/applovin_ic_privacy_icon.png` | res | 498 |
| 1152 | `res/drawable/applovin_ic_privacy_icon_layered_list.xml` | res | 760 |
| 1153 | `res/drawable/applovin_ic_unmute_to_mute.xml` | res | 916 |
| 1154 | `res/drawable/applovin_ic_white_small.png` | res | 1909 |
| 1155 | `res/drawable/applovin_mediation_debugger_switch_thumb.xml` | res | 1260 |
| 1156 | `res/drawable/applovin_mediation_debugger_switch_track.xml` | res | 1236 |
| 1157 | `res/drawable/applovin_rounded_black_background.xml` | res | 448 |
| 1158 | `res/drawable/applovin_rounded_button.xml` | res | 1316 |
| 1159 | `res/drawable/applovin_rounded_text_view_border.xml` | res | 612 |
| 1160 | `res/drawable/arrow_mark.xml` | res | 464 |
| 1161 | `res/drawable/avd_hide_password.xml` | res | 660 |
| 1162 | `res/drawable/avd_show_password.xml` | res | 660 |
| 1163 | `res/drawable/bg_btn_get.xml` | res | 448 |
| 1164 | `res/drawable/bigo_ad_advertiser_background.xml` | res | 484 |
| 1165 | `res/drawable/bigo_ad_arrow_right.png` | res | 291 |
| 1166 | `res/drawable/bigo_ad_banner_advertiser_background.xml` | res | 576 |
| 1167 | `res/drawable/bigo_ad_banner_button_bg_rectangle_blue.xml` | res | 628 |
| 1168 | `res/drawable/bigo_ad_banner_clickguide.png` | res | 1693 |
| 1169 | `res/drawable/bigo_ad_bg_ad_tag_white_border.xml` | res | 576 |
| 1170 | `res/drawable/bigo_ad_bg_mask.xml` | res | 672 |
| 1171 | `res/drawable/bigo_ad_btn_background.xml` | res | 568 |
| 1172 | `res/drawable/bigo_ad_btn_background_white.xml` | res | 612 |
| 1173 | `res/drawable/bigo_ad_btn_background_white_dark.xml` | res | 484 |
| 1174 | `res/drawable/bigo_ad_button_bg_circle_grey.xml` | res | 516 |
| 1175 | `res/drawable/bigo_ad_button_bg_rectangle_blue.xml` | res | 628 |
| 1176 | `res/drawable/bigo_ad_button_bg_rectangle_blue_2.xml` | res | 628 |
| 1177 | `res/drawable/bigo_ad_button_bg_rectangle_green.xml` | res | 628 |
| 1178 | `res/drawable/bigo_ad_button_bg_rectangle_green_radius8.xml` | res | 484 |
| 1179 | `res/drawable/bigo_ad_button_countdown_close.xml` | res | 1252 |
| 1180 | `res/drawable/bigo_ad_button_stroke_circle_white.xml` | res | 536 |
| 1181 | `res/drawable/bigo_ad_cardview_bg.xml` | res | 628 |
| 1182 | `res/drawable/bigo_ad_click_guide.png` | res | 1693 |
| 1183 | `res/drawable/bigo_ad_click_guide2.xml` | res | 552 |
| 1184 | `res/drawable/bigo_ad_click_guide_landscape.png` | res | 19589 |
| 1185 | `res/drawable/bigo_ad_click_guide_shadow.png` | res | 10133 |
| 1186 | `res/drawable/bigo_ad_click_ripple.xml` | res | 516 |
| 1187 | `res/drawable/bigo_ad_close_btn_background.xml` | res | 1108 |
| 1188 | `res/drawable/bigo_ad_default_base_image.jpg` | res | 12131 |
| 1189 | `res/drawable/bigo_ad_default_progressbar.xml` | res | 988 |
| 1190 | `res/drawable/bigo_ad_domain_left_drawable.xml` | res | 684 |
| 1191 | `res/drawable/bigo_ad_double_video_play.png` | res | 2345 |
| 1192 | `res/drawable/bigo_ad_double_video_small_play.png` | res | 282 |
| 1193 | `res/drawable/bigo_ad_edit_background.xml` | res | 760 |
| 1194 | `res/drawable/bigo_ad_edit_background_dark.xml` | res | 668 |
| 1195 | `res/drawable/bigo_ad_feeback_arrow_right.png` | res | 283 |
| 1196 | `res/drawable/bigo_ad_feedback_advertise_info_background.xml` | res | 484 |
| 1197 | `res/drawable/bigo_ad_feedback_background.xml` | res | 500 |
| 1198 | `res/drawable/bigo_ad_feedback_top_slide_bar.xml` | res | 448 |
| 1199 | `res/drawable/bigo_ad_flash.png` | res | 466 |
| 1200 | `res/drawable/bigo_ad_form_icon_back_up.png` | res | 1676 |
| 1201 | `res/drawable/bigo_ad_gift_widget.png` | res | 1068 |
| 1202 | `res/drawable/bigo_ad_gradual_background.xml` | res | 600 |
| 1203 | `res/drawable/bigo_ad_heart_widget.png` | res | 1097 |
| 1204 | `res/drawable/bigo_ad_ic_ad_label.png` | res | 1492 |
| 1205 | `res/drawable/bigo_ad_ic_back_gray.png` | res | 221 |
| 1206 | `res/drawable/bigo_ad_ic_close.png` | res | 436 |
| 1207 | `res/drawable/bigo_ad_ic_close2.png` | res | 248 |
| 1208 | `res/drawable/bigo_ad_ic_close3.png` | res | 257 |
| 1209 | `res/drawable/bigo_ad_ic_close4.png` | res | 438 |
| 1210 | `res/drawable/bigo_ad_ic_close5.png` | res | 560 |
| 1211 | `res/drawable/bigo_ad_ic_close_gray.png` | res | 401 |
| 1212 | `res/drawable/bigo_ad_ic_close_gray_light.png` | res | 1009 |
| 1213 | `res/drawable/bigo_ad_ic_download.png` | res | 584 |
| 1214 | `res/drawable/bigo_ad_ic_download_box.png` | res | 681 |
| 1215 | `res/drawable/bigo_ad_ic_download_box_white.png` | res | 604 |
| 1216 | `res/drawable/bigo_ad_ic_everyone.png` | res | 1106 |
| 1217 | `res/drawable/bigo_ad_ic_everyone_ic_info.png` | res | 1168 |
| 1218 | `res/drawable/bigo_ad_ic_everyone_white.png` | res | 908 |
| 1219 | `res/drawable/bigo_ad_ic_info.png` | res | 1168 |
| 1220 | `res/drawable/bigo_ad_ic_info_gray.png` | res | 1168 |
| 1221 | `res/drawable/bigo_ad_ic_info_popup.png` | res | 232 |
| 1222 | `res/drawable/bigo_ad_ic_info_white.png` | res | 504 |
| 1223 | `res/drawable/bigo_ad_ic_left_round_black.xml` | res | 640 |
| 1224 | `res/drawable/bigo_ad_ic_left_round_white.xml` | res | 640 |
| 1225 | `res/drawable/bigo_ad_ic_loading_gp.png` | res | 1829 |
| 1226 | `res/drawable/bigo_ad_ic_media_mute.png` | res | 637 |
| 1227 | `res/drawable/bigo_ad_ic_media_pause.png` | res | 1145 |
| 1228 | `res/drawable/bigo_ad_ic_media_play.png` | res | 1267 |
| 1229 | `res/drawable/bigo_ad_ic_media_unmute.png` | res | 558 |
| 1230 | `res/drawable/bigo_ad_ic_phone.png` | res | 462 |
| 1231 | `res/drawable/bigo_ad_ic_phone_white.png` | res | 376 |
| 1232 | `res/drawable/bigo_ad_ic_recommend.png` | res | 1080 |
| 1233 | `res/drawable/bigo_ad_ic_recommend_white.png` | res | 467 |
| 1234 | `res/drawable/bigo_ad_ic_right_black.png` | res | 528 |
| 1235 | `res/drawable/bigo_ad_ic_right_round_black.png` | res | 616 |
| 1236 | `res/drawable/bigo_ad_ic_right_round_white.png` | res | 633 |
| 1237 | `res/drawable/bigo_ad_ic_right_white.png` | res | 201 |
| 1238 | `res/drawable/bigo_ad_ic_safe.png` | res | 442 |
| 1239 | `res/drawable/bigo_ad_ic_star.png` | res | 876 |
| 1240 | `res/drawable/bigo_ad_ic_star2.png` | res | 705 |
| 1241 | `res/drawable/bigo_ad_ic_star2_white.png` | res | 554 |
| 1242 | `res/drawable/bigo_ad_ic_star_half.png` | res | 711 |
| 1243 | `res/drawable/bigo_ad_ic_star_half_white.png` | res | 445 |
| 1244 | `res/drawable/bigo_ad_ic_star_normal.png` | res | 700 |
| 1245 | `res/drawable/bigo_ad_ic_star_normal_white.png` | res | 664 |
| 1246 | `res/drawable/bigo_ad_ic_star_white.png` | res | 651 |
| 1247 | `res/drawable/bigo_ad_ic_web.png` | res | 580 |
| 1248 | `res/drawable/bigo_ad_ic_web_back.xml` | res | 800 |
| 1249 | `res/drawable/bigo_ad_ic_web_back_disable.png` | res | 334 |
| 1250 | `res/drawable/bigo_ad_ic_web_copy.png` | res | 559 |
| 1251 | `res/drawable/bigo_ad_ic_web_forward.xml` | res | 800 |
| 1252 | `res/drawable/bigo_ad_ic_web_forward_enable.png` | res | 691 |
| 1253 | `res/drawable/bigo_ad_ic_web_open_browser.png` | res | 1455 |
| 1254 | `res/drawable/bigo_ad_ic_web_refresh.png` | res | 1427 |
| 1255 | `res/drawable/bigo_ad_ic_web_white.png` | res | 485 |
| 1256 | `res/drawable/bigo_ad_icon_background.xml` | res | 760 |
| 1257 | `res/drawable/bigo_ad_icon_background_dark.xml` | res | 628 |
| 1258 | `res/drawable/bigo_ad_icon_default.xml` | res | 888 |
| 1259 | `res/drawable/bigo_ad_icon_default_img.png` | res | 334 |
| 1260 | `res/drawable/bigo_ad_icon_default_only_icon.xml` | res | 484 |
| 1261 | `res/drawable/bigo_ad_icon_default_radius22.xml` | res | 888 |
| 1262 | `res/drawable/bigo_ad_icon_default_radius6.xml` | res | 888 |
| 1263 | `res/drawable/bigo_ad_icon_novideo_default.png` | res | 3648 |
| 1264 | `res/drawable/bigo_ad_interaction_animation_bg.xml` | res | 456 |
| 1265 | `res/drawable/bigo_ad_interaction_shake_arrow.png` | res | 1596 |
| 1266 | `res/drawable/bigo_ad_interaction_shake_landscape_phone.png` | res | 4139 |
| 1267 | `res/drawable/bigo_ad_interaction_shake_phone.png` | res | 4782 |
| 1268 | `res/drawable/bigo_ad_interaction_slide_hand.png` | res | 4095 |
| 1269 | `res/drawable/bigo_ad_interaction_slide_line.png` | res | 567 |
| 1270 | `res/drawable/bigo_ad_interaction_twist_arrow.png` | res | 842 |
| 1271 | `res/drawable/bigo_ad_interaction_twist_landscape_phone.png` | res | 1626 |
| 1272 | `res/drawable/bigo_ad_interaction_twist_phone.png` | res | 1502 |
| 1273 | `res/drawable/bigo_ad_interstitial_landscape_ad_info_bg.xml` | res | 420 |
| 1274 | `res/drawable/bigo_ad_interstitial_rich_video_end_landscape_mask.xml` | res | 492 |
| 1275 | `res/drawable/bigo_ad_label_advertiser_background_raduis7.xml` | res | 484 |
| 1276 | `res/drawable/bigo_ad_label_background.xml` | res | 576 |
| 1277 | `res/drawable/bigo_ad_landing_bottom_bar_bg.xml` | res | 816 |
| 1278 | `res/drawable/bigo_ad_landing_progress_bar.xml` | res | 1328 |
| 1279 | `res/drawable/bigo_ad_landing_top_bar_bg.xml` | res | 1076 |
| 1280 | `res/drawable/bigo_ad_landing_top_indicator.xml` | res | 484 |
| 1281 | `res/drawable/bigo_ad_landscape_slide_view.png` | res | 1252 |
| 1282 | `res/drawable/bigo_ad_layer_gift_color.png` | res | 180 |
| 1283 | `res/drawable/bigo_ad_layer_gift_shadow.png` | res | 730 |
| 1284 | `res/drawable/bigo_ad_layer_heart_color.png` | res | 977 |
| 1285 | `res/drawable/bigo_ad_layer_heart_shadow.png` | res | 1062 |
| 1286 | `res/drawable/bigo_ad_mid_page_ic_gp_right.png` | res | 396 |
| 1287 | `res/drawable/bigo_ad_multi_img_advertiser_background.xml` | res | 576 |
| 1288 | `res/drawable/bigo_ad_multi_img_horizontal_mask.xml` | res | 456 |
| 1289 | `res/drawable/bigo_ad_multi_img_vertical_mask.xml` | res | 456 |
| 1290 | `res/drawable/bigo_ad_popup_ad_info_background.xml` | res | 576 |
| 1291 | `res/drawable/bigo_ad_progressbar_white.xml` | res | 1036 |
| 1292 | `res/drawable/bigo_ad_ru_background.xml` | res | 576 |
| 1293 | `res/drawable/bigo_ad_slide.png` | res | 1009 |
| 1294 | `res/drawable/bigo_ad_slide_gesture.png` | res | 3125 |
| 1295 | `res/drawable/bigo_ad_splash_endpage_bg.xml` | res | 456 |
| 1296 | `res/drawable/bigo_ad_splash_endpage_slide_bg.xml` | res | 456 |
| 1297 | `res/drawable/bigo_ad_splash_header_bg.xml` | res | 748 |
| 1298 | `res/drawable/bigo_ad_star_icon.xml` | res | 592 |
| 1299 | `res/drawable/bigo_ad_success.png` | res | 6014 |
| 1300 | `res/drawable/bigo_ad_tag_background.xml` | res | 576 |
| 1301 | `res/drawable/bigo_ad_top_left_advertiser_background.xml` | res | 484 |
| 1302 | `res/drawable/bigo_ad_view_ripple.xml` | res | 516 |
| 1303 | `res/drawable/bigo_ad_warning_background_radius8.xml` | res | 484 |
| 1304 | `res/drawable/bigo_ad_web_loading_progress_bar.xml` | res | 1328 |
| 1305 | `res/drawable/bm_bg_privacy_button.xml` | res | 980 |
| 1306 | `res/drawable/bm_bg_privacy_divider.xml` | res | 480 |
| 1307 | `res/drawable/bm_bg_privacy_sheet.xml` | res | 536 |
| 1308 | `res/drawable/bm_bg_ripple_circle.xml` | res | 788 |
| 1309 | `res/drawable/bm_ic_close.xml` | res | 644 |
| 1310 | `res/drawable/browser_background_selector.xml` | res | 484 |
| 1311 | `res/drawable/browser_backward_selector.xml` | res | 484 |
| 1312 | `res/drawable/browser_bottom_button_layout_bg.xml` | res | 720 |
| 1313 | `res/drawable/browser_forward_selector.xml` | res | 484 |
| 1314 | `res/drawable/browser_progress_bar.xml` | res | 1212 |
| 1315 | `res/drawable/browser_secure_connection.xml` | res | 480 |
| 1316 | `res/drawable/browser_top_button_layout_bg.xml` | res | 720 |
| 1317 | `res/drawable/btn_checkbox_checked_mtrl.xml` | res | 2688 |
| 1318 | `res/drawable/btn_checkbox_checked_to_unchecked_mtrl_animation.xml` | res | 688 |
| 1319 | `res/drawable/btn_checkbox_unchecked_mtrl.xml` | res | 2660 |
| 1320 | `res/drawable/btn_checkbox_unchecked_to_checked_mtrl_animation.xml` | res | 688 |
| 1321 | `res/drawable/btn_radio_off_mtrl.xml` | res | 1728 |
| 1322 | `res/drawable/btn_radio_off_to_on_mtrl_animation.xml` | res | 680 |
| 1323 | `res/drawable/btn_radio_on_mtrl.xml` | res | 1656 |
| 1324 | `res/drawable/btn_radio_on_to_off_mtrl_animation.xml` | res | 680 |
| 1325 | `res/drawable/button_background.xml` | res | 592 |
| 1326 | `res/drawable/circle_progress_background.xml` | res | 372 |
| 1327 | `res/drawable/circular_shape.xml` | res | 804 |
| 1328 | `res/drawable/close_card_button.xml` | res | 564 |
| 1329 | `res/drawable/common_google_signin_btn_icon_dark.xml` | res | 588 |
| 1330 | `res/drawable/common_google_signin_btn_icon_dark_focused.xml` | res | 892 |
| 1331 | `res/drawable/common_google_signin_btn_icon_dark_normal.xml` | res | 548 |
| 1332 | `res/drawable/common_google_signin_btn_icon_disabled.xml` | res | 1132 |
| 1333 | `res/drawable/common_google_signin_btn_icon_light.xml` | res | 588 |
| 1334 | `res/drawable/common_google_signin_btn_icon_light_focused.xml` | res | 892 |
| 1335 | `res/drawable/common_google_signin_btn_icon_light_normal.xml` | res | 548 |
| 1336 | `res/drawable/common_google_signin_btn_text_dark.xml` | res | 588 |
| 1337 | `res/drawable/common_google_signin_btn_text_dark_focused.xml` | res | 892 |
| 1338 | `res/drawable/common_google_signin_btn_text_dark_normal.xml` | res | 584 |
| 1339 | `res/drawable/common_google_signin_btn_text_disabled.xml` | res | 1152 |
| 1340 | `res/drawable/common_google_signin_btn_text_light.xml` | res | 588 |
| 1341 | `res/drawable/common_google_signin_btn_text_light_focused.xml` | res | 892 |
| 1342 | `res/drawable/common_google_signin_btn_text_light_normal.xml` | res | 584 |
| 1343 | `res/drawable/cursor_drawable.xml` | res | 444 |
| 1344 | `res/drawable/debug_panel_icon_arrow_back.xml` | res | 692 |
| 1345 | `res/drawable/debug_panel_icon_arrow_navigate.xml` | res | 612 |
| 1346 | `res/drawable/debug_panel_icon_button_background.xml` | res | 304 |
| 1347 | `res/drawable/debug_panel_icon_error.xml` | res | 980 |
| 1348 | `res/drawable/debug_panel_icon_share.xml` | res | 2836 |
| 1349 | `res/drawable/debug_panel_icon_success.xml` | res | 632 |
| 1350 | `res/drawable/debug_panel_icon_warning.xml` | res | 996 |
| 1351 | `res/drawable/debug_panel_item_action_button_background.xml` | res | 776 |
| 1352 | `res/drawable/debug_panel_item_mediation_adapter_background.xml` | res | 648 |
| 1353 | `res/drawable/debug_panel_item_mediation_adapter_icon_shape.xml` | res | 448 |
| 1354 | `res/drawable/debug_panel_item_navigation_background.xml` | res | 568 |
| 1355 | `res/drawable/design_fab_background.xml` | res | 372 |
| 1356 | `res/drawable/design_ic_visibility.xml` | res | 540 |
| 1357 | `res/drawable/design_ic_visibility_off.xml` | res | 1144 |
| 1358 | `res/drawable/design_password_eye.xml` | res | 816 |
| 1359 | `res/drawable/design_snackbar_background.xml` | res | 484 |
| 1360 | `res/drawable/div_button_background.xml` | res | 340 |
| 1361 | `res/drawable/div_gallery_item_border.xml` | res | 612 |
| 1362 | `res/drawable/error_counter_background.xml` | res | 648 |
| 1363 | `res/drawable/exo_rounded_rectangle.xml` | res | 484 |
| 1364 | `res/drawable/fab_label_background.xml` | res | 700 |
| 1365 | `res/drawable/home.png` | res | 1741 |
| 1366 | `res/drawable/ia_bg_circle_overlay.xml` | res | 500 |
| 1367 | `res/drawable/ia_bg_green.xml` | res | 628 |
| 1368 | `res/drawable/ia_bg_text_overlay.xml` | res | 760 |
| 1369 | `res/drawable/ia_circle_overlay_bg.xml` | res | 516 |
| 1370 | `res/drawable/ia_fyber_clip_drawable.xml` | res | 484 |
| 1371 | `res/drawable/ia_progress_bar_drawable.xml` | res | 904 |
| 1372 | `res/drawable/ia_round_overlay_bg.xml` | res | 484 |
| 1373 | `res/drawable/ia_round_overlay_bg_fyber.xml` | res | 628 |
| 1374 | `res/drawable/ia_round_overlay_bg_with_close.xml` | res | 808 |
| 1375 | `res/drawable/ia_sel_expand_collapse.xml` | res | 488 |
| 1376 | `res/drawable/ia_sel_mute.xml` | res | 488 |
| 1377 | `res/drawable/ic_100tb.xml` | res | 4184 |
| 1378 | `res/drawable/ic_add.xml` | res | 652 |
| 1379 | `res/drawable/ic_arrow_back_black_24.xml` | res | 652 |
| 1380 | `res/drawable/ic_book.xml` | res | 1532 |
| 1381 | `res/drawable/ic_clear_black_24.xml` | res | 684 |
| 1382 | `res/drawable/ic_clock_black_24dp.xml` | res | 752 |
| 1383 | `res/drawable/ic_exit.xml` | res | 744 |
| 1384 | `res/drawable/ic_expand_less_22px.xml` | res | 840 |
| 1385 | `res/drawable/ic_expand_more_22px.xml` | res | 840 |
| 1386 | `res/drawable/ic_group_people.xml` | res | 1848 |
| 1387 | `res/drawable/ic_human.png` | res | 1068 |
| 1388 | `res/drawable/ic_info.xml` | res | 1292 |
| 1389 | `res/drawable/ic_keyboard_black_24dp.xml` | res | 852 |
| 1390 | `res/drawable/ic_launcher_foreground.xml` | res | 1572 |
| 1391 | `res/drawable/ic_layers.xml` | res | 720 |
| 1392 | `res/drawable/ic_list.xml` | res | 948 |
| 1393 | `res/drawable/ic_m3_chip_check.xml` | res | 704 |
| 1394 | `res/drawable/ic_m3_chip_checked_circle.xml` | res | 900 |
| 1395 | `res/drawable/ic_m3_chip_close.xml` | res | 648 |
| 1396 | `res/drawable/ic_map.xml` | res | 1000 |
| 1397 | `res/drawable/ic_mtrl_arrow_circle.xml` | res | 828 |
| 1398 | `res/drawable/ic_mtrl_checked_circle.xml` | res | 672 |
| 1399 | `res/drawable/ic_mtrl_chip_checked_black.xml` | res | 600 |
| 1400 | `res/drawable/ic_mtrl_chip_checked_circle.xml` | res | 940 |
| 1401 | `res/drawable/ic_mtrl_chip_close_circle.xml` | res | 808 |
| 1402 | `res/drawable/ic_person.xml` | res | 1508 |
| 1403 | `res/drawable/ic_round_close_24.xml` | res | 908 |
| 1404 | `res/drawable/ic_round_replay_24.xml` | res | 948 |
| 1405 | `res/drawable/ic_round_skip_next_24.xml` | res | 780 |
| 1406 | `res/drawable/ic_round_volume_off_24.xml` | res | 1332 |
| 1407 | `res/drawable/ic_round_volume_up_24.xml` | res | 1028 |
| 1408 | `res/drawable/ic_save.xml` | res | 1176 |
| 1409 | `res/drawable/ic_search.xml` | res | 832 |
| 1410 | `res/drawable/ic_search_black_24.xml` | res | 800 |
| 1411 | `res/drawable/ic_tune.xml` | res | 928 |
| 1412 | `res/drawable/ic_upload.xml` | res | 840 |
| 1413 | `res/drawable/info_badge.xml` | res | 956 |
| 1414 | `res/drawable/m3_avd_hide_password.xml` | res | 660 |
| 1415 | `res/drawable/m3_avd_show_password.xml` | res | 660 |
| 1416 | `res/drawable/m3_bottom_sheet_drag_handle.xml` | res | 628 |
| 1417 | `res/drawable/m3_password_eye.xml` | res | 816 |
| 1418 | `res/drawable/m3_popupmenu_background_overlay.xml` | res | 740 |
| 1419 | `res/drawable/m3_split_button_chevron_avd.xml` | res | 1136 |
| 1420 | `res/drawable/m3_tabs_line_indicator.xml` | res | 480 |
| 1421 | `res/drawable/m3_tabs_rounded_line_indicator.xml` | res | 956 |
| 1422 | `res/drawable/material_cursor_drawable.xml` | res | 588 |
| 1423 | `res/drawable/material_ic_calendar_black_24dp.xml` | res | 696 |
| 1424 | `res/drawable/material_ic_clear_black_24dp.xml` | res | 752 |
| 1425 | `res/drawable/material_ic_edit_black_24dp.xml` | res | 716 |
| 1426 | `res/drawable/material_ic_keyboard_arrow_left_black_24dp.xml` | res | 712 |
| 1427 | `res/drawable/material_ic_keyboard_arrow_right_black_24dp.xml` | res | 700 |
| 1428 | `res/drawable/material_ic_menu_arrow_down_black_24dp.xml` | res | 648 |
| 1429 | `res/drawable/material_ic_menu_arrow_up_black_24dp.xml` | res | 648 |
| 1430 | `res/drawable/mbridge_arrow_down_white.png` | res | 284 |
| 1431 | `res/drawable/mbridge_arrow_down_white_blackbg.png` | res | 1536 |
| 1432 | `res/drawable/mbridge_arrow_up_black.png` | res | 373 |
| 1433 | `res/drawable/mbridge_arrow_up_white.png` | res | 291 |
| 1434 | `res/drawable/mbridge_cm_alertview_bg.xml` | res | 484 |
| 1435 | `res/drawable/mbridge_cm_alertview_cancel_bg.xml` | res | 484 |
| 1436 | `res/drawable/mbridge_cm_alertview_cancel_bg_nor.xml` | res | 628 |
| 1437 | `res/drawable/mbridge_cm_alertview_cancel_bg_pressed.xml` | res | 628 |
| 1438 | `res/drawable/mbridge_cm_alertview_confirm_bg.xml` | res | 484 |
| 1439 | `res/drawable/mbridge_cm_alertview_confirm_bg_nor.xml` | res | 628 |
| 1440 | `res/drawable/mbridge_cm_alertview_confirm_bg_pressed.xml` | res | 628 |
| 1441 | `res/drawable/mbridge_cm_backward.xml` | res | 840 |
| 1442 | `res/drawable/mbridge_cm_circle_50black.xml` | res | 556 |
| 1443 | `res/drawable/mbridge_cm_exits.xml` | res | 716 |
| 1444 | `res/drawable/mbridge_cm_feedback_btn_bg.xml` | res | 760 |
| 1445 | `res/drawable/mbridge_cm_feedback_choice_btn_bg.xml` | res | 484 |
| 1446 | `res/drawable/mbridge_cm_feedback_choice_btn_bg_nor.xml` | res | 484 |
| 1447 | `res/drawable/mbridge_cm_feedback_choice_btn_bg_pressed.xml` | res | 484 |
| 1448 | `res/drawable/mbridge_cm_feedback_dialog_view_bg.xml` | res | 484 |
| 1449 | `res/drawable/mbridge_cm_feedback_dialog_view_btn_bg.xml` | res | 484 |
| 1450 | `res/drawable/mbridge_cm_forward.xml` | res | 840 |
| 1451 | `res/drawable/mbridge_cm_progress_drawable.xml` | res | 1124 |
| 1452 | `res/drawable/mbridge_cm_refresh.xml` | res | 716 |
| 1453 | `res/drawable/mbridge_interstitial_over.xml` | res | 540 |
| 1454 | `res/drawable/mbridge_native_advanced_close_icon.png` | res | 1634 |
| 1455 | `res/drawable/mbridge_native_bg_loading_camera.xml` | res | 500 |
| 1456 | `res/drawable/mbridge_nativex_cta_por_nor.xml` | res | 372 |
| 1457 | `res/drawable/mbridge_nativex_cta_por_pre.xml` | res | 372 |
| 1458 | `res/drawable/mbridge_nativex_full_land_close.xml` | res | 372 |
| 1459 | `res/drawable/mbridge_nativex_full_protial_close.xml` | res | 372 |
| 1460 | `res/drawable/mbridge_nativex_fullview_background.xml` | res | 792 |
| 1461 | `res/drawable/mbridge_nativex_play_bg.xml` | res | 628 |
| 1462 | `res/drawable/mbridge_nativex_play_progress.xml` | res | 1544 |
| 1463 | `res/drawable/mbridge_nativex_sound_animation.xml` | res | 1124 |
| 1464 | `res/drawable/mbridge_nativex_sound_bg.xml` | res | 372 |
| 1465 | `res/drawable/mbridge_order_layout_list_bg.xml` | res | 564 |
| 1466 | `res/drawable/mbridge_reward_end_card_shape_bg.xml` | res | 612 |
| 1467 | `res/drawable/mbridge_reward_end_close_shape_oval.xml` | res | 372 |
| 1468 | `res/drawable/mbridge_reward_end_shape_oval.xml` | res | 372 |
| 1469 | `res/drawable/mbridge_reward_more_offer_default_bg.xml` | res | 668 |
| 1470 | `res/drawable/mbridge_reward_shape_choice.xml` | res | 540 |
| 1471 | `res/drawable/mbridge_reward_shape_choice_rl.xml` | res | 612 |
| 1472 | `res/drawable/mbridge_reward_shape_end_pager.xml` | res | 484 |
| 1473 | `res/drawable/mbridge_reward_shape_mf_selector.xml` | res | 484 |
| 1474 | `res/drawable/mbridge_reward_shape_mof_like_normal.xml` | res | 536 |
| 1475 | `res/drawable/mbridge_reward_shape_mof_like_pressed.xml` | res | 536 |
| 1476 | `res/drawable/mbridge_reward_shape_order.xml` | res | 540 |
| 1477 | `res/drawable/mbridge_reward_shape_order_history.xml` | res | 684 |
| 1478 | `res/drawable/mbridge_reward_shape_progress.xml` | res | 412 |
| 1479 | `res/drawable/mbridge_reward_shape_videoend_buttonbg.xml` | res | 676 |
| 1480 | `res/drawable/mbridge_reward_video_progress_bg.xml` | res | 484 |
| 1481 | `res/drawable/mbridge_reward_video_progressbar_bg.xml` | res | 1000 |
| 1482 | `res/drawable/mbridge_reward_video_time_count_num_bg.xml` | res | 484 |
| 1483 | `res/drawable/mbridge_shape_btn.xml` | res | 724 |
| 1484 | `res/drawable/mbridge_shape_corners_bg.xml` | res | 484 |
| 1485 | `res/drawable/mbridge_shape_line.xml` | res | 792 |
| 1486 | `res/drawable/mbridge_shape_splash_circle_14.xml` | res | 484 |
| 1487 | `res/drawable/mbridge_shape_splash_corners_14.xml` | res | 484 |
| 1488 | `res/drawable/mbridge_shape_splash_rightbottom_corners_10.xml` | res | 632 |
| 1489 | `res/drawable/mbridge_splash_button_bg_gray.xml` | res | 760 |
| 1490 | `res/drawable/mbridge_splash_button_bg_gray_55.xml` | res | 760 |
| 1491 | `res/drawable/mbridge_splash_button_bg_green.xml` | res | 632 |
| 1492 | `res/drawable/mbridge_splash_close_bg.xml` | res | 760 |
| 1493 | `res/drawable/moloco_close.xml` | res | 760 |
| 1494 | `res/drawable/moloco_replay.xml` | res | 972 |
| 1495 | `res/drawable/moloco_star.xml` | res | 668 |
| 1496 | `res/drawable/moloco_twotone_pause_24.xml` | res | 612 |
| 1497 | `res/drawable/moloco_twotone_play_arrow_24.xml` | res | 848 |
| 1498 | `res/drawable/moloco_twotone_volume_off_24.xml` | res | 1288 |
| 1499 | `res/drawable/moloco_twotone_volume_up_24.xml` | res | 1072 |
| 1500 | `res/drawable/moloco_volume_off.xml` | res | 980 |
| 1501 | `res/drawable/moloco_volume_on.xml` | res | 1112 |
| 1502 | `res/drawable/monetization_ads_instream_internal_advertiser.xml` | res | 2748 |
| 1503 | `res/drawable/monetization_ads_instream_internal_advertiser_social.xml` | res | 12640 |
| 1504 | `res/drawable/monetization_ads_instream_internal_click_button.xml` | res | 5176 |
| 1505 | `res/drawable/monetization_ads_instream_internal_progress_display.xml` | res | 588 |
| 1506 | `res/drawable/monetization_ads_instream_internal_progress_display_v2.xml` | res | 588 |
| 1507 | `res/drawable/monetization_ads_instream_internal_skip.xml` | res | 484 |
| 1508 | `res/drawable/monetization_ads_instream_internal_skip_focused.xml` | res | 4500 |
| 1509 | `res/drawable/monetization_ads_instream_internal_skip_unfocused.xml` | res | 4500 |
| 1510 | `res/drawable/monetization_ads_internal_adtune_control_shape_white.xml` | res | 520 |
| 1511 | `res/drawable/monetization_ads_internal_adtune_webview_container_shape_white.xml` | res | 572 |
| 1512 | `res/drawable/monetization_ads_internal_adtune_white.xml` | res | 1284 |
| 1513 | `res/drawable/monetization_ads_internal_adtune_white_background.xml` | res | 484 |
| 1514 | `res/drawable/monetization_ads_internal_app_install_circle_for_dark_background.xml` | res | 516 |
| 1515 | `res/drawable/monetization_ads_internal_app_install_circle_for_light_background.xml` | res | 516 |
| 1516 | `res/drawable/monetization_ads_internal_app_install_close_grey.xml` | res | 1644 |
| 1517 | `res/drawable/monetization_ads_internal_app_install_close_white.xml` | res | 2888 |
| 1518 | `res/drawable/monetization_ads_internal_app_install_feedback_grey.xml` | res | 1228 |
| 1519 | `res/drawable/monetization_ads_internal_app_install_feedback_white.xml` | res | 2096 |
| 1520 | `res/drawable/monetization_ads_internal_call_to_action_background.xml` | res | 424 |
| 1521 | `res/drawable/monetization_ads_internal_call_to_action_background_dark.xml` | res | 376 |
| 1522 | `res/drawable/monetization_ads_internal_call_to_action_background_light.xml` | res | 376 |
| 1523 | `res/drawable/monetization_ads_internal_call_to_action_background_shape_blue.xml` | res | 520 |
| 1524 | `res/drawable/monetization_ads_internal_call_to_action_blue_solid_background.xml` | res | 484 |
| 1525 | `res/drawable/monetization_ads_internal_call_to_action_blue_stroke_background.xml` | res | 612 |
| 1526 | `res/drawable/monetization_ads_internal_call_to_action_gradient_yellow_background.xml` | res | 600 |
| 1527 | `res/drawable/monetization_ads_internal_call_to_action_green_solid_background.xml` | res | 484 |
| 1528 | `res/drawable/monetization_ads_internal_circle_progress_bar.xml` | res | 960 |
| 1529 | `res/drawable/monetization_ads_internal_circular_close_progress.xml` | res | 692 |
| 1530 | `res/drawable/monetization_ads_internal_close_light.xml` | res | 648 |
| 1531 | `res/drawable/monetization_ads_internal_controls_background.xml` | res | 492 |
| 1532 | `res/drawable/monetization_ads_internal_default_adtune_feedback_icon.xml` | res | 1176 |
| 1533 | `res/drawable/monetization_ads_internal_feed_progress.xml` | res | 964 |
| 1534 | `res/drawable/monetization_ads_internal_fullscreen_content_v1_background_tile_dark.xml` | res | 620 |
| 1535 | `res/drawable/monetization_ads_internal_fullscreen_content_v1_close_disabled.xml` | res | 908 |
| 1536 | `res/drawable/monetization_ads_internal_fullscreen_content_v1_close_enabled.xml` | res | 908 |
| 1537 | `res/drawable/monetization_ads_internal_fullscreen_content_v1_close_selector.xml` | res | 840 |
| 1538 | `res/drawable/monetization_ads_internal_fullscreen_content_v1_icon_background.xml` | res | 484 |
| 1539 | `res/drawable/monetization_ads_internal_google_play_icon_white.xml` | res | 920 |
| 1540 | `res/drawable/monetization_ads_internal_ic_close_gray.xml` | res | 804 |
| 1541 | `res/drawable/monetization_ads_internal_ic_sound_off_app_install.xml` | res | 2452 |
| 1542 | `res/drawable/monetization_ads_internal_ic_sound_off_default.xml` | res | 484 |
| 1543 | `res/drawable/monetization_ads_internal_ic_sound_off_default_focused.xml` | res | 1608 |
| 1544 | `res/drawable/monetization_ads_internal_ic_sound_off_default_unfocused.xml` | res | 1608 |
| 1545 | `res/drawable/monetization_ads_internal_ic_sound_on_app_install.xml` | res | 4808 |
| 1546 | `res/drawable/monetization_ads_internal_ic_sound_on_default.xml` | res | 484 |
| 1547 | `res/drawable/monetization_ads_internal_ic_sound_on_default_focused.xml` | res | 1452 |
| 1548 | `res/drawable/monetization_ads_internal_ic_sound_on_default_unfocused.xml` | res | 1452 |
| 1549 | `res/drawable/monetization_ads_internal_icon_empty.xml` | res | 1516 |
| 1550 | `res/drawable/monetization_ads_internal_icon_grey_solid_background_large_corners.xml` | res | 484 |
| 1551 | `res/drawable/monetization_ads_internal_icon_grey_solid_background_small_corners.xml` | res | 484 |
| 1552 | `res/drawable/monetization_ads_internal_instream_adtune_control_v2.xml` | res | 484 |
| 1553 | `res/drawable/monetization_ads_internal_instream_adtune_control_v2_focused.xml` | res | 1640 |
| 1554 | `res/drawable/monetization_ads_internal_instream_adtune_control_v2_unfocused.xml` | res | 1640 |
| 1555 | `res/drawable/monetization_ads_internal_instream_advertiser_block_background.xml` | res | 484 |
| 1556 | `res/drawable/monetization_ads_internal_instream_advertiser_block_background_focused.xml` | res | 484 |
| 1557 | `res/drawable/monetization_ads_internal_instream_advertiser_block_background_unfocused.xml` | res | 484 |
| 1558 | `res/drawable/monetization_ads_internal_instream_call_to_action_background.xml` | res | 484 |
| 1559 | `res/drawable/monetization_ads_internal_instream_ic_sound_off_v2.xml` | res | 484 |
| 1560 | `res/drawable/monetization_ads_internal_instream_ic_sound_on_v2.xml` | res | 484 |
| 1561 | `res/drawable/monetization_ads_internal_instream_mute_checkable_v2.xml` | res | 484 |
| 1562 | `res/drawable/monetization_ads_internal_instream_skip_arrow.xml` | res | 876 |
| 1563 | `res/drawable/monetization_ads_internal_landscape_close_button.xml` | res | 1356 |
| 1564 | `res/drawable/monetization_ads_internal_multibanner_left_button_icon.xml` | res | 936 |
| 1565 | `res/drawable/monetization_ads_internal_multibanner_right_button_icon.xml` | res | 936 |
| 1566 | `res/drawable/monetization_ads_internal_mute_checkable_app_install.xml` | res | 484 |
| 1567 | `res/drawable/monetization_ads_internal_mute_checkable_default.xml` | res | 484 |
| 1568 | `res/drawable/monetization_ads_internal_mute_checkable_fullscreen_content_v1.xml` | res | 860 |
| 1569 | `res/drawable/monetization_ads_internal_qrcode_container_background.xml` | res | 484 |
| 1570 | `res/drawable/monetization_ads_internal_qrcode_view_background.xml` | res | 484 |
| 1571 | `res/drawable/monetization_ads_internal_rating_bar.xml` | res | 576 |
| 1572 | `res/drawable/monetization_ads_internal_reward_timer_view_shape_gray.xml` | res | 412 |
| 1573 | `res/drawable/monetization_ads_internal_rewarded_close.xml` | res | 1676 |
| 1574 | `res/drawable/monetization_ads_internal_rewarded_close_shape_gray.xml` | res | 412 |
| 1575 | `res/drawable/monetization_ads_internal_rewarded_close_verification_background.xml` | res | 484 |
| 1576 | `res/drawable/monetization_ads_internal_rewarded_close_verification_button_close_background.xml` | res | 484 |
| 1577 | `res/drawable/monetization_ads_internal_rewarded_close_verification_button_dismiss_background.xml` | res | 484 |
| 1578 | `res/drawable/monetization_ads_internal_rewarded_close_with_padding.xml` | res | 484 |
| 1579 | `res/drawable/monetization_ads_internal_rewarded_feedback.xml` | res | 1180 |
| 1580 | `res/drawable/monetization_ads_internal_rewarded_feedback_with_padding.xml` | res | 484 |
| 1581 | `res/drawable/monetization_ads_internal_rewarded_timer_background.xml` | res | 516 |
| 1582 | `res/drawable/monetization_ads_internal_sponsored_container_background.xml` | res | 484 |
| 1583 | `res/drawable/monetization_ads_internal_star_black.xml` | res | 804 |
| 1584 | `res/drawable/monetization_ads_internal_star_half_black.xml` | res | 804 |
| 1585 | `res/drawable/monetization_ads_mediation_api_feedback_icon.xml` | res | 1176 |
| 1586 | `res/drawable/monetization_ads_progress_view_background.xml` | res | 484 |
| 1587 | `res/drawable/monetization_ads_video_progress_bar_background.xml` | res | 908 |
| 1588 | `res/drawable/mtrl_bottomsheet_drag_handle.xml` | res | 628 |
| 1589 | `res/drawable/mtrl_checkbox_button.xml` | res | 1332 |
| 1590 | `res/drawable/mtrl_checkbox_button_checked_unchecked.xml` | res | 724 |
| 1591 | `res/drawable/mtrl_checkbox_button_icon.xml` | res | 1572 |
| 1592 | `res/drawable/mtrl_checkbox_button_icon_checked_indeterminate.xml` | res | 524 |
| 1593 | `res/drawable/mtrl_checkbox_button_icon_checked_unchecked.xml` | res | 724 |
| 1594 | `res/drawable/mtrl_checkbox_button_icon_indeterminate_checked.xml` | res | 524 |
| 1595 | `res/drawable/mtrl_checkbox_button_icon_indeterminate_unchecked.xml` | res | 724 |
| 1596 | `res/drawable/mtrl_checkbox_button_icon_unchecked_checked.xml` | res | 724 |
| 1597 | `res/drawable/mtrl_checkbox_button_icon_unchecked_indeterminate.xml` | res | 724 |
| 1598 | `res/drawable/mtrl_checkbox_button_unchecked_checked.xml` | res | 724 |
| 1599 | `res/drawable/mtrl_dialog_background.xml` | res | 716 |
| 1600 | `res/drawable/mtrl_dropdown_arrow.xml` | res | 464 |
| 1601 | `res/drawable/mtrl_ic_arrow_drop_down.xml` | res | 564 |
| 1602 | `res/drawable/mtrl_ic_arrow_drop_up.xml` | res | 564 |
| 1603 | `res/drawable/mtrl_ic_cancel.xml` | res | 724 |
| 1604 | `res/drawable/mtrl_ic_check_mark.xml` | res | 744 |
| 1605 | `res/drawable/mtrl_ic_checkbox_checked.xml` | res | 904 |
| 1606 | `res/drawable/mtrl_ic_checkbox_unchecked.xml` | res | 612 |
| 1607 | `res/drawable/mtrl_ic_error.xml` | res | 644 |
| 1608 | `res/drawable/mtrl_ic_indeterminate.xml` | res | 744 |
| 1609 | `res/drawable/mtrl_navigation_bar_item_background.xml` | res | 264 |
| 1610 | `res/drawable/mtrl_popupmenu_background.xml` | res | 740 |
| 1611 | `res/drawable/mtrl_switch_thumb.xml` | res | 2004 |
| 1612 | `res/drawable/mtrl_switch_thumb_checked.xml` | res | 744 |
| 1613 | `res/drawable/mtrl_switch_thumb_checked_pressed.xml` | res | 524 |
| 1614 | `res/drawable/mtrl_switch_thumb_checked_unchecked.xml` | res | 784 |
| 1615 | `res/drawable/mtrl_switch_thumb_pressed.xml` | res | 744 |
| 1616 | `res/drawable/mtrl_switch_thumb_pressed_checked.xml` | res | 524 |
| 1617 | `res/drawable/mtrl_switch_thumb_pressed_unchecked.xml` | res | 524 |
| 1618 | `res/drawable/mtrl_switch_thumb_unchecked.xml` | res | 744 |
| 1619 | `res/drawable/mtrl_switch_thumb_unchecked_checked.xml` | res | 784 |
| 1620 | `res/drawable/mtrl_switch_thumb_unchecked_pressed.xml` | res | 524 |
| 1621 | `res/drawable/mtrl_switch_track.xml` | res | 588 |
| 1622 | `res/drawable/mtrl_switch_track_decoration.xml` | res | 636 |
| 1623 | `res/drawable/mtrl_tabs_default_indicator.xml` | res | 628 |
| 1624 | `res/drawable/native_animation_background.xml` | res | 568 |
| 1625 | `res/drawable/navigation_empty_icon.xml` | res | 516 |
| 1626 | `res/drawable/nbt_byte.png` | res | 428 |
| 1627 | `res/drawable/nbt_byte_arr.png` | res | 412 |
| 1628 | `res/drawable/nbt_compound.png` | res | 1097 |
| 1629 | `res/drawable/nbt_double.png` | res | 388 |
| 1630 | `res/drawable/nbt_float.png` | res | 391 |
| 1631 | `res/drawable/nbt_int.png` | res | 390 |
| 1632 | `res/drawable/nbt_int_arr.png` | res | 410 |
| 1633 | `res/drawable/nbt_list.png` | res | 396 |
| 1634 | `res/drawable/nbt_long.png` | res | 386 |
| 1635 | `res/drawable/nbt_short.png` | res | 397 |
| 1636 | `res/drawable/nbt_string.png` | res | 445 |
| 1637 | `res/drawable/neutral_counter_background.xml` | res | 648 |
| 1638 | `res/drawable/notification_bg.xml` | res | 532 |
| 1639 | `res/drawable/notification_bg_low.xml` | res | 532 |
| 1640 | `res/drawable/notification_icon_background.xml` | res | 372 |
| 1641 | `res/drawable/notification_tile_bg.xml` | res | 304 |
| 1642 | `res/drawable/offline_dialog_background.xml` | res | 448 |
| 1643 | `res/drawable/offline_dialog_default_icon_42dp.xml` | res | 1216 |
| 1644 | `res/drawable/open_url_background.xml` | res | 484 |
| 1645 | `res/drawable/pn_circular_progress.xml` | res | 916 |
| 1646 | `res/drawable/pob_close_button.png` | res | 1014 |
| 1647 | `res/drawable/pob_ic_action_back.png` | res | 320 |
| 1648 | `res/drawable/pob_ic_action_cancel.png` | res | 323 |
| 1649 | `res/drawable/pob_ic_action_forward.png` | res | 333 |
| 1650 | `res/drawable/pob_ic_action_refresh.png` | res | 717 |
| 1651 | `res/drawable/pob_ic_action_web_site.png` | res | 1276 |
| 1652 | `res/drawable/pob_install_btn_drawable.xml` | res | 612 |
| 1653 | `res/drawable/pob_template_button_drawable.xml` | res | 592 |
| 1654 | `res/drawable/question_mark.png` | res | 2583 |
| 1655 | `res/drawable/rab_action_bg.xml` | res | 916 |
| 1656 | `res/drawable/rab_badge_bg.xml` | res | 484 |
| 1657 | `res/drawable/round_18_ec3a48.xml` | res | 448 |
| 1658 | `res/drawable/round_7_white_bg.xml` | res | 448 |
| 1659 | `res/drawable/round_80000000_100.xml` | res | 448 |
| 1660 | `res/drawable/round_80000000_3.xml` | res | 448 |
| 1661 | `res/drawable/round_9_black_bg.xml` | res | 448 |
| 1662 | `res/drawable/round_b3000000.xml` | res | 448 |
| 1663 | `res/drawable/round_b4001b.xml` | res | 448 |
| 1664 | `res/drawable/round_bottom_b2c4ff00.xml` | res | 504 |
| 1665 | `res/drawable/round_edc900_100.xml` | res | 448 |
| 1666 | `res/drawable/round_edc900_3.xml` | res | 448 |
| 1667 | `res/drawable/round_ff81a700.xml` | res | 448 |
| 1668 | `res/drawable/round_ff81a700_100.xml` | res | 448 |
| 1669 | `res/drawable/round_gray.xml` | res | 448 |
| 1670 | `res/drawable/round_white_bg.xml` | res | 448 |
| 1671 | `res/drawable/seekbar_progress_drawable.xml` | res | 948 |
| 1672 | `res/drawable/sentry_edit_text_border.xml` | res | 828 |
| 1673 | `res/drawable/sentry_logo_dark.xml` | res | 1124 |
| 1674 | `res/drawable/sentry_oval_button_ripple_background.xml` | res | 648 |
| 1675 | `res/drawable/sentry_user_feedback_button_logo_24.xml` | res | 1012 |
| 1676 | `res/drawable/smaato_sdk_core_browser_bottom_button_layout_bg.xml` | res | 720 |
| 1677 | `res/drawable/smaato_sdk_core_browser_progress_bar.xml` | res | 1212 |
| 1678 | `res/drawable/smaato_sdk_core_browser_top_button_layout_bg.xml` | res | 720 |
| 1679 | `res/drawable/smaato_sdk_core_count_down_bg.xml` | res | 516 |
| 1680 | `res/drawable/smaato_sdk_core_ic_browser_background_selector.xml` | res | 484 |
| 1681 | `res/drawable/smaato_sdk_core_ic_browser_backward_selector.xml` | res | 484 |
| 1682 | `res/drawable/smaato_sdk_core_ic_browser_forward_selector.xml` | res | 484 |
| 1683 | `res/drawable/smaato_sdk_core_ic_browser_secure_connection.xml` | res | 480 |
| 1684 | `res/drawable/smaato_sdk_core_progress_bar.xml` | res | 948 |
| 1685 | `res/drawable/smaato_sdk_core_video_progress_bar.xml` | res | 1172 |
| 1686 | `res/drawable/step1_tut.png` | res | 44322 |
| 1687 | `res/drawable/step2_tut.png` | res | 2070 |
| 1688 | `res/drawable/step3_tut.png` | res | 1595 |
| 1689 | `res/drawable/step4_tut.png` | res | 27602 |
| 1690 | `res/drawable/steve.png` | res | 10887 |
| 1691 | `res/drawable/table_cell_background.xml` | res | 372 |
| 1692 | `res/drawable/taurusx_ic_privacy.png` | res | 673 |
| 1693 | `res/drawable/taurusx_inner_video_mute.png` | res | 1589 |
| 1694 | `res/drawable/taurusx_inner_video_no_mute.png` | res | 1627 |
| 1695 | `res/drawable/taurusx_progress_rotate.xml` | res | 424 |
| 1696 | `res/drawable/taurusx_tax_btn_dark_bg.xml` | res | 372 |
| 1697 | `res/drawable/taurusx_tax_round_36_bg.xml` | res | 448 |
| 1698 | `res/drawable/taurusx_tax_round_36_bg2.xml` | res | 448 |
| 1699 | `res/drawable/taurusx_tax_round_white_bg.xml` | res | 448 |
| 1700 | `res/drawable/taurusx_tax_volume_bg.xml` | res | 468 |
| 1701 | `res/drawable/test_level_drawable.xml` | res | 448 |
| 1702 | `res/drawable/tile.png` | res | 134 |
| 1703 | `res/drawable/tiles.xml` | res | 304 |
| 1704 | `res/drawable/timer_count_down_background.xml` | res | 484 |
| 1705 | `res/drawable/tooltip_frame_dark.xml` | res | 484 |
| 1706 | `res/drawable/tooltip_frame_light.xml` | res | 484 |
| 1707 | `res/drawable/tt_app_open_top_bg.xml` | res | 576 |
| 1708 | `res/drawable/tt_button_back.xml` | res | 612 |
| 1709 | `res/drawable/tt_button_blue_back.xml` | res | 612 |
| 1710 | `res/drawable/tt_ugen_rating_star.png` | res | 869 |
| 1711 | `res/drawable/tt_user_info.xml` | res | 484 |
| 1712 | `res/drawable/warning_counter_background.xml` | res | 648 |
| 1713 | `res/drawable/warning_error_counter_background.xml` | res | 1032 |
| 1714 | `res/font/inter.xml` | res | 500 |
| 1715 | `res/font/monetization_ads_internal_font_bold.xml` | res | 640 |
| 1716 | `res/font/monetization_ads_internal_font_light.xml` | res | 640 |
| 1717 | `res/font/monetization_ads_internal_font_medium.xml` | res | 640 |
| 1718 | `res/font/monetization_ads_internal_font_regular.xml` | res | 820 |
| 1719 | `res/font/monetization_ads_internal_font_text_bold.ttf` | res | 193796 |
| 1720 | `res/font/monetization_ads_internal_font_text_light.ttf` | res | 193752 |
| 1721 | `res/font/monetization_ads_internal_font_text_medium.ttf` | res | 193976 |
| 1722 | `res/font/monetization_ads_internal_font_text_regular.ttf` | res | 192536 |
| 1723 | `res/font/monetization_ads_internal_font_text_regular_italic.ttf` | res | 198420 |
| 1724 | `res/font/roboto_medium_numbers.ttf` | res | 3316 |
| 1725 | `res/interpolator/btn_checkbox_checked_mtrl_animation_interpolator_0.xml` | res | 316 |
| 1726 | `res/interpolator/btn_checkbox_checked_mtrl_animation_interpolator_1.xml` | res | 328 |
| 1727 | `res/interpolator/btn_checkbox_unchecked_mtrl_animation_interpolator_0.xml` | res | 316 |
| 1728 | `res/interpolator/btn_checkbox_unchecked_mtrl_animation_interpolator_1.xml` | res | 328 |
| 1729 | `res/interpolator/btn_radio_to_off_mtrl_animation_interpolator_0.xml` | res | 320 |
| 1730 | `res/interpolator/btn_radio_to_on_mtrl_animation_interpolator_0.xml` | res | 320 |
| 1731 | `res/interpolator/fast_out_slow_in.xml` | res | 400 |
| 1732 | `res/interpolator/m3_sys_motion_easing_emphasized.xml` | res | 280 |
| 1733 | `res/interpolator/m3_sys_motion_easing_emphasized_accelerate.xml` | res | 400 |
| 1734 | `res/interpolator/m3_sys_motion_easing_emphasized_decelerate.xml` | res | 400 |
| 1735 | `res/interpolator/m3_sys_motion_easing_linear.xml` | res | 400 |
| 1736 | `res/interpolator/m3_sys_motion_easing_standard.xml` | res | 400 |
| 1737 | `res/interpolator/m3_sys_motion_easing_standard_accelerate.xml` | res | 400 |
| 1738 | `res/interpolator/m3_sys_motion_easing_standard_decelerate.xml` | res | 400 |
| 1739 | `res/interpolator/mtrl_fast_out_linear_in.xml` | res | 400 |
| 1740 | `res/interpolator/mtrl_fast_out_slow_in.xml` | res | 400 |
| 1741 | `res/interpolator/mtrl_linear.xml` | res | 132 |
| 1742 | `res/interpolator/mtrl_linear_out_slow_in.xml` | res | 400 |
| 1743 | `res/layout-land/instream_layout_detail_pause_overlay.xml` | res | 5268 |
| 1744 | `res/layout-land/instream_layout_detail_preroll.xml` | res | 5832 |
| 1745 | `res/layout-land/material_clock_period_toggle_land.xml` | res | 1232 |
| 1746 | `res/layout-land/material_timepicker.xml` | res | 1676 |
| 1747 | `res/layout-land/mtrl_picker_header_dialog.xml` | res | 1588 |
| 1748 | `res/layout-land/simpledialogfragment_color_wheel.xml` | res | 4516 |
| 1749 | `res/layout-land/taurusx_activity_fullscreen.xml` | res | 7996 |
| 1750 | `res/layout-sw600dp-v13/design_layout_snackbar.xml` | res | 528 |
| 1751 | `res/layout-sw600dp-v13/ia_layout_fullscreen_activity.xml` | res | 1756 |
| 1752 | `res/layout-sw600dp-v13/ia_layout_rich_media_video.xml` | res | 1152 |
| 1753 | `res/layout-sw600dp-v13/mtrl_layout_snackbar.xml` | res | 492 |
| 1754 | `res/layout-v21/notification_action.xml` | res | 1052 |
| 1755 | `res/layout-v21/notification_action_tombstone.xml` | res | 1228 |
| 1756 | `res/layout-v21/notification_template_custom_big.xml` | res | 2456 |
| 1757 | `res/layout-v21/notification_template_icon_group.xml` | res | 988 |
| 1758 | `res/layout-v23/exo_player_control_ffwd_button.xml` | res | 408 |
| 1759 | `res/layout-v23/exo_player_control_rewind_button.xml` | res | 408 |
| 1760 | `res/layout-v23/exo_styled_player_control_ffwd_button.xml` | res | 368 |
| 1761 | `res/layout-v23/exo_styled_player_control_rewind_button.xml` | res | 368 |
| 1762 | `res/layout-v26/abc_screen_toolbar.xml` | res | 1560 |
| 1763 | `res/layout-v26/mtrl_calendar_month.xml` | res | 744 |
| 1764 | `res/layout-watch-v20/abc_alert_dialog_button_bar_material.xml` | res | 1208 |
| 1765 | `res/layout-watch-v20/abc_alert_dialog_title_material.xml` | res | 1352 |
| 1766 | `res/layout/abc_action_bar_title_item.xml` | res | 872 |
| 1767 | `res/layout/abc_action_bar_up_container.xml` | res | 440 |
| 1768 | `res/layout/abc_action_menu_item_layout.xml` | res | 768 |
| 1769 | `res/layout/abc_action_menu_layout.xml` | res | 576 |
| 1770 | `res/layout/abc_action_mode_bar.xml` | res | 464 |
| 1771 | `res/layout/abc_action_mode_close_item_material.xml` | res | 840 |
| 1772 | `res/layout/abc_activity_chooser_view.xml` | res | 1684 |
| 1773 | `res/layout/abc_activity_chooser_view_list_item.xml` | res | 1304 |
| 1774 | `res/layout/abc_alert_dialog_button_bar_material.xml` | res | 1584 |
| 1775 | `res/layout/abc_alert_dialog_material.xml` | res | 2648 |
| 1776 | `res/layout/abc_alert_dialog_title_material.xml` | res | 1560 |
| 1777 | `res/layout/abc_cascading_menu_item_layout.xml` | res | 1868 |
| 1778 | `res/layout/abc_dialog_title_material.xml` | res | 1116 |
| 1779 | `res/layout/abc_expanded_menu_layout.xml` | res | 388 |
| 1780 | `res/layout/abc_list_menu_item_checkbox.xml` | res | 528 |
| 1781 | `res/layout/abc_list_menu_item_icon.xml` | res | 780 |
| 1782 | `res/layout/abc_list_menu_item_layout.xml` | res | 1396 |
| 1783 | `res/layout/abc_list_menu_item_radio.xml` | res | 532 |
| 1784 | `res/layout/abc_popup_menu_header_item_layout.xml` | res | 848 |
| 1785 | `res/layout/abc_popup_menu_item_layout.xml` | res | 2072 |
| 1786 | `res/layout/abc_screen_content_include.xml` | res | 548 |
| 1787 | `res/layout/abc_screen_simple.xml` | res | 832 |
| 1788 | `res/layout/abc_screen_simple_overlay_action_mode.xml` | res | 792 |
| 1789 | `res/layout/abc_search_dropdown_item_icons_2line.xml` | res | 1916 |
| 1790 | `res/layout/abc_search_view.xml` | res | 3472 |
| 1791 | `res/layout/abc_select_dialog_material.xml` | res | 1020 |
| 1792 | `res/layout/abc_tooltip.xml` | res | 1056 |
| 1793 | `res/layout/activity_create_world.xml` | res | 3100 |
| 1794 | `res/layout/activity_debug.xml` | res | 1280 |
| 1795 | `res/layout/activity_drawer.xml` | res | 1568 |
| 1796 | `res/layout/activity_entities_nbt.xml` | res | 1384 |
| 1797 | `res/layout/activity_help.xml` | res | 3592 |
| 1798 | `res/layout/activity_internal_browser.xml` | res | 3860 |
| 1799 | `res/layout/activity_main.xml` | res | 3100 |
| 1800 | `res/layout/ad_mob_native_banner_template_basic_layout.xml` | res | 2652 |
| 1801 | `res/layout/ad_mob_native_banner_template_icon_text_layout.xml` | res | 2332 |
| 1802 | `res/layout/ad_mob_native_banner_template_rect_layout.xml` | res | 3012 |
| 1803 | `res/layout/ad_mob_native_banner_template_text_cta_layout.xml` | res | 2264 |
| 1804 | `res/layout/admob_empty_layout.xml` | res | 396 |
| 1805 | `res/layout/apd_admob_banner.xml` | res | 3864 |
| 1806 | `res/layout/applovin_debugger_list_item_detail.xml` | res | 2172 |
| 1807 | `res/layout/applovin_native_ad_media_view.xml` | res | 600 |
| 1808 | `res/layout/aps_interstitial_activity.xml` | res | 680 |
| 1809 | `res/layout/bigo_ad_activity_form.xml` | res | 880 |
| 1810 | `res/layout/bigo_ad_activity_interstitial.xml` | res | 1040 |
| 1811 | `res/layout/bigo_ad_activity_interstitial_banner.xml` | res | 408 |
| 1812 | `res/layout/bigo_ad_activity_interstitial_bottom_card.xml` | res | 3748 |
| 1813 | `res/layout/bigo_ad_activity_interstitial_landingpage.xml` | res | 4780 |
| 1814 | `res/layout/bigo_ad_activity_interstitial_landingpage_7_8.xml` | res | 1236 |
| 1815 | `res/layout/bigo_ad_activity_interstitial_native_center.xml` | res | 4160 |
| 1816 | `res/layout/bigo_ad_activity_interstitial_native_top.xml` | res | 3748 |
| 1817 | `res/layout/bigo_ad_activity_interstitial_percent_warning.xml` | res | 5296 |
| 1818 | `res/layout/bigo_ad_activity_interstitial_percent_warning_landscape.xml` | res | 6288 |
| 1819 | `res/layout/bigo_ad_activity_interstitial_rich_video.xml` | res | 5392 |
| 1820 | `res/layout/bigo_ad_activity_interstitial_rich_video_19_29.xml` | res | 3384 |
| 1821 | `res/layout/bigo_ad_activity_interstitial_rich_video_2.xml` | res | 5820 |
| 1822 | `res/layout/bigo_ad_activity_interstitial_rich_video_3.xml` | res | 6104 |
| 1823 | `res/layout/bigo_ad_activity_interstitial_rich_video_4.xml` | res | 5344 |
| 1824 | `res/layout/bigo_ad_activity_interstitial_rich_video_5.xml` | res | 5952 |
| 1825 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_19.xml` | res | 2100 |
| 1826 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_20.xml` | res | 1968 |
| 1827 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_21.xml` | res | 2684 |
| 1828 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_22.xml` | res | 2216 |
| 1829 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_23.xml` | res | 2864 |
| 1830 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_24.xml` | res | 2260 |
| 1831 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_25.xml` | res | 1304 |
| 1832 | `res/layout/bigo_ad_activity_interstitial_rich_video_component_26.xml` | res | 2388 |
| 1833 | `res/layout/bigo_ad_activity_interstitial_rich_video_download_10.xml` | res | 4952 |
| 1834 | `res/layout/bigo_ad_activity_interstitial_rich_video_download_11.xml` | res | 5872 |
| 1835 | `res/layout/bigo_ad_activity_interstitial_rich_video_download_12.xml` | res | 5536 |
| 1836 | `res/layout/bigo_ad_activity_interstitial_rich_video_download_6.xml` | res | 5464 |
| 1837 | `res/layout/bigo_ad_activity_interstitial_rich_video_download_7.xml` | res | 5796 |
| 1838 | `res/layout/bigo_ad_activity_interstitial_rich_video_download_8.xml` | res | 5616 |
| 1839 | `res/layout/bigo_ad_activity_interstitial_rich_video_download_9.xml` | res | 5704 |
| 1840 | `res/layout/bigo_ad_activity_interstitial_rich_video_end.xml` | res | 4676 |
| 1841 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_14_15.xml` | res | 3216 |
| 1842 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_16_17.xml` | res | 3436 |
| 1843 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_18.xml` | res | 2912 |
| 1844 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_19.xml` | res | 3836 |
| 1845 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_2.xml` | res | 5136 |
| 1846 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_3.xml` | res | 4352 |
| 1847 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_4.xml` | res | 4952 |
| 1848 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_6.xml` | res | 2648 |
| 1849 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_7.xml` | res | 3220 |
| 1850 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_8.xml` | res | 4196 |
| 1851 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_9.xml` | res | 4436 |
| 1852 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_landscape.xml` | res | 4752 |
| 1853 | `res/layout/bigo_ad_activity_interstitial_rich_video_end_landscape_2.xml` | res | 5572 |
| 1854 | `res/layout/bigo_ad_activity_interstitial_rich_video_land_material_31_32.xml` | res | 7336 |
| 1855 | `res/layout/bigo_ad_activity_interstitial_rich_video_left_material_31.xml` | res | 7036 |
| 1856 | `res/layout/bigo_ad_activity_interstitial_rich_video_multi_img_13.xml` | res | 4396 |
| 1857 | `res/layout/bigo_ad_activity_interstitial_rich_video_multi_img_14.xml` | res | 2648 |
| 1858 | `res/layout/bigo_ad_activity_interstitial_rich_video_multi_img_15.xml` | res | 3220 |
| 1859 | `res/layout/bigo_ad_activity_interstitial_rich_video_multi_img_16.xml` | res | 4196 |
| 1860 | `res/layout/bigo_ad_activity_interstitial_rich_video_multi_img_17.xml` | res | 4436 |
| 1861 | `res/layout/bigo_ad_activity_interstitial_rich_video_multi_img_item_layout.xml` | res | 1272 |
| 1862 | `res/layout/bigo_ad_activity_interstitial_rich_video_multi_img_media_layout.xml` | res | 1636 |
| 1863 | `res/layout/bigo_ad_activity_interstitial_rich_video_right_material_32.xml` | res | 7096 |
| 1864 | `res/layout/bigo_ad_activity_interstitial_sab.xml` | res | 2624 |
| 1865 | `res/layout/bigo_ad_activity_interstitial_style_landscape_1.xml` | res | 5568 |
| 1866 | `res/layout/bigo_ad_activity_interstitial_style_landscape_1_full_media.xml` | res | 5460 |
| 1867 | `res/layout/bigo_ad_activity_interstitial_style_landscape_2.xml` | res | 5916 |
| 1868 | `res/layout/bigo_ad_activity_interstitial_style_landscape_2_full_media.xml` | res | 5952 |
| 1869 | `res/layout/bigo_ad_activity_interstitial_style_landscape_3.xml` | res | 7676 |
| 1870 | `res/layout/bigo_ad_activity_interstitial_style_landscape_3_no_gp_element.xml` | res | 6872 |
| 1871 | `res/layout/bigo_ad_activity_interstitial_style_landscape_4.xml` | res | 6904 |
| 1872 | `res/layout/bigo_ad_activity_interstitial_style_landscape_4_full_media.xml` | res | 6792 |
| 1873 | `res/layout/bigo_ad_activity_interstitial_vpaid.xml` | res | 1012 |
| 1874 | `res/layout/bigo_ad_activity_pop_up_style_1.xml` | res | 3832 |
| 1875 | `res/layout/bigo_ad_activity_pop_up_style_2.xml` | res | 5796 |
| 1876 | `res/layout/bigo_ad_activity_pop_up_style_3.xml` | res | 4536 |
| 1877 | `res/layout/bigo_ad_activity_pop_up_style_4.xml` | res | 4304 |
| 1878 | `res/layout/bigo_ad_activity_pop_up_style_5.xml` | res | 5212 |
| 1879 | `res/layout/bigo_ad_activity_pop_up_style_6.xml` | res | 4904 |
| 1880 | `res/layout/bigo_ad_activity_popup.xml` | res | 512 |
| 1881 | `res/layout/bigo_ad_activity_popup_banner.xml` | res | 1108 |
| 1882 | `res/layout/bigo_ad_activity_popup_vpaid.xml` | res | 2608 |
| 1883 | `res/layout/bigo_ad_activity_reward.xml` | res | 4080 |
| 1884 | `res/layout/bigo_ad_activity_webview.xml` | res | 2420 |
| 1885 | `res/layout/bigo_ad_banner_placeholder_img.xml` | res | 2424 |
| 1886 | `res/layout/bigo_ad_default_loading_layout.xml` | res | 940 |
| 1887 | `res/layout/bigo_ad_dialog_reward_retain.xml` | res | 2432 |
| 1888 | `res/layout/bigo_ad_download_msg.xml` | res | 4392 |
| 1889 | `res/layout/bigo_ad_download_msg_white.xml` | res | 4392 |
| 1890 | `res/layout/bigo_ad_form_content.xml` | res | 1480 |
| 1891 | `res/layout/bigo_ad_form_content_dark.xml` | res | 1480 |
| 1892 | `res/layout/bigo_ad_form_fill_question.xml` | res | 1536 |
| 1893 | `res/layout/bigo_ad_form_fill_question_dark.xml` | res | 1600 |
| 1894 | `res/layout/bigo_ad_form_layout.xml` | res | 3692 |
| 1895 | `res/layout/bigo_ad_form_layout_dark.xml` | res | 3712 |
| 1896 | `res/layout/bigo_ad_form_notice_privacy.xml` | res | 3164 |
| 1897 | `res/layout/bigo_ad_form_notice_privacy_dark.xml` | res | 3244 |
| 1898 | `res/layout/bigo_ad_form_question.xml` | res | 2752 |
| 1899 | `res/layout/bigo_ad_form_question_dark.xml` | res | 2752 |
| 1900 | `res/layout/bigo_ad_form_select_question.xml` | res | 1448 |
| 1901 | `res/layout/bigo_ad_form_select_question_dark.xml` | res | 1468 |
| 1902 | `res/layout/bigo_ad_item_inter_countdown_bg.xml` | res | 1400 |
| 1903 | `res/layout/bigo_ad_item_interaction_horizontal.xml` | res | 1832 |
| 1904 | `res/layout/bigo_ad_item_interaction_vertical.xml` | res | 1684 |
| 1905 | `res/layout/bigo_ad_item_splash_countdown_bg.xml` | res | 1448 |
| 1906 | `res/layout/bigo_ad_label_layout.xml` | res | 1780 |
| 1907 | `res/layout/bigo_ad_layout_gesture_slide.xml` | res | 708 |
| 1908 | `res/layout/bigo_ad_layout_gesture_zoom.xml` | res | 872 |
| 1909 | `res/layout/bigo_ad_layout_ic_item.xml` | res | 1912 |
| 1910 | `res/layout/bigo_ad_layout_interstitial_ad_mark_top_left.xml` | res | 1756 |
| 1911 | `res/layout/bigo_ad_layout_interstitial_ad_warning_bottom.xml` | res | 1016 |
| 1912 | `res/layout/bigo_ad_layout_interstitial_icon_item_cta_des_style1.xml` | res | 2356 |
| 1913 | `res/layout/bigo_ad_layout_interstitial_icon_item_cta_des_style2.xml` | res | 2364 |
| 1914 | `res/layout/bigo_ad_layout_interstitial_icon_item_cta_style1.xml` | res | 1996 |
| 1915 | `res/layout/bigo_ad_layout_interstitial_icon_item_cta_style2.xml` | res | 1896 |
| 1916 | `res/layout/bigo_ad_layout_interstitial_icon_item_style.xml` | res | 1304 |
| 1917 | `res/layout/bigo_ad_layout_interstitial_icon_view_flow.xml` | res | 692 |
| 1918 | `res/layout/bigo_ad_layout_interstitial_mid_page_loading_view.xml` | res | 6532 |
| 1919 | `res/layout/bigo_ad_layout_interstitial_mid_page_loading_view_landscape.xml` | res | 4828 |
| 1920 | `res/layout/bigo_ad_layout_interstitial_mid_page_native_fallback_view_download_info_landscape.xml` | res | 8596 |
| 1921 | `res/layout/bigo_ad_layout_interstitial_mid_page_native_fallback_view_landscape.xml` | res | 8452 |
| 1922 | `res/layout/bigo_ad_layout_interstitial_mid_page_native_view.xml` | res | 8152 |
| 1923 | `res/layout/bigo_ad_layout_interstitial_mid_page_native_view_landscape.xml` | res | 8408 |
| 1924 | `res/layout/bigo_ad_layout_interstitial_rich_video_multi_img_bottom.xml` | res | 2436 |
| 1925 | `res/layout/bigo_ad_layout_more.xml` | res | 808 |
| 1926 | `res/layout/bigo_ad_layout_space.xml` | res | 356 |
| 1927 | `res/layout/bigo_ad_layout_space_black.xml` | res | 356 |
| 1928 | `res/layout/bigo_ad_native_banner_medium.xml` | res | 4228 |
| 1929 | `res/layout/bigo_ad_native_banner_small.xml` | res | 4048 |
| 1930 | `res/layout/bigo_ad_optionview_feedback.xml` | res | 7548 |
| 1931 | `res/layout/bigo_ad_pop_up_ad_msg.xml` | res | 2112 |
| 1932 | `res/layout/bigo_ad_popup_close_btn.xml` | res | 976 |
| 1933 | `res/layout/bigo_ad_splash_ad_label.xml` | res | 1644 |
| 1934 | `res/layout/bigo_ad_splash_endpage1.xml` | res | 3600 |
| 1935 | `res/layout/bigo_ad_splash_endpage1_slide.xml` | res | 3660 |
| 1936 | `res/layout/bigo_ad_splash_endpage_item_click_guide.xml` | res | 1136 |
| 1937 | `res/layout/bigo_ad_splash_endpage_item_cta.xml` | res | 1996 |
| 1938 | `res/layout/bigo_ad_splash_endpage_item_flash.xml` | res | 356 |
| 1939 | `res/layout/bigo_ad_splash_endpage_item_slide.xml` | res | 884 |
| 1940 | `res/layout/bigo_ad_splash_item_arrow.xml` | res | 448 |
| 1941 | `res/layout/bigo_ad_splash_item_click_guide.xml` | res | 1096 |
| 1942 | `res/layout/bigo_ad_splash_item_cta.xml` | res | 1884 |
| 1943 | `res/layout/bigo_ad_splash_item_cta_bg.xml` | res | 544 |
| 1944 | `res/layout/bigo_ad_splash_item_flash.xml` | res | 356 |
| 1945 | `res/layout/bigo_ad_splash_item_ripple.xml` | res | 400 |
| 1946 | `res/layout/bigo_ad_splash_item_slide.xml` | res | 544 |
| 1947 | `res/layout/bigo_ad_splash_style_3_item_cta.xml` | res | 1836 |
| 1948 | `res/layout/bigo_ad_splash_style_3_multi_img.xml` | res | 3408 |
| 1949 | `res/layout/bigo_ad_splash_style_4_cta_widget.xml` | res | 5320 |
| 1950 | `res/layout/bigo_ad_splash_style_5_card_widget.xml` | res | 5200 |
| 1951 | `res/layout/bigo_ad_splash_style_fullscreen.xml` | res | 2880 |
| 1952 | `res/layout/bigo_ad_splash_style_fullscreen_immersive.xml` | res | 1764 |
| 1953 | `res/layout/bigo_ad_splash_style_fullscreen_interaction.xml` | res | 3116 |
| 1954 | `res/layout/bigo_ad_splash_style_fullscreen_interaction_immersive.xml` | res | 2052 |
| 1955 | `res/layout/bigo_ad_splash_style_fullscreen_vpaid.xml` | res | 1456 |
| 1956 | `res/layout/bigo_ad_splash_style_halfscreen.xml` | res | 3100 |
| 1957 | `res/layout/bigo_ad_splash_style_halfscreen_interaction.xml` | res | 3372 |
| 1958 | `res/layout/bigo_ad_splash_style_halfscreen_vpaid.xml` | res | 2768 |
| 1959 | `res/layout/bigo_ad_splash_style_root.xml` | res | 1012 |
| 1960 | `res/layout/bigo_ad_success_submit.xml` | res | 1544 |
| 1961 | `res/layout/bigo_ad_success_submit_dark.xml` | res | 1760 |
| 1962 | `res/layout/bigo_ad_view_click_guide.xml` | res | 2736 |
| 1963 | `res/layout/bigo_ad_view_click_guide_1.xml` | res | 3152 |
| 1964 | `res/layout/bigo_ad_view_click_guide_2.xml` | res | 3264 |
| 1965 | `res/layout/bigo_ad_view_click_guide_3.xml` | res | 4212 |
| 1966 | `res/layout/bigo_ad_view_click_guide_landscape_1.xml` | res | 1844 |
| 1967 | `res/layout/bigo_ad_view_click_guide_landscape_2.xml` | res | 2916 |
| 1968 | `res/layout/bigo_ad_view_click_guide_landscape_3.xml` | res | 2664 |
| 1969 | `res/layout/bigo_ad_view_slide_gesture.xml` | res | 1100 |
| 1970 | `res/layout/bigo_ad_webview_loading.xml` | res | 996 |
| 1971 | `res/layout/biome_item.xml` | res | 1856 |
| 1972 | `res/layout/bm_exo_legacy_player_control_view.xml` | res | 2628 |
| 1973 | `res/layout/bm_exo_list_divider.xml` | res | 356 |
| 1974 | `res/layout/bm_exo_player_control_ffwd_button.xml` | res | 828 |
| 1975 | `res/layout/bm_exo_player_control_rewind_button.xml` | res | 828 |
| 1976 | `res/layout/bm_exo_player_control_view.xml` | res | 4056 |
| 1977 | `res/layout/bm_exo_player_view.xml` | res | 2320 |
| 1978 | `res/layout/bm_exo_styled_settings_list.xml` | res | 468 |
| 1979 | `res/layout/bm_exo_styled_settings_list_item.xml` | res | 1584 |
| 1980 | `res/layout/bm_exo_styled_sub_settings_list_item.xml` | res | 1280 |
| 1981 | `res/layout/bm_exo_track_selection_dialog.xml` | res | 504 |
| 1982 | `res/layout/bm_privacy_sheet.xml` | res | 2072 |
| 1983 | `res/layout/browser_actions_context_menu_page.xml` | res | 1660 |
| 1984 | `res/layout/browser_actions_context_menu_row.xml` | res | 1212 |
| 1985 | `res/layout/content_info_left_aligned_layout.xml` | res | 1168 |
| 1986 | `res/layout/controls.xml` | res | 1832 |
| 1987 | `res/layout/creative_debugger_displayed_ad_detail_activity.xml` | res | 1204 |
| 1988 | `res/layout/custom_dialog.xml` | res | 612 |
| 1989 | `res/layout/debug_panel_item_action_button.xml` | res | 600 |
| 1990 | `res/layout/debug_panel_item_ad_unit.xml` | res | 2152 |
| 1991 | `res/layout/debug_panel_item_ad_units.xml` | res | 668 |
| 1992 | `res/layout/debug_panel_item_divider.xml` | res | 388 |
| 1993 | `res/layout/debug_panel_item_mediation_adapter.xml` | res | 2308 |
| 1994 | `res/layout/debug_panel_item_switch.xml` | res | 420 |
| 1995 | `res/layout/debug_panel_toolbar.xml` | res | 1100 |
| 1996 | `res/layout/design_bottom_navigation_item.xml` | res | 2356 |
| 1997 | `res/layout/design_bottom_sheet_dialog.xml` | res | 1224 |
| 1998 | `res/layout/design_layout_snackbar.xml` | res | 528 |
| 1999 | `res/layout/design_layout_snackbar_include.xml` | res | 1444 |
| 2000 | `res/layout/design_layout_tab_icon.xml` | res | 408 |
| 2001 | `res/layout/design_layout_tab_text.xml` | res | 436 |
| 2002 | `res/layout/design_menu_item_action_area.xml` | res | 320 |
| 2003 | `res/layout/design_navigation_item.xml` | res | 632 |
| 2004 | `res/layout/design_navigation_item_header.xml` | res | 440 |
| 2005 | `res/layout/design_navigation_item_separator.xml` | res | 472 |
| 2006 | `res/layout/design_navigation_item_subheader.xml` | res | 608 |
| 2007 | `res/layout/design_navigation_menu.xml` | res | 528 |
| 2008 | `res/layout/design_navigation_menu_item.xml` | res | 908 |
| 2009 | `res/layout/design_text_input_end_icon.xml` | res | 656 |
| 2010 | `res/layout/design_text_input_start_icon.xml` | res | 652 |
| 2011 | `res/layout/deubg_panel_item_header.xml` | res | 384 |
| 2012 | `res/layout/deubg_panel_item_key_value.xml` | res | 908 |
| 2013 | `res/layout/dialog_fullscreen.xml` | res | 1272 |
| 2014 | `res/layout/drawer_header.xml` | res | 1184 |
| 2015 | `res/layout/dt_store_promo_layout.xml` | res | 412 |
| 2016 | `res/layout/exo_legacy_player_control_view.xml` | res | 2628 |
| 2017 | `res/layout/exo_list_divider.xml` | res | 356 |
| 2018 | `res/layout/exo_player_control_view.xml` | res | 4056 |
| 2019 | `res/layout/exo_player_view.xml` | res | 2308 |
| 2020 | `res/layout/exo_styled_player_control_view.xml` | res | 4036 |
| 2021 | `res/layout/exo_styled_player_view.xml` | res | 2128 |
| 2022 | `res/layout/exo_styled_settings_list.xml` | res | 468 |
| 2023 | `res/layout/exo_styled_settings_list_item.xml` | res | 1584 |
| 2024 | `res/layout/exo_styled_sub_settings_list_item.xml` | res | 1280 |
| 2025 | `res/layout/exo_track_selection_dialog.xml` | res | 496 |
| 2026 | `res/layout/expand_button.xml` | res | 1720 |
| 2027 | `res/layout/ia_endcard_container.xml` | res | 396 |
| 2028 | `res/layout/ia_layout_activity_internal_store_webpage.xml` | res | 396 |
| 2029 | `res/layout/ia_layout_buffering_overlay.xml` | res | 516 |
| 2030 | `res/layout/ia_layout_click_overlay.xml` | res | 700 |
| 2031 | `res/layout/ia_layout_default_video_end_card.xml` | res | 1620 |
| 2032 | `res/layout/ia_layout_fullscreen_activity.xml` | res | 1756 |
| 2033 | `res/layout/ia_layout_fyber_ad_identifier.xml` | res | 932 |
| 2034 | `res/layout/ia_layout_fyber_ad_identifier_relative.xml` | res | 300 |
| 2035 | `res/layout/ia_layout_rich_media_video.xml` | res | 1152 |
| 2036 | `res/layout/ia_layout_skip_rewarded_dialog.xml` | res | 2020 |
| 2037 | `res/layout/ia_layout_video_view.xml` | res | 3216 |
| 2038 | `res/layout/ia_video_view_remote_ui.xml` | res | 1008 |
| 2039 | `res/layout/image_frame.xml` | res | 1088 |
| 2040 | `res/layout/ime_base_split_test_activity.xml` | res | 364 |
| 2041 | `res/layout/ime_secondary_split_test_activity.xml` | res | 876 |
| 2042 | `res/layout/instream_layout_detail_pause_overlay.xml` | res | 5268 |
| 2043 | `res/layout/instream_layout_detail_play_overlay.xml` | res | 5444 |
| 2044 | `res/layout/instream_layout_detail_play_overlay_simple.xml` | res | 5232 |
| 2045 | `res/layout/instream_layout_detail_preroll.xml` | res | 5676 |
| 2046 | `res/layout/linear_player_count_down.xml` | res | 936 |
| 2047 | `res/layout/loading_alert.xml` | res | 1004 |
| 2048 | `res/layout/m3_alert_dialog.xml` | res | 2760 |
| 2049 | `res/layout/m3_alert_dialog_actions.xml` | res | 1584 |
| 2050 | `res/layout/m3_alert_dialog_title.xml` | res | 956 |
| 2051 | `res/layout/m3_auto_complete_simple_item.xml` | res | 612 |
| 2052 | `res/layout/m3_navigation_menu_divider.xml` | res | 712 |
| 2053 | `res/layout/m3_navigation_menu_subheader.xml` | res | 716 |
| 2054 | `res/layout/m3_side_sheet_dialog.xml` | res | 1140 |
| 2055 | `res/layout/mainlayout.xml` | res | 408 |
| 2056 | `res/layout/map_selection_view.xml` | res | 668 |
| 2057 | `res/layout/material_chip_input_combo.xml` | res | 372 |
| 2058 | `res/layout/material_clock_display.xml` | res | 796 |
| 2059 | `res/layout/material_clock_display_divider.xml` | res | 444 |
| 2060 | `res/layout/material_clock_period_toggle.xml` | res | 1220 |
| 2061 | `res/layout/material_clockface_textview.xml` | res | 476 |
| 2062 | `res/layout/material_clockface_view.xml` | res | 1012 |
| 2063 | `res/layout/material_radial_view_group.xml` | res | 768 |
| 2064 | `res/layout/material_textinput_timepicker.xml` | res | 720 |
| 2065 | `res/layout/material_time_chip.xml` | res | 380 |
| 2066 | `res/layout/material_time_input.xml` | res | 956 |
| 2067 | `res/layout/material_timepicker.xml` | res | 1608 |
| 2068 | `res/layout/material_timepicker_dialog.xml` | res | 2892 |
| 2069 | `res/layout/material_timepicker_textinput_display.xml` | res | 728 |
| 2070 | `res/layout/max_native_ad_banner_icon_and_text_layout.xml` | res | 2056 |
| 2071 | `res/layout/max_native_ad_banner_view.xml` | res | 1000 |
| 2072 | `res/layout/max_native_ad_leader_view.xml` | res | 2680 |
| 2073 | `res/layout/max_native_ad_media_banner_view.xml` | res | 2468 |
| 2074 | `res/layout/max_native_ad_medium_template_1.xml` | res | 3324 |
| 2075 | `res/layout/max_native_ad_mrec_view.xml` | res | 1240 |
| 2076 | `res/layout/max_native_ad_recycler_view_item.xml` | res | 500 |
| 2077 | `res/layout/max_native_ad_small_template_1.xml` | res | 2804 |
| 2078 | `res/layout/max_native_ad_vertical_banner_view.xml` | res | 2880 |
| 2079 | `res/layout/max_native_ad_vertical_leader_view.xml` | res | 2504 |
| 2080 | `res/layout/max_native_ad_vertical_media_banner_view.xml` | res | 2980 |
| 2081 | `res/layout/mbridge_activity.xml` | res | 396 |
| 2082 | `res/layout/mbridge_alertview_layout.xml` | res | 1716 |
| 2083 | `res/layout/mbridge_bt_container.xml` | res | 356 |
| 2084 | `res/layout/mbridge_cm_alertview.xml` | res | 2144 |
| 2085 | `res/layout/mbridge_cm_feedback_notice_layout.xml` | res | 724 |
| 2086 | `res/layout/mbridge_cm_feedbackview.xml` | res | 3180 |
| 2087 | `res/layout/mbridge_cm_loading_layout.xml` | res | 600 |
| 2088 | `res/layout/mbridge_interstitial_activity.xml` | res | 1144 |
| 2089 | `res/layout/mbridge_more_offer_activity.xml` | res | 768 |
| 2090 | `res/layout/mbridge_nativex_fullbasescreen.xml` | res | 2328 |
| 2091 | `res/layout/mbridge_nativex_fullscreen_top.xml` | res | 1684 |
| 2092 | `res/layout/mbridge_nativex_mbmediaview.xml` | res | 1636 |
| 2093 | `res/layout/mbridge_nativex_playerview.xml` | res | 2848 |
| 2094 | `res/layout/mbridge_order_layout_item.xml` | res | 4888 |
| 2095 | `res/layout/mbridge_order_layout_list_landscape.xml` | res | 3612 |
| 2096 | `res/layout/mbridge_order_layout_list_portrait.xml` | res | 3040 |
| 2097 | `res/layout/mbridge_playercommon_player_view.xml` | res | 1040 |
| 2098 | `res/layout/mbridge_reward_activity_video_templete.xml` | res | 1252 |
| 2099 | `res/layout/mbridge_reward_activity_video_templete_transparent.xml` | res | 1292 |
| 2100 | `res/layout/mbridge_reward_clickable_cta.xml` | res | 1928 |
| 2101 | `res/layout/mbridge_reward_end_card_layout_landscape.xml` | res | 4988 |
| 2102 | `res/layout/mbridge_reward_end_card_layout_landscape_1302.xml` | res | 4608 |
| 2103 | `res/layout/mbridge_reward_end_card_layout_portrait.xml` | res | 4812 |
| 2104 | `res/layout/mbridge_reward_end_card_layout_portrait_1302.xml` | res | 4544 |
| 2105 | `res/layout/mbridge_reward_end_card_more_offer_item.xml` | res | 1204 |
| 2106 | `res/layout/mbridge_reward_endcard_h5.xml` | res | 968 |
| 2107 | `res/layout/mbridge_reward_endcard_native_half_landscape.xml` | res | 5428 |
| 2108 | `res/layout/mbridge_reward_endcard_native_half_portrait.xml` | res | 4976 |
| 2109 | `res/layout/mbridge_reward_endcard_native_hor.xml` | res | 5580 |
| 2110 | `res/layout/mbridge_reward_endcard_native_land.xml` | res | 5920 |
| 2111 | `res/layout/mbridge_reward_endcard_vast.xml` | res | 1492 |
| 2112 | `res/layout/mbridge_reward_layer_floor.xml` | res | 4228 |
| 2113 | `res/layout/mbridge_reward_layer_floor_302.xml` | res | 5336 |
| 2114 | `res/layout/mbridge_reward_layer_floor_5002010.xml` | res | 5800 |
| 2115 | `res/layout/mbridge_reward_layer_floor_802.xml` | res | 6176 |
| 2116 | `res/layout/mbridge_reward_layer_floor_902.xml` | res | 4248 |
| 2117 | `res/layout/mbridge_reward_layer_floor_904.xml` | res | 4064 |
| 2118 | `res/layout/mbridge_reward_layer_floor_bottom.xml` | res | 2308 |
| 2119 | `res/layout/mbridge_reward_layer_floor_half_portrait.xml` | res | 4064 |
| 2120 | `res/layout/mbridge_reward_more_offer_view.xml` | res | 1332 |
| 2121 | `res/layout/mbridge_reward_videoend_cover.xml` | res | 2028 |
| 2122 | `res/layout/mbridge_reward_videoview_item.xml` | res | 3900 |
| 2123 | `res/layout/mbridge_reward_view_tag_item.xml` | res | 920 |
| 2124 | `res/layout/mbridge_same_choice_one_layout_landscape.xml` | res | 5252 |
| 2125 | `res/layout/mbridge_same_choice_one_layout_portrait.xml` | res | 5536 |
| 2126 | `res/layout/mbridge_splash_landscape.xml` | res | 5988 |
| 2127 | `res/layout/mbridge_splash_portrait.xml` | res | 6072 |
| 2128 | `res/layout/mdtb_interstitial_ad.xml` | res | 936 |
| 2129 | `res/layout/mediation_debugger_ad_unit_detail_activity.xml` | res | 1412 |
| 2130 | `res/layout/mediation_debugger_list_item_right_detail.xml` | res | 2152 |
| 2131 | `res/layout/mediation_debugger_list_section.xml` | res | 808 |
| 2132 | `res/layout/mediation_debugger_list_section_centered.xml` | res | 848 |
| 2133 | `res/layout/mediation_debugger_list_view.xml` | res | 656 |
| 2134 | `res/layout/mediation_debugger_multi_ad_activity.xml` | res | 6160 |
| 2135 | `res/layout/mediation_debugger_text_view_activity.xml` | res | 676 |
| 2136 | `res/layout/mobilefuse_ad_fullscreen.xml` | res | 560 |
| 2137 | `res/layout/mobilefuse_mraid_fullscreen_content.xml` | res | 688 |
| 2138 | `res/layout/mobilefuse_native_interstitial_fullscreen.xml` | res | 932 |
| 2139 | `res/layout/mobilefuse_native_interstitial_layout.xml` | res | 2688 |
| 2140 | `res/layout/mobilefuse_splash_ad_fullscreen.xml` | res | 476 |
| 2141 | `res/layout/mobilefuse_vast_fullscreen_content.xml` | res | 556 |
| 2142 | `res/layout/mobilefuse_vast_player_view.xml` | res | 748 |
| 2143 | `res/layout/monetization_ads_feed_item.xml` | res | 420 |
| 2144 | `res/layout/monetization_ads_feed_progressbar.xml` | res | 780 |
| 2145 | `res/layout/monetization_ads_internal_adtune_container.xml` | res | 2032 |
| 2146 | `res/layout/monetization_ads_internal_divkit.xml` | res | 360 |
| 2147 | `res/layout/monetization_ads_internal_instream_skin_v1.xml` | res | 2392 |
| 2148 | `res/layout/monetization_ads_internal_instream_skin_v2.xml` | res | 7132 |
| 2149 | `res/layout/monetization_ads_internal_multibanner_controls.xml` | res | 1272 |
| 2150 | `res/layout/monetization_ads_internal_native_interstitial_close.xml` | res | 580 |
| 2151 | `res/layout/monetization_ads_internal_native_interstitial_content.xml` | res | 4904 |
| 2152 | `res/layout/monetization_ads_internal_native_interstitial_landscape_horizontal_media.xml` | res | 5200 |
| 2153 | `res/layout/monetization_ads_internal_native_interstitial_landscape_sponsored_container.xml` | res | 1472 |
| 2154 | `res/layout/monetization_ads_internal_native_interstitial_landscape_vetrical_media.xml` | res | 6160 |
| 2155 | `res/layout/monetization_ads_internal_native_interstitial_portrait.xml` | res | 1956 |
| 2156 | `res/layout/monetization_ads_internal_native_popup_item.xml` | res | 1028 |
| 2157 | `res/layout/monetization_ads_internal_native_rewarded_controls.xml` | res | 1604 |
| 2158 | `res/layout/monetization_ads_internal_native_rewarded_landscape.xml` | res | 1416 |
| 2159 | `res/layout/monetization_ads_internal_native_rewarded_portrait.xml` | res | 1436 |
| 2160 | `res/layout/monetization_ads_internal_outstream_controls_app_install.xml` | res | 988 |
| 2161 | `res/layout/monetization_ads_internal_outstream_controls_default.xml` | res | 1096 |
| 2162 | `res/layout/monetization_ads_internal_outstream_controls_fullscreen_content_v1.xml` | res | 1224 |
| 2163 | `res/layout/monetization_ads_internal_rewarded_close_verification.xml` | res | 2048 |
| 2164 | `res/layout/mrec_layout.xml` | res | 3716 |
| 2165 | `res/layout/mtrl_alert_dialog.xml` | res | 2476 |
| 2166 | `res/layout/mtrl_alert_dialog_actions.xml` | res | 1676 |
| 2167 | `res/layout/mtrl_alert_dialog_title.xml` | res | 956 |
| 2168 | `res/layout/mtrl_alert_select_dialog_item.xml` | res | 588 |
| 2169 | `res/layout/mtrl_alert_select_dialog_multichoice.xml` | res | 940 |
| 2170 | `res/layout/mtrl_alert_select_dialog_singlechoice.xml` | res | 940 |
| 2171 | `res/layout/mtrl_auto_complete_simple_item.xml` | res | 612 |
| 2172 | `res/layout/mtrl_calendar_day.xml` | res | 352 |
| 2173 | `res/layout/mtrl_calendar_day_of_week.xml` | res | 352 |
| 2174 | `res/layout/mtrl_calendar_days_of_week.xml` | res | 436 |
| 2175 | `res/layout/mtrl_calendar_horizontal.xml` | res | 1176 |
| 2176 | `res/layout/mtrl_calendar_month_labeled.xml` | res | 728 |
| 2177 | `res/layout/mtrl_calendar_month_navigation.xml` | res | 1804 |
| 2178 | `res/layout/mtrl_calendar_months.xml` | res | 428 |
| 2179 | `res/layout/mtrl_calendar_vertical.xml` | res | 740 |
| 2180 | `res/layout/mtrl_calendar_year.xml` | res | 352 |
| 2181 | `res/layout/mtrl_layout_snackbar.xml` | res | 492 |
| 2182 | `res/layout/mtrl_layout_snackbar_include.xml` | res | 952 |
| 2183 | `res/layout/mtrl_navigation_rail_item.xml` | res | 2432 |
| 2184 | `res/layout/mtrl_picker_actions.xml` | res | 984 |
| 2185 | `res/layout/mtrl_picker_dialog.xml` | res | 1172 |
| 2186 | `res/layout/mtrl_picker_fullscreen.xml` | res | 880 |
| 2187 | `res/layout/mtrl_picker_header_dialog.xml` | res | 1548 |
| 2188 | `res/layout/mtrl_picker_header_fullscreen.xml` | res | 2812 |
| 2189 | `res/layout/mtrl_picker_header_selection_text.xml` | res | 668 |
| 2190 | `res/layout/mtrl_picker_header_title_text.xml` | res | 624 |
| 2191 | `res/layout/mtrl_picker_header_toggle.xml` | res | 528 |
| 2192 | `res/layout/mtrl_picker_text_input_date.xml` | res | 1104 |
| 2193 | `res/layout/mtrl_picker_text_input_date_range.xml` | res | 1696 |
| 2194 | `res/layout/mtrl_search_bar.xml` | res | 844 |
| 2195 | `res/layout/mtrl_search_view.xml` | res | 3444 |
| 2196 | `res/layout/notification_media_action.xml` | res | 564 |
| 2197 | `res/layout/notification_media_cancel_action.xml` | res | 744 |
| 2198 | `res/layout/notification_template_big_media.xml` | res | 1696 |
| 2199 | `res/layout/notification_template_big_media_custom.xml` | res | 3044 |
| 2200 | `res/layout/notification_template_big_media_narrow.xml` | res | 1824 |
| 2201 | `res/layout/notification_template_big_media_narrow_custom.xml` | res | 3216 |
| 2202 | `res/layout/notification_template_lines_media.xml` | res | 2872 |
| 2203 | `res/layout/notification_template_media.xml` | res | 1292 |
| 2204 | `res/layout/notification_template_media_custom.xml` | res | 2756 |
| 2205 | `res/layout/notification_template_part_chronometer.xml` | res | 440 |
| 2206 | `res/layout/notification_template_part_time.xml` | res | 440 |
| 2207 | `res/layout/offline_ads_dialog.xml` | res | 1528 |
| 2208 | `res/layout/open_url.xml` | res | 1132 |
| 2209 | `res/layout/player_count_down.xml` | res | 996 |
| 2210 | `res/layout/pob_custom_product_layout.xml` | res | 1440 |
| 2211 | `res/layout/pob_layout_rewardedad_skip_alert.xml` | res | 1380 |
| 2212 | `res/layout/pob_medium_template.xml` | res | 3164 |
| 2213 | `res/layout/pob_monitor_view.xml` | res | 1176 |
| 2214 | `res/layout/pob_small_template.xml` | res | 2704 |
| 2215 | `res/layout/preference.xml` | res | 2352 |
| 2216 | `res/layout/preference_category.xml` | res | 384 |
| 2217 | `res/layout/preference_category_material.xml` | res | 1768 |
| 2218 | `res/layout/preference_dialog_edittext.xml` | res | 1272 |
| 2219 | `res/layout/preference_dropdown.xml` | res | 2544 |
| 2220 | `res/layout/preference_dropdown_material.xml` | res | 712 |
| 2221 | `res/layout/preference_information.xml` | res | 1732 |
| 2222 | `res/layout/preference_information_material.xml` | res | 2056 |
| 2223 | `res/layout/preference_list_fragment.xml` | res | 812 |
| 2224 | `res/layout/preference_material.xml` | res | 2052 |
| 2225 | `res/layout/preference_recyclerview.xml` | res | 544 |
| 2226 | `res/layout/preference_widget_checkbox.xml` | res | 472 |
| 2227 | `res/layout/preference_widget_seekbar.xml` | res | 2896 |
| 2228 | `res/layout/preference_widget_seekbar_material.xml` | res | 3056 |
| 2229 | `res/layout/preference_widget_switch.xml` | res | 472 |
| 2230 | `res/layout/preference_widget_switch_compat.xml` | res | 504 |
| 2231 | `res/layout/progress_count_down.xml` | res | 884 |
| 2232 | `res/layout/rab_card_large.xml` | res | 4516 |
| 2233 | `res/layout/select_dialog_item_material.xml` | res | 640 |
| 2234 | `res/layout/select_dialog_multichoice_material.xml` | res | 864 |
| 2235 | `res/layout/select_dialog_singlechoice_material.xml` | res | 864 |
| 2236 | `res/layout/selection_drag_btn.xml` | res | 596 |
| 2237 | `res/layout/sentry_dialog_user_feedback.xml` | res | 3404 |
| 2238 | `res/layout/simple_list_item.xml` | res | 644 |
| 2239 | `res/layout/simple_list_item_action.xml` | res | 736 |
| 2240 | `res/layout/simple_list_item_multiple_choice.xml` | res | 692 |
| 2241 | `res/layout/simple_list_item_single_choice.xml` | res | 692 |
| 2242 | `res/layout/simpledialogfragment_check_box.xml` | res | 748 |
| 2243 | `res/layout/simpledialogfragment_color_item.xml` | res | 936 |
| 2244 | `res/layout/simpledialogfragment_color_wheel.xml` | res | 4324 |
| 2245 | `res/layout/simpledialogfragment_custom_view.xml` | res | 1400 |
| 2246 | `res/layout/simpledialogfragment_form.xml` | res | 908 |
| 2247 | `res/layout/simpledialogfragment_form_item_check.xml` | res | 644 |
| 2248 | `res/layout/simpledialogfragment_form_item_color.xml` | res | 1364 |
| 2249 | `res/layout/simpledialogfragment_form_item_datetime.xml` | res | 1404 |
| 2250 | `res/layout/simpledialogfragment_form_item_hint.xml` | res | 596 |
| 2251 | `res/layout/simpledialogfragment_form_item_input.xml` | res | 1004 |
| 2252 | `res/layout/simpledialogfragment_form_item_spinner.xml` | res | 952 |
| 2253 | `res/layout/simpledialogfragment_grid.xml` | res | 1396 |
| 2254 | `res/layout/simpledialogfragment_image.xml` | res | 1084 |
| 2255 | `res/layout/simpledialogfragment_image_hor_scroll.xml` | res | 1052 |
| 2256 | `res/layout/simpledialogfragment_image_vert_scroll.xml` | res | 1040 |
| 2257 | `res/layout/simpledialogfragment_input.xml` | res | 1096 |
| 2258 | `res/layout/simpledialogfragment_list.xml` | res | 1444 |
| 2259 | `res/layout/simpledialogfragment_list_item.xml` | res | 616 |
| 2260 | `res/layout/simpledialogfragment_pin.xml` | res | 1464 |
| 2261 | `res/layout/simpledialogfragment_progress.xml` | res | 1596 |
| 2262 | `res/layout/simpledialogfragment_progress_circle.xml` | res | 1912 |
| 2263 | `res/layout/smaato_sdk_core_activity_internal_browser.xml` | res | 4092 |
| 2264 | `res/layout/smaato_sdk_interstitial_activity.xml` | res | 1476 |
| 2265 | `res/layout/smaato_sdk_interstitial_splash_activity.xml` | res | 672 |
| 2266 | `res/layout/smaato_sdk_rewarded_ads_activity.xml` | res | 872 |
| 2267 | `res/layout/smaato_sdk_richmedia_layout_closable.xml` | res | 856 |
| 2268 | `res/layout/smaato_sdk_video_player_view.xml` | res | 2172 |
| 2269 | `res/layout/smaato_sdk_video_vast_video_player_view.xml` | res | 1576 |
| 2270 | `res/layout/support_simple_spinner_dropdown_item.xml` | res | 464 |
| 2271 | `res/layout/taurusx_activity_appopen.xml` | res | 5124 |
| 2272 | `res/layout/taurusx_activity_appopen_native.xml` | res | 5076 |
| 2273 | `res/layout/taurusx_activity_fullscreen.xml` | res | 7932 |
| 2274 | `res/layout/taurusx_activity_h5.xml` | res | 884 |
| 2275 | `res/layout/taurusx_activity_web_view.xml` | res | 400 |
| 2276 | `res/layout/taurusx_dialog_adtune_layout.xml` | res | 1092 |
| 2277 | `res/layout/temple904.xml` | res | 1408 |
| 2278 | `res/layout/timer_count_down.xml` | res | 916 |
| 2279 | `res/layout/tree_node_item.xml` | res | 1272 |
| 2280 | `res/layout/world_item.xml` | res | 2120 |
| 2281 | `res/layout/world_layer_item.xml` | res | 2080 |
| 2282 | `res/menu/create_world_activity_toolbar_menu.xml` | res | 560 |
| 2283 | `res/menu/creative_debugger_displayed_ad_activity_menu.xml` | res | 424 |
| 2284 | `res/menu/dialog_buttons.xml` | res | 832 |
| 2285 | `res/menu/drawer_menu.xml` | res | 2064 |
| 2286 | `res/menu/main_activity_toolbar_menu.xml` | res | 712 |
| 2287 | `res/menu/mediation_debugger_activity_menu.xml` | res | 424 |
| 2288 | `res/mipmap-anydpi-v26/ic_launcher.xml` | res | 448 |
| 2289 | `res/mipmap-anydpi-v26/ic_launcher_round.xml` | res | 448 |
| 2290 | `res/mipmap-hdpi-v4/ic_launcher.webp` | res | 1222 |
| 2291 | `res/mipmap-hdpi-v4/ic_launcher_background.webp` | res | 40 |
| 2292 | `res/mipmap-hdpi-v4/ic_launcher_foreground.webp` | res | 1078 |
| 2293 | `res/mipmap-hdpi-v4/ic_launcher_round.webp` | res | 2460 |
| 2294 | `res/mipmap-mdpi-v4/ic_launcher.webp` | res | 904 |
| 2295 | `res/mipmap-mdpi-v4/ic_launcher_background.webp` | res | 40 |
| 2296 | `res/mipmap-mdpi-v4/ic_launcher_foreground.webp` | res | 754 |
| 2297 | `res/mipmap-mdpi-v4/ic_launcher_round.webp` | res | 1618 |
| 2298 | `res/mipmap-xhdpi-v4/ic_launcher.webp` | res | 1532 |
| 2299 | `res/mipmap-xhdpi-v4/ic_launcher_background.webp` | res | 42 |
| 2300 | `res/mipmap-xhdpi-v4/ic_launcher_foreground.webp` | res | 1462 |
| 2301 | `res/mipmap-xhdpi-v4/ic_launcher_round.webp` | res | 3446 |
| 2302 | `res/mipmap-xxhdpi-v4/ic_launcher.webp` | res | 2372 |
| 2303 | `res/mipmap-xxhdpi-v4/ic_launcher_background.webp` | res | 48 |
| 2304 | `res/mipmap-xxhdpi-v4/ic_launcher_foreground.webp` | res | 2222 |
| 2305 | `res/mipmap-xxhdpi-v4/ic_launcher_round.webp` | res | 5490 |
| 2306 | `res/mipmap-xxxhdpi-v4/ic_launcher.webp` | res | 3170 |
| 2307 | `res/mipmap-xxxhdpi-v4/ic_launcher_background.webp` | res | 48 |
| 2308 | `res/mipmap-xxxhdpi-v4/ic_launcher_foreground.webp` | res | 2956 |
| 2309 | `res/mipmap-xxxhdpi-v4/ic_launcher_round.webp` | res | 7710 |
| 2310 | `res/mipmap/close.png` | res | 4884 |
| 2311 | `res/mipmap/mute.png` | res | 6635 |
| 2312 | `res/mipmap/replay.png` | res | 507 |
| 2313 | `res/mipmap/skip.png` | res | 3821 |
| 2314 | `res/mipmap/unmute.png` | res | 5415 |
| 2315 | `res/raw/applovin_consent_flow_unified_cmp.json` | res | 2201 |
| 2316 | `res/raw/com_android_billingclient_heterodyne_info` | res | 46 |
| 2317 | `res/raw/com_android_billingclient_registration_info.binarypb` | res | 34 |
| 2318 | `res/raw/html_player_vast.js` | res | 2070 |
| 2319 | `res/raw/keep_cronet_api.xml` | res | 156 |
| 2320 | `res/raw/license` | res | 24647 |
| 2321 | `res/raw/mobilefuse_omsdk_license` | res | 24647 |
| 2322 | `res/raw/mobilefuse_omsdk_v1.js` | res | 44294 |
| 2323 | `res/raw/monetization_ads_bundled_cert.crt` | res | 1830 |
| 2324 | `res/raw/monetization_ads_keep.xml` | res | 468 |
| 2325 | `res/raw/monetization_ads_sdkinternalca.crt` | res | 1931 |
| 2326 | `res/raw/omid_session_client_v1_5_3.js` | res | 69821 |
| 2327 | `res/raw/omsdk_v1.js` | res | 45650 |
| 2328 | `res/raw/omsdk_v1_5_3.js` | res | 46064 |
| 2329 | `res/raw/russian_trusted_root_ca.cer` | res | 2056 |
| 2330 | `res/raw/russian_trusted_sub_ca.cer` | res | 2576 |
| 2331 | `res/raw/shape_corner.svg` | res | 881 |
| 2332 | `res/xml/ad_services_config.xml` | res | 344 |
| 2333 | `res/xml/com_android_billingclient_phenotype.xml` | res | 492 |
| 2334 | `res/xml/debug_panel_file_paths.xml` | res | 292 |
| 2335 | `res/xml/file_paths.xml` | res | 404 |
| 2336 | `res/xml/image_share_filepaths.xml` | res | 308 |
| 2337 | `res/xml/m3_button_group_child_size_change.xml` | res | 572 |
| 2338 | `res/xml/m3_split_button_inner_corner_size_state_list.xml` | res | 740 |
| 2339 | `res/xml/m3expressive_button_shape_state_list.xml` | res | 700 |
| 2340 | `res/xml/m3expressive_connected_buttons_inner_corner_size_state_list.xml` | res | 740 |
| 2341 | `res/xml/nath_tax_file_path.xml` | res | 300 |
| 2342 | `res/xml/network_security_config.xml` | res | 468 |
| 2343 | `res/xml/progress_bg.xml` | res | 1036 |
| 2344 | `res/xml/splits0.xml` | res | 9840 |
| 2345 | `resources.arsc` | resource-table | 1062028 |
| 2346 | `src/google/protobuf/any.proto` | other | 6154 |
| 2347 | `src/google/protobuf/api.proto` | other | 7728 |
| 2348 | `src/google/protobuf/duration.proto` | other | 4892 |
| 2349 | `src/google/protobuf/empty.proto` | other | 2363 |
| 2350 | `src/google/protobuf/field_mask.proto` | other | 8185 |
| 2351 | `src/google/protobuf/source_context.proto` | other | 2341 |
| 2352 | `src/google/protobuf/struct.proto` | other | 3778 |
| 2353 | `src/google/protobuf/timestamp.proto` | other | 6449 |
| 2354 | `src/google/protobuf/type.proto` | other | 6367 |
| 2355 | `src/google/protobuf/wrappers.proto` | other | 4044 |
| 2356 | `stamp-cert-sha256` | other | 32 |
| 2357 | `transport-api.properties` | other | 62 |
| 2358 | `transport-backend-cct.properties` | other | 78 |
| 2359 | `transport-runtime.properties` | other | 70 |
| 2360 | `user-messaging-platform.properties` | other | 82 |

### config.arm64_v8a (17 file)

| # | File | Kategori | Ukuran |
|---:|---|---|---:|
| 1 | `AndroidManifest.xml` | manifest | 1052 |
| 2 | `META-INF/BNDLTOOL.RSA` | META-INF | 2172 |
| 3 | `META-INF/BNDLTOOL.SF` | META-INF | 1296 |
| 4 | `META-INF/MANIFEST.MF` | META-INF | 1188 |
| 5 | `lib/arm64-v8a/libapminsighta.so` | native-libs | 102256 |
| 6 | `lib/arm64-v8a/libapminsightb.so` | native-libs | 124352 |
| 7 | `lib/arm64-v8a/libapplovin-native-crash-reporter.so` | native-libs | 860200 |
| 8 | `lib/arm64-v8a/libbuffer_pgl.so` | native-libs | 9000 |
| 9 | `lib/arm64-v8a/libfile_lock_pgl.so` | native-libs | 6312 |
| 10 | `lib/arm64-v8a/libleveldb.so` | native-libs | 1261744 |
| 11 | `lib/arm64-v8a/libnms.so` | native-libs | 297832 |
| 12 | `lib/arm64-v8a/libpglarmor.so` | native-libs | 68600 |
| 13 | `lib/arm64-v8a/libsentry-android.so` | native-libs | 16832 |
| 14 | `lib/arm64-v8a/libsentry.so` | native-libs | 681784 |
| 15 | `lib/arm64-v8a/libtobEmbedPagEncrypt.so` | native-libs | 8304 |
| 16 | `lib/arm64-v8a/libtt_ugen_layout.so` | native-libs | 440672 |
| 17 | `stamp-cert-sha256` | other | 32 |

### config.en (9 file)

| # | File | Kategori | Ukuran |
|---:|---|---|---:|
| 1 | `AndroidManifest.xml` | manifest | 1020 |
| 2 | `META-INF/BNDLTOOL.RSA` | META-INF | 2172 |
| 3 | `META-INF/BNDLTOOL.SF` | META-INF | 713 |
| 4 | `META-INF/MANIFEST.MF` | META-INF | 605 |
| 5 | `res/drawable-en/monetization_ads_instream_internal_click_button.xml` | res | 6636 |
| 6 | `res/drawable-en/monetization_ads_instream_internal_skip_focused.xml` | res | 3836 |
| 7 | `res/drawable-en/monetization_ads_instream_internal_skip_unfocused.xml` | res | 3836 |
| 8 | `resources.arsc` | resource-table | 76392 |
| 9 | `stamp-cert-sha256` | other | 32 |

### config.mdpi (205 file)

| # | File | Kategori | Ukuran |
|---:|---|---|---:|
| 1 | `AndroidManifest.xml` | manifest | 1052 |
| 2 | `META-INF/BNDLTOOL.RSA` | META-INF | 2172 |
| 3 | `META-INF/BNDLTOOL.SF` | META-INF | 21328 |
| 4 | `META-INF/MANIFEST.MF` | META-INF | 21220 |
| 5 | `res/drawable-anydpi-v21/applovin_ic_baseline_add_circle_outline.xml` | res | 724 |
| 6 | `res/drawable-anydpi-v21/applovin_ic_check_mark_bordered.xml` | res | 816 |
| 7 | `res/drawable-anydpi-v21/applovin_ic_check_mark_borderless.xml` | res | 700 |
| 8 | `res/drawable-anydpi-v21/applovin_ic_disclosure_arrow.xml` | res | 620 |
| 9 | `res/drawable-anydpi-v21/applovin_ic_mediation_adjoe.xml` | res | 2196 |
| 10 | `res/drawable-anydpi-v21/applovin_ic_mediation_admob.xml` | res | 1284 |
| 11 | `res/drawable-anydpi-v21/applovin_ic_mediation_amazon_marketplace.xml` | res | 1528 |
| 12 | `res/drawable-anydpi-v21/applovin_ic_mediation_applovin.xml` | res | 1568 |
| 13 | `res/drawable-anydpi-v21/applovin_ic_mediation_bidmachine.xml` | res | 1184 |
| 14 | `res/drawable-anydpi-v21/applovin_ic_mediation_bigo.xml` | res | 2116 |
| 15 | `res/drawable-anydpi-v21/applovin_ic_mediation_chartboost.xml` | res | 980 |
| 16 | `res/drawable-anydpi-v21/applovin_ic_mediation_facebook.xml` | res | 744 |
| 17 | `res/drawable-anydpi-v21/applovin_ic_mediation_fyber.xml` | res | 6728 |
| 18 | `res/drawable-anydpi-v21/applovin_ic_mediation_google_ad_manager.xml` | res | 1824 |
| 19 | `res/drawable-anydpi-v21/applovin_ic_mediation_inmobi.xml` | res | 1260 |
| 20 | `res/drawable-anydpi-v21/applovin_ic_mediation_ironsource.xml` | res | 1264 |
| 21 | `res/drawable-anydpi-v21/applovin_ic_mediation_line.xml` | res | 1604 |
| 22 | `res/drawable-anydpi-v21/applovin_ic_mediation_linkedin.xml` | res | 2852 |
| 23 | `res/drawable-anydpi-v21/applovin_ic_mediation_mintegral.xml` | res | 3404 |
| 24 | `res/drawable-anydpi-v21/applovin_ic_mediation_mobilefuse.xml` | res | 8464 |
| 25 | `res/drawable-anydpi-v21/applovin_ic_mediation_moloco.xml` | res | 1188 |
| 26 | `res/drawable-anydpi-v21/applovin_ic_mediation_mytarget.xml` | res | 1724 |
| 27 | `res/drawable-anydpi-v21/applovin_ic_mediation_ogury_presage.xml` | res | 1564 |
| 28 | `res/drawable-anydpi-v21/applovin_ic_mediation_pangle.xml` | res | 19972 |
| 29 | `res/drawable-anydpi-v21/applovin_ic_mediation_pubmatic.xml` | res | 1996 |
| 30 | `res/drawable-anydpi-v21/applovin_ic_mediation_smaato.xml` | res | 1312 |
| 31 | `res/drawable-anydpi-v21/applovin_ic_mediation_tiktok.xml` | res | 2376 |
| 32 | `res/drawable-anydpi-v21/applovin_ic_mediation_unity.xml` | res | 1020 |
| 33 | `res/drawable-anydpi-v21/applovin_ic_mediation_verve.xml` | res | 904 |
| 34 | `res/drawable-anydpi-v21/applovin_ic_mediation_vungle.xml` | res | 2312 |
| 35 | `res/drawable-anydpi-v21/applovin_ic_mediation_yandex.xml` | res | 672 |
| 36 | `res/drawable-anydpi-v21/applovin_ic_pause_icon.xml` | res | 856 |
| 37 | `res/drawable-anydpi-v21/applovin_ic_play_icon.xml` | res | 760 |
| 38 | `res/drawable-anydpi-v21/applovin_ic_replay_icon.xml` | res | 688 |
| 39 | `res/drawable-anydpi-v21/applovin_ic_warning.xml` | res | 600 |
| 40 | `res/drawable-anydpi-v21/applovin_ic_warning_outline.xml` | res | 1128 |
| 41 | `res/drawable-anydpi-v21/applovin_ic_x_mark.xml` | res | 940 |
| 42 | `res/drawable-anydpi-v21/exo_edit_mode_logo.xml` | res | 4112 |
| 43 | `res/drawable-anydpi-v21/exo_ic_audiotrack.xml` | res | 1120 |
| 44 | `res/drawable-anydpi-v21/exo_ic_check.xml` | res | 700 |
| 45 | `res/drawable-anydpi-v21/exo_ic_chevron_left.xml` | res | 596 |
| 46 | `res/drawable-anydpi-v21/exo_ic_chevron_right.xml` | res | 596 |
| 47 | `res/drawable-anydpi-v21/exo_ic_default_album_image.xml` | res | 844 |
| 48 | `res/drawable-anydpi-v21/exo_ic_forward.xml` | res | 880 |
| 49 | `res/drawable-anydpi-v21/exo_ic_fullscreen_enter.xml` | res | 644 |
| 50 | `res/drawable-anydpi-v21/exo_ic_fullscreen_exit.xml` | res | 644 |
| 51 | `res/drawable-anydpi-v21/exo_ic_pause_circle_filled.xml` | res | 656 |
| 52 | `res/drawable-anydpi-v21/exo_ic_play_circle_filled.xml` | res | 640 |
| 53 | `res/drawable-anydpi-v21/exo_ic_rewind.xml` | res | 880 |
| 54 | `res/drawable-anydpi-v21/exo_ic_settings.xml` | res | 1420 |
| 55 | `res/drawable-anydpi-v21/exo_ic_skip_next.xml` | res | 584 |
| 56 | `res/drawable-anydpi-v21/exo_ic_skip_previous.xml` | res | 584 |
| 57 | `res/drawable-anydpi-v21/exo_ic_speed.xml` | res | 1148 |
| 58 | `res/drawable-anydpi-v21/exo_ic_subtitle_off.xml` | res | 1212 |
| 59 | `res/drawable-anydpi-v21/exo_ic_subtitle_on.xml` | res | 960 |
| 60 | `res/drawable-anydpi-v21/exo_icon_fastforward.xml` | res | 588 |
| 61 | `res/drawable-anydpi-v21/exo_icon_fullscreen_enter.xml` | res | 684 |
| 62 | `res/drawable-anydpi-v21/exo_icon_fullscreen_exit.xml` | res | 684 |
| 63 | `res/drawable-anydpi-v21/exo_icon_next.xml` | res | 584 |
| 64 | `res/drawable-anydpi-v21/exo_icon_pause.xml` | res | 588 |
| 65 | `res/drawable-anydpi-v21/exo_icon_play.xml` | res | 564 |
| 66 | `res/drawable-anydpi-v21/exo_icon_previous.xml` | res | 584 |
| 67 | `res/drawable-anydpi-v21/exo_icon_repeat_all.xml` | res | 620 |
| 68 | `res/drawable-anydpi-v21/exo_icon_repeat_off.xml` | res | 620 |
| 69 | `res/drawable-anydpi-v21/exo_icon_repeat_one.xml` | res | 656 |
| 70 | `res/drawable-anydpi-v21/exo_icon_rewind.xml` | res | 600 |
| 71 | `res/drawable-anydpi-v21/exo_icon_shuffle_off.xml` | res | 724 |
| 72 | `res/drawable-anydpi-v21/exo_icon_shuffle_on.xml` | res | 724 |
| 73 | `res/drawable-anydpi-v21/exo_icon_stop.xml` | res | 660 |
| 74 | `res/drawable-anydpi-v21/ic_call_answer.xml` | res | 1356 |
| 75 | `res/drawable-anydpi-v21/ic_call_answer_low.xml` | res | 1320 |
| 76 | `res/drawable-anydpi-v21/ic_call_answer_video.xml` | res | 772 |
| 77 | `res/drawable-anydpi-v21/ic_call_answer_video_low.xml` | res | 736 |
| 78 | `res/drawable-anydpi-v21/ic_call_decline.xml` | res | 1528 |
| 79 | `res/drawable-anydpi-v21/ic_call_decline_low.xml` | res | 1492 |
| 80 | `res/drawable-anydpi-v21/ic_launcher_background.xml` | res | 5696 |
| 81 | `res/drawable-anydpi-v21/im_back.xml` | res | 860 |
| 82 | `res/drawable-anydpi-v21/im_close_button.xml` | res | 1064 |
| 83 | `res/drawable-anydpi-v21/im_close_icon.xml` | res | 800 |
| 84 | `res/drawable-anydpi-v21/im_close_transparent.xml` | res | 684 |
| 85 | `res/drawable-anydpi-v21/im_forward_active.xml` | res | 844 |
| 86 | `res/drawable-anydpi-v21/im_forward_inactive.xml` | res | 928 |
| 87 | `res/drawable-anydpi-v21/pob_ad_icon.xml` | res | 1280 |
| 88 | `res/drawable-anydpi-v21/pob_ad_info_icon.xml` | res | 1208 |
| 89 | `res/drawable-anydpi-v21/pob_ad_info_icon_native.xml` | res | 960 |
| 90 | `res/drawable-anydpi-v21/pob_ic_close_black_24dp.xml` | res | 684 |
| 91 | `res/drawable-anydpi-v21/pob_ic_forward_24.xml` | res | 600 |
| 92 | `res/drawable-anydpi-v21/pob_ic_volume_off_black_24dp.xml` | res | 984 |
| 93 | `res/drawable-anydpi-v21/pob_ic_volume_up_black_24dp.xml` | res | 796 |
| 94 | `res/drawable-anydpi-v21/pob_privacy_icon.xml` | res | 2540 |
| 95 | `res/drawable-anydpi-v24/applovin_ic_mediation_yso.xml` | res | 3368 |
| 96 | `res/drawable-anydpi-v24/cb_close_icon.xml` | res | 1004 |
| 97 | `res/drawable-anydpi-v24/cb_info_icon.xml` | res | 916 |
| 98 | `res/drawable-anydpi-v24/cb_skip_icon.xml` | res | 1236 |
| 99 | `res/drawable-anydpi-v24/cb_volume_off_icon.xml` | res | 1376 |
| 100 | `res/drawable-anydpi-v24/cb_volume_on_icon.xml` | res | 1628 |
| 101 | `res/drawable-anydpi-v24/ic_palette_color.xml` | res | 1180 |
| 102 | `res/drawable-anydpi-v24/im_mute.xml` | res | 1464 |
| 103 | `res/drawable-anydpi-v24/im_pause.xml` | res | 1116 |
| 104 | `res/drawable-anydpi-v24/im_play.xml` | res | 1092 |
| 105 | `res/drawable-anydpi-v24/im_refresh.xml` | res | 1204 |
| 106 | `res/drawable-anydpi-v24/im_unmute.xml` | res | 1156 |
| 107 | `res/drawable-anydpi-v24/moloco_skip.xml` | res | 768 |
| 108 | `res/drawable-anydpi-v24/monetization_ads_internal_instream_ic_sound_off_v2_focused.xml` | res | 1400 |
| 109 | `res/drawable-anydpi-v24/monetization_ads_internal_instream_ic_sound_on_v2_focused.xml` | res | 1848 |
| 110 | `res/drawable-anydpi-v24/monetization_ads_internal_instream_trademark_icon.xml` | res | 1000 |
| 111 | `res/drawable-hdpi-v4/btn_ogury_mraid_close.png` | res | 1044 |
| 112 | `res/drawable-hdpi-v4/common_full_open_on_phone.png` | res | 681 |
| 113 | `res/drawable-hdpi-v4/ia_close.png` | res | 147 |
| 114 | `res/drawable-hdpi-v4/ia_collapse.png` | res | 254 |
| 115 | `res/drawable-hdpi-v4/ia_expand.png` | res | 246 |
| 116 | `res/drawable-hdpi-v4/ia_mute.png` | res | 565 |
| 117 | `res/drawable-hdpi-v4/ia_play.png` | res | 1845 |
| 118 | `res/drawable-hdpi-v4/ia_unmute.png` | res | 557 |
| 119 | `res/drawable-ldrtl-mdpi-v17/abc_spinner_mtrl_am_alpha.9.png` | res | 318 |
| 120 | `res/drawable-mdpi-v4/abc_ab_share_pack_mtrl_alpha.9.png` | res | 267 |
| 121 | `res/drawable-mdpi-v4/abc_btn_check_to_on_mtrl_000.png` | res | 214 |
| 122 | `res/drawable-mdpi-v4/abc_btn_check_to_on_mtrl_015.png` | res | 321 |
| 123 | `res/drawable-mdpi-v4/abc_btn_radio_to_on_mtrl_000.png` | res | 324 |
| 124 | `res/drawable-mdpi-v4/abc_btn_radio_to_on_mtrl_015.png` | res | 356 |
| 125 | `res/drawable-mdpi-v4/abc_btn_switch_to_on_mtrl_00001.9.png` | res | 754 |
| 126 | `res/drawable-mdpi-v4/abc_btn_switch_to_on_mtrl_00012.9.png` | res | 825 |
| 127 | `res/drawable-mdpi-v4/abc_cab_background_top_mtrl_alpha.9.png` | res | 216 |
| 128 | `res/drawable-mdpi-v4/abc_ic_commit_search_api_mtrl_alpha.png` | res | 173 |
| 129 | `res/drawable-mdpi-v4/abc_list_divider_mtrl_alpha.9.png` | res | 167 |
| 130 | `res/drawable-mdpi-v4/abc_list_focused_holo.9.png` | res | 222 |
| 131 | `res/drawable-mdpi-v4/abc_list_longpressed_holo.9.png` | res | 211 |
| 132 | `res/drawable-mdpi-v4/abc_list_pressed_holo_dark.9.png` | res | 207 |
| 133 | `res/drawable-mdpi-v4/abc_list_pressed_holo_light.9.png` | res | 207 |
| 134 | `res/drawable-mdpi-v4/abc_list_selector_disabled_holo_dark.9.png` | res | 217 |
| 135 | `res/drawable-mdpi-v4/abc_list_selector_disabled_holo_light.9.png` | res | 217 |
| 136 | `res/drawable-mdpi-v4/abc_menu_hardkey_panel_mtrl_mult.9.png` | res | 541 |
| 137 | `res/drawable-mdpi-v4/abc_popup_background_mtrl_mult.9.png` | res | 776 |
| 138 | `res/drawable-mdpi-v4/abc_scrubber_control_off_mtrl_alpha.png` | res | 159 |
| 139 | `res/drawable-mdpi-v4/abc_scrubber_control_to_pressed_mtrl_000.png` | res | 145 |
| 140 | `res/drawable-mdpi-v4/abc_scrubber_control_to_pressed_mtrl_005.png` | res | 197 |
| 141 | `res/drawable-mdpi-v4/abc_scrubber_primary_mtrl_alpha.9.png` | res | 203 |
| 142 | `res/drawable-mdpi-v4/abc_scrubber_track_mtrl_alpha.9.png` | res | 194 |
| 143 | `res/drawable-mdpi-v4/abc_spinner_mtrl_am_alpha.9.png` | res | 327 |
| 144 | `res/drawable-mdpi-v4/abc_switch_track_mtrl_alpha.9.png` | res | 395 |
| 145 | `res/drawable-mdpi-v4/abc_tab_indicator_mtrl_alpha.9.png` | res | 186 |
| 146 | `res/drawable-mdpi-v4/abc_text_select_handle_left_mtrl.png` | res | 203 |
| 147 | `res/drawable-mdpi-v4/abc_text_select_handle_middle_mtrl.png` | res | 310 |
| 148 | `res/drawable-mdpi-v4/abc_text_select_handle_right_mtrl.png` | res | 186 |
| 149 | `res/drawable-mdpi-v4/abc_textfield_activated_mtrl_alpha.9.png` | res | 181 |
| 150 | `res/drawable-mdpi-v4/abc_textfield_default_mtrl_alpha.9.png` | res | 178 |
| 151 | `res/drawable-mdpi-v4/abc_textfield_search_activated_mtrl_alpha.9.png` | res | 178 |
| 152 | `res/drawable-mdpi-v4/abc_textfield_search_default_mtrl_alpha.9.png` | res | 178 |
| 153 | `res/drawable-mdpi-v4/browser_back.png` | res | 128 |
| 154 | `res/drawable-mdpi-v4/browser_back_disabled.png` | res | 152 |
| 155 | `res/drawable-mdpi-v4/browser_background.png` | res | 164 |
| 156 | `res/drawable-mdpi-v4/browser_circle_close.png` | res | 701 |
| 157 | `res/drawable-mdpi-v4/browser_close.png` | res | 193 |
| 158 | `res/drawable-mdpi-v4/browser_core_lock.png` | res | 154 |
| 159 | `res/drawable-mdpi-v4/browser_core_refresh.png` | res | 264 |
| 160 | `res/drawable-mdpi-v4/browser_forward.png` | res | 126 |
| 161 | `res/drawable-mdpi-v4/browser_forward_disabled.png` | res | 150 |
| 162 | `res/drawable-mdpi-v4/browser_open_in_browser.png` | res | 197 |
| 163 | `res/drawable-mdpi-v4/common_google_signin_btn_icon_dark_normal_background.9.png` | res | 610 |
| 164 | `res/drawable-mdpi-v4/common_google_signin_btn_icon_light_normal_background.9.png` | res | 500 |
| 165 | `res/drawable-mdpi-v4/common_google_signin_btn_text_dark_normal_background.9.png` | res | 615 |
| 166 | `res/drawable-mdpi-v4/common_google_signin_btn_text_light_normal_background.9.png` | res | 465 |
| 167 | `res/drawable-mdpi-v4/exo_icon_circular_play.png` | res | 672 |
| 168 | `res/drawable-mdpi-v4/exo_icon_vr.png` | res | 207 |
| 169 | `res/drawable-mdpi-v4/googleg_disabled_color_18.png` | res | 281 |
| 170 | `res/drawable-mdpi-v4/googleg_standard_color_18.png` | res | 562 |
| 171 | `res/drawable-mdpi-v4/ic_more_vert_white_24dp.png` | res | 127 |
| 172 | `res/drawable-mdpi-v4/moloco_privacy.png` | res | 607 |
| 173 | `res/drawable-mdpi-v4/monetization_ads_internal_ic_sound_off_fullscreen_content_v1.png` | res | 459 |
| 174 | `res/drawable-mdpi-v4/monetization_ads_internal_ic_sound_off_fullscreen_content_v2.png` | res | 744 |
| 175 | `res/drawable-mdpi-v4/monetization_ads_internal_ic_sound_on_fullscreen_content_v1.png` | res | 511 |
| 176 | `res/drawable-mdpi-v4/monetization_ads_internal_ic_sound_on_fullscreen_content_v2.png` | res | 680 |
| 177 | `res/drawable-mdpi-v4/monetization_ads_internal_star_black_16dp.png` | res | 193 |
| 178 | `res/drawable-mdpi-v4/monetization_ads_internal_star_half_black_16dp.png` | res | 146 |
| 179 | `res/drawable-mdpi-v4/monetization_ads_video_ic_replay.png` | res | 1245 |
| 180 | `res/drawable-mdpi-v4/mraid_close.png` | res | 2373 |
| 181 | `res/drawable-mdpi-v4/notification_bg_low_normal.9.png` | res | 215 |
| 182 | `res/drawable-mdpi-v4/notification_bg_low_pressed.9.png` | res | 223 |
| 183 | `res/drawable-mdpi-v4/notification_bg_normal.9.png` | res | 215 |
| 184 | `res/drawable-mdpi-v4/notification_bg_normal_pressed.9.png` | res | 223 |
| 185 | `res/drawable-mdpi-v4/notify_panel_notification_icon_bg.png` | res | 98 |
| 186 | `res/drawable-mdpi-v4/overflow_menu_button.png` | res | 112 |
| 187 | `res/drawable-mdpi-v4/smaato_sdk_core_back.png` | res | 128 |
| 188 | `res/drawable-mdpi-v4/smaato_sdk_core_back_disabled.png` | res | 152 |
| 189 | `res/drawable-mdpi-v4/smaato_sdk_core_background.png` | res | 164 |
| 190 | `res/drawable-mdpi-v4/smaato_sdk_core_circle_close.png` | res | 701 |
| 191 | `res/drawable-mdpi-v4/smaato_sdk_core_close.png` | res | 193 |
| 192 | `res/drawable-mdpi-v4/smaato_sdk_core_forward.png` | res | 126 |
| 193 | `res/drawable-mdpi-v4/smaato_sdk_core_forward_disabled.png` | res | 150 |
| 194 | `res/drawable-mdpi-v4/smaato_sdk_core_lock.png` | res | 154 |
| 195 | `res/drawable-mdpi-v4/smaato_sdk_core_open_in_browser.png` | res | 197 |
| 196 | `res/drawable-mdpi-v4/smaato_sdk_core_refresh.png` | res | 264 |
| 197 | `res/drawable-mdpi-v4/smaato_sdk_core_watermark.png` | res | 719 |
| 198 | `res/drawable-mdpi-v4/smaato_sdk_native_ic_privacy.png` | res | 302 |
| 199 | `res/drawable-mdpi-v4/smaato_sdk_video_muted.png` | res | 990 |
| 200 | `res/drawable-mdpi-v4/smaato_sdk_video_skip.png` | res | 637 |
| 201 | `res/drawable-mdpi-v4/smaato_sdk_video_unmuted.png` | res | 826 |
| 202 | `res/drawable-night-anydpi-v21/applovin_ic_baseline_add_circle_outline.xml` | res | 724 |
| 203 | `res/drawable-night-anydpi-v21/applovin_ic_warning_outline.xml` | res | 1128 |
| 204 | `resources.arsc` | resource-table | 48052 |
| 205 | `stamp-cert-sha256` | other | 32 |

## 9.3 Inventarisasi SELURUH sumber Java hasil dekompilasi (13263 file)

Setiap file Java dihitung dalam tabel paket berikut. Kode aplikasi (113 class) dianalisis per-method di Bagian 2–4; PairIP (22 class) di Bagian 6; sisanya adalah library pihak ketiga yang diinventaris per paket di Bagian 5. Tidak ada file yang dibuang dari inventaris.

| Paket (2 level pertama) | Jumlah file |
|---|---:|
| `com/google` | 3515 |
| `com/fyber` | 1526 |
| `com/smaato` | 875 |
| `com/startapp` | 853 |
| `com/moloco` | 740 |
| `com/unity3d` | 731 |
| `sg/bigo` | 705 |
| `org/bidon` | 618 |
| `com/my` | 602 |
| `net/pubnative` | 549 |
| `com/ogury` | 534 |
| `org/checkerframework` | 346 |
| `com/pubmatic` | 345 |
| `com/taurusx` | 284 |
| `kotlinx/serialization` | 100 |
| `net/lingala` | 98 |
| `com/ironsource` | 94 |
| `okhttp3/internal` | 90 |
| `com/monetization` | 76 |
| `com/pgl` | 64 |
| `okio` | 58 |
| `org/chromium` | 58 |
| `com/mobilefuse` | 53 |
| `com/tiktok` | 41 |
| `com/squareup` | 38 |
| `okhttp3` | 37 |
| `org/slf4j` | 29 |
| `org/jetbrains` | 20 |
| `com/hextetrahedral` | 15 |
| `okio/internal` | 14 |
| `y2` | 12 |
| `z` | 12 |
| `org/intellij` | 10 |
| `t` | 10 |
| `s0` | 9 |
| `d0` | 7 |
| `f0` | 7 |
| `i1` | 7 |
| `y` | 7 |
| `k1` | 6 |
| `c1` | 4 |
| `com/qozix` | 4 |
| `g1` | 4 |
| `org/jspecify` | 4 |
| `b0` | 3 |
| `h0` | 3 |
| `s` | 3 |
| `x` | 3 |
| `a0` | 2 |
| `c0` | 2 |
| `d1` | 2 |
| `e0` | 2 |
| `e1` | 2 |
| `f1` | 2 |
| `i0` | 2 |
| `w1` | 2 |
| `z0` | 2 |
| `a1` | 1 |
| `a3` | 1 |
| `b1` | 1 |
| `b2` | 1 |
| `d2` | 1 |
| `h1` | 1 |
| `j1` | 1 |
| `m1` | 1 |
| `o1` | 1 |
| `p1` | 1 |
| `q` | 1 |
| `r` | 1 |
| `r2` | 1 |
| `s1` | 1 |
| `s2` | 1 |
| `t2` | 1 |
| `u` | 1 |
| `v2` | 1 |
| `w` | 1 |
| `w0` | 1 |
| `x1` | 1 |
| `x2` | 1 |
| **TOTAL** | **13263** |
# Bagian 10 — Mekanisme referensi untuk keluhan UI/UX (analisis mendalam)

> Bagian ini menjawab secara spesifik lima keluhan terhadap `NBTEDITOR.apk`: (1) tidak ada ikon/gambar tipe tag, (2) tidak ada Show Map, (3) mekanisme kaku, (4) tidak memakai file manager bawaan OS, (5) layar Edit seharusnya memunculkan panel geser samping (aside), serta (6) tata letak tidak center. Semua dijelaskan **cara kerja di XAPK** — kelas, resource, alur, dan angka — agar bisa dipakai sebagai spesifikasi perbaikan.

## 10.1 Ikon / logo jenis data tag (I, B, dll.)

**Resource milik aplikasi:** `res/drawable/nbt_byte`, `nbt_short`, `nbt_int`, `nbt_long`, `nbt_float`, `nbt_double`, `nbt_string`, `nbt_list`, `nbt_compound`, `nbt_byte_arr`, `nbt_int_arr` (11 drawable). Tidak ada `nbt_long_arr` — referensi memang hanya 12 tipe (TAG_End bukan kelas).

**Pemasangan di baris tree** (`res/layout/tree_node_item.xml`, xmltree):
```
LinearLayout id=@id/node_container  — horizontal, minHeight=48dp,
  paddingHorizontal=12dp, paddingVertical=6dp, gravity=center_vertical,
  background=?attr/selectableItemBackground, focusable=true, clickable=true
 ├── ImageView id=@id/node_icon  — 32×32dp, contentDescription="icon", marginEnd=12dp
 └── TextView  id=@id/key_text  — 18sp, bold, maxLines=1, ellipsize=end, placeholder "Key"
```

**Pemilihan ikon di adapter** (`h0/c.java`, `onBindViewHolder`) — sumber kebenaran pemetaan ordinal `x2.a` → drawable:

| Ordinal / tipe | Ikon resource | Ordinal / tipe | Ikon resource |
|---:|---|---:|---|
| 1 BYTE | `2131231652` | 7 BYTE_ARRAY | `2131231653` |
| 2 SHORT | `2131231661` | 8 STRING | `2131231662` |
| 3 INT | `2131231657` | 9 LIST | `2131231659` |
| 4 LONG | `2131231660` | 10 COMPOUND | `2131231654` |
| 5 FLOAT | `2131231656` | 11 INT_ARRAY | `2131231658` |
| 6 DOUBLE | `2131231655` | elemen array (g==null, parent 7/11) | ikon Byte/Int dari parent |

Selain ikon, `h0.c` mewarnai latar kartu dengan `ColorUtils.blendARGB(-1, warnaBlok, 0.3f)` untuk layer (`z/i`, `CreateWorldActivity`), bukan untuk tag NBT. Type icon bersifat **statik per tipe**, tidak berubah saat nilai diedit.

**Mekanisme ringkas:** `f0.g.c(node)` membuat adapter `h0.c` baru untuk node saat ini → `onBindViewHolder` membaca `dVar.c.j().ordinal()` (atau tipe induk untuk elemen array) → `setImageResource(...)` → baris 32dp + 12dp gap + key 18sp bold. Inilah sebabnya pengguna langsung mengenali "I", "B", dll. secara visual.

## 10.2 Show Map — cara kerja lengkap (`d0.*`, 7 file, ~760 baris)

### 10.2.1 Komponen
| File | Peran |
|---|---|
| `d0/e.java` (431) | Fragment `Show Map`; orkestrasi TileView + overlay + gesture + dialog |
| `d0/a.java` (142) | `BitmapProvider` + gesture callback; render tiap tile 256px |
| `d0/b.java` (73) | `Consumer<u.a>` penempatan marker/ikon entity; scroll ke entity |
| `d0/g.java` (58) | Subclass TileView kustom; meneruskan long-press/tap/scroll |
| `d0/i.java` (160) | Overlay seleksi (FrameLayout + 4 handle drag) |
| `d0/f.java`, `d0/h.java` (5+5) | Interface marker (long-press listener & selection-change stub) |

### 10.2.2 Inisialisasi (d0/e.onCreateView)
TileView Qozix (`com.qozix.tileview.*`) dibangun manual: `Scroller`, `GestureDetector`, `ScaleGestureDetector`, tile renderer + cache, transform `d1.a`, manajer tile dengan thread pool (`LinkedBlockingDeque` + `Looper.getMainLooper()`), layer path, layer overlay, marker selection, marker tambahan. Konfigurasi:
- world width/height = `h = i = 536870912/16 = 2^25` (33.554.432)
- skala awal `d1.b.s(...)`; level zoom `{0.0625,0.125,0.25,0.5,1.0}`; **min scale 0.2**, max 1.0; `setScale(0.5f)`
- latar `#FF444444`; `setShouldLoopScale(false)`, transisi & render-saat-pan dimatikan; `setBitmapProvider(new a(this))`; `setOnLongPressListener(new a(this))`.
- Overlay `d0/i` disembunyikan (`GONE`), state dipulihkan dari bundle `map_save` / `selection_save`, lalu `post(new d.a(this,bundle,1))` menjadwalkan render marker.

### 10.2.3 Render tile → blok (`d0/a.a(i1.a, Context)`)
```
f = aVar.o.b (scale)
iRound  = round(1/f);  iRound2 = round(16*f)   // px per chunk
i2 = iRound2*1048576/256;  i4 = (aVar.f-i2)*iRound  // chunkX awal
i5 = (aVar.e-i3)*iRound                          // chunkZ awal
bitmap RGB_565 256×256, checkerboard 16×16:
   ((cx+cz)&1)==1 ? 0xFF2E2E2E : 0xFF444444
data chunk: x.b.a().b(chunkX, chunkZ, 1)          // dimension 1 = Overworld
e0.a.b(chunk, canvas, x, y, w, h, paint)          // render 16×16 kolom blok
border putih 1px (256 px tepi) + teks koordinat "(x, z)" 24sp putih
```
Dedup marker: `long key = (chunkX<<32) | (chunkZ & 0xFFFFFFFF)` disimpan di `HashSet g`; baru diproses sekali. Semua dibungkus `try/catch` → `System.err.println("ERROR: " + e.getMessage())`, peta tetap tampil walau sebagian rusak.

### 10.2.4 Gesture & koordinat (`d0/a.b(MotionEvent)`)
```
scale = tileView.getScale();  f = 16*scale
x = (event.getX()+scrollX)/(16*scale) - 1048576.0f
z = (event.getY()+scrollY)/(16*scale) - 1048576.0f
chunkX = floorDiv(x,16); chunkZ = floorDiv(z,16)
bundle worldX=x, worldZ=z → SimpleListDialog "mapview_options_dialog"
```
- Tanpa seleksi: judul `Coords(x, z) Chunk(cx, cz)` → **Begin Selection** / **View Entities**.
- Dengan seleksi: `Selection Options` → **Take Picture** / **End Selection**.
- `Begin Selection`: `round((min(w,h)/4)/(scale*16))` → `Rect` blok, overlay VISIBLE.
- `Take Picture`: tolak ≤0 (`"No selection made."`) atau skala <1 (`"Selection is too large."`); slider `z/l` MIN=1 MAX≤24; hasil disimpan via `ContentResolver` (request code 14); toast `"Successfully saved picture."` / `"Error saving picture."`.
- `View Entities`: `Intent(EntitiesNBTActivity)` extra `chunkX=worldX/16`, `chunkZ=worldZ/16`.

### 10.2.5 Marker entity (`d0/e.b(s.a)`)
1. `cacheDir/entityIcons` dibuat; kalau kosong → `d1.b.d(assets,"entityIcons",file)` (ekstrak 82 PNG).
2. Tiap key entitas (`actorprefix`+UUID) → NBT → `identifier` → `t.f.d.get(...)` → nama file ikon.
3. `Pos` list float → elemen [0]=X, [2]=Z; fraksi = blok/1048576 (`2^20`); `d0/b.accept` normalisasi `(x+1)/2` → `0..1` → `×2^25` → scale TileView; ikon di-scale 42×42, cache key `nama+"_42"`; placeholder 28px.
4. Bila tidak ada Bundle (kasus fokus ke entity), TileView `scrollTo(centered)`.

### 10.2.6 Lifecycle
`onPause`: hapus handler, lepas bitmap tile, `setWillNotDraw(true)`; `onResume`: pulihkan + `requestLayout`; `onDestroy`: shutdown thread pool tile. State disimpan di `map_save` & `selection_save`.

### 10.2.7 Analisis ulang byte-level (koreksi terhadap JADX)

Analisis mendalam ulang terhadap `d0/a.java`, `s/a.java`, `s/c.java`, `e0/a.java`, dan **smali** `t/c.smali` / `s/a.smali` menghasilkan spesifikasi yang lebih tepat (Bagian 10.2 sebelumnya tetap valid sebagai ringkasan, tetapi poin-poin ini adalah sumber kebenaran):

**Dua jebakan dekompiler JADX:**
1. `t/c.java` kehilangan kanal MERAH pada warna blok — contoh `Qi(85,167,…)` untuk air. Smali `t/c.smali` menunjukkan konstruktor `(IIIILjava/lang/String;…)`, `Color.rgb(p2,p3,p4)` → air = **(46,85,167)**, stone = (125,125,125), dirt = (134,96,67), bedrock = (85,85,85), air = (255,255,255). Tabel warna harus diambil dari smali (1.104 entitas), bukan dari JADX.
2. `s/a.java:347` JADX menulis `if (iA >= 0 || iA >= strArr.length)` (terbalik). Smali `s/a.smali:1356-1366` benar: `if (iA < 0 || iA >= panjang) skip; else pakai palette[iA]`. Mengikuti teks JADX membuat semua blok terlewat → checkerboard saja.

**LevelDB & chunk (dimensi 1 / Overworld):**
- Heightmap: key **9 byte** = `LE32(chunkX) | LE32(chunkZ) | 0x2B`; 256 entri LE uint16, indeks **`z*16+x`**, 512 byte pertama. `Y_top = -64 + nilai` (`minY` dari `net.pubnative…a.d(1) = -64`), tanpa +1.
- Subchunk: key **10 byte** = `LE32(cx) | LE32(cz) | 0x2F | (byte)floorDiv(Y,16)` — byte index **signed 8-bit**.
- Decode subchunk: versi `{1,8,9}`; v8/v9 baca jumlah storage (1/2); v9 Konsumsi 1 byte chunk-index; per storage: `v=read()`, `bits = v >>> 1` (bit rendah = persistence).
- **Panjang data packed = `ceil(4096.0 / (32 / bits)) * 4` dengan `32 / bits` pembagian INTEGER** (bits=5 → 683 word = 2732 byte, BUKAN 2560). Ini kebiasaan Bedrock; salah menghitung menggeser palet.
- Ukuran palet = 4 byte LE (maks 4096). **Tiap entri palet adalah NBT Compound lengkap** (bukan string polos): `d1.b.o(false, in)` → `y2.c` → child pertama `y2.k` (TAG_String) → value = identifier blok. Seluruh compound harus dikonsumsi sebelum entri berikutnya.
- Ekstraksi index blok: `word = LE32(b[w..w+3]); pal = (word >>> ((i % (32/bits))*bits)) & ((1<<bits)-1)`; `i = (x << 8) | (z << 4) | localY` (**X-major**, bukan Y-major). Hanya storage ke-0 yang dipakai.
- Udara = tepat `"minecraft:air"`; `cave_air`/`void_air` dianggap blok tak dikenal → magenta `0xFFFF00FF`.

**Warna & shading (`e0.a.a`, `s.a.d`):**
`f = clamp(1 + 0.05*((Y−Y_west)+(Y−Y_north)), 0.5, 1.5)`; warna = `Color.rgb(clamp(r*f), clamp(g*f), clamp(b*f))`. Barat = chunk `(cx−1,cz)` kolom `x==0 ? 15 : x−1`; Utara = chunk `(cx,cz−1)` kolom `z==0 ? 15 : z−1`. Jika tetangga tak ada → pakai Y sendiri (f=1). Bitmap tile RGB_565 256×256, checkerboard `0xFF2E2E2E`/`0xFF444444`, border putih 1px, teks koordinat putih 24sp di (5,5).

**Gejala jika salah:** palet gagal dibaca / index Y-major / pembagian `32/bits` float → semua kolom jadi air (biru) atau checkerboard; tabel JADX → warna salah; `>= 0` JADX → checkerboard semua.

## 10.3 File manager bawaan OS (bukan browser internal)

Referensi **tidak punya** file browser sendiri untuk import. Alur `d1.b.m(Context,int)`:
```java
Intent intent = new Intent("android.intent.action.GET_CONTENT");
intent.setType("*/*");
intent.putExtra("android.intent.extra.MIME_TYPES", new String[]{
  "application/octet-stream", "application/x-mcpack", "application/x-mcworld",
  "application/x-mctemplate", "application/x-mcaddon"});
intent.addCategory("android.intent.category.OPENABLE");
startActivityForResult(Intent.createChooser(intent, "Select a .<ext> file"), b(type));
```
- `b(type)` request code: **1001..1004**; `c(type)` (**2001..2004**) = jalur "buka dengan aplikasi lain".
- `onActivityResult`: baca `_display_name` lewat `ContentResolver.query` untuk skema `content://`, fallback `Uri.getLastPathSegment()`; validasi `endsWith("."+ext)` → Toast `"Please select a valid .<ext> file"`.
- File disalin ke `<externalFilesDir>/import.<ext>` (`d1.b.l`) lalu di-extract dengan zip4j ke `worlds/<nama>` (`d1.b.n`).
- `MainActivity` juga tidak menampilkan direktori bebas; `res/xml/file_paths.xml` hanya `<external-files-path name="external_files" path="."/>` untuk FileProvider (share/export).

**Kontras dengan NBTEDITOR kita:** kita memakai `S_BROWSER` + `buildBrowser()` (`/sdcard`, `A_BROWSE_UP`, `A_FILE_SELECT`) — file manager buatan sendiri. Untuk meniru referensi, tombol Open/Import harus memakai **SAF/ACTION_GET_CONTENT** (atau `ACTION_OPEN_DOCUMENT` di API 26+) dan hasil `Uri` disalin via `ContentResolver`.

**STATUS (v1.2.2): Show Map sudah diimplementasikan penuh** — `libleveldb.so` referensi disertakan dan dipanggil lewat JNI exports; `decodeChunkTop()` membaca heightmap+palet, mewarnai 16×16 kolom, checkerboard + border + label koordinat, pan/tap-dialog, terpusat ke SpawnX/SpawnZ. Marker entity ikon belum dipetakan (t.f 123 entitas). Detail: `DOKUMENTASI.md` §8.

**STATUS (v1.1.0): SAF sudah diimplementasikan.** `FilePickerActivity.java` (DEX 5.464 byte) menjalankan persis mekanisme di atas; `A_WORLD_IMPORT`/`A_OPEN_NBT` tidak lagi membuka `S_BROWSER`. Browser internal tetap dipertahankan hanya untuk **Save-As**. Jembatan native: `launchFilePicker()` → JNI `startActivityForResult` → `onFilePicked()` → `A_PICK_RESULT` → `startWorldImport(tempCopy=true)` → hapus temp copy. Verifikasi emulator: `DocumentsUI PickActivity` tampil dan `SAFTest.mcworld` berhasil menjadi world **SAF Test World**. Detail: `DOKUMENTASI.md` Bagian D/§7.

## 10.4 Panel geser samping (aside/drawer) saat Edit

Referensi memakai **`androidx.drawerlayout.widget.DrawerLayout` + `NavigationView`**, bukan panel buatan:

`res/layout/activity_drawer.xml`:
```
DrawerLayout id=@id/drawer_layout (match_parent)
├── LinearLayout vertikal
│   ├── AppBarLayout → Toolbar id=@id/toolbar (?colorPrimary, title=worldName, white)
│   └── FrameLayout id=@id/fragment_container (match_parent)
└── NavigationView id=@id/nav_view, gravity=start,
      width=wrap_content, height=match_parent,
      headerLayout=@layout/drawer_header, menu=@menu/drawer_menu
```
`drawer_header.xml`: LinearLayout **160dp**, primary, padding 16dp, `gravity=center_vertical`; ImageView 64dp; TextView 20sp white; TextView subtitle white.

Kode `DrawerActivity`:
- `ActionBarDrawerToggle(this, drawer, toolbar, 2131952114, 2131952113)` + `syncState()` → tombol hamburger di toolbar membuka/tutup drawer.
- `setNavigationItemSelectedListener` → `onNavigationItemSelected`: item yang sama = `closeDrawer(GravityCompat.START)`; `nav_change_world` → dialog; editor aktif → `Close NBT Editor`; else `replace(fragment_container, fragment, tag)` + `closeDrawer`.
- `onBackPressedDispatcher` (`w.a` mode 0): tutup drawer dulu → breadcrumb → dialog close editor/world.
- Drawer **menimpa sebagian layar** (wrap_content, dari kiri/start), konten utama tetap terlihat di bawah scrim — itulah "geser samping/aside" yang dimaksud.

`res/menu/drawer_menu.xml`: grup checkableBehavior=2 (single) — 5 item utama + submenu **Advanced** berisi 7 item; ikon `ic_map`, `ic_tune`, `ic_person`, `ic_group_people`, `ic_exit`, `ic_list`, `ic_search`.

**Kontras dengan NBTEDITOR kita:** kita langsung `setScreen(S_TREE)` saat edit; tidak ada `DrawerLayout`, tidak ada NavigationView, tidak ada panel geser. Spesifikasi perbaikannya: bungkus editor dengan layout drawer, pindahkan `buildDrawer()` (header world + 12 item) ke `NavigationView` equivalen di C++ (panel geser dari kiri dengan scrim), dan jadikan tap Edit = buka drawer + frame editor.

**STATUS (v1.7.6): Persistent Background During Aside Overlay.** Scene tidak pernah di-reset — `buildMap()` menggambar peta lalu menumpuk drawer di atasnya dalam satu `renderFrame`. Background hitam berasal dari rasteriser yang menulis RGBA mentah (`rgba(0,0,0,0x60)` → hitam opak), bukan dari scene stacking. `Canvas::blendPixel` kini melakukan alpha-blend untuk semua primitive transparan; scrim drawer `0x50` menghasilkan dim alih-alih hitam. Scroll aside memakai target + easing eksponensial berbasis delta-time (`~70ms`) dan pelacakan jari absolut. Detail: `DOKUMENTASI.md` §14.

**STATUS (v1.7.5): UI/UX aside sesuai NavigationView.** Pita status bar 24dp kini primary (bukan putih), ikon menu di-rasterisasi dari vector drawable asli XAPK (`ic_map`, `ic_tune`, `ic_person`, `ic_group_people`, `ic_exit`, `ic_list`, `ic_search` → `scripts/gen_nav_icons.py`, 28 PNG light/dark + selected), baris 48dp dengan daftar scrollable (tidak ada item yang terpotong), item aktif tersorot (checkableBehavior=single), semua item menutup drawer saat dipilih, dan gesture vertikal di dalam panel tidak lagi menggeser peta. Detail: `DOKUMENTASI.md` §13.

**STATUS (v1.7.0): entity PNG + save + ANR diperbaiki.** 83 PNG `entityIcons` dari XAPK dipakai sebagai marker peta lewat `AAssetManager` + stb_image; entity key = `"digp"` → 8-byte id → `"actorprefix"+id` (sesuai `s.a.c()`), 123 mapping dari `t.f`. LevelDB NBT kini bisa disimpan (`nativePut`), SAF menulis balik ke Uri, world scan & entity scan di worker (ANR hilang), `JNIEnv` thread-local. Detail: `DOKUMENTASI.md` §10.

**STATUS (v1.5.1): BACK langsung ke beranda.** `goHome()` dipakai oleh `back()` dan `A_NAV_CHANGE_WORLD`; modal ditutup lebih dulu, konfirmasi `"Close NBT Editor"` hanya saat `ed.dirty`, BACK dari home baru `wantExit`. `MainActivity.java` + `enableOnBackInvokedCallback="false"` menjamin event BACK diterima dispatcher native. Detail: `DOKUMENTASI.md` §9.

**STATUS (v1.3.0): sudah diimplementasikan dengan alur yang benar.** Tap world → aside **Options** (`MainActivity` `select_world_options_dialog`); tap **Edit** → `S_MAP` dengan drawer **tertutup** (persis `DrawerActivity.onCreate`, tidak ada `openDrawer()`); **swipe dari tepi kiri 28–56dp ke kanan → aside grup** (5 utama + Advanced 7 + Actions); geser horizontal di area lain → pan peta X+Y (`k1.f.onScroll`). Back menutup drawer dulu. Header drawer 160dp memakai world icon asli. Status bar 24dp direservasi agar judul tidak menimpa jam. Detail: `DOKUMENTASI.md` §8 koreksi v1.3.0.

## 10.5 Tata letak & centering (kenapa UI kita terasa "kacau")

**Teknik referensi (dari xmltree, Bagian 5 §4):**
| Layar | Strategi |
|---|---|
| Semua | `CoordinatorLayout` + `AppBarLayout` + konten `match_parent` dengan `layout_behavior=AppBarLayout$ScrollingViewBehavior` |
| Home | `RecyclerView match_parent` + `FAB menu gravity=bottom\|end, margin=16dp`; child holder `gravity=center`, label sebelum FAB |
| World card | `CardView` + `LinearLayout horizontal, padding=12dp, gravity=center_vertical`; ikon 48dp, `marginEnd=12dp`; teks `layout_weight=1` → sisa ruang terisi, `maxLines=1` + `ellipsize=end` |
| Tree row | `minHeight=48dp`, `gravity=center_vertical`, ikon 32dp + gap 12dp, key `weight=1` |
| Drawer header | `gravity=center_vertical`, ikon 64dp, teks `maxLines=1` |
| Help/SimpleDialog | `ScrollView` + `LinearLayout` vertikal; teks 16sp; gambar `FIT_CENTER` + `adjustViewBounds` |
| Form | `RecyclerView weight=1` + tombol bawah tetap; `ImageButton 48×48dp`, `tint` |

Prinsipnya: **hampir tidak ada koordinat absolut** — `match_parent`, `wrap_content`, `weight`, `gravity`, `margin` konsisten 8/12/16dp. Toolbar seluruhnya `?colorPrimary` + `titleTextColor=@android:color/white`.

**NBTEDITOR kita:** hampir semua posisi dihitung manual (`Rect{x, y, w, h}`) di `buildHome/buildWorlds/buildTree/...`; `a.scrollY`, `kPad`, `kRowH`, `a.h - 122`, dll. Akibatnya:
- elemen tidak otomatis ter-center saat lebar layar berubah (tablet/landscape),
- jarak antar blok tidak konsisten (14/16/18/24 mixed),
- teks panjang menimpa tombol (kita sudah `ellipsize` hanya di sebagian tempat),
- FAB sering berada di posisi yang tidak mengikuti area aman sistem.

Spesifikasi perbaikan: perkenalkan **layout helper** (kolom dengan weight, `centerRect(...)`, `pad=12/16`, `rowH=56/64`) dan pakai secara konsisten; ganti angka "ajaib" dengan konstanta tematik; gunakan `a.w/a.h` + `kPad` untuk semua penempatan, bukan koordinat fix.

## 10.6 Ringkasan mapping keluhan → mekanisme referensi

| Keluhan | Mekanisme di `.xapk` | Spesifikasi untuk NBTEDITOR |
|---|---|---|
| Tidak ada ikon tipe tag (I/B/…) | 11 drawable `nbt_*`; `h0.c` `setImageResource` per ordinal; `tree_node_item.xml` 32dp + 12dp | Tambah 11 glyph/ikON tipe di renderer C++ (bukan huruf polos) |
| Map tidak ada | `d0.e` TileView + `d0/a` bitmap provider + `d0/i` selection + marker entity + dialog koordinat | Port modul peta ke C++ (minimal: bitmap chunk + tap→koordinat + View Entities) |
| Mekanisme kaku | Drawer + fragment + dialog + breadcrumb + progress | Pindahkan navigasi ke model drawer/fragment, kurangi modal beruntun |
| Tidak pakai file manager OS | `ACTION_GET_CONTENT` + MIME + `OPENABLE` + chooser, `_display_name` | Ganti `S_BROWSER` dengan intent SAF / `ACTION_OPEN_DOCUMENT` |
| Edit harus muncul aside geser | `DrawerLayout` + `NavigationView` + `ActionBarDrawerToggle`, `GravityCompat.START` | Bangun panel geser kiri + scrim; Edit = buka drawer + frame editor |
| UI kacau / tidak center | `match_parent`/`weight`/`gravity`/margin 8-12-16dp, CardView | Refactor layout helper C++: centerRect, weight column, konstanta pad |

---

## 9.4 Daftar kelas dari SELURUH 13 DEX (inventaris biner)

Dekompilasi Java tersedia untuk 3 himpunan (`jadx_out` ≈ classes9, `jadx_c6`, `jadx_c12`). Agar tidak ada yang terlewat, berikut inventaris **kelas biner dari 13 file DEX** yang dihasilkan `dexclasses.py`.

| DEX | Jumlah class | Paket terbanyak (top 5) |
|---|---:|---|
| `classes.dex` | 5056 | androidx/compose/foundation/layout (164), androidx/compose/material (161), androidx/compose/ui/platform (143), androidx/compose/ui/graphics (124), androidx/compose/runtime (120) |
| `classes2.dex` | 22 | com/pairip/licensecheck (22) |
| `classes3.dex` | 6459 | com/yandex/div2 (1893), com/yandex/div/evaluable/function (280), io/bidmachine/media3/common (170), com/vungle/ads (160), io/bidmachine/media3/exoplayer (126) |
| `classes4.dex` | 7506 | com/yandex/mobile/ads/impl (3991), io/appmetrica/analytics/impl (1343), gatewayprotocol/v1 (483), io/bidmachine (142), io/bidmachine/protobuf (133) |
| `classes5.dex` | 5130 | io/sentry (306), kotlinx/coroutines/flow (171), io/ktor/http (132), kotlinx/coroutines (122), io/bidmachine/protobuf/rendering (109) |
| `classes6.dex` | 3630 | okhttp3 (94), net/pubnative/lite/sdk/models (92), sg/bigo/ads/api (76), okio (73), net/pubnative/lite/sdk/vpaid/models/vast (71) |
| `classes7.dex` | 6081 | com/applovin/impl (347), com/appodeal/ads (312), androidx/media3/common (177), com/android/billingclient/api (154), androidx/room (133) |
| `classes8.dex` | 4020 | com/chartboost/sdk/impl (1268), com/explorestack/protobuf (330), com/explorestack/protobuf/adcom (153), com/bytedance/sdk/openadsdk/utils (50), com/bytedance/sdk/openadsdk (46) |
| `classes9.dex` | 5722 | com/google/protobuf (276), com/google/android/exoplayer2 (207), com/google/common/collect (165), com/fyber/inneractive/sdk/protobuf (153), com/google/android/gms/ads/internal/client (143) |
| `classes10.dex` | 5856 | com/google/android/gms/internal/ads (3864), com/google/android/gms/internal/play_billing (197), com/google/android/gms/internal/auth (120), com/mobilefuse/sdk (110), com/google/android/gms/internal/consent_sdk (97) |
| `classes11.dex` | 6970 | com/ironsource (1274), com/inmobi/media (861), com/ironsource/adqualitysdk/sdk/i (337), com/mobilefuse/sdk/identity (63), com/mbridge/msdk/out (62) |
| `classes12.dex` | 8264 | com/startapp/sdk/internal (581), com/my/target (564), com/ogury/ad/internal (423), com/moloco/sdk (321), com/my/tracker/obfuscated (161) |
| **TOTAL** | **64716** | — |
# Bagian 11 — Inventarisasi FUNGSI/METHOD seluruh kode aplikasi

Diekstrak otomatis dari sumber JADX (method signature) dan smali PairIP. Ini melengkapi Bagian 9 (file) dan Bagian 10 (cara kerja): setiap method aplikasi tercantum; library pihak ketiga tetap dihitung per paket di Bagian 9.3/9.4.

## 11.1 Ringkasan

67 class aplikasi, **302 method/signature** tercatat.

| Paket | Jumlah method |
|---|---:|
| `d0` | 40 |
| `z` | 37 |
| `com.hextetrahedral.minetuner` | 32 |
| `t` | 32 |
| `s` | 25 |
| `f0` | 24 |
| `com.hextetrahedral.minetuner.app` | 22 |
| `y` | 19 |
| `com.hextetrahedral.leveldb` | 17 |
| `h0` | 12 |
| `e0` | 11 |
| `x` | 8 |
| `a0` | 7 |
| `b0` | 6 |
| `q` | 3 |
| `i0` | 2 |
| `u` | 2 |
| `w` | 2 |
| `c0` | 1 |

## 11.2 Daftar method per class aplikasi

### `a0.a` — 2 method

- `public /* synthetic */ a(Object obj, int i)`
- `public final void onClick(View view)`

### `a0.c` — 5 method

- `for (final View view : this.b)`
- `public c(FloatingActionButton floatingActionButton, ArrayList arrayList)`
- `public final OvershootInterpolator d = new OvershootInterpolator()`
- `public final void a()`
- `public final void run()`

### `b0.a` — 2 method

- `public /* synthetic */ a(int i)`
- `public final void run()`

### `b0.b` — 2 method

- `public static boolean a()`
- `public static final Handler a = new Handler(Looper.getMainLooper())`

### `b0.c` — 2 method

- `public c(long j, long j2)`
- `public final boolean a(String str)`

### `c0.a` — 1 method

- `void a(RecyclerView.ViewHolder viewHolder, int i)`

### `com.hextetrahedral.leveldb.LevelDB` — 17 method

- `private static native byte[] nativeGet(long j, byte[] bArr) throws Exception`
- `private static native byte[][] nativeIterateKeys(long j, byte[] bArr) throws Exception`
- `private static native long nativeOpen(String str) throws Exception`
- `private static native void nativeClose(long j) throws Exception`
- `private static native void nativePut(long j, byte[] bArr, byte[] bArr2) throws Exception`
- `public final ArrayList c() throws a`
- `public final byte[] b(byte[] bArr) throws a`
- `public final void a() throws a`
- `public final void d(String str) throws a`
- `public final void e(byte[] bArr, byte[] bArr2) throws a`
- `return nativeGet(this.a, bArr)`
- `throw new a("Error closing LevelDB", e)`
- `throw new a("Error getting value for key: " + Arrays.toString(bArr), e)`
- `throw new a("Error iterating keys with binary prefix", e)`
- `throw new a("Error putting key-value pair: " + Arrays.toString(bArr), e)`
- `throw new a(android.support.v4.media.a.m("Failed to open LevelDB at path: ", str))`
- `throw new a(coil.intercept.a.o(e, android.support.v4.media.a.B("Error opening LevelDB at path: ", str, " - More info: ")), e)`

### `com.hextetrahedral.minetuner.DrawerActivity` — 11 method

- `public final Fragment l(int i)`
- `public final boolean onNavigationItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void m(int i, boolean z2)`
- `public final void n()`
- `public final void o(int i, Fragment fragment, String str)`
- `public final void onCreate(Bundle bundle)`
- `public final void onSaveInstanceState(Bundle bundle)`
- `return new b()`
- `return new e()`
- `return new h()`

### `com.hextetrahedral.minetuner.MainActivity` — 21 method

- `final Context applicationContext = getApplicationContext()`
- `final String strD = jVar.d()`
- `final Uri data = intent.getData()`
- `final f fVar = (f) viewHolder`
- `final j jVar = ((e) this.k.get(i)).a`
- `final j jVar = new j(new File(string2))`
- `final j jVar2 = new j(new File(string10))`
- `public final boolean onCreateOptionsMenu(Menu menu)`
- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void l()`
- `public final void onActivityResult(final int i, int i2, Intent intent)`
- `public final void onClick(View view)`
- `public final void onClick(View view2)`
- `public final void onCreate(Bundle bundle)`
- `public final void onRequestPermissionsResult(int i, String[] strArr, int[] iArr)`
- `public final void onResume()`
- `public final void run()`
- `throw new RuntimeException(e)`
- `throw new RuntimeException(e2)`

### `com.hextetrahedral.minetuner.app.CreateWorldActivity` — 12 method

- `new ItemTouchHelper(new d(this)).attachToRecyclerView(recyclerViewFindViewById)`
- `new e().pos("CANCEL").show(createWorldActivity, "pick_biome_dialog")`
- `new j().pos("CANCEL").show(createWorldActivity, "pick_block_dialog")`
- `public final String validate(String str, String str2, Bundle bundle)`
- `public final boolean onCreateOptionsMenu(Menu menu)`
- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void l(t.a aVar)`
- `public final void onClick(View view)`
- `public final void onCreate(Bundle bundle)`
- `public final void onSaveInstanceState(Bundle bundle)`

### `com.hextetrahedral.minetuner.app.EntitiesNBTActivity` — 7 method

- `public final ArrayList n = new ArrayList()`
- `public final boolean l()`
- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void m()`
- `public final void onCreate(Bundle bundle)`

### `com.hextetrahedral.minetuner.app.HelpActivity` — 3 method

- `public final boolean onOptionsItemSelected(MenuItem menuItem)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final void onCreate(Bundle bundle)`

### `d0.a` — 3 method

- `public /* synthetic */ a(e eVar)`
- `public Bitmap a(i1.a aVar, Context context)`
- `public void b(MotionEvent motionEvent)`

### `d0.b` — 3 method

- `public /* synthetic */ b(e eVar, Bundle bundle)`
- `public final void accept(Object obj)`
- `public final void run()`

### `d0.e` — 17 method

- `final t.f fVar = (t.f) t.f.d.get(((l) optionalM.get()).g().c)`
- `new Path()`
- `public final HashMap f = new HashMap()`
- `public final HashSet g = new HashSet()`
- `public final View onCreateView(LayoutInflater layoutInflater, ViewGroup viewGroup, Bundle bundle)`
- `public final boolean onResult(String str, int i2, Bundle bundle)`
- `public final e0.a d = new e0.a()`
- `public final e0.a e = new e0.a()`
- `public final void b(s.a aVar)`
- `public final void onDestroy()`
- `public final void onPause()`
- `public final void onResume()`
- `public final void onSaveInstanceState(Bundle bundle)`
- `public final void run()`
- `public static final int h = Math.floorDiv(536870912, 16)`
- `public static final int i = Math.floorDiv(536870912, 16)`
- `public static void c(String str, Canvas canvas)`

### `d0.g` — 7 method

- `public final boolean onSingleTapConfirmed(MotionEvent motionEvent)`
- `public final boolean onTouchEvent(MotionEvent motionEvent)`
- `public final void onLongPress(MotionEvent motionEvent)`
- `public final void onScrollChanged(int i, int i2, int i3, int i4)`
- `public void setOnLongPressListener(f fVar)`
- `public void setSelectionView(i iVar)`
- `public void setTouchable(boolean z2)`

### `d0.i` — 10 method

- `public Rect getSelection()`
- `public final Parcelable onSaveInstanceState()`
- `public final void onDraw(Canvas canvas)`
- `public final void onLayout(boolean z2, int i, int i2, int i3, int i4)`
- `public final void onRestoreInstanceState(Parcelable parcelable)`
- `public final void onViewAdded(View view)`
- `public i(Context context)`
- `public void setSelectionChangedListener(@Nullable h hVar)`
- `public void setTileView(g gVar)`
- `return new Rect(this.h)`

### `e0.a` — 10 method

- `public static int a(t.b bVar, int i, int i2, int i3)`
- `public void b(s.a aVar, Canvas canvas, int i, int i2, int i3, int i4, Paint paint)`
- `public void onInterstitialClicked()`
- `public void onInterstitialClosed()`
- `public void onInterstitialExpired()`
- `public void onInterstitialFailedToLoad()`
- `public void onInterstitialLoaded(boolean z2)`
- `public void onInterstitialShowFailed()`
- `public void onInterstitialShown()`
- `throw new RuntimeException(e)`

### `e0.b` — 1 method

- `public /* synthetic */ b(a aVar, int i, int i2, int i3, int i4, s.a aVar2, s.a aVar3, s.a aVar4, Paint paint, Canvas canvas)`

### `f0.a` — 4 method

- `public final void d()`
- `public final void onCreate(Bundle bundle)`
- `public final void onViewCreated(View view, Bundle bundle)`
- `public static a f(String str)`

### `f0.b` — 4 method

- `public final ArrayList d = new ArrayList()`
- `public final View onCreateView(LayoutInflater layoutInflater, ViewGroup viewGroup, Bundle bundle)`
- `public final void a(RecyclerView.ViewHolder viewHolder, int i)`
- `public final void onViewCreated(View view, Bundle bundle)`

### `f0.d` — 1 method

- `public final boolean test(Object obj)`

### `f0.e` — 1 method

- `public final Object apply(int i)`

### `f0.g` — 12 method

- `final String strTrim = ((hVar.j() == aVar2) || (hVar.j() == aVar3 || hVar.j() == x2.a.n)) ? null : str.trim()`
- `public final String validate(String str, String str2, Bundle bundle)`
- `public final View onCreateView(LayoutInflater layoutInflater, ViewGroup viewGroup, Bundle bundle)`
- `public final boolean onResult(String str, int i, Bundle bundle)`
- `public final boolean test(Object obj)`
- `public final void b(Context context, String str, String str2, x2.a aVar, int i, Bundle bundle)`
- `public final void c(h0.d dVar)`
- `public final void e(h0.d dVar, int i)`
- `public final void onClick(View view)`
- `public final void onSaveInstanceState(Bundle bundle)`
- `public final void onViewStateRestored(Bundle bundle)`
- `public void d()`

### `f0.h` — 2 method

- `public final void d()`
- `public final void onViewCreated(View view, Bundle bundle)`

### `h0.b` — 1 method

- `public b(View view)`

### `h0.c` — 9 method

- `final b bVar = (b) viewHolder`
- `final d dVar = (d) this.j.get(i)`
- `public c(Context context, ArrayList arrayList)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final boolean onLongClick(View view)`
- `public final d b(int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new b(LayoutInflater.from(this.k).inflate(2131558924, viewGroup, false))`

### `h0.d` — 2 method

- `public d(String str, boolean z2, l lVar)`
- `public final ArrayList d = new ArrayList()`

### `i0.a` — 1 method

- `public final Object apply(Object obj)`

### `i0.b` — 1 method

- `public final void accept(Object obj)`

### `q.a` — 3 method

- `public a(Runnable runnable)`
- `public final Object doInBackground(Object[] objArr)`
- `public final void onPostExecute(Object obj)`

### `s.a` — 19 method

- `public a(int i, int i2, j jVar, int i3)`
- `public final ArrayList c()`
- `public final AtomicReference f = new AtomicReference(null)`
- `public final ConcurrentHashMap e = new ConcurrentHashMap()`
- `public final Optional b(byte[] bArr)`
- `public final ThreadLocal g = ThreadLocal.withInitial(new androidx.emoji2.text.flatbuffer.a(1))`
- `public final byte[] a(int i, int i2, int i3, byte b, boolean z2)`
- `public final int e(int i, int i2, t.b bVar) throws Throwable`
- `public final void d(e0.b bVar)`
- `throw new IllegalArgumentException("Not enough block data.")`
- `throw new IllegalArgumentException("Unexpected chunk index value: " + i8)`
- `throw new IllegalArgumentException("Unexpected end of stream when reading palette size.")`
- `throw new IllegalArgumentException("Unexpected non-save persistence version found.")`
- `throw new IllegalArgumentException("Unexpected number of storage areas: " + i5)`
- `throw new IllegalArgumentException("Unexpectedly large number of palette entries: " + iJ)`
- `throw new IllegalArgumentException("Unknown or corrupted subchunk version: " + i19)`
- `throw new RuntimeException("Error reading entity data", e)`
- `throw new RuntimeException("Error reading subchunk data", e)`
- `throw new RuntimeException(e)`

### `s.b` — 2 method

- `public static /* synthetic */ byte a(int i)`
- `public static /* synthetic */ int b(int i)`

### `s.c` — 4 method

- `public c(int i, int i2, int i3, int i4)`
- `public final ReentrantReadWriteLock g = new ReentrantReadWriteLock()`
- `public final int a(int i)`
- `throw new IllegalArgumentException("Block data too short for expected number of blocks")`

### `t.b` — 2 method

- `public final u.a b = new u.a(0, 0, 0)`
- `public final u.a c = new u.a(0, 0, 0)`

### `t.c` — 1 method

- `public static final Map Ql = (Map) Arrays.stream(values()).collect(Collectors.toMap(new i0.a(1), new i0.a(2)))`

### `t.d` — 2 method

- `public d(j jVar)`
- `public final Object create(Object obj)`

### `t.e` — 3 method

- `public e(int i, int i2, int i3)`
- `public final boolean equals(Object obj)`
- `public final int hashCode()`

### `t.f` — 1 method

- `public static final Map d = (Map) Arrays.stream(values()).collect(Collectors.toMap(new i0.a(3), new i0.a(4)))`

### `t.g` — 1 method

- `public final int applyAsInt(Object obj)`

### `t.h` — 1 method

- `public h(c cVar, int i)`

### `t.i` — 4 method

- `public final FileVisitResult postVisitDirectory(Object obj, IOException iOException)`
- `public final FileVisitResult visitFile(Object obj, BasicFileAttributes basicFileAttributes)`
- `public final FileVisitResult visitFileFailed(Object obj, IOException iOException)`
- `public i(AtomicLong atomicLong)`

### `t.j` — 17 method

- `public final ArrayList e()`
- `public final Bitmap h()`
- `public final Optional f(l lVar)`
- `public final ReentrantReadWriteLock d = new ReentrantReadWriteLock()`
- `public final String d()`
- `public final String g()`
- `public final boolean i()`
- `public final boolean j()`
- `public final byte[] c(byte[] bArr)`
- `public final l l()`
- `public final s.a b(int i, int i2, int i3)`
- `public final void a()`
- `public final void k()`
- `public final void m(l lVar)`
- `public j(File file)`
- `return new File(this.c, "world_icon.jpeg").exists()`
- `return new String(Files.readAllBytes(file.toPath()))`

### `u.a` — 2 method

- `public a(int i, int i2, int i3)`
- `public final String toString()`

### `w.a` — 2 method

- `public /* synthetic */ a(AppCompatActivity appCompatActivity, int i)`
- `public final void handleOnBackPressed()`

### `x.a` — 1 method

- `public final void onInitializationFinished(List list)`

### `x.b` — 2 method

- `public static final AtomicReference a = new AtomicReference(null)`
- `public static j a()`

### `x.d` — 5 method

- `public d(CreateWorldActivity createWorldActivity)`
- `public final boolean isLongPressDragEnabled()`
- `public final boolean onMove(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder, RecyclerView.ViewHolder viewHolder2)`
- `public final int getMovementFlags(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder)`
- `public final void onSwiped(RecyclerView.ViewHolder viewHolder, int i)`

### `y.a` — 1 method

- `public a(View view)`

### `y.b` — 5 method

- `public b(ArrayList arrayList, CreateWorldActivity createWorldActivity)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new a(LayoutInflater.from(viewGroup.getContext()).inflate(2131558926, viewGroup, false))`

### `y.c` — 1 method

- `public c(View view)`

### `y.d` — 5 method

- `public d(ArrayList arrayList, c0.a aVar)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new c(LayoutInflater.from(viewGroup.getContext()).inflate(2131558924, viewGroup, false))`

### `y.e` — 1 method

- `public e(j jVar)`

### `y.f` — 1 method

- `public f(View view)`

### `y.g` — 5 method

- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `public g(ArrayList arrayList, MainActivity mainActivity)`
- `return new f(LayoutInflater.from(viewGroup.getContext()).inflate(2131558925, viewGroup, false))`

### `z.a` — 3 method

- `public a(e eVar)`
- `public final boolean onQueryTextChange(String str)`
- `public final boolean onQueryTextSubmit(String str)`

### `z.b` — 5 method

- `public b(d dVar, ArrayList arrayList)`
- `public final boolean areContentsTheSame(int i, int i2)`
- `public final boolean areItemsTheSame(int i, int i2)`
- `public final int getNewListSize()`
- `public final int getOldListSize()`

### `z.c` — 1 method

- `public c(View view)`

### `z.d` — 5 method

- `public d(t.a[] aVarArr)`
- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `return new c(LayoutInflater.from(viewGroup.getContext()).inflate(2131558606, viewGroup, false))`

### `z.e` — 1 method

- `public final View onCreateContentView(Bundle bundle)`

### `z.f` — 3 method

- `public f(j jVar)`
- `public final boolean onQueryTextChange(String str)`
- `public final boolean onQueryTextSubmit(String str)`

### `z.g` — 5 method

- `public final boolean areContentsTheSame(int i, int i2)`
- `public final boolean areItemsTheSame(int i, int i2)`
- `public final int getNewListSize()`
- `public final int getOldListSize()`
- `public g(i iVar, ArrayList arrayList)`

### `z.h` — 1 method

- `public h(View view)`

### `z.i` — 5 method

- `public final RecyclerView.ViewHolder onCreateViewHolder(ViewGroup viewGroup, int i)`
- `public final int getItemCount()`
- `public final void onBindViewHolder(RecyclerView.ViewHolder viewHolder, int i)`
- `public i(t.c[] cVarArr)`
- `return new h(LayoutInflater.from(viewGroup.getContext()).inflate(2131558926, viewGroup, false))`

### `z.j` — 1 method

- `public final View onCreateContentView(Bundle bundle)`

### `z.k` — 4 method

- `public final void onProgressChanged(SeekBar seekBar, int i, boolean z2)`
- `public final void onStartTrackingTouch(SeekBar seekBar)`
- `public final void onStopTrackingTouch(SeekBar seekBar)`
- `public k(l lVar, int i)`

### `z.l` — 3 method

- `public final Bundle onResult(int i)`
- `public final View onCreateContentView(Bundle bundle)`
- `public final void onSaveInstanceState(Bundle bundle)`


## 11.3 Method PairIP (dari smali, 22 class)

### `ILicenseV2ResultListener$Stub` — 4 method

- `.method public constructor <init>()V`
- `.method private static readTypedObject(Landroid/os/Parcel;Landroid/os/Parcelable$Creator;)Ljava/lang/Object;`
- `.method public asBinder()Landroid/os/IBinder;`
- `.method public onTransact(ILandroid/os/Parcel;Landroid/os/Parcel;I)Z`

### `ILicenseV2ResultListener` — 1 method

- `.method public abstract verifyLicense(ILandroid/os/Bundle;)V`

### `LicenseActivity$$ExternalSyntheticLambda0` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseActivity;Landroid/app/PendingIntent;)V`
- `.method public final run()V`

### `LicenseActivity$$ExternalSyntheticLambda1` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseActivity;)V`
- `.method public final run()V`

### `LicenseActivity$$ExternalSyntheticLambda2` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseActivity;)V`
- `.method public final onClick(Landroid/content/DialogInterface;I)V`

### `LicenseActivity$ActivityType` — 5 method

- `.method private static synthetic $values()[Lcom/pairip/licensecheck/LicenseActivity$ActivityType;`
- `.method static constructor <clinit>()V`
- `.method private constructor <init>(Ljava/lang/String;I)V`
- `.method public static valueOf(Ljava/lang/String;)Lcom/pairip/licensecheck/LicenseActivity$ActivityType;`
- `.method public static values()[Lcom/pairip/licensecheck/LicenseActivity$ActivityType;`

### `LicenseActivity` — 13 method

- `.method public static synthetic $r8$lambda$N5_Pzpb-eSKmOONXn3Kn0QvMbys(Lcom/pairip/licensecheck/LicenseActivity;)V`
- `.method public static synthetic $r8$lambda$fE_XZ7S0hhHsxQNTfy8mxeJ7kEU(Lcom/pairip/licensecheck/LicenseActivity;Landroid/content/DialogInterface;I)V`
- `.method public static synthetic $r8$lambda$x-JmBIDmuVzGN23Wk7Dd1TBpzO0(Lcom/pairip/licensecheck/LicenseActivity;Landroid/app/PendingIntent;)V`
- `.method public constructor <init>()V`
- `.method private synthetic lambda$showErrorDialog$0()V`
- `.method private synthetic lambda$showErrorDialog$1(Landroid/content/DialogInterface;I)V`
- `.method private synthetic lambda$showPaywallAndCloseApp$0(Landroid/app/PendingIntent;)V`
- `.method private logAndShowErrorDialog(Ljava/lang/String;)V`
- `.method private logAndShowErrorDialog(Ljava/lang/String;Ljava/lang/Exception;)V`
- `.method private showErrorDialog()V`
- `.method private showPaywallAndCloseApp()V`
- `.method protected closeApp()V`
- `.method public onStart()V`

### `LicenseCheckException` — 2 method

- `.method public constructor <init>(Ljava/lang/String;)V`
- `.method public constructor <init>(Ljava/lang/String;Ljava/lang/Throwable;)V`

### `LicenseClient$$ExternalSyntheticLambda0` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda1` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;Z)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda2` — 2 method

- `.method public synthetic constructor <init>()V`
- `.method public final run(Ljava/lang/Runnable;)V`

### `LicenseClient$$ExternalSyntheticLambda3` — 2 method

- `.method public synthetic constructor <init>(Landroid/os/Handler;)V`
- `.method public final run(Ljava/lang/Runnable;)V`

### `LicenseClient$$ExternalSyntheticLambda4` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda5` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda6` — 2 method

- `.method public synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method public final run()V`

### `LicenseClient$$ExternalSyntheticLambda7` — 2 method

- `.method public synthetic constructor <init>()V`
- `.method public final run()V`

### `LicenseClient$1` — 2 method

- `.method constructor <init>()V`
- `.method public run()V`

### `LicenseClient$2` — 2 method

- `.method constructor <init>(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public verifyLicense(ILandroid/os/Bundle;)V`

### `LicenseClient$DelayedTaskExecutor` — 3 method

- `.method private constructor <init>()V`
- `.method synthetic constructor <init>(Lcom/pairip/licensecheck/LicenseClient-IA;)V`
- `.method public schedule(Ljava/lang/Runnable;J)V`

### `LicenseClient$ImmediateTaskExecutor` — 1 method

- `.method public abstract run(Ljava/lang/Runnable;)V`

### `LicenseClient$LicenseCheckState` — 5 method

- `.method private static synthetic $values()[Lcom/pairip/licensecheck/LicenseClient$LicenseCheckState;`
- `.method static constructor <clinit>()V`
- `.method private constructor <init>(Ljava/lang/String;I)V`
- `.method public static valueOf(Ljava/lang/String;)Lcom/pairip/licensecheck/LicenseClient$LicenseCheckState;`
- `.method public static values()[Lcom/pairip/licensecheck/LicenseClient$LicenseCheckState;`

### `LicenseClient` — 33 method

- `.method public static synthetic $r8$lambda$GS82Fij7VQePgSFog-s63-Rcyb0(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public static synthetic $r8$lambda$gb-vmUiJUmqdCloCudVdY-igh7I(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method public static synthetic $r8$lambda$q2q7YKfx3jIZHqiUNn7fQ55wwzI(Lcom/pairip/licensecheck/LicenseClient;Z)V`
- `.method public static synthetic $r8$lambda$tTRuJInP7s484yRu-m6AsnoI1z4(Lcom/pairip/licensecheck/LicenseClient;)V`
- `.method public static synthetic $r8$lambda$xzrAfByzooHDT9oIsgTdQvzthuE(Lcom/pairip/licensecheck/LicenseClient;Landroid/os/IBinder;)V`
- `.method static bridge synthetic -$$Nest$mprocessResponse(Lcom/pairip/licensecheck/LicenseClient;ILandroid/os/Bundle;)V`
- `.method static constructor <clinit>()V`
- `.method public constructor <init>(Landroid/content/Context;)V`
- `.method private checkLicenseInternal(Landroid/os/IBinder;)V`
- `.method private connectToLicensingService()V`
- `.method private createCloseAppIntentOrExitIfAppInBackground()Landroid/content/Intent;`
- `.method private static createResultListener(Lcom/pairip/licensecheck/LicenseClient;)Lcom/pairip/licensecheck/ILicenseV2ResultListener;`
- `.method public static getLicensePubKey()Ljava/lang/String;`
- `.method private handleError(Lcom/pairip/licensecheck/LicenseCheckException;)V`
- `.method private isForeground()Z`
- `.method private synthetic lambda$initializeLicenseCheck$0()V`
- `.method private synthetic lambda$initializeLicenseCheck$1(Z)V`
- `.method private synthetic lambda$onServiceConnected$0(Landroid/os/IBinder;)V`
- `.method private synthetic lambda$onServiceConnected$1(Landroid/os/IBinder;)V`
- `.method static synthetic lambda$reportSuccessfulLicenseCheck$0()V`
- `.method static synthetic lambda$static$0(Ljava/lang/Runnable;)V`
- `.method private performLocalInstallerCheck()Z`
- `.method private populateInputDataForLicenseCheckV2(Landroid/os/Parcel;Landroid/os/IBinder;)V`
- `.method private populateInputDataForReportAutoVerifiedLicense(Landroid/os/Parcel;Landroid/os/IBinder;)V`
- `.method private processResponse(ILandroid/os/Bundle;)V`
- `.method private retryOrThrow(Lcom/pairip/licensecheck/LicenseCheckException;)V`
- `.method private retryOrThrow(Lcom/pairip/licensecheck/LicenseCheckException;Z)V`
- `.method private startErrorDialogActivity()V`
- `.method private startPaywallActivity(Landroid/app/PendingIntent;)V`
- `.method public initializeLicenseCheck()V`
- `.method public onServiceConnected(Landroid/content/ComponentName;Landroid/os/IBinder;)V`
- `.method public onServiceDisconnected(Landroid/content/ComponentName;)V`
- `.method public reportSuccessfulLicenseCheck(Landroid/os/IBinder;)V`

### `LicenseContentProvider` — 7 method

- `.method public constructor <init>()V`
- `.method public delete(Landroid/net/Uri;Ljava/lang/String;[Ljava/lang/String;)I`
- `.method public getType(Landroid/net/Uri;)Ljava/lang/String;`
- `.method public insert(Landroid/net/Uri;Landroid/content/ContentValues;)Landroid/net/Uri;`
- `.method public onCreate()Z`
- `.method public query(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;`
- `.method public update(Landroid/net/Uri;Landroid/content/ContentValues;Ljava/lang/String;[Ljava/lang/String;)I`

### `ResponseValidator` — 6 method

- `.method static constructor <clinit>()V`
- `.method private constructor <init>()V`
- `.method private static base64ToJson(Ljava/lang/String;)Lorg/json/JSONObject;`
- `.method private static getPublicKey()Ljava/security/PublicKey;`
- `.method public static validateResponse(Landroid/os/Bundle;Ljava/lang/String;)V`
- `.method private static verifySignature(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/security/PublicKey;)V`
