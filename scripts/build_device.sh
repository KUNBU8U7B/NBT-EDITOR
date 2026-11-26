#!/data/data/com.termux/files/usr/bin/bash
# Compile libnbt.so untuk arm64-v8a memakai sysroot NDK30 dari Termux.
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
cd "$HERE"
PREFIX="${PREFIX:-/data/data/com.termux/files/usr}"
OUT="$HERE/out/lib/arm64-v8a"
mkdir -p "$OUT"
# Link-time stubs for the Android system libraries (see scripts/make_stubs.sh).
# Termux's own libandroid.so/libz.so cannot be used: they carry SONAMEs and
# dependencies (libtermux-platform-ns.so, libz.so.1) that do not exist on a
# device, which made the app die at load time before the first frame.
[ -f build/stubs/libandroid.so ] || bash "$HERE/scripts/make_stubs.sh" >/dev/null

# clang (aarch64) secara default meng-"outline" operasi atomic menjadi helper
# __aarch64_ldadd*_acq_rel / __aarch64_ldadd*_relax milik compiler-rt. Karena
# build ini memakai -nostdlib, archive compiler-rt TIDAK ter-link, sehingga
# helper itu tetap undefined di libnbt.so dan diharapkanProvided oleh libc.
# libc perangkat ini (Android 13 Unisoc) ternyata TIDAK mengekspor simbol
# tersebut sama sekali -> dlopen() gagal -> NativeActivity tidak pernah bisa
# memuat library -> aplikasi langsung menutup diri (terbukti di perangkat:
# "dlopen failed: cannot locate symbol __aarch64_ldadd8_acq_rel").
# -mno-outline-atomics memaksa compiler memakai atomic LL/SC inline (baseline
# armv8-a): tidak ada helper eksternal, jadi .so bisa dimuat di Android 7+.
BUILTINS="$(ls -1 "$PREFIX"/lib/clang/*/lib/linux/libclang_rt.builtins-aarch64-android.a 2>/dev/null | head -1 || true)"

# VERSION adalah sumber kebenaran tunggal; suntikkan ke source native.
VERSION_CODE="$(awk -F= '/^VERSION_CODE=/{print $2}' VERSION)"
VERSION_NAME="$(awk -F= '/^VERSION_NAME=/{print $2}' VERSION)"
: "${VERSION_CODE:?VERSION_CODE belum diisi di berkas VERSION}"
: "${VERSION_NAME:?VERSION_NAME belum diisi di berkas VERSION}"

echo "== compile arm64-v8a =="
clang++ -std=c++20 -O2 -fPIC -shared -fvisibility=hidden -DNDEBUG \
  --target=aarch64-linux-android26 -nostdlib \
  -march=armv8-a -mno-outline-atomics \
  -Wall -Wextra -Werror \
  "-DNBT_APP_VERSION_NAME=\"$VERSION_NAME\"" "-DNBT_APP_VERSION_CODE=$VERSION_CODE" \
  app/src/main.native.cpp -o "$OUT/libnbt.so" \
  "$PREFIX/lib/libc++_shared.so" \
  ${BUILTINS:+"$BUILTINS"} \
  build/stubs/libandroid.so build/stubs/libz.so build/stubs/liblog.so

# Guard: .so tidak boleh bergantung pada helper atomic yang tidak ada di
# Android. Kalau helper muncul lagi, build GAGAL (bukan diam-diam shipped).
READELF="$PREFIX/bin/aarch64-linux-android-readelf"
[ -x "$READELF" ] || READELF="readelf"
if "$READELF" --dyn-syms "$OUT/libnbt.so" 2>/dev/null \
     | grep -E "UND .*(__aarch64_|__atomic_)" | grep -q .; then
  echo "ERROR: libnbt.so masih membutuhkan helper atomic eksternal:" >&2
  "$READELF" --dyn-syms "$OUT/libnbt.so" 2>/dev/null \
     | grep -E "UND .*(__aarch64_|__atomic_)" >&2
  exit 1
fi
echo "guard: tidak ada helper atomic eksternal yang dibutuhkan"
# libc++_shared.so must ship inside the APK; copy it next to libnbt.so
cp "${PREFIX}/lib/libc++_shared.so" "$OUT/libc++_shared.so"

# Termux's clang bakes its own library path into DT_RUNPATH. That directory does
# not exist on a device; drop it so the binary has no Termux-specific metadata.
if command -v patchelf >/dev/null 2>&1; then
  patchelf --remove-rpath "$OUT/libnbt.so" 2>/dev/null || true
fi
# patchelf removes the tag but leaves the string in .dynstr; blank it out too so
# the shipped binary carries no Termux path at all.
python3 - "$OUT/libnbt.so" <<'PY' 2>/dev/null || true
import sys
p = sys.argv[1]
d = bytearray(open(p, 'rb').read())
i = d.find(b'/data/data/com.termux')
if i != -1:
    end = d.find(b'\x00', i)
    d[i:end] = b'\x00' * (end - i)
    open(p, 'wb').write(d)
    print("stub: Termux path string removed")
PY
echo "ok: $OUT/libnbt.so (+ libc++_shared.so)"
llvm-objdump -p "$OUT/libnbt.so" | grep -E 'NEEDED|SONAME' || true
