"""Chemins centralisés — assets séparés proprement."""

import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def _bundle_root() -> str:
    """Racine en mode PyInstaller (onefile)."""
    if getattr(sys, "frozen", False):
        return sys._MEIPASS  # type: ignore[attr-defined]
    return ROOT


BUNDLE = _bundle_root()

# Brand / icône bureau (.exe, raccourci)
BRAND_DIR = os.path.join(BUNDLE, "assets", "brand")
FAVICON_ICO = os.path.join(BRAND_DIR, "favicon.ico")
FAVICON_SVG = os.path.join(BRAND_DIR, "favicon.svg")
FAVICON_PNG = os.path.join(BRAND_DIR, "favicon-96x96.png")

# UI web
UI_WEB_DIR = os.path.join(BUNDLE, "ui-web")

# Assets interface
UI_ICONS_DIR = os.path.join(BUNDLE, "assets", "ui", "icons")
UI_BG_DIR = os.path.join(BUNDLE, "assets", "ui", "backgrounds")
DEVICES_DIR = os.path.join(BUNDLE, "assets", "devices")

# Legacy (compat PyQt)
LEGACY_FAVICON = os.path.join(BUNDLE, "assets", "favicon", "favicon.svg")
LEGACY_BG = os.path.join(BUNDLE, "assets", "bg", "diablo_bg.svg")
LEGACY_MOUSE = os.path.join(BUNDLE, "assets", "mouse.svg")

PROFILES_DIR = os.path.join(BUNDLE, "profiles")
NODEJS_DIR = os.path.join(BUNDLE, "nodejs")


def resolve_icon() -> str:
    for path in (FAVICON_ICO, FAVICON_PNG, FAVICON_SVG, LEGACY_FAVICON):
        if os.path.exists(path):
            return path
    return FAVICON_SVG
