"""Tests UI Sanctuary — chaque onglet et bouton principal."""

import pytest

pytest.importorskip("PyQt6")

from PyQt6.QtWidgets import QApplication

from core.engine import MacroManager
from services.bootstrap import bootstrap
from services.engine_proxy import EngineProxy
from ui.sanctuary_window import MainWindow, SanctuaryWindow


@pytest.fixture
def boot_ctx():
    return bootstrap()


@pytest.fixture
def window(qapp, boot_ctx):
    w = SanctuaryWindow(boot_ctx.proxy, boot=boot_ctx)
    w.show()
    qapp.processEvents()
    yield w
    w.close()
    qapp.processEvents()


class TestWindowBasics:
    def test_mainwindow_is_sanctuary(self):
        assert MainWindow is SanctuaryWindow

    def test_legacy_attrs_exist(self, window):
        assert hasattr(window, "master_combo")
        assert hasattr(window, "name_edit")
        assert window.master_combo is not None
        assert window.name_edit is not None

    def test_opens_without_crash(self, window):
        assert window.isVisible()
        window.refresh_all()
        QApplication.processEvents()


class TestHeaderTabs:
    @pytest.mark.parametrize("tab_key,index", [
        ("home", 0),
        ("dashboard", 1),
        ("devices", 2),
        ("macros", 3),
        ("settings", 4),
    ])
    def test_header_tab_switch(self, window, tab_key, index):
        window.header._select_tab(tab_key, emit=True)
        QApplication.processEvents()
        assert window.stack.currentIndex() == index

    def test_macros_tab_no_recursion(self, window):
        """Reproduit le clic MACROS — ne doit pas lever RecursionError."""
        for _ in range(5):
            window.header._select_tab("macros", emit=True)
            QApplication.processEvents()
            window.macros.refresh()
            QApplication.processEvents()
        assert window.stack.currentWidget() is window.macros

    def test_stasis_toggle(self, window):
        before = window.engine.enabled
        window.header.stasis_clicked.emit()
        assert window.engine.enabled != before


class TestSidebarSections:
    @pytest.mark.parametrize("section", [
        "performance", "graphing", "lighting",
        "channel1", "channel2",
        "macro1", "macro2", "macro3", "macro4", "macro5", "macro6",
    ])
    def test_sidebar_section(self, window, section):
        btn = window.sidebar._buttons[section]
        btn.click()
        QApplication.processEvents()
        window.refresh_all()
        QApplication.processEvents()


class TestMacrosPage:
    def test_master_combo_switch(self, window):
        window.header._select_tab("macros", emit=True)
        QApplication.processEvents()
        combo = window.macros.master_combo
        for i in range(combo.count()):
            combo.setCurrentIndex(i)
            QApplication.processEvents()
            window.macros.refresh()
            QApplication.processEvents()

    def test_burst_buttons_no_recursion(self, window):
        window.header._select_tab("macros", emit=True)
        window.macros.focus_section("macro2")
        QApplication.processEvents()
        panel = window.macros.panel
        for val, btn in panel.burst_buttons:
            btn.click()
            QApplication.processEvents()
            panel.sync_from_engine()
            QApplication.processEvents()

    def test_name_edit_profile(self, window):
        window.macros.set_profile_name("test_profile")
        assert window.name_edit.text() == "test_profile"


class TestHomeTiles:
    def test_home_refresh(self, window):
        window.home.refresh(api_online=True)
        QApplication.processEvents()

    def test_rescan_devices(self, window):
        window.sensor_panel.rescan_btn.click()
        QApplication.processEvents()


class TestEngineProxy:
    def test_count_and_cps(self, boot_ctx):
        proxy = boot_ctx.proxy
        assert proxy.count_active_macros() >= 0
        assert proxy.get_total_cps() >= 0
