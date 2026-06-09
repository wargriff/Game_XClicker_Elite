from PyQt6.QtCore import pyqtSignal
from PyQt6.QtGui import QFont
from PyQt6.QtWidgets import QButtonGroup, QComboBox, QLabel, QPushButton, QVBoxLayout, QWidget

from ui.styles.diablo_theme import COLORS, SIDEBAR_STYLE
from utils.debug import log


class Sidebar(QWidget):
    section_changed = pyqtSignal(str)

    SECTIONS = [
        ("PERFORMANCE", "performance"),
        ("GRAPHING", "graphing"),
        ("LIGHTING SETUP", "lighting"),
        ("LIGHTING CHANNEL 1", "channel1"),
        ("LIGHTING CHANNEL 2", "channel2"),
        ("MACRO 1", "macro1"),
        ("MACRO 2", "macro2"),
    ]

    def __init__(self, profiles: list, parent=None):
        super().__init__(parent)
        self.setObjectName("Sidebar")
        self.setFixedWidth(200)
        self._buttons = {}
        self._section_ids = {}
        self._updating = False
        self._build(profiles)
        self.setStyleSheet(SIDEBAR_STYLE)

    def _build(self, profiles: list):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(12, 16, 12, 16)
        layout.setSpacing(4)

        logo = QLabel("★")
        logo.setFont(QFont("Segoe UI", 22))
        logo.setStyleSheet(f"color:{COLORS['gold_bright']};")
        brand = QLabel("XCLICKER ELITE\nSANCTUARY EDITION")
        brand.setFont(QFont("Segoe UI", 8, QFont.Weight.Bold))
        brand.setStyleSheet(
            f"color:{COLORS['gold']}; letter-spacing:1px; line-height:140%;"
        )
        layout.addWidget(logo)
        layout.addWidget(brand)
        layout.addSpacing(12)

        prof_lbl = QLabel("PROFILES")
        prof_lbl.setStyleSheet(
            "color:#9a9a9a; font-size:10px; letter-spacing:1px;"
        )
        layout.addWidget(prof_lbl)

        self.profile_combo = QComboBox()
        self.profile_combo.addItems(profiles)
        layout.addWidget(self.profile_combo)
        layout.addSpacing(16)

        self._group = QButtonGroup(self)
        self._group.setExclusive(True)

        for idx, (label, key) in enumerate(self.SECTIONS):
            btn = QPushButton(label)
            btn.setCheckable(True)
            btn.setProperty("section", key)
            btn.setStyleSheet(self._nav_style(False))
            self._group.addButton(btn, idx)
            self._buttons[key] = btn
            self._section_ids[idx] = key
            layout.addWidget(btn)

        self._group.idClicked.connect(self._on_section_id_clicked)

        layout.addStretch()
        self._select("lighting", emit=False)

    def _nav_style(self, active: bool) -> str:
        if active:
            return (
                "QPushButton { text-align:left; padding:8px 10px; border:none;"
                "border-left:3px solid #f5c518;"
                "background:rgba(245,197,24,0.1); color:#f5c518;"
                "font-weight:600; border-radius:0; font-size:11px; }"
            )
        return (
            "QPushButton { text-align:left; padding:8px 10px; border:none;"
            "border-left:3px solid transparent; background:transparent;"
            "color:#9a9a9a; border-radius:0; font-size:11px; }"
            "QPushButton:hover { color:#e0e0e0; background:rgba(255,255,255,0.05); }"
        )

    def _on_section_id_clicked(self, btn_id: int):
        key = self._section_ids.get(btn_id)
        log("SIDEBAR", f"idClicked id={btn_id} key={key}")
        if self._updating or not key:
            log("SIDEBAR", "idClicked ignoré (_updating ou key absente)")
            return
        self._apply_section_style(key)
        self.section_changed.emit(key)

    def _apply_section_style(self, key: str):
        for k, btn in self._buttons.items():
            btn.setStyleSheet(self._nav_style(k == key))

    def _select(self, key: str, emit: bool = True):
        if key not in self._buttons:
            log("SIDEBAR", f"_select key={key} INCONNUE")
            return

        log("SIDEBAR", f"_select key={key} emit={emit}")
        self._updating = True
        try:
            self._group.blockSignals(True)
            self._buttons[key].setChecked(True)
            self._apply_section_style(key)
            self._group.blockSignals(False)
        finally:
            self._updating = False

        if emit:
            self.section_changed.emit(key)

    def set_profiles(self, profiles: list):
        current = self.profile_combo.currentText()
        self.profile_combo.blockSignals(True)
        self.profile_combo.clear()
        self.profile_combo.addItems(profiles)
        if current in profiles:
            self.profile_combo.setCurrentText(current)
        self.profile_combo.blockSignals(False)
