# ZYGISK SRC RAMA MODZ — dual build

Source modul Zygisk Free Fire (OB55) — **credits: RAMA MODZ / @RAMA_MODZ**.

## Dual CI

| Mode | Artifact | Use |
|------|----------|-----|
| Zygisk | `RAMA_MODZ-Zygisk.zip` | Magisk / ZygiskNext |
| Root APK | overlay injector (WIP in CI) | root + SYSTEM_ALERT_WINDOW |

Push to `main` → **Dual Build + Release** → GitHub Release.

## Offset auto-update

1. Put Il2CppDumper output in `dumps/OBxx/`
2. Run workflow **Offset pipeline** or `python3 scripts/offset_pipeline/resolve_offsets.py`
3. Header: `app/src/main/jni/generated/Offsets_current.h`

## Target

`com.dts.freefireth`, `com.dts.freefiremax` — Aimbot, Aim Silent, ESP, Jump/Speed/Camera, etc.

Teknik: Zygisk, Dobby, IL2CPP by name, ImGui, KittyMemory, xdl.
