#!/usr/bin/env bash
# Build injector executable ke root-apk/src/main/jniLibs (dipackage Gradle sebagai prebuilt).
# Jalankan SEBELUM ./gradlew :root-apk:assembleRelease. Butuh ANDROID_NDK_HOME (r25c).
set -euo pipefail
cd "$(dirname "$0")"
NDK="${ANDROID_NDK_HOME:-${ANDROID_HOME:-$HOME/android-sdk}/ndk/25.2.9519653}"
"$NDK/ndk-build" \
  NDK_PROJECT_PATH=null \
  APP_BUILD_SCRIPT="$PWD/root-apk/injector.mk" \
  NDK_APPLICATION_MK="$PWD/root-apk/injector-app.mk" \
  NDK_LIBS_OUT="$PWD/root-apk/src/main/jniLibs" \
  NDK_OUT="$PWD/build/injector-obj" \
  -j4
for f in root-apk/src/main/jniLibs/*/librama_inject; do
  if [ -f "$f" ]; then mv "$f" "$f.so"; fi
done
find root-apk/src/main/jniLibs -type f -exec ls -la {} \;
