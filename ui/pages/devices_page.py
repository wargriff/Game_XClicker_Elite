from PyQt6.QtWidgets import QHBoxLayout, QLabel, QPushButton, QVBoxLayout, QWidget

from core.rgb_engine import RGBEngine
from ui.styles.icue_theme import ICUE
from ui.widgets.device_center_view import DeviceCenterView
from ui.widgets.device_strip import DeviceStrip
from ui.widgets.lighting_setup_panel import LightingSetupPanel


class DevicesPage(QWidget):
    """Page INSTANT LIGHTING / DEVICES style Corsair iCUE."""

    def __init__(self, engine, rgb: RGBEngine, parent=None):
        super().__init__(parent)
        self.engine = engine
        self.rgb = rgb
        self._build()

    def _build(self):
        self.setStyleSheet(f"background:{ICUE['bg_main']};")

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)

        top = QWidget()
        top.setStyleSheet(f"background:{ICUE['bg_panel']};")
        top.setFixedHeight(36)
        top_row = QHBoxLayout(top)
        top_row.setContentsMargins(16, 0, 16, 0)
        title = QLabel("DEVICES")
        title.setStyleSheet(
            f"color:{ICUE['text']}; font-weight:600; letter-spacing:1px; font-size:12px;"
        )
        top_row.addWidget(title)
        top_row.addStretch()
        self.rescan_btn = QPushButton("Rescanner périphériques")
        self.rescan_btn.setObjectName("revertBtn")
        self.rescan_btn.setStyleSheet(
            f"background:transparent;border:1px solid {ICUE['border']};"
            f"padding:4px 12px;color:{ICUE['text_dim']};font-size:11px;"
        )
        top_row.addWidget(self.rescan_btn)
        layout.addWidget(top)

        self.device_strip = DeviceStrip()
        layout.addWidget(self.device_strip)

        self.device_view = DeviceCenterView()
        layout.addWidget(self.device_view, 1)

        self.lighting_panel = LightingSetupPanel()
        layout.addWidget(self.lighting_panel)

        self.device_strip.device_changed.connect(self.device_view.set_device)
        self.lighting_panel.config_changed.connect(self._on_lighting_config)
        self.lighting_panel.reverted.connect(self._on_revert)

    def _on_lighting_config(self, channel: int, device_type: str, qty: str):
        zone_map = {1: ("side1", "left", "wheel"), 2: ("side2", "right", "dpi")}
        zones = zone_map.get(channel, ())
        for zone in zones:
            if zone not in self.rgb.zones:
                continue
            if "Fan" in device_type or "Hub" in device_type:
                self.rgb.set_mode(zone, "rainbow")
            elif "Strip" in device_type:
                self.rgb.set_mode(zone, "breathing")
            else:
                self.rgb.set_mode(zone, "static")

    def _on_revert(self):
        for zone in self.rgb.zones:
            self.rgb.set_mode(zone, "static")
        self.lighting_panel.focus_channel(0)

    def focus_channel(self, channel: int):
        self.lighting_panel.focus_channel(channel)

    def refresh_rgb(self):
        """Appelé par rgb_timer — pas de repaint inutile."""
        pass

    def refresh_status(self, active: int, total_cps: int):
        """Mise à jour légère depuis ui_timer."""
        pass

    def refresh(self):
        pass
