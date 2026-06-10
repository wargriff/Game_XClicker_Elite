"""Double-clic Windows — ouvre Mission Control (pas de console)."""

from __future__ import annotations

import importlib.util
import os
import sys

_ROOT = os.path.dirname(os.path.abspath(__file__))
os.chdir(_ROOT)
if _ROOT not in sys.path:
    sys.path.insert(0, _ROOT)

_launcher_path = os.path.join(_ROOT, "launcher.py")
if not os.path.isfile(_launcher_path):
    import ctypes

    ctypes.windll.user32.MessageBoxW(
        0,
        f"launcher.py introuvable dans :\n{_ROOT}",
        "Game XClicker Elite",
        0x10,
    )
    raise SystemExit(1)

_spec = importlib.util.spec_from_file_location("gx_launcher", _launcher_path)
_mod = importlib.util.module_from_spec(_spec)
assert _spec.loader is not None
_spec.loader.exec_module(_mod)

raise SystemExit(_mod.run_safe())
