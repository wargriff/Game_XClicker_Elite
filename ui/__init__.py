"""UI entry points — Sanctuary Edition."""

import utils.legacy_patch  # noqa: F401 — patch QMainWindow avant import fenêtre

from ui.sanctuary_window import MainWindow, SanctuaryWindow

UI = SanctuaryWindow

__all__ = ["MainWindow", "SanctuaryWindow", "UI"]
