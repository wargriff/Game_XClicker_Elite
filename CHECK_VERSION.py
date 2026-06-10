"""Verification Game XClicker Elite."""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)

FILES = [
    "START.bat",
    "GameXClicker.py",
    "ui/mission_control.py",
    "BUILD.bat",
    "REPARER.bat",
    "native_app.py",
    "gxclicker.py",
    "build.spec",
    "config/asset_system.py",
    "ui-web/index.html",
    "services/bootstrap.py",
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
    print()
    if ok:
        print("PRET — un seul programme:")
        print("  GameXClicker.py  ou  START.bat  → Mission Control")
        print("  PyCharm: import pycharm/Game_XClicker_Elite.run.xml")
        print("  Build .exe → copie auto sur le Bureau")
    else:
        print("INCOMPLET — REPARER.bat")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
