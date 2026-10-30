#!/usr/bin/env python3
from __future__ import annotations
import argparse, json, re, sys
from pathlib import Path

NEEDED = [
    ("UnityEngine.Camera", "get_main"),
    ("UnityEngine.Camera", "WorldToScreenPoint"),
    ("UnityEngine.Camera", "set_fieldOfView"),
]

def parse_dump_cs(text: str) -> dict[str, int]:
    found = {}
    pattern = re.compile(
        r"//\s*RVA:\s*(0x[0-9A-Fa-f]+).*?\n\s*(?:public|private|internal|protected).*\b(\w+)\s*\(",
        re.MULTILINE,
    )
    for m in pattern.finditer(text):
        found[m.group(2)] = int(m.group(1), 16)
    return found

def parse_script_json(data: dict) -> dict[str, int]:
    found = {}
    for entry in data.get("ScriptMethod", data.get("ScriptMethods", [])):
        name = entry.get("Name") or entry.get("name") or ""
        addr = entry.get("Address") or entry.get("address")
        if name and addr is not None:
            short = name.split("::")[-1].split("(")[0]
            try:
                found[short] = int(addr) if not isinstance(addr, str) else int(addr, 0)
            except ValueError:
                pass
    return found

def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--dumps-dir", default="dumps")
    ap.add_argument("--ob", default="current")
    ap.add_argument("--out", default="app/src/main/jni/generated/Offsets_current.h")
    args = ap.parse_args()
    dumps = Path(args.dumps_dir)
    found: dict[str, int] = {}
    if dumps.is_dir():
        for p in sorted(dumps.rglob("*")):
            if p.suffix.lower() == ".json":
                try:
                    found.update(parse_script_json(json.loads(p.read_text(encoding="utf-8", errors="ignore"))))
                except Exception as e:
                    print(f"skip {p}: {e}", file=sys.stderr)
            elif p.name.lower() in ("dump.cs", "dump.cs.txt") or p.suffix == ".cs":
                found.update(parse_dump_cs(p.read_text(encoding="utf-8", errors="ignore")))
    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "#pragma once",
        f"// Auto-generated offset header — OB label: {args.ob}",
        "// Prefer Il2CppGetMethodOffset by name; RVAs are fallbacks after dump.",
        "",
    ]
    for cls, method in NEEDED:
        if method in found:
            lines.append(f"#define OFF_{method} 0x{found[method]:X}  // {cls}")
        else:
            lines.append(f"// #define OFF_{method}  /* not found in dumps — use name resolve */")
    lines.append("")
    out.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {out}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
