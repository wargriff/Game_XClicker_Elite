#!/usr/bin/env python3
"""
Réparation complète — maintenance uniquement (pas le lanceur quotidien).

  python REPARER.py

Git fetch + reset, pip, Node, puis ouverture de Mission Control.
"""

from __future__ import annotations

import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)

from scripts.find_python import find_python
from scripts.setup import install_node_deps, install_pip_deps, unblock_windows

REQUIRED = (
    "GameXClicker.py",
    "launcher.py",
    "gxclicker.py",
    "rgb_engine.py",
    "core/rgb_engine.py",
    "services/bootstrap.py",
    "ui-web/index.html",
)


def _run(cmd: list[str], cwd: str = ROOT) -> int:
    print(">", " ".join(cmd))
    return subprocess.call(cmd, cwd=cwd)


def main(*, launch: bool = True) -> int:
    os.chdir(ROOT)
    py = find_python(ROOT)
    print("=" * 60)
    print("  REPARER.py — Game XClicker Elite")
    print(f"  Python: {py}")
    print("=" * 60)

    print("[0/7] Déblocage Windows...")
    unblock_windows(ROOT)

    if not os.path.isdir(os.path.join(ROOT, ".git")):
        print("ERREUR: pas de dépôt git. Clonez :")
        print("  git clone https://github.com/wargriff/Game_XClicker_Elite.git")
        input("Entrée...")
        return 1

    backup = os.path.join(ROOT, "_backup_local")
    os.makedirs(backup, exist_ok=True)
    for name in ("OUVRE_MOI.pyw", "GameXClicker.py", "profiles/default.json"):
        src = os.path.join(ROOT, name.replace("/", os.sep))
        if os.path.isfile(src):
            shutil.copy2(src, os.path.join(backup, os.path.basename(name) + ".bak"))

    print("[1/7] git fetch...")
    if _run(["git", "fetch", "origin", "main"]) != 0:
        input("git fetch échoué — Entrée...")
        return 1

    print("[2/7] git reset main...")
    _run(["git", "checkout", "-B", "main", "origin/main"])
    _run(["git", "reset", "--hard", "origin/main"])

    print("[3/7] Vérification fichiers...")
    missing = [f for f in REQUIRED if not os.path.isfile(os.path.join(ROOT, f.replace("/", os.sep)))]
    if missing:
        print("MANQUE:", ", ".join(missing))
        input("Entrée...")
        return 1
    print("  OK")

    from utils.bootstrap import ensure_project_ready

    ensure_project_ready(ROOT)

    print("[4/7] pip install...")
    if not install_pip_deps(py, ROOT, quiet=False):
        input("pip échoué — Entrée...")
        return 1

    print("[5/7] Node.js...")
    install_node_deps(ROOT)

    print("[6/7] CHECK_VERSION...")
    subprocess.call([py, "CHECK_VERSION.py"], cwd=ROOT)

    print("[7/7] Terminé.")
    if not launch:
        print("Réparation OK. Lancez OUVRE_MOI.pyw ou python OUVRE_MOI.py")
        return 0

    print("Ouverture Mission Control...\n")
    from launcher import run

    return run()


if __name__ == "__main__":
    raise SystemExit(main())
