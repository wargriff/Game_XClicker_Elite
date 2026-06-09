from PyQt6.QtCore import Qt, pyqtSignal
from PyQt6.QtGui import QFont, QPainter, QColor, QPen
from PyQt6.QtWidgets import QLabel, QVBoxLayout, QWidget

from ui.styles.icue_theme import ICUE, ICUE_DEVICE_VIEW


class DeviceCenterView(QWidget):
    """Vue centrale — représentation du hub / contrôleur style iCUE."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("DeviceCenterView")
        self._device_key = "commander"
        self._device_name = "COMMANDER CORE XT"
        self._build()
        self.setStyleSheet(ICUE_DEVICE_VIEW)
        self.setMinimumHeight(280)

    def _build(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(24, 16, 24, 16)

        title = QLabel("DEVICES")
        title.setObjectName("deviceTitle")
        layout.addWidget(title)

        self.name_lbl = QLabel(self._device_name)
        self.name_lbl.setObjectName("deviceName")
        layout.addWidget(self.name_lbl)
        layout.addStretch()

    def set_device(self, key: str):
        self._device_key = key
        names = {
            "keyboard": "K70 RGB PRO — Clavier Sanctuaire",
            "mouse": "Spirit of Gamer — Souris Elite",
            "headset": "HS80 RGB — Casque",
            "commander": "COMMANDER CORE XT",
            "lighting": "Lighting Node PRO",
        }
        self._device_name = names.get(key, "Périphérique")
        self.name_lbl.setText(self._device_name)
        self.update()

    def paintEvent(self, event):
        super().paintEvent(event)
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)

        w, h = self.width(), self.height()
        cx, cy = w // 2, h // 2 + 20

        # Boîtier Commander Core stylisé
        painter.setPen(QPen(QColor(ICUE["border"]), 2))
        painter.setBrush(QColor(ICUE["bg_panel"]))
        painter.drawRoundedRect(cx - 120, cy - 50, 240, 100, 8, 8)

        painter.setPen(QPen(QColor(ICUE["yellow"]), 1))
        for i, label in enumerate(("CH1", "CH2", "USB", "SATA")):
            x = cx - 90 + i * 55
            painter.drawEllipse(x, cy - 15, 28, 28)
            painter.setPen(QColor(ICUE["text_dim"]))
            font = QFont("Segoe UI", 7)
            painter.setFont(font)
            painter.drawText(x, cy + 28, 28, 16, Qt.AlignmentFlag.AlignCenter, label)
            painter.setPen(QPen(QColor(ICUE["yellow"]), 1))

        painter.end()
