from typing import Dict, Optional, Tuple

from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import (
    QButtonGroup,
    QHBoxLayout,
    QLabel,
    QPushButton,
    QSlider,
    QVBoxLayout,
    QWidget,
)


class MacroPanel(QWidget):
    def __init__(
        self,
        engine,
        key: str,
        presets: Optional[Dict[str, Tuple[int, int]]] = None,
        show_burst: bool = False,
        parent=None,
    ):
        super().__init__(parent)
        self.engine = engine
        self.key = key
        self.presets = presets or {}
        self._build(show_burst)

    def _build(self, show_burst: bool):
        layout = QVBoxLayout(self)
        layout.setSpacing(10)

        self.cps_label = QLabel()
        self.cps_slider = QSlider(Qt.Orientation.Horizontal)
        self.cps_slider.setRange(1, 200)

        self.delay_label = QLabel()
        self.delay_slider = QSlider(Qt.Orientation.Horizontal)
        self.delay_slider.setRange(1, 1000)

        self.micro_label = QLabel("fine 0 µs")
        self.micro_slider = QSlider(Qt.Orientation.Horizontal)
        self.micro_slider.setRange(0, 1000)

        self.slow_btn = QPushButton("ULTRA SLOW")
        self.slow_btn.setCheckable(True)

        self.live = QLabel("REAL CPS: 0")
        self.live.setStyleSheet("color:#00ff88;font-weight:bold;")

        self.burst_buttons = []
        self.burst_group = None
        if show_burst:
            burst_row = QHBoxLayout()
            burst_lbl = QLabel("Burst à l'activation:")
            burst_lbl.setStyleSheet("color:#00ffaa;font-weight:bold;")
            layout.addWidget(burst_lbl)
            self.burst_group = QButtonGroup(self)
            self.burst_group.setExclusive(True)
            for val in (0, 5, 10, 20):
                btn = QPushButton(str(val))
                btn.setCheckable(True)
                self.burst_group.addButton(btn, val)
                burst_row.addWidget(btn)
                self.burst_buttons.append((val, btn))
            self.burst_group.idClicked.connect(self._on_burst_selected)
            layout.addLayout(burst_row)

        def update_all():
            cps = self.cps_slider.value()
            delay = self.delay_slider.value() / 1000
            micro = self.micro_slider.value() / 1_000_000
            if self.slow_btn.isChecked():
                cps = max(1, cps // 10)
            self.engine.set_cps(self.key, cps)
            self.engine.set_delay(self.key, delay + micro)
            self.cps_label.setText(f"CPS {cps}")
            self.delay_label.setText(f"{int(delay * 1000)} ms")
            self.micro_label.setText(f"fine {self.micro_slider.value()} µs")

        self.cps_slider.valueChanged.connect(update_all)
        self.delay_slider.valueChanged.connect(update_all)
        self.micro_slider.valueChanged.connect(update_all)
        self.slow_btn.toggled.connect(update_all)

        if self.presets:
            preset_row = QHBoxLayout()
            for name, (c, d) in self.presets.items():
                btn = QPushButton(name)
                btn.clicked.connect(lambda _, c=c, d=d: self._apply_preset(c, d))
                preset_row.addWidget(btn)
            layout.addLayout(preset_row)

        for row_widgets in (
            (self.cps_label, self.cps_slider),
            (self.delay_label, self.delay_slider),
            (self.micro_label, self.micro_slider),
        ):
            row = QHBoxLayout()
            for w in row_widgets:
                row.addWidget(w)
            layout.addLayout(row)

        layout.addWidget(self.slow_btn)
        layout.addWidget(self.live)
        self.sync_from_engine()

    def _on_burst_selected(self, val: int):
        self.engine.set_burst_count(self.key, val)

    def sync_from_engine(self):
        btn = self.engine.buttons.get(self.key)
        if not btn:
            return
        self.cps_slider.blockSignals(True)
        self.delay_slider.blockSignals(True)
        self.cps_slider.setValue(btn.cps)
        self.delay_slider.setValue(int(btn.delay * 1000))
        self.cps_label.setText(f"CPS {btn.cps}")
        self.delay_label.setText(f"{int(btn.delay * 1000)} ms")
        self.cps_slider.blockSignals(False)
        self.delay_slider.blockSignals(False)
        if self.burst_group:
            burst = self.engine.get_burst_count(self.key)
            btn = self.burst_group.button(burst)
            if btn:
                self.burst_group.blockSignals(True)
                btn.setChecked(True)
                self.burst_group.blockSignals(False)

    def _apply_preset(self, cps: int, delay_ms: int):
        self.cps_slider.setValue(cps)
        self.delay_slider.setValue(delay_ms)

    def update_live_cps(self):
        cps = self.engine.get_real_cps(self.key)
        self.live.setText(f"REAL CPS: {cps}")
