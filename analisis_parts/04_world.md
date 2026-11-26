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
