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
