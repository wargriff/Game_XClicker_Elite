#!/usr/bin/env python3
"""
Point d'entrée PyCharm / START.bat.

Par défaut : interface native PyQt6 iCUE (fenêtre Windows).
Option web : python main.py --web
"""

from __future__ import annotations

import sys


def main() -> int:
    if "--web" in sys.argv:
        from gxclicker import main as web_main

        return web_main()

    from native_app import main as native_main

    return native_main()


if __name__ == "__main__":
    raise SystemExit(main())
