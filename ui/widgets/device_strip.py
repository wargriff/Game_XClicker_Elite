from PyQt6.QtCore import Qt, pyqtSignal
from PyQt6.QtWidgets import QButtonGroup, QHBoxLayout, QPushButton, QWidget

from ui.styles.icue_theme import ICUE_DEVICE_STRIP


class DeviceStrip(QWidget):
    """Barre horizontale d'icônes périphériques style iCUE."""

    device_changed = pyqtSignal(str)

    DEVICES = [
        ("keyboard", "⌨", "Clavier"),
        ("mouse", "🖱", "Souris"),
        ("headset", "🎧", "Casque"),
        ("commander", "⚙", "Commander Core"),
        ("lighting", "💡", "Éclairage"),
    ]

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("DeviceStrip")
        self._group = QButtonGroup(self)
        self._group.setExclusive(True)
        self._build()
        self.setStyleSheet(ICUE_DEVICE_STRIP)

    def _build(self):
        layout = QHBoxLayout(self)
        layout.setContentsMargins(12, 0, 12, 0)
        layout.setSpacing(0)

        for key, icon, tip in self.DEVICES:
            btn = QPushButton(icon)
            btn.setObjectName("deviceIcon")
            btn.setCheckable(True)
            btn.setToolTip(tip)
            btn.setFixedSize(56, 48)
            self._group.addButton(btn)
            btn.clicked.connect(lambda checked, k=key: self._on_click(k))
            layout.addWidget(btn)

        layout.addStretch()
        self._group.buttons()[3].setChecked(True)  # Commander Core par défaut

    def _on_click(self, key: str):
        self.device_changed.emit(key)

    def select(self, key: str):
        for i, (k, _, _) in enumerate(self.DEVICES):
            if k == key:
                self._group.buttons()[i].setChecked(True)
                break
