"""Vérifie que la build v3.0 est complète et sans conflit ui.py."""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, ROOT)

REQUIRED = [
    ("run.py", "ensure_project_ready"),
    ("main.py", "config.asset_system"),
    ("config/asset_system.py", "AssetSystem"),
    ("utils/bootstrap.py", "_fix_ui_py_conflict"),
    ("launcher/desktop_main.py", "webview"),
    ("ui-web/index.html", "Game XClicker Elite"),
    ("ui-web/js/app.js", "LIGHTING SETUP"),
    ("assets/brand/favicon.ico", None),
    ("services/sidecar_api.py", "MACRO_KEYS"),
    ("nodejs/server.js", "ui-web"),
]


def main() -> int:
    print("=== Game XClicker Elite v3.0 — verification ===")
    ok = True

    ui_py = os.path.join(ROOT, "ui.py")
    ui_dir = os.path.join(ROOT, "ui")
    if os.path.isfile(ui_py) and os.path.isdir(ui_dir):
        print("[WARN] ui.py detecte — sera renomme auto au lancement (scripts/fix_ui_conflict.py)")
        print("       Ou renommez manuellement: ren ui.py ui_old.py")

    for rel, needle in REQUIRED:
        path = os.path.join(ROOT, rel.replace("/", os.sep))
        if not os.path.isfile(path):
            print(f"[FAIL] manquant: {rel}")
            ok = False
            continue
        if needle:
            with open(path, encoding="utf-8", errors="ignore") as f:
                content = f.read()
            if needle not in content:
                print(f"[FAIL] {rel} — '{needle}' introuvable (git pull requis)")
                ok = False
            else:
                print(f"[ OK ] {rel}")
        else:
            print(f"[ OK ] {rel}")

    if ok:
        print("\nResultat: PRET — START.bat ou python run.py")
        return 0
    print("\nResultat: INCOMPLET — git pull origin cursor/icue-web-launcher-9626")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
