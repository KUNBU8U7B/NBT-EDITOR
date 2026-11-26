#!/usr/bin/env bash
# Host build (Linux + Android SDK/NDK) dari app/src/main.native.cpp -> out/NBTEditor.apk.
# Setara dengan scripts/build_apk.sh (Termux), tetapi memakai toolchain host yang
# tersedia: g++ untuk tes host, NDK clang++ untuk arm64, aapt2/apksigner SDK.
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
cd "$HERE"

SDK="${ANDROID_SDK_ROOT:-/home/kunbu8u7b/Android/Sdk}"
NDK="$SDK/ndk/30.0.16248370"
TC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin"
BT="$SDK/build-tools/36.0.0"
ANDROID_JAR="$SDK/platforms/android-37.0/android.jar"
CXX_HOST="${CXX:-g++}"

command -v aapt2 >/dev/null 2>&1 || AAPT2="$BT/aapt2" || true
AAPT2="${AAPT2:-aapt2}"
APKSIGNER="$BT/apksigner"
D8="$BT/d8"
# javac isn't in the system JRE; Android Studio bundles a full JDK (JBR).
JAVAC="${JAVAC:-}"
if [ -z "$JAVAC" ]; then
  for c in /home/kunbu8u7b/Unduhan/android-studio/jbr/bin/javac \
           /usr/lib/jvm/default-java/bin/javac "$(command -v javac 2>/dev/null || true)"; do
    if [ -n "$c" ] && [ -x "$c" ]; then JAVAC="$c"; break; fi
  done
fi
[ -f "$ANDROID_JAR" ] || { echo "android.jar tidak ada: $ANDROID_JAR" >&2; exit 1; }
[ -x "$TC/clang++" ] || { echo "NDK clang++ tidak ada: $TC/clang++" >&2; exit 1; }
[ -n "$JAVAC" ] && [ -x "$JAVAC" ] || { echo "javac tidak ditemukan (set JAVAC=...)" >&2; exit 1; }
[ -x "$D8" ] || { echo "d8 tidak ada: $D8" >&2; exit 1; }

VERSION_CODE="$(awk -F= '/^VERSION_CODE=/{print $2}' VERSION)"
VERSION_NAME="$(awk -F= '/^VERSION_NAME=/{print $2}' VERSION)"
: "${VERSION_CODE:?VERSION_CODE kosong}"
: "${VERSION_NAME:?VERSION_NAME kosong}"

OUT="$HERE/out/lib/arm64-v8a"
mkdir -p "$OUT" out

echo "== decoder gambar (stb_image, C) =="
gcc -std=c99 -O2 -w -fPIC -I app/src -c app/src/stb_image.c -o out/stb_image_host.o
"$TC/clang" -std=c99 -O2 -fPIC -w -I app/src --target=aarch64-linux-android26 \
  -march=armv8-a -c app/src/stb_image.c -o out/stb_image_device.o

echo "== host regression (g++ + ASan/UBSan) =="
"$CXX_HOST" -std=c++20 -O1 -g -fsanitize=address,undefined -Wall -Wextra -Werror \
  "-DNBT_APP_VERSION_NAME=\"$VERSION_NAME\"" "-DNBT_APP_VERSION_CODE=$VERSION_CODE" \
  -DNBT_SELFTEST app/src/main.native.cpp out/stb_image_host.o -o out/nbt_test -lz
echo "== jalankan tes =="
out/nbt_test

echo "== compile arm64-v8a libnbt.so (NDK clang++) =="
STDLIB="$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so"
"$TC/clang++" -std=c++20 -O2 -fPIC -shared -fvisibility=hidden -DNDEBUG \
  --target=aarch64-linux-android26 -nostdlib \
  -march=armv8-a -mno-outline-atomics \
  -Wall -Wextra -Werror \
  "-DNBT_APP_VERSION_NAME=\"$VERSION_NAME\"" "-DNBT_APP_VERSION_CODE=$VERSION_CODE" \
  app/src/main.native.cpp out/stb_image_device.o -o "$OUT/libnbt.so" \
  "$STDLIB" \
  build/stubs/libandroid.so build/stubs/libz.so build/stubs/liblog.so

