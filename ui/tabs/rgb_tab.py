from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import QComboBox, QGridLayout, QLabel, QVBoxLayout, QWidget

from core.rgb_engine import RGBEngine


ZONE_LABELS = {
    "left": "Gauche",
    "right": "Droite",
    "wheel": "Molette",
    "dpi": "DPI",
    "side1": "Side 1",
    "side2": "Side 2",
}

MODES = ["static", "breathing", "rainbow", "reactive"]


class RGBTab(QWidget):
    def __init__(self, rgb: RGBEngine, parent=None):
        super().__init__(parent)
        self.rgb = rgb
        self.zone_labels = {}
        self.mode_combos = {}
        self._build()

    def _build(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(24, 24, 24, 24)

        title = QLabel("Éclairage RGB")
        title.setStyleSheet("font-size:18px;font-weight:bold;color:#00ff88;")
        layout.addWidget(title)

        grid = QGridLayout()
        grid.setSpacing(12)

        for i, (zone, label) in enumerate(ZONE_LABELS.items()):
            row, col = divmod(i, 3)

            card = QWidget()
            card.setStyleSheet(
                "background:#151515;border:1px solid #1a3a2a;border-radius:8px;"
            )
            card_layout = QVBoxLayout(card)

            swatch = QLabel("■")
            swatch.setAlignment(Qt.AlignmentFlag.AlignCenter)
            swatch.setStyleSheet("font-size:36px;")
            self.zone_labels[zone] = swatch

            name = QLabel(label)
            name.setAlignment(Qt.AlignmentFlag.AlignCenter)
            name.setStyleSheet("color:#00ffaa;font-weight:bold;")

            combo = QComboBox()
            combo.addItems(MODES)
            combo.setCurrentText(self.rgb.zones[zone].mode)
            combo.currentTextChanged.connect(
                lambda m, z=zone: self.rgb.set_mode(z, m)
            )
            self.mode_combos[zone] = combo

            card_layout.addWidget(swatch)
            card_layout.addWidget(name)
            card_layout.addWidget(combo)
            grid.addWidget(card, row, col)

        layout.addLayout(grid)
        layout.addStretch()

    def refresh(self):
        for zone, lbl in self.zone_labels.items():
            c = self.rgb.get_color(zone)
            lbl.setStyleSheet(f"font-size:36px;color:rgb({c.red()},{c.green()},{c.blue()});")
