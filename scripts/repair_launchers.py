#!/usr/bin/env python3
"""Repare run.py / main.py si anciennes versions (ui.asset_system)."""

from __future__ import annotations

import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

RUN_PY = '''#!/usr/bin/env python3
"""
Compat PyCharm — NE PAS MODIFIER.
Redirige vers gxclicker.py (sans ui.asset_system).
"""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)

# Fix conflit ui.py / dossier ui/
_ui = os.path.join(ROOT, "ui.py")
if os.path.isfile(_ui) and os.path.isdir(os.path.join(ROOT, "ui")):
    _bak = _ui + ".bak"
    n = 0
    while os.path.exists(_bak):
        n += 1
        _bak = _ui + f".bak{n}"
    try:
        os.rename(_ui, _bak)
        print(f"[run.py] ui.py -> {os.path.basename(_bak)}")
    except OSError:
        pass
if "ui" in sys.modules:
    del sys.modules["ui"]

from gxclicker import main

if __name__ == "__main__":
    raise SystemExit(main())
'''

MAIN_PY = '''#!/usr/bin/env python3
"""Alias — delegue a gxclicker.py."""

import os
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)

from gxclicker import main

if __name__ == "__main__":
    raise SystemExit(main())
'''


def _read_lines(path: str) -> list[str]:
    try:
        return open(path, encoding="utf-8", errors="replace").read().splitlines()
    except OSError:
        return []


def _needs_repair_run(path: str) -> bool:
    if not os.path.isfile(path):
        return True
    for line in _read_lines(path):
        s = line.strip()
        if s.startswith("#") or not s:
            continue
        if s.startswith("from gxclicker import main"):
            return False
        if s == "from main import main" or s.startswith("from main import "):
            return True
        if s.startswith("from ui.") or s.startswith("import ui"):
            return True
    return True


def _needs_repair_main(path: str) -> bool:
    if not os.path.isfile(path):
        return True
    for line in _read_lines(path):
        s = line.strip()
        if s.startswith("#") or not s:
            continue
        if "ui.asset_system" in s or s.startswith("from ui.") or s.startswith("import ui"):
            return True
        if s.startswith("from gxclicker import main"):
            return False
    return True


def repair() -> bool:
    changed = False
    run_path = os.path.join(ROOT, "run.py")
    main_path = os.path.join(ROOT, "main.py")

    if _needs_repair_run(run_path):
        open(run_path, "w", encoding="utf-8", newline="\n").write(RUN_PY)
        print("[repair] run.py corrige -> gxclicker.py")
        changed = True

    if _needs_repair_main(main_path):
        open(main_path, "w", encoding="utf-8", newline="\n").write(MAIN_PY)
        print("[repair] main.py corrige -> gxclicker.py")
        changed = True

    ui_py = os.path.join(ROOT, "ui.py")
    ui_dir = os.path.join(ROOT, "ui")
    if os.path.isfile(ui_py) and os.path.isdir(ui_dir):
        bak = ui_py + ".bak"
        n = 0
        while os.path.exists(bak):
            n += 1
            bak = ui_py + f".bak{n}"
        try:
            os.rename(ui_py, bak)
            print(f"[repair] ui.py -> {os.path.basename(bak)}")
            changed = True
        except OSError as exc:
            print(f"[repair] Fermez PyCharm puis: ren ui.py ui_old.py ({exc})")

    return changed


if __name__ == "__main__":
    repair()
