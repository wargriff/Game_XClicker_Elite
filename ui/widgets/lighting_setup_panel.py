from PyQt6.QtCore import pyqtSignal
from PyQt6.QtWidgets import (
    QComboBox,
    QHBoxLayout,
    QLabel,
    QPushButton,
    QVBoxLayout,
    QWidget,
)

from ui.styles.icue_theme import ICUE, ICUE_LIGHTING_PANEL


DEFAULTS = {
    "ch1_type": "RGB Light Strip",
    "ch1_qty": "4 Strips are connected",
    "ch2_type": "LL Fan Hub",
    "ch2_qty": "4 Fans are connected",
}

CH1_TYPES = [
    "RGB Light Strip",
    "Commander Core",
    "Lighting Node PRO",
    "Smart Lighting Controller",
]
CH1_QTY = [
    "1 Strip is connected",
    "2 Strips are connected",
    "3 Strips are connected",
    "4 Strips are connected",
]
CH2_TYPES = [
    "LL Fan Hub",
    "QL Fan Hub",
    "RGB Light Strip",
    "Commander Core",
]
CH2_QTY = [
    "1 Fan is connected",
    "2 Fans are connected",
    "3 Fans are connected",
    "4 Fans are connected",
]


class LightingSetupPanel(QWidget):
    """Panneau LIGHTING SETUP style Corsair iCUE (bas de page DEVICES)."""

    config_changed = pyqtSignal(int, str, str)  # channel, type, qty
    reverted = pyqtSignal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("LightingSetupPanel")
        self._focused_channel = 0
        self._build()
        self.setStyleSheet(ICUE_LIGHTING_PANEL)
        self.setFixedHeight(160)

    def _build(self):
        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        # Barre titre
        header = QWidget()
        header.setStyleSheet(f"background:{ICUE['header_bar']};")
        header.setFixedHeight(36)
        h = QHBoxLayout(header)
        h.setContentsMargins(16, 0, 16, 0)

        title = QLabel("✎  LIGHTING SETUP")
        title.setObjectName("panelTitle")
        h.addWidget(title)
        h.addStretch()

        self.revert_btn = QPushButton("Revert")
        self.revert_btn.setObjectName("revertBtn")
        self.revert_btn.clicked.connect(self._on_revert)
        h.addWidget(self.revert_btn)
        root.addWidget(header)

        body = QVBoxLayout()
        body.setContentsMargins(16, 12, 16, 12)
        body.setSpacing(10)

        self.ch1_type, self.ch1_qty, row1 = self._channel_row(
            "Lighting Channel 1", CH1_TYPES, CH1_QTY, 1
        )
        self.ch2_type, self.ch2_qty, row2 = self._channel_row(
            "Lighting Channel 2", CH2_TYPES, CH2_QTY, 2
        )
        body.addLayout(row1)
        body.addLayout(row2)
        root.addLayout(body)

        self._apply_defaults()

    def _channel_row(self, label: str, types: list, qtys: list, channel: int):
        row = QHBoxLayout()
        row.setSpacing(12)

        lbl = QLabel(label)
        lbl.setObjectName("channelLabel")
        row.addWidget(lbl)

        type_combo = QComboBox()
        type_combo.setObjectName("icueCombo")
        type_combo.addItems(types)

        qty_combo = QComboBox()
        qty_combo.setObjectName("icueCombo")
        qty_combo.addItems(qtys)

        type_combo.currentTextChanged.connect(
            lambda t, c=channel: self._emit_change(c)
        )
        qty_combo.currentTextChanged.connect(
            lambda q, c=channel: self._emit_change(c)
        )

        row.addWidget(type_combo, 1)
        row.addWidget(qty_combo, 1)
        row.addStretch()
        return type_combo, qty_combo, row

    def _emit_change(self, channel: int):
        if channel == 1:
            self.config_changed.emit(
                1, self.ch1_type.currentText(), self.ch1_qty.currentText()
            )
        else:
            self.config_changed.emit(
                2, self.ch2_type.currentText(), self.ch2_qty.currentText()
            )

    def _apply_defaults(self):
        self.ch1_type.setCurrentText(DEFAULTS["ch1_type"])
        self.ch1_qty.setCurrentText(DEFAULTS["ch1_qty"])
        self.ch2_type.setCurrentText(DEFAULTS["ch2_type"])
        self.ch2_qty.setCurrentText(DEFAULTS["ch2_qty"])

    def _on_revert(self):
        self._apply_defaults()
        self.reverted.emit()

    def focus_channel(self, channel: int):
        """Met en surbrillance jaune le combo du canal (1 ou 2)."""
        self._focused_channel = channel
        combo = self.ch1_qty if channel == 1 else self.ch2_qty
        normal = f"""
            QComboBox#icueCombo {{
                background: {ICUE['bg_input']};
                border: 1px solid {ICUE['border']};
                border-radius: 3px;
                padding: 6px 10px;
                color: {ICUE['text']};
            }}
        """
        focus = f"""
            QComboBox#icueCombo {{
                background: {ICUE['bg_input']};
                border: 2px solid {ICUE['border_focus']};
                border-radius: 3px;
                padding: 6px 10px;
                color: {ICUE['text']};
            }}
        """
        self.ch1_type.setStyleSheet(normal)
        self.ch1_qty.setStyleSheet(normal)
        self.ch2_type.setStyleSheet(normal)
        self.ch2_qty.setStyleSheet(normal)
        if channel == 1:
            self.ch1_qty.setStyleSheet(focus)
        elif channel == 2:
            self.ch2_qty.setStyleSheet(focus)
