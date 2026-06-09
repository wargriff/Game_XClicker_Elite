#!/usr/bin/env python3
"""Corrige le conflit ui.py vs dossier ui/ — a lancer une fois si erreur import."""

import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT)

from utils.bootstrap import ensure_project_ready

if __name__ == "__main__":
    ensure_project_ready(ROOT)
    ui_py = os.path.join(ROOT, "ui.py")
    if os.path.isfile(ui_py):
        print("[FIX] ui.py existe encore — fermez PyCharm et relancez ce script.")
        sys.exit(1)
    print("[FIX] OK — lancez START.bat ou python run.py")
    sys.exit(0)
