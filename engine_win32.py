"""Backward-compatible alias for core.engine."""
try:
    import utils.legacy_patch  # noqa: F401 — master_combo/name_edit sur QMainWindow
except ImportError:
    pass
from core.engine import *  # noqa: F401, F403
from core.models import Btn, Stats  # noqa: F401
