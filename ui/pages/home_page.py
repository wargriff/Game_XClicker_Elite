import os

from PyQt6.QtCore import Qt, pyqtSignal
from PyQt6.QtGui import QFont, QPixmap
from PyQt6.QtWidgets import (
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QScrollArea,
    QVBoxLayout,
    QWidget,
)

from config.paths import DEVICES_DIR
from services.device_scanner import scan_devices, scan_sensors
from ui.styles.icue_theme import ICUE
from ui.widgets.mural_panel import MuralPanel
from ui.widgets.sensor_panel import SensorPanel


class IcueDeviceCard(QWidget):
    clicked = pyqtSignal(str)

    def __init__(self, device: dict, parent=None):
        super().__init__(parent)
        self._device_id = device.get("id", "")
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        self.setMinimumHeight(170)
        self.setMaximumHeight(210)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(12, 10, 12, 10)
        layout.setSpacing(6)

        head = QHBoxLayout()
        title = QLabel(device.get("name", "DEVICE"))
        title.setFont(QFont("Segoe UI", 9, QFont.Weight.Bold))
        title.setStyleSheet(f"color:{ICUE['text']}; letter-spacing:0.8px;")
        gear = QLabel("⚙")
        gear.setStyleSheet(f"color:{ICUE['text_dim']};")
        head.addWidget(title)
        head.addStretch()
        head.addWidget(gear)
        layout.addLayout(head)

        img_wrap = QWidget()
        img_row = QHBoxLayout(img_wrap)
        img_row.setContentsMargins(0, 8, 0, 8)
        img_lbl = QLabel()
        img_lbl.setAlignment(Qt.AlignmentFlag.AlignCenter)
        img_path = device.get("image", "").replace("/devices/", "")
        full = os.path.join(DEVICES_DIR, img_path) if img_path else ""
        if full and os.path.isfile(full):
            pix = QPixmap(full)
            if not pix.isNull():
                img_lbl.setPixmap(
                    pix.scaled(
                        120,
                        72,
                        Qt.AspectRatioMode.KeepAspectRatio,
                        Qt.TransformationMode.SmoothTransformation,
                    )
                )
        if img_lbl.pixmap() is None:
            img_lbl.setText("⬡")
            img_lbl.setStyleSheet(f"font-size:36px; color:{ICUE['yellow']};")
        img_row.addStretch()
        img_row.addWidget(img_lbl)
        img_row.addStretch()
        layout.addWidget(img_wrap, 1)

        foot = QHBoxLayout()
        online = device.get("online", False)
        dot_color = "#e53935" if online else "#555"
        dot = QLabel("●")
        dot.setStyleSheet(f"color:{dot_color}; font-size:10px;")
        detail = QLabel(device.get("detail", ""))
        detail.setStyleSheet(f"color:{ICUE['text_dim']}; font-size:9px;")
        foot.addWidget(dot)
        foot.addWidget(detail, 1)
        layout.addLayout(foot)

        border = ICUE["border_focus"] if device.get("status") == "active" else ICUE["border"]
        self.setStyleSheet(
            f"background:{ICUE['bg_panel']}; border:1px solid {border}; border-radius:4px;"
        )

    def mousePressEvent(self, event):
        self.clicked.emit(self._device_id)
        super().mousePressEvent(event)


class HomePage(QWidget):
    """Page HOME iCUE — Murals, capteurs, grille devices auto-détectés."""

    device_clicked = pyqtSignal(str)

    def __init__(self, engine, rgb, sensor_panel: SensorPanel, node=None, parent=None):
        super().__init__(parent)
        self.engine = engine
        self.rgb = rgb
        self.sensor_panel = sensor_panel
        self.node = node
        self._build()

    def _build(self):
        self.setStyleSheet(f"background:{ICUE['bg_main']};")

        root = QVBoxLayout(self)
        root.setContentsMargins(16, 12, 16, 12)
        root.setSpacing(12)

        body = QHBoxLayout()
        body.setSpacing(16)

        left = QVBoxLayout()
        left.addWidget(MuralPanel(self.rgb))
        left.addWidget(self.sensor_panel)
        left.addStretch()
        body.addLayout(left, 0)

        right = QVBoxLayout()
        head = QHBoxLayout()
        title = QLabel("DEVICES")
        title.setStyleSheet(
            f"color:{ICUE['text']}; font-weight:600; letter-spacing:1px; font-size:11px;"
        )
        self.detect_lbl = QLabel("Détection…")
        self.detect_lbl.setStyleSheet(f"color:{ICUE['yellow']}; font-size:10px;")
        head.addWidget(title)
        head.addStretch()
        head.addWidget(self.detect_lbl)
        right.addLayout(head)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setStyleSheet("QScrollArea { border: none; background: transparent; }")
        self.grid_host = QWidget()
        self.grid = QGridLayout(self.grid_host)
        self.grid.setSpacing(10)
        scroll.setWidget(self.grid_host)
        right.addWidget(scroll, 1)
        body.addLayout(right, 1)

        root.addLayout(body, 1)
        self.rescan_devices()

    def rescan_devices(self):
        devices = scan_devices(self.engine)
        sensors = scan_sensors()

        while self.grid.count():
            item = self.grid.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        cols = 3
        for i, dev in enumerate(devices):
            card = IcueDeviceCard(dev)
            card.clicked.connect(self.device_clicked.emit)
            self.grid.addWidget(card, i // cols, i % cols)

        detected = sum(1 for d in devices if d.get("detected"))
        self.detect_lbl.setText(f"{detected} détecté{'s' if detected > 1 else ''}")

        if hasattr(self.sensor_panel, "update_from_scan"):
            self.sensor_panel.update_from_scan(sensors)

    def refresh(self, api_online: bool = False, node_online: bool = False):
        detected = sum(1 for i in range(self.grid.count()) if self.grid.itemAt(i))
        self.detect_lbl.setText(f"{detected} périphérique(s)")
