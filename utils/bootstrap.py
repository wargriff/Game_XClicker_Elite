"""
Bootstrap projet — corrige conflit ui.py / dossier ui/ et vérifie les fichiers.
"""

import os
import sys


def ensure_project_ready(root: str | None = None) -> bool:
    root = root or os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    if root not in sys.path:
        sys.path.insert(0, root)

    _fix_ui_py_conflict(root)
    _ensure_asset_system(root)
    return True


def _fix_ui_py_conflict(root: str):
    """ui.py à la racine empêche d'importer le package ui/."""
    ui_py = os.path.join(root, "ui.py")
    ui_dir = os.path.join(root, "ui")
    if not (os.path.isfile(ui_py) and os.path.isdir(ui_dir)):
        return

    backup = ui_py + ".bak"
    counter = 1
    while os.path.exists(backup):
        backup = ui_py + f".bak{counter}"
        counter += 1

    try:
        os.rename(ui_py, backup)
        print(f"[BOOT] Conflit corrige: ui.py -> {os.path.basename(backup)}")
        print("[BOOT] Le dossier ui/ peut maintenant etre utilise comme package.")
    except OSError as exc:
        print(f"[BOOT] ERREUR: impossible de renommer ui.py: {exc}")
        print("[BOOT] Renommez manuellement ui.py en ui_old.py puis relancez.")

    if "ui" in sys.modules:
        mod = sys.modules["ui"]
        mod_file = getattr(mod, "__file__", "") or ""
        if mod_file.replace("\\", "/").endswith("/ui.py"):
            del sys.modules["ui"]


def _ensure_asset_system(root: str):
    """Garantit config/asset_system.py — copie depuis ui/ si absent."""
    cfg_path = os.path.join(root, "config", "asset_system.py")
    ui_path = os.path.join(root, "ui", "asset_system.py")
    if os.path.isfile(cfg_path):
        return
    if os.path.isfile(ui_path):
        import shutil
        os.makedirs(os.path.dirname(cfg_path), exist_ok=True)
        shutil.copy2(ui_path, cfg_path)
        print("[BOOT] config/asset_system.py cree depuis ui/asset_system.py")
