#!/usr/bin/env python3
"""
Met a jour le projet depuis GitHub puis relance Mission Control.

Lancez dans PowerShell (dans CE dossier) :
  python METTRE_A_JOUR.py
"""

from __future__ import annotations

import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
os.chdir(ROOT)


def main() -> int:
    print("=" * 55)
    print("  Mise a jour — Game XClicker Elite")
    print(f"  Dossier: {ROOT}")
    print("=" * 55)

    if not os.path.isdir(os.path.join(ROOT, ".git")):
        print("\nERREUR: pas de depot git ici.")
        print("Clonez d'abord le projet dans ce dossier.")
        input("Entree...")
        return 1

    env = {**os.environ, "GIT_MERGE_AUTOEDIT": "no"}

    print("\n[1/3] git pull origin main...")
    code = subprocess.call(
        ["git", "pull", "--no-edit", "origin", "main"],
        cwd=ROOT,
        env=env,
    )
    if code != 0:
        print("\nEchec git pull. Essayez: python REPARER.py")
        input("Entree...")
        return code

    print("\n[2/3] pip install...")
    subprocess.call(
        [sys.executable, "-m", "pip", "install", "-r", "requirements.txt", "-q"],
        cwd=ROOT,
    )

    print("\n[3/3] Lancement OUVRE_MOI...")
    print()
    from launcher import run

    return run()


if __name__ == "__main__":
    raise SystemExit(main())
