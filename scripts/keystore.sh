#!/data/data/com.termux/files/usr/bin/bash
# Membuat debug keystore sekali jalan (JDK 21 sudah tersedia di Termux).
set -euo pipefail
HERE="$(cd "$(dirname "$0")/.." && pwd)"
KS="$HERE/out/debug.keystore"
mkdir -p "$HERE/out"
if [ -f "$KS" ]; then echo "keystore sudah ada: $KS"; exit 0; fi
keytool -genkeypair -keystore "$KS" -storepass android -keypass android \
  -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000 \
  -dname "CN=Android Debug,O=Android,C=US" 2>&1 | tail -1
echo "dibuat: $KS"
