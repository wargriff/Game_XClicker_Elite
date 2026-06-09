"""Vérifie que la build v3.0 (JS iCUE + launcher) est complète."""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)

REQUIRED = [
    ("launcher/desktop_main.py", "webview"),
    ("ui-web/index.html", "Game XClicker Elite"),
    ("ui-web/js/app.js", "MACROS"),
    ("assets/brand/favicon.ico", None),
    ("config/paths.py", "BRAND_DIR"),
    ("services/sidecar_api.py", "MACRO_KEYS"),
    ("nodejs/server.js", "ui-web"),
    ("utils/legacy_patch.py", "apply_legacy_patch"),
]


def main() -> int:
    print("=== Game XClicker Elite v3.0 — verification ===")
    ok = True
    for rel, needle in REQUIRED:
        path = os.path.join(ROOT, rel.replace("/", os.sep))
        if not os.path.isfile(path):
            print(f"[FAIL] manquant: {rel}")
            ok = False
            continue
        if needle:
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()
            if needle not in content:
                print(f"[FAIL] {rel} — '{needle}' introuvable")
                ok = False
            else:
                print(f"[ OK ] {rel}")
        else:
            print(f"[ OK ] {rel}")

    if ok:
        print("\nResultat: PRET — START.bat ou launcher\\LAUNCH_DESKTOP.bat")
        return 0
    print("\nResultat: INCOMPLET — git pull")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
