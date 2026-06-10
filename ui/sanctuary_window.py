import os
import sys
import webbrowser

from config.runtime import project_root

_root = project_root()
if _root not in sys.path:
    sys.path.insert(0, _root)

import utils.legacy_patch  # noqa: F401

from PyQt6.QtCore import QTimer
from PyQt6.QtGui import QIcon
from PyQt6.QtWidgets import (
    QComboBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QMainWindow,
    QStackedWidget,
    QVBoxLayout,
)

from core.rgb_engine import RGBEngine
from services.bootstrap import BootContext
from services.node_bridge import NodeBridge
from services.profile_manager import ProfileManager
from services.sidecar_api import SidecarAPI
from ui.pages.dashboard_page import DashboardPage
from ui.pages.devices_page import DevicesPage
from ui.pages.home_page import HomePage
from ui.pages.macros_page import MACRO_KEYS, MacrosPage
from ui.pages.settings_page import SettingsPage
from ui.styles.diablo_theme import FOOTER_STYLE, GLOBAL_STYLE, WINDOW_SIZE, WINDOW_TITLE
from ui.widgets.background_widget import BackgroundWidget
from ui.widgets.header_bar import HeaderBar
from ui.widgets.sensor_panel import SensorPanel
from ui.widgets.sidebar import Sidebar
from utils.debug import log, log_exc, log_verbose

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BG_PATH = os.path.join(BASE_DIR, "assets", "bg", "diablo_bg.svg")
ICON_PATH = os.path.join(BASE_DIR, "assets", "favicon", "favicon.svg")


