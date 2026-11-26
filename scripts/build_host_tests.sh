#!/data/data/com.termux/files/usr/bin/bash
# Uji engine NBT di host (Termux) dengan ASan/UBSan. Tidak butuh perangkat/APK.
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
cd "$HERE"
mkdir -p out
# VERSION adalah sumber kebenaran tunggal; suntikkan ke source native.
VERSION_CODE="$(awk -F= '/^VERSION_CODE=/{print $2}' VERSION)"
VERSION_NAME="$(awk -F= '/^VERSION_NAME=/{print $2}' VERSION)"
: "${VERSION_CODE:?VERSION_CODE belum diisi di berkas VERSION}"
: "${VERSION_NAME:?VERSION_NAME belum diisi di berkas VERSION}"
echo "== screenshot batching tests =="
python3 scripts/test_batch_uishots.py
echo "== compile host tests =="
clang++ -std=c++20 -O1 -g -fsanitize=address,undefined -Wall -Wextra -Werror \
  "-DNBT_APP_VERSION_NAME=\"$VERSION_NAME\"" "-DNBT_APP_VERSION_CODE=$VERSION_CODE" \
  -DNBT_SELFTEST app/src/main.native.cpp -o out/nbt_test -lz
echo "== run =="
out/nbt_test
