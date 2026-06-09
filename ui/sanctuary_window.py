import os
import webbrowser

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

from rgb_engine import RGBEngine
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
from utils.debug import log, log_exc

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
        log("WINDOW", "master_combo/name_edit legacy hooks OK")

    @property
    def master_combo(self):
        """Compatibilité code legacy / hooks macro."""
        macros = getattr(self, "macros", None)
        if macros is not None and hasattr(macros, "master_combo"):
            return macros.master_combo
        return self._legacy_master_combo

    @property
    def name_edit(self):
        """Compatibilité code legacy / hooks macro."""
        macros = getattr(self, "macros", None)
        if macros is not None and hasattr(macros, "name_edit"):
            return macros.name_edit
        return self._legacy_name_edit

    def _connect_mission_control(self):
        mission = self.home._tiles.get("mission")
        sidecar = self.home._tiles.get("sidecar")
        node = self.home._tiles.get("node")
        if mission:
            mission.clicked.connect(self._open_mission_control)
        if sidecar:
            sidecar.clicked.connect(self._open_mission_control)
        if node:
            node.clicked.connect(self._open_mission_control)

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

        self.engine.set_on_toggle(self._on_macro_toggle)

        self.ui_timer = QTimer()
        self.ui_timer.timeout.connect(self.refresh_all)
        self.ui_timer.start(250)

        self.rgb_timer = QTimer()
        self.rgb_timer.timeout.connect(self._update_rgb)
        self.rgb_timer.start(50)

    def _sync_profile_name(self):
        self.macros.set_profile_name(self.profiles.current_name)

    def _on_tab(self, tab: str):
        log("WINDOW", f"_on_tab tab={tab} navigating={self._navigating}")
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
            elif tab == "devices":
                self.sidebar._select("lighting", emit=False)
            elif tab == "dashboard":
                self.sidebar._select("performance", emit=False)
            log("WINDOW", f"_on_tab OK index={idx}")
        except Exception as exc:
            log_exc("WINDOW", exc)
        finally:
            self._navigating = False

    def _on_section(self, section: str):
        log("WINDOW", f"_on_section section={section} navigating={self._navigating}")
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
            elif section in ("macro1", "macro2"):
                self.stack.setCurrentIndex(self.PAGE_MAP["macros"])
                self.header._select_tab("macros", emit=False)
                QTimer.singleShot(0, lambda s=section: self._focus_macro_section(s))
            log("WINDOW", f"_on_section OK section={section}")
        except Exception as exc:
            log_exc("WINDOW", exc)
        finally:
            self._navigating = False

    def _focus_macro_section(self, section: str):
        log("WINDOW", f"_focus_macro_section section={section}")
        try:
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

    def _update_rgb(self):
        self.rgb.update()
        if self.stack.currentWidget() == self.devices:
            self.devices.refresh()

    def _rescan_devices(self):
        self.sidebar.set_profiles(self.profiles.list_profiles())

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
            self.sensor_panel.update_stats(
                cpu,
                ram_used,
                ram_total,
                active,
                total_cps,
                self.sidecar.online,
                node_online=node_online,
            )

            current = self.stack.currentWidget()
            self.home.refresh(
                api_online=self.sidecar.online,
                node_online=node_online,
            )
            if current == self.dashboard:
                self.dashboard.refresh()
            elif current == self.devices:
                self.devices.refresh()
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
