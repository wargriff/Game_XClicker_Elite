"""Correctifs legacy — master_combo / name_edit sur TOUT QMainWindow."""

from __future__ import annotations

_APPLIED = False
_LEGACY_NAMES = ("master_combo", "name_edit")


def _create_legacy(name: str):
    from PyQt6.QtWidgets import QComboBox, QLineEdit

    if name == "name_edit":
        edit = QLineEdit("default")
        edit.setReadOnly(True)
        return edit

    combo = QComboBox()
    try:
        from ui.pages.macros_page import MACRO_KEYS

        for _, label in MACRO_KEYS:
            combo.addItem(label)
    except Exception:
        combo.addItem("Macro 1")
        combo.addItem("Macro 2")
    return combo


def _resolve_legacy(self, name: str):
    storage = f"_xclicker_legacy_{name}"
    data = object.__getattribute__(self, "__dict__")
    if storage not in data:
        data[storage] = _create_legacy(name)

    macros = data.get("macros")
    if macros is not None and hasattr(macros, name):
        return getattr(macros, name)
    return data[storage]


def apply_legacy_patch() -> bool:
    global _APPLIED
    if _APPLIED:
        return True

    try:
        from PyQt6.QtWidgets import QMainWindow
    except Exception:
        return False

    if getattr(QMainWindow, "_xclicker_legacy_patch", False):
        _APPLIED = True
        return True

    _orig = QMainWindow.__getattribute__

    def _patched_getattribute(self, name: str):
        if name in _LEGACY_NAMES:
            try:
                return _orig(self, name)
            except AttributeError:
                return _resolve_legacy(self, name)
        return _orig(self, name)

    QMainWindow.__getattribute__ = _patched_getattribute
    QMainWindow._xclicker_legacy_patch = True
    _APPLIED = True
    return True


apply_legacy_patch()
