"""Point d'entrée legacy — redirige toujours vers SanctuaryWindow."""

import utils.legacy_patch  # noqa: F401

from ui.sanctuary_window import MainWindow, SanctuaryWindow

__all__ = ["MainWindow", "SanctuaryWindow"]
