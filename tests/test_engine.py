"""Tests bootstrap (sans PyQt6)."""

import pytest

from core.engine import MacroManager
from services.engine_proxy import EngineProxy
from services.profile_manager import ProfileManager
from services.sidecar_api import SidecarAPI


def test_engine_starts():
    mgr = MacroManager()
    assert mgr.mouse.enabled
    mgr.stop()


def test_proxy_buttons():
    proxy = EngineProxy(MacroManager())
    assert "left" in proxy.buttons
    assert "right" in proxy.buttons
    proxy.stop()


def test_profile_load_default():
    pm = ProfileManager()
    data = pm.load("default")
    assert isinstance(data, dict)


def test_sidecar_health_paths():
    proxy = EngineProxy(MacroManager())
    api = SidecarAPI(proxy)
    api.start()
    if not api.online:
        pytest.skip("Port 17840 déjà utilisé (app en cours d'exécution)")
    api.stop()
    proxy.stop()
