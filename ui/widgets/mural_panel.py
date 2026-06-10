from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import QGridLayout, QLabel, QVBoxLayout, QWidget

from core.rgb_engine import RGBEngine
from ui.styles.diablo_theme import COLORS


MURAL_NAMES = [
    ("Ember", "#8b1a1a"),
    ("Gold", "#c9a227"),
    ("Blood", "#6b0000"),
    ("Ash", "#3d3d3d"),
    ("Inferno", "#ff4500"),
    ("Sanctuary", "#ffd700"),
]


class MuralPanel(QWidget):
    def __init__(self, rgb: RGBEngine, parent=None):
        super().__init__(parent)
        self.rgb = rgb
        self._swatches = []
        self._build()

    def _build(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(12, 10, 12, 10)

        title = QLabel("MURALS")
        title.setStyleSheet(
            f"color:{COLORS['gold']}; font-weight:bold; letter-spacing:2px; font-size:11px;"
        )
        layout.addWidget(title)

        grid = QGridLayout()
        grid.setSpacing(6)
        zones = list(self.rgb.zones.keys())

        for i, (name, hex_color) in enumerate(MURAL_NAMES):
            sw = QLabel()
            sw.setFixedSize(52, 36)
            sw.setAlignment(Qt.AlignmentFlag.AlignCenter)
            sw.setText(name[:4])
            sw.setStyleSheet(
                f"background:{hex_color}; border:1px solid {COLORS['border_gold']};"
                f"border-radius:4px; color:#fff; font-size:9px; font-weight:bold;"
            )
            zone = zones[i % len(zones)]
            sw.mousePressEvent = lambda e, z=zone, h=hex_color: self._apply(z, h)
            self._swatches.append(sw)
            grid.addWidget(sw, i // 3, i % 3)

        layout.addLayout(grid)
        self.setStyleSheet(
            f"background:rgba(14,12,10,0.92); border:1px solid {COLORS['border']};"
            f"border-radius:6px;"
        )

    def _apply(self, zone: str, hex_color: str):
        from PyQt6.QtGui import QColor

        c = QColor(hex_color)
        self.rgb.set_color(zone, c)
        self.rgb.set_mode(zone, "static")