class SanctuaryWindow(QMainWindow):
    """Main Sanctuary UI — also exposed as MainWindow for compatibility."""

    PAGE_MAP = {
        "home": 0,
        "dashboard": 1,
        "devices": 2,
        "macros": 3,
        "settings": 4,
    }

    def __init__(
        self,
        engine,
        boot: BootContext = None,
        image_path: str = "assets/mouse.svg",
    ):
        super().__init__()
        self.engine = engine

        # Legacy hooks — créés IMMÉDIATEMENT (code externe y accède avant _build_ui)
        self._legacy_name_edit = QLineEdit("default")
        self._legacy_name_edit.setReadOnly(True)
        self._legacy_master_combo = QComboBox()
        for _, label in MACRO_KEYS:
            self._legacy_master_combo.addItem(label)

        # Attributs RÉELS dès t=0 (pas @property — ancien code lit __dict__)
        self.master_combo = self._legacy_master_combo
        self.name_edit = self._legacy_name_edit

        self.rgb = RGBEngine()
        self.profiles = boot.profiles if boot else ProfileManager()
        if not boot:
            self.profiles.load("default")
            self.profiles.apply_to_engine(engine.manager)
        self.profiles.apply_to_rgb(self.rgb)

        self.sidecar = boot.sidecar if boot else SidecarAPI(engine)
        if not boot:
            self.sidecar.start()

        self.node = boot.node if boot else None
        if not boot:
            self.node = NodeBridge()
            self.node.start()

        self._navigating = False
        self._last_stats = {}
        self._build_ui()
        self._wire_events()
        self._sync_profile_name()
        log(
            "WINDOW",
            f"init OK sidecar={self.sidecar.online} node="
            f"{self.node.online if self.node else False}",
        )

    def _build_ui(self):
        self.setWindowTitle(WINDOW_TITLE)
        self.resize(*WINDOW_SIZE)
        self.setStyleSheet(GLOBAL_STYLE)
        if os.path.exists(ICON_PATH):
            self.setWindowIcon(QIcon(ICON_PATH))

        bg = BackgroundWidget(BG_PATH)
        self.setCentralWidget(bg)

        root = QVBoxLayout(bg)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        self.header = HeaderBar()
        root.addWidget(self.header)

        body = QHBoxLayout()
        body.setSpacing(0)

        self.sidebar = Sidebar(self.profiles.list_profiles())
        body.addWidget(self.sidebar)

        self.stack = QStackedWidget()
        self.sensor_panel = SensorPanel()
        self.home = HomePage(self.engine, self.rgb, self.sensor_panel, node=self.node)
        self.dashboard = DashboardPage(self.engine)
        self.devices = DevicesPage(self.engine, self.rgb)
        self.macros = MacrosPage(self.engine)
        self.settings = SettingsPage(
            self.engine,
            self.profiles,
            on_profile_loaded=self._on_profile_loaded,
            on_profile_saved=lambda: self.profiles.capture_from_rgb(self.rgb),
        )

        for page in (self.home, self.dashboard, self.devices, self.macros, self.settings):
            self.stack.addWidget(page)

        body.addWidget(self.stack, 1)
        root.addLayout(body, 1)

        self.footer = QLabel(
            "Latéral 2 = pause globale · En pause rien ne démarre · Journal en temps réel"
        )
        self.footer.setFixedHeight(28)
        self.footer.setStyleSheet(FOOTER_STYLE + " padding-left:12px;")
        root.addWidget(self.footer)

        self._connect_mission_control()
        self._bind_macro_attrs()

    def _bind_macro_attrs(self):
        """Pointe master_combo / name_edit vers les widgets réels de MacrosPage."""
        if getattr(self, "macros", None) is None:
            log("WINDOW", "_bind_macro_attrs — macros page absente, legacy conservé")
            return
        self.master_combo = self.macros.master_combo
        self.name_edit = self.macros.name_edit
        log_verbose("WINDOW", "_bind_macro_attrs OK")

    def _connect_mission_control(self):
        pass

    def _mission_url(self) -> str:
        if self.node and self.node.online:
            return self.node.URL
        return self.sidecar.MISSION_URL

    def _open_mission_control(self):
        url = self._mission_url()
        log("WINDOW", f"Mission Control → {url}")
        webbrowser.open(url)

    def _wire_events(self):
        self.header.tab_changed.connect(self._on_tab)
        self.header.seal_clicked.connect(self.close)
        self.header.stasis_clicked.connect(self.engine.toggle)
        self.sidebar.section_changed.connect(self._on_section)
        self.sidebar.profile_combo.currentTextChanged.connect(self._on_profile_change)
        self.sensor_panel.rescan_btn.clicked.connect(self._rescan_devices)
        self.devices.rescan_btn.clicked.connect(self._rescan_devices)
        self.home.device_clicked.connect(self._on_home_device)

        self.engine.set_on_toggle(self._on_macro_toggle)

        # UI stats — 500 ms (léger)
        self.ui_timer = QTimer()
        self.ui_timer.timeout.connect(self.refresh_all)
        self.ui_timer.start(500)

        # RGB moteur — 100 ms, actif seulement sur HOME / INSTANT LIGHTING
        self.rgb_timer = QTimer()
        self.rgb_timer.timeout.connect(self._update_rgb)
        self._sync_rgb_timer()
        self.stack.currentChanged.connect(lambda _: self._sync_rgb_timer())

    def _sync_profile_name(self):
        self.macros.set_profile_name(self.profiles.current_name)

    def _on_tab(self, tab: str):
        log_verbose("WINDOW", f"_on_tab {tab}")
        if self._navigating:
            return
        idx = self.PAGE_MAP.get(tab, 0)
        self._navigating = True
        try:
            self.stack.blockSignals(True)
            self.stack.setCurrentIndex(idx)
            self.stack.blockSignals(False)
            if tab == "macros":
                self.sidebar._select("macro1", emit=False)
                self._bind_macro_attrs()
            elif tab == "devices":
                self.sidebar._select("lighting", emit=False)
            elif tab == "dashboard":
                self.sidebar._select("performance", emit=False)
            log_verbose("WINDOW", f"_on_tab OK idx={idx}")
        except Exception as exc:
            log_exc("WINDOW", exc)
        finally:
            self._navigating = False

    def _on_section(self, section: str):
        log_verbose("WINDOW", f"_on_section {section}")
        if self._navigating:
            return
        self._navigating = True
        try:
            if section in ("performance", "graphing"):
                self.stack.setCurrentIndex(self.PAGE_MAP["dashboard"])
                self.header._select_tab("dashboard", emit=False)
            elif section in ("lighting", "channel1", "channel2"):
                self.stack.setCurrentIndex(self.PAGE_MAP["devices"])
                self.header._select_tab("devices", emit=False)
                if section == "channel1":
                    QTimer.singleShot(0, lambda: self.devices.focus_channel(1))
                elif section == "channel2":
                    QTimer.singleShot(0, lambda: self.devices.focus_channel(2))
            elif section in ("macro1", "macro2", "macro3", "macro4", "macro5", "macro6"):
                self.stack.setCurrentIndex(self.PAGE_MAP["macros"])
                self.header._select_tab("macros", emit=False)
                QTimer.singleShot(0, lambda s=section: self._focus_macro_section(s))
            log_verbose("WINDOW", f"_on_section OK {section}")
        except Exception as exc:
            log_exc("WINDOW", exc)
        finally:
            self._navigating = False

    def _focus_macro_section(self, section: str):
        try:
            self._bind_macro_attrs()
            self.macros.focus_section(section)
        except Exception as exc:
            log_exc("WINDOW", exc)

    def _on_profile_change(self, name: str):
        self.profiles.load(name)
        self.profiles.apply_to_engine(self.engine.manager)
        self.profiles.apply_to_rgb(self.rgb)
        self.macros.set_profile_name(name)
        self._on_profile_loaded()

    def _on_profile_loaded(self):
        self.profiles.apply_to_rgb(self.rgb)
        self._sync_profile_name()
        self.refresh_all()

    def _on_macro_toggle(self, key: str, active: bool):
        zone_map = {
            "left": "left", "right": "right",
            "1": "side1", "2": "side2", "3": "dpi", "4": "wheel",
        }
        zone = zone_map.get(key)
        if zone and active:
            self.rgb.trigger_reactive(zone)

    def _sync_rgb_timer(self):
        current = self.stack.currentWidget()
        if current in (self.devices, self.home):
            if not self.rgb_timer.isActive():
                self.rgb_timer.start(100)
        elif self.rgb_timer.isActive():
            self.rgb_timer.stop()

    def _update_rgb(self):
        current = self.stack.currentWidget()
        if current not in (self.devices, self.home):
            return
        self.rgb.update()
        if current == self.devices:
            self.devices.refresh_rgb()

    def _rescan_devices(self):
        self.sidebar.set_profiles(self.profiles.list_profiles())
        if hasattr(self.home, "rescan_devices"):
            self.home.rescan_devices()

    def _on_home_device(self, device_id: str):
        if device_id == "commander":
            self.stack.setCurrentIndex(self.PAGE_MAP["devices"])
            self.header._select_tab("devices", emit=False)
            self.sidebar._select("lighting", emit=False)
        elif device_id == "mouse":
            self.stack.setCurrentIndex(self.PAGE_MAP["macros"])
            self.header._select_tab("macros", emit=False)
            self.sidebar._select("macro1", emit=False)
            self._focus_macro_section("macro1")
        elif device_id == "keyboard":
            self.stack.setCurrentIndex(self.PAGE_MAP["macros"])
            self.header._select_tab("macros", emit=False)
            self.sidebar._select("macro3", emit=False)

    def _get_system_stats(self):
        try:
            import psutil
            cpu = psutil.cpu_percent(interval=None)
            mem = psutil.virtual_memory()
            return cpu, mem.used // (1024 * 1024), mem.total // (1024 * 1024)
        except ImportError:
            return 0.0, 0, 0

    def refresh_all(self):
        if self._navigating:
            return
        try:
            cpu, ram_used, ram_total = self._get_system_stats()
            total_cps = self.engine.get_total_cps()
            active = self.engine.count_active_macros()
            node_online = self.node.online if self.node else False

            self.header.update_engine(self.engine.enabled)
            self.header.update_cps(total_cps)

            current = self.stack.currentWidget()

            # Capteurs seulement sur HOME (évite latence autres pages)
            if current == self.home:
                self.sensor_panel.update_stats(
                    cpu,
                    ram_used,
                    ram_total,
                    active,
                    total_cps,
                    self.sidecar.online,
                    node_online=node_online,
                )
                self.home.refresh(
                    api_online=self.sidecar.online,
                    node_online=node_online,
                )
            elif current == self.dashboard:
                self.dashboard.refresh()
            elif current == self.devices:
                self.devices.refresh_status(active, total_cps)
            elif current == self.macros:
                self.macros.refresh()
        except Exception as exc:
            log_exc("WINDOW", exc)

    def closeEvent(self, event):
        log("WINDOW", "closeEvent — arrêt moteur + services")
        self.engine.running = False
        self.sidecar.stop()
        if self.node:
            self.node.stop()
        event.accept()


# Compatibility alias — legacy code imports MainWindow
MainWindow = SanctuaryWindow
