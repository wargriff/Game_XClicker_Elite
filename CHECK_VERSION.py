"""Verification START.bat + gxclicker.py."""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)

FILES = [
    "START.bat",
    "REPARER.bat",
    "main.py",
    "native_app.py",
    "gxclicker.py",
    "build.spec",
    "config/asset_system.py",
    "ui-web/index.html",
    "assets/brand/favicon.ico",
    "services/bootstrap.py",
    "services/sidecar_api.py",
]

DEAD = [
    "run.py",
    "launch.py",
    "BUILD.bat",
    "FIX_START.bat",
    "launchers/START.bat",
]


def main() -> int:
    print("=== Verification Game XClicker Elite ===")
    ok = True
    for f in FILES:
        p = os.path.join(ROOT, f.replace("/", os.sep))
        if os.path.isfile(p):
            print(f"[ OK ] {f}")
        else:
            print(f"[FAIL] {f}")
            ok = False
    for f in DEAD:
        if os.path.isfile(os.path.join(ROOT, f.replace("/", os.sep))):
            print(f"[WARN] obsolete — supprimez: {f}")
    if os.path.isfile(os.path.join(ROOT, "ui.py")):
        print("[WARN] ui.py present — START.bat le renomme")
    print()
    if not ok:
        print("INCOMPLET — double-cliquez REPARER.bat")
        print("  ou: git pull origin main")
    else:
        print("PRET — double-clic START.bat")
        print("PyCharm script: main.py  (PyQt6 natif — pas pygame)")
    print("Mode web optionnel: python main.py --web")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
