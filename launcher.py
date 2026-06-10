"""
Game XClicker Elite — lanceur unique.

Utilisateur :
  - Windows : double-clic OUVRE_MOI.pyw
  - Terminal : python OUVRE_MOI.py  ou  python launcher.py

Mission Control (interface) gère ensuite native / web / build / .exe.
Maintenance seule : python REPARER.py
"""

from __future__ import annotations

import os
import subprocess
import sys
import traceback

ENTRY = "GameXClicker.py"


def find_project_root(start: str | None = None) -> str:
    """Trouve le dossier qui contient GameXClicker.py."""
    start = os.path.abspath(start or os.path.dirname(__file__))
    candidates = [
        start,
        os.path.join(start, "Game_XClicker_Elite"),
        os.path.dirname(start),
        os.path.join(os.path.dirname(start), "Game_XClicker_Elite"),
    ]
    seen: set[str] = set()
    for root in candidates:
        root = os.path.abspath(root)
        if root in seen:
            continue
        seen.add(root)
        if os.path.isfile(os.path.join(root, ENTRY)):
            return root
    return start


ROOT = find_project_root()


def prepare(root: str | None = None) -> str:
    root = root or ROOT
    os.chdir(root)
    if root not in sys.path:
        sys.path.insert(0, root)
    from utils.bootstrap import ensure_project_ready

    ensure_project_ready(root)
    return root


def show_error(message: str, *, title: str = "Game XClicker Elite") -> None:
    if sys.platform == "win32":
        try:
            import ctypes

            ctypes.windll.user32.MessageBoxW(0, message, title, 0x10)
            return
        except Exception:
            pass
    print(message, file=sys.stderr)


def verify_installation(root: str | None = None) -> str | None:
    root = root or ROOT
    if os.path.isfile(os.path.join(root, ENTRY)):
        return None
    return (
        f"Dossier projet introuvable.\n\n"
        f"Dossier actuel : {root}\n\n"
        f"Ouvrez PowerShell dans Game_XClicker_Elite et lancez :\n"
        f"  python REPARER.py"
    )


def ensure_dependencies(*, quiet: bool = True) -> bool:
    try:
        import PyQt6.QtWidgets  # noqa: F401

        return True
    except ImportError:
        pass

    req = os.path.join(ROOT, "requirements.txt")
    if not os.path.isfile(req):
        return False

    print("[launcher] Installation des dépendances Python...")
    flags = ["-q"] if quiet else []
    return subprocess.call(
        [sys.executable, "-m", "pip", "install", "-r", req, *flags],
        cwd=ROOT,
    ) == 0


def run(argv: list[str] | None = None) -> int:
    """Prépare l'environnement puis ouvre Mission Control (ou mode CLI)."""
    root = prepare()
    err = verify_installation(root)
    if err:
        show_error(err)
        return 1
    if not ensure_dependencies():
        show_error(
            "Impossible d'installer les dépendances.\n\n"
            "python -m pip install -r requirements.txt"
        )
        return 1

    if argv is not None:
        saved = sys.argv
        sys.argv = [ENTRY, *argv]
        try:
            from GameXClicker import main

            return main()
        finally:
            sys.argv = saved

    from GameXClicker import main

    return main()


def run_safe(argv: list[str] | None = None) -> int:
    """Comme run(), avec boîte d'erreur pour pythonw (double-clic)."""
    try:
        return run(argv)
    except SystemExit:
        raise
    except Exception:
        show_error(traceback.format_exc())
        return 1


def main() -> int:
    if len(sys.argv) > 1:
        return run(sys.argv[1:])
    return run()


if __name__ == "__main__":
    raise SystemExit(main())
