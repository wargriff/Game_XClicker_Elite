from PyQt6.QtWidgets import (
    QComboBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QVBoxLayout,
    QWidget,
)

from config_ui import PRESETS, RIGHT_CLICK_PRESETS
from ui.styles.diablo_theme import COLORS
from ui.widgets.macro_panel import MacroPanel
from ui.widgets.status_card import StatusCard

MACRO_KEYS = [
    ("left", "Macro 1 — Clic gauche"),
    ("right", "Macro 2 — Clic droit"),
    ("1", "Clavier — Touche 1"),
    ("2", "Clavier — Touche 2"),
    ("3", "Clavier — Touche 3"),
    ("4", "Clavier — Touche 4"),
]


class MacrosPage(QWidget):
    """Page macros autonome — master_combo + name_edit requis par l'UI Sanctuary."""

    def __init__(self, engine, parent=None):
        super().__init__(parent)
        self.engine = engine
        self._current_key = "left"
        self._updating = False
        self._build()

    def _build(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(16, 16, 16, 16)
        layout.setSpacing(12)

        title = QLabel("MACROS")
        title.setStyleSheet(
            f"color:{COLORS['gold_bright']}; font-size:16px; font-weight:bold;"
            f"letter-spacing:2px;"
        )
        layout.addWidget(title)

        top = QHBoxLayout()
        top.setSpacing(12)

        name_lbl = QLabel("Nom:")
        name_lbl.setStyleSheet(f"color:{COLORS['parchment_dim']};")
        self.name_edit = QLineEdit("default")
        self.name_edit.setReadOnly(True)
        self.name_edit.setStyleSheet(
            f"background:rgba(18,16,14,0.95); border:1px solid {COLORS['border']};"
            f"padding:6px; color:{COLORS['parchment']};"
        )

        master_lbl = QLabel("Canal:")
        master_lbl.setStyleSheet(f"color:{COLORS['parchment_dim']};")
        self.master_combo = QComboBox()
        for _, label in MACRO_KEYS:
            self.master_combo.addItem(label)

        top.addWidget(name_lbl)
        top.addWidget(self.name_edit, 1)
        top.addWidget(master_lbl)
        top.addWidget(self.master_combo, 1)
        layout.addLayout(top)

        self.status = StatusCard("Macro")
        layout.addWidget(self.status)

        presets = {k: (c, int(d * 1000)) for k, (c, d) in PRESETS.items()}
        self.panel = MacroPanel(
            self.engine,
            "left",
            presets=presets,
            show_burst=True,
        )
        layout.addWidget(self.panel)
        layout.addStretch()

        self.master_combo.currentIndexChanged.connect(self._on_master_changed)

    def _key_for_index(self, index: int) -> str:
        index = max(0, min(index, len(MACRO_KEYS) - 1))
        return MACRO_KEYS[index][0]

    def _index_for_key(self, key: str) -> int:
        for i, (k, _) in enumerate(MACRO_KEYS):
            if k == key:
                return i
        return 0

    def _on_master_changed(self, index: int):
        if self._updating:
            return
        key = self._key_for_index(index)
        self._switch_key(key)

    def _switch_key(self, key: str):
        self._current_key = key
        self.panel.key = key
        self.panel.sync_from_engine()
        self.status.title.setText(f"Macro — {key}")
        self.refresh()

    def set_profile_name(self, name: str):
        self.name_edit.setText(name or "default")

    def focus_section(self, section: str):
        mapping = {
            "macro1": "left",
            "macro2": "right",
            "channel1": "left",
            "channel2": "right",
            "keyboard": "1",
        }
        key = mapping.get(section, "left")
        idx = self._index_for_key(key)
        self._updating = True
        try:
            self.master_combo.blockSignals(True)
            self.master_combo.setCurrentIndex(idx)
            self.master_combo.blockSignals(False)
            self._switch_key(key)
        finally:
            self._updating = False

    def refresh(self):
        key = self._current_key
        active = self.engine.is_active(key)
        real = self.engine.get_real_cps(key)
        target = self.engine.get_cps(key)
        self.status.set_active(active, f"CPS {real} / cible {target}")
        self.panel.update_live_cps()
