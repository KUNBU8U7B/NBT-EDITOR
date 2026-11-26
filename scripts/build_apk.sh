#!/data/data/com.termux/files/usr/bin/bash
# Bangun APK: aapt2 (compile+link) -> tempel libnbt.so tanpa kompresi -> apksigner.
# Tanpa Gradle, tanpa Android SDK, tanpa JDK untuk build (JDK hanya untuk keystore).
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
cd "$HERE"
mkdir -p out

command -v aapt2 >/dev/null || { echo "aapt2 tidak ditemukan"; exit 1; }

# APK packaging is fail-closed: it must never reuse a stale native library or
# ship when the engine/UI regression suite fails.  These builds are quick and
# deterministic; repeating them here prevents a source change from being
# masked by an older out/lib/arm64-v8a/libnbt.so.
echo "== host regression sebelum packaging =="
bash "$HERE/scripts/build_host_tests.sh"
echo "== fresh device library build =="
bash "$HERE/scripts/build_device.sh"

bash "$HERE/scripts/keystore.sh" 2>&1 | tail -2

echo "== aapt2 compile resources =="
rm -f out/res.zip
aapt2 compile --dir app/res -o out/res.zip

echo "== aapt2 link =="
rm -f out/base.apk
ANDROID_JAR="${HOME}/android-sdk/platforms/android-35/android.jar"
if [ ! -f "$ANDROID_JAR" ]; then
  echo "android.jar missing: download platform-35_r02.zip from dl.google.com and place it at"
  echo "  ~/android-sdk/platforms/android-35/android.jar   (see DEVELOPMENT.md)"
  exit 1
fi
# versi diambil dari berkas VERSION (sumber kebenaran tunggal)
VERSION_CODE="$(awk -F= '/^VERSION_CODE=/{print $2}' VERSION)"
VERSION_NAME="$(awk -F= '/^VERSION_NAME=/{print $2}' VERSION)"
: "${VERSION_CODE:?VERSION_CODE belum diisi di berkas VERSION}"
: "${VERSION_NAME:?VERSION_NAME belum diisi di berkas VERSION}"
echo "== versi aplikasi: $VERSION_NAME (code $VERSION_CODE) =="

aapt2 link -o out/base.apk -I "$ANDROID_JAR" \
  --manifest app/AndroidManifest.xml \
  out/res.zip \
  --min-sdk-version 26 --target-sdk-version 35 \
  --version-code "$VERSION_CODE" --version-name "$VERSION_NAME" \
  --no-compress

echo "== tambah native library (disimpan, bukan di-zip) =="
# libc++_shared.so is required by libnbt.so (Termux NDK sysroot has no static variant).
[ -f "${PREFIX}/lib/libc++_shared.so" ] && cp "${PREFIX}/lib/libc++_shared.so" out/lib/arm64-v8a/

rm -rf out/stage && mkdir -p out/stage/lib/arm64-v8a
cp out/lib/arm64-v8a/*.so out/stage/lib/arm64-v8a/
( cd out/stage && zip -q -r -0 -X ../base.apk lib/arm64-v8a/ )

echo "== sign =="
# v1 keeps old devices (API < 24) happy; v2/v3 are required by new ones.
apksigner sign \
  --v1-signing-enabled true --v2-signing-enabled true --v3-signing-enabled true \
  --ks "$HERE/out/debug.keystore" --ks-key-alias androiddebugkey \
  --ks-pass pass:android --key-pass pass:android \
  --out out/NBTEditor.apk out/base.apk

echo "== verifikasi =="
# Signature verification must succeed; a broken signature is never a warning.
apksigner verify --verbose out/NBTEditor.apk >/dev/null
apksigner verify --print-certs out/NBTEditor.apk | head -4

# Verify metadata and the exact native-library bytes carried by the APK.
BADGING="$(aapt2 dump badging out/NBTEditor.apk)"
printf '%s\n' "$BADGING" | grep -F "package: name='com.nbteditor' versionCode='$VERSION_CODE' versionName='$VERSION_NAME'" >/dev/null
python3 - "$HERE/out/lib/arm64-v8a/libnbt.so" "$HERE/out/NBTEditor.apk" <<'PY'
import hashlib
import sys
import zipfile

library_path, apk_path = sys.argv[1], sys.argv[2]
with open(library_path, "rb") as f:
    expected = f.read()
with zipfile.ZipFile(apk_path, "r") as zf:
    names = set(zf.namelist())
    required = {"lib/arm64-v8a/libnbt.so", "lib/arm64-v8a/libc++_shared.so"}
    missing = required - names
    if missing:
        raise SystemExit("APK missing native libraries: " + ", ".join(sorted(missing)))
    packed = zf.read("lib/arm64-v8a/libnbt.so")
if packed != expected:
    raise SystemExit("APK libnbt.so differs from the fresh device build")
print("native library sha256: " + hashlib.sha256(packed).hexdigest())
PY

# A shipped library must not retain a host-only runtime path.
READELF="${PREFIX:-/data/data/com.termux/files/usr}/bin/aarch64-linux-android-readelf"
[ -x "$READELF" ] || READELF="readelf"
if "$READELF" -d out/lib/arm64-v8a/libnbt.so 2>/dev/null \
     | grep -E '(RPATH|RUNPATH)' | grep -F 'termux' >/dev/null; then
  echo "ERROR: libnbt.so still contains a Termux runtime path" >&2
  exit 1
fi

ls -l out/NBTEditor.apk
echo "APK siap: out/NBTEditor.apk"
