"""
Système d'assets — chemins brand / ui / devices.
Import: from config.asset_system import assets
"""

import os

from config.paths import (
    BRAND_DIR,
    DEVICES_DIR,
    FAVICON_ICO,
    FAVICON_PNG,
    FAVICON_SVG,
    LEGACY_BG,
    LEGACY_FAVICON,
    LEGACY_MOUSE,
    PROFILES_DIR,
    UI_BG_DIR,
    UI_ICONS_DIR,
    resolve_icon,
)


class AssetSystem:
    brand_dir = BRAND_DIR
    ui_icons_dir = UI_ICONS_DIR
    ui_bg_dir = UI_BG_DIR
    devices_dir = DEVICES_DIR
    profiles_dir = PROFILES_DIR

    @staticmethod
    def icon() -> str:
        return resolve_icon()

    @staticmethod
    def favicon_svg() -> str:
        for path in (FAVICON_SVG, LEGACY_FAVICON):
            if os.path.isfile(path):
                return path
        return FAVICON_SVG

    @staticmethod
    def favicon_ico() -> str:
        return FAVICON_ICO if os.path.isfile(FAVICON_ICO) else AssetSystem.icon()

    @staticmethod
    def background() -> str:
        legacy_bg = os.path.join(UI_BG_DIR, "diablo_bg.svg")
        if os.path.isfile(legacy_bg):
            return legacy_bg
        return LEGACY_BG if os.path.isfile(LEGACY_BG) else legacy_bg

    @staticmethod
    def device_image(name: str = "mouse") -> str:
        path = os.path.join(DEVICES_DIR, f"{name}.svg")
        if os.path.isfile(path):
            return path
        return LEGACY_MOUSE if os.path.isfile(LEGACY_MOUSE) else path

    @staticmethod
    def ui_icon(name: str) -> str:
        path = os.path.join(UI_ICONS_DIR, f"{name}.svg")
        if os.path.isfile(path):
            return path
        legacy = os.path.normpath(
            os.path.join(os.path.dirname(UI_ICONS_DIR), "..", "icons", f"{name}.svg")
        )
        return legacy if os.path.isfile(legacy) else path

    @staticmethod
    def brand_url(filename: str) -> str:
        return f"/brand/{filename}"

    @staticmethod
    def verify() -> list[str]:
        missing = []
        for label, path in (
            ("icon", AssetSystem.icon()),
            ("favicon_svg", AssetSystem.favicon_svg()),
        ):
            if not os.path.isfile(path):
                missing.append(f"{label}: {path}")
        return missing


assets = AssetSystem()

__all__ = ["AssetSystem", "assets"]
