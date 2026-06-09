"""Fixtures partagées — mock Win32 avant tout import core."""

import ctypes
import sys
from unittest.mock import MagicMock

import pytest


class _FakeUser32:
    @staticmethod
    def GetAsyncKeyState(_vk):
        return 0

    @staticmethod
    def SendInput(*_args, **_kwargs):
        return 1


# Mock Win32 AVANT import de core.engine (Linux / CI)
if not hasattr(ctypes, "windll"):
    ctypes.windll = MagicMock()
ctypes.windll.user32 = _FakeUser32()


@pytest.fixture(scope="session")
def qapp():
    from PyQt6.QtWidgets import QApplication

    app = QApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
    yield app
