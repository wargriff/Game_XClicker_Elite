"""
Point d'entrée principal — Game XClicker Elite v3.0

Usage:
  python run.py               # Interface JS iCUE (défaut)
  python run.py --pyqt        # PyQt Sanctuary legacy
  python run.py --browser     # Navigateur
"""

import argparse
import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)

from utils.bootstrap import ensure_project_ready

ensure_project_ready(ROOT)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description="Game XClicker Elite — SOURIS WARGRIFF")
    parser.add_argument("--pyqt", action="store_true", help="Interface PyQt legacy")
    parser.add_argument("--browser", action="store_true", help="UI web dans navigateur")
    args = parser.parse_args(argv)

    try:
        from config.asset_system import assets
        missing = assets.verify()
        if missing:
            print("[MAIN] Assets manquants (non bloquant):")
            for m in missing:
                print(f"  - {m}")
    except ImportError as exc:
        print(f"[MAIN] WARN assets: {exc}")

    if args.pyqt:
        from Xmacro_main import main as pyqt_main
        return pyqt_main()

    if args.browser:
        os.environ["XCLICKER_UI"] = "browser"

    from launcher.desktop_main import main as desktop_main
    return desktop_main()


if __name__ == "__main__":
    raise SystemExit(main())
