#!/usr/bin/env python3
"""
Game XClicker Elite — point d'entrée UNIQUE.

  GameXClicker.py           → Mission Control (hub visuel)
  GameXClicker.py --native → interface PyQt6 iCUE
  GameXClicker.py --web      → interface web
  GameXClicker.py --build    → compile .exe + copie Bureau

PyCharm / START.bat : lancez uniquement ce fichier.
"""

from __future__ import annotations

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)


def main() -> int:
    args = sys.argv[1:]

    if "--native" in args:
        from native_app import main as native_main

        return native_main()

    if "--web" in args:
        from gxclicker import main as web_main

        return web_main()

    if "--build" in args:
        import subprocess

        script = os.path.join(ROOT, "scripts", "build_exe.py")
        extra = ["--desktop"] if "--desktop" in args else []
        return subprocess.call([sys.executable, script] + extra, cwd=ROOT)

    from ui.mission_control import main as hub_main

    return hub_main()


if __name__ == "__main__":
    raise SystemExit(main())
