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

