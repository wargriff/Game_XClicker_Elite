"""Vérifie que les correctifs macro sont bien présents."""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)

REQUIRED = [
    ("utils/legacy_patch.py", "apply_legacy_patch"),
    ("utils/debug.py", "def log"),
    ("ui/sanctuary_window.py", "def master_combo"),
    ("ui/main_window.py", "SanctuaryWindow"),
    ("nodejs/server.js", "express"),
]


def main() -> int:
    print("=== Game XClicker Elite — verification ===")
    ok = True
    for rel, needle in REQUIRED:
        path = os.path.join(ROOT, rel.replace("/", os.sep))
        if not os.path.isfile(path):
            print(f"[FAIL] manquant: {rel}")
            ok = False
            continue
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()
        if needle not in content:
            print(f"[FAIL] {rel} — '{needle}' introuvable (fichier ancien?)")
            ok = False
        else:
            print(f"[ OK ] {rel}")

    try:
        import utils.legacy_patch  # noqa: F401

        from PyQt6.QtWidgets import QApplication, QMainWindow

        app = QApplication.instance() or QApplication([])
        w = QMainWindow()
        _ = w.master_combo
        _ = w.name_edit
        print("[ OK ] patch QMainWindow — master_combo/name_edit accessibles")
        w.close()
    except ImportError as exc:
        print(f"[WARN] test Qt ignore (PyQt6 indisponible): {exc}")
    except Exception as exc:
        print(f"[FAIL] patch runtime: {exc}")
        ok = False

    if ok:
        print("\nResultat: PRET — lance python Xmacro_main.py")
        return 0
    print("\nResultat: ANCIENNE VERSION — git pull origin cursor/sanctuary-diablo-ui-9626")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
