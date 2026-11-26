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
