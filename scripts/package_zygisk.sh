#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
mkdir -p dist module/zygisk
cp -f app/src/main/libs/arm64-v8a/libRAMA_MODZ.so module/zygisk/arm64-v8a.so
cp -f app/src/main/libs/armeabi-v7a/libRAMA_MODZ.so module/zygisk/armeabi-v7a.so
cp -f module.prop module/module.prop
(cd module && zip -r -9 "$ROOT/dist/RAMA_MODZ-Zygisk.zip" .)
unzip -l dist/RAMA_MODZ-Zygisk.zip
echo "Zygisk package OK"
