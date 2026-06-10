"""Verification Game XClicker Elite."""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)

FILES = [
    "OUVRE_MOI.py",
    "OUVRE_MOI.pyw",
    "launcher.py",
    "cpp/CMakeLists.txt",
    "GameXClicker.py",
    "ui/control_panel.py",
    "REPARER.py",
    "native_app.py",
    "rgb_engine.py",
    "core/rgb_engine.py",
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
        print("PRET — lanceur unique :")
        print("  double-clic OUVRE_MOI.pyw   # Windows")
        print("  python OUVRE_MOI.py         # PyCharm / terminal")
        print("  python REPARER.py           # maintenance seulement")
    else:
        print("INCOMPLET — python REPARER.py")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
