import os

from PyQt6.QtCore import QObject, QTimer, pyqtSignal
from PyQt6.QtGui import QIcon
from PyQt6.QtWidgets import QMainWindow, QTabWidget


class ToggleBridge(QObject):
    macro_toggled = pyqtSignal(str, bool)

from config_ui import CONFIG
from rgb_engine import RGBEngine
from services.profile_manager import ProfileManager
from ui.styles import GLOBAL_STYLE, WINDOW_SIZE, WINDOW_TITLE
from ui.tabs.dashboard_tab import DashboardTab
from ui.tabs.keyboard_tab import KeyboardTab
from ui.tabs.mouse_tab import MouseTab
from ui.tabs.rgb_tab import RGBTab
from ui.tabs.right_click_tab import RightClickTab
from ui.tabs.settings_tab import SettingsTab

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ICONS_DIR = os.path.join(BASE_DIR, "assets", "icons")


def _icon(name: str) -> QIcon:
    path = os.path.join(ICONS_DIR, name)
    if os.path.exists(path):
        return QIcon(path)
    return QIcon()


class MainWindow(QMainWindow):
    def __init__(self, engine, image_path: str = "assets/mouse.svg"):
        super().__init__()
        self.engine = engine
        self.rgb = RGBEngine()
        self.profiles = ProfileManager()
        self.profiles.load("default")
        self.profiles.apply_to_engine(engine.manager)
        self.profiles.apply_to_rgb(self.rgb)

        abs_image = image_path if os.path.isabs(image_path) else os.path.join(BASE_DIR, image_path)

        self.setWindowTitle(WINDOW_TITLE)
        self.resize(*WINDOW_SIZE)
        self.setStyleSheet(GLOBAL_STYLE)

        self.tabs = QTabWidget()
        self.setCentralWidget(self.tabs)

        self.dashboard = DashboardTab(engine, abs_image)
        self.mouse_tab = MouseTab(engine)
        self.right_click_tab = RightClickTab(engine)
        self.keyboard_tab = KeyboardTab(engine)
        self.rgb_tab = RGBTab(self.rgb)
        self.settings_tab = SettingsTab(
            engine,
            self.profiles,
            on_profile_loaded=self._on_profile_loaded,
            on_profile_saved=lambda: self.profiles.capture_from_rgb(self.rgb),
        )

        tab_defs = [
            (self.dashboard, "Dashboard", "home.svg"),
            (self.mouse_tab, "Souris", "mouse.svg"),
            (self.right_click_tab, "Clic Droit", "right-click.svg"),
            (self.keyboard_tab, "Clavier", "keyboard.svg"),
            (self.rgb_tab, "RGB", "palette.svg"),
            (self.settings_tab, "Paramètres", "settings.svg"),
        ]
        for widget, label, icon_file in tab_defs:
            idx = self.tabs.addTab(widget, label)
            self.tabs.setTabIcon(idx, _icon(icon_file))

        self._toggle_bridge = ToggleBridge()
        self._toggle_bridge.macro_toggled.connect(self._on_macro_toggle)
        engine.set_on_toggle(
            lambda key, active: self._toggle_bridge.macro_toggled.emit(key, active)
        )

        self.ui_timer = QTimer()
        self.ui_timer.timeout.connect(self.refresh_all)
        self.ui_timer.start(200)

        self.rgb_timer = QTimer()
        self.rgb_timer.timeout.connect(self._update_rgb)
        self.rgb_timer.start(50)

    def _on_macro_toggle(self, key: str, active: bool):
        zone_map = {
            "left": "left",
            "right": "right",
            "1": "side1",
            "2": "side2",
            "3": "dpi",
            "4": "wheel",
        }
        zone = zone_map.get(key)
        if zone and active:
            self.rgb.trigger_reactive(zone)

    def _update_rgb(self):
        self.rgb.update()
        self.rgb_tab.refresh()

    def _on_profile_loaded(self):
        self.profiles.apply_to_rgb(self.rgb)
        self.refresh_all()

    def refresh_all(self):
        self.dashboard.refresh()
        self.mouse_tab.refresh()
        self.right_click_tab.refresh()
        self.keyboard_tab.refresh()

    def closeEvent(self, event):
        self.engine.running = False
        event.accept()
