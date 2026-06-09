"""
Entrée PyCharm / START.bat — bootstrap + main
"""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)

from utils.bootstrap import ensure_project_ready

ensure_project_ready(ROOT)

from main import main

if __name__ == "__main__":
    raise SystemExit(main())