echo "== guard simbol atomic =="
if "$TC/llvm-readelf" --dyn-syms "$OUT/libnbt.so" 2>/dev/null \
     | grep -E 'UND .*(__aarch64_|__atomic_)' | grep -q .; then
  echo "ERROR: helper atomic eksternal masih dibutuhkan" >&2
  exit 1
fi

cp "$STDLIB" "$OUT/libc++_shared.so"

echo "== kompilasi jembatan SAF (Java -> classes.dex) =="
rm -rf out/javaclasses out/dex && mkdir -p out/javaclasses out/dex
"$JAVAC" -source 8 -target 8 -encoding UTF-8 -bootclasspath "$ANDROID_JAR" \
  -d out/javaclasses $(find app/java -name '*.java' | sort)
"$D8" --lib "$ANDROID_JAR" --min-api 26 --output out/dex \
  $(find out/javaclasses -name '*.class' | sort)
[ -f out/dex/classes.dex ] || { echo "d8 gagal menghasilkan classes.dex" >&2; exit 1; }
echo "classes.dex: $(stat -c%s out/dex/classes.dex) byte"

echo "== aapt2 compile + link =="
rm -f out/res.zip out/base.apk
"$AAPT2" compile --dir app/res -o out/res.zip
"$AAPT2" link -o out/base.apk -I "$ANDROID_JAR" \
  --manifest app/AndroidManifest.xml out/res.zip -A app/assets \
  --min-sdk-version 26 --target-sdk-version 37 \
  --version-code "$VERSION_CODE" --version-name "$VERSION_NAME" --no-compress

echo "== tambah native library (store, tanpa kompresi) + DEX =="
rm -rf out/stage && mkdir -p out/stage/lib/arm64-v8a
cp "$OUT"/*.so out/stage/lib/arm64-v8a/
# Reference LevelDB native library: gives the Show Map real Bedrock chunk data.
cp app/libs/arm64-v8a/libleveldb.so out/stage/lib/arm64-v8a/libleveldb.so
cp out/dex/classes.dex out/stage/classes.dex
( cd out/stage && zip -q -r -0 -X ../base.apk lib/arm64-v8a/ && zip -q -X ../base.apk classes.dex )

echo "== tanda tangan =="
"$APKSIGNER" sign \
  --v1-signing-enabled true --v2-signing-enabled true --v3-signing-enabled true \
  --ks "$HERE/out/debug.keystore" --ks-key-alias androiddebugkey \
  --ks-pass pass:android --key-pass pass:android \
  --out out/NBTEditor.apk out/base.apk
"$APKSIGNER" verify --verbose out/NBTEditor.apk >/dev/null

BADGING="$("$AAPT2" dump badging out/NBTEditor.apk)"
printf '%s\n' "$BADGING" | grep -F "package: name='com.nbteditor' versionCode='$VERSION_CODE' versionName='$VERSION_NAME'" >/dev/null
printf '%s\n' "$BADGING" | head -3

python3 - "$OUT/libnbt.so" "$HERE/out/NBTEditor.apk" <<'PY'
import hashlib, sys, zipfile
lib, apk = sys.argv[1:]
expected = open(lib, 'rb').read()
with zipfile.ZipFile(apk) as z:
    names = set(z.namelist())
    need = {"lib/arm64-v8a/libnbt.so", "lib/arm64-v8a/libc++_shared.so",
            "lib/arm64-v8a/libleveldb.so", "classes.dex",
            "assets/entityIcons/cow.png", "assets/entityIcons/steve.png",
            "assets/ui/home.png"}
    if need - names:
        raise SystemExit("APK missing: " + ", ".join(sorted(need - names)))
    packed = z.read("lib/arm64-v8a/libnbt.so")
    dex = z.read("classes.dex")
if packed != expected:
    raise SystemExit("APK libnbt.so tidak cocok dengan build segar")
if b"FilePickerActivity" not in dex:
    raise SystemExit("classes.dex tidak berisi FilePickerActivity")
print("native library sha256:", hashlib.sha256(packed).hexdigest())
print("classes.dex sha256:", hashlib.sha256(dex).hexdigest())
PY

ls -l out/NBTEditor.apk
echo "APK siap: out/NBTEditor.apk"
