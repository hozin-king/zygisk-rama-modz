#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
mkdir -p dist
if ! ls dist/*.apk >/dev/null 2>&1; then
  echo "Root APK full Gradle tree not wired yet — placeholder for CI." > dist/ROOT_APK_BUILD_PENDING.txt
fi
ls -la dist/
