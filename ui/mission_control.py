"""Mission Control — hub unique avant lancement (style gaming rouge)."""

from __future__ import annotations

import os
import shutil
import subprocess
import sys
from typing import Optional

from PyQt6.QtCore import QProcess, Qt, QTimer
from PyQt6.QtGui import QFont, QPixmap
from PyQt6.QtWidgets import (
    QApplication,
    QButtonGroup,
    QFrame,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QMainWindow,
    QPushButton,
    QScrollArea,
    QStackedWidget,
    QTextEdit,
    QVBoxLayout,
    QWidget,
)

from config.paths import DEVICES_DIR
from config.runtime import exe_dir, project_root
from services.device_scanner import scan_devices, scan_sensors
from ui.styles.mission_theme import MC, MC_CARD, MC_LAUNCH_BTN, MC_SIDEBAR

ROOT = project_root()
EXE_NAME = "Game XClicker Elite.exe"
EXE_DIST = os.path.join("dist", "Game XClicker Elite", EXE_NAME)
DESKTOP_FOLDER = "Game XClicker Elite"


class LaunchCard(QFrame):
    def __init__(self, title: str, desc: str, badge: str, on_click, parent=None):
        super().__init__(parent)
        self.setObjectName("mcCard")
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        self._on_click = on_click
        self.setMinimumHeight(140)

        lay = QVBoxLayout(self)
        lay.setContentsMargins(18, 16, 18, 16)
        badge_lbl = QLabel(badge)
        badge_lbl.setFont(QFont("Segoe UI", 20))
        badge_lbl.setStyleSheet(f"color:{MC['red']};")
        lay.addWidget(badge_lbl)

        t = QLabel(title)
        t.setFont(QFont("Segoe UI", 12, QFont.Weight.Bold))
        t.setStyleSheet(f"color:{MC['text']};")
        lay.addWidget(t)

        d = QLabel(desc)
        d.setWordWrap(True)
        d.setStyleSheet(f"color:{MC['text_dim']}; font-size:10px;")
        lay.addWidget(d)
        lay.addStretch()
        self.setObjectName("mcCard")
        self.setStyleSheet(MC_CARD)

    def mousePressEvent(self, event):
        if self._on_click:
            self._on_click()
        super().mousePressEvent(event)


class MissionControlWindow(QMainWindow):
    NAV = [
        ("Mission Control", "hub"),
        ("Device Center", "devices"),
        ("Macro Studio", "native"),
        ("Interface Web", "web"),
        ("Build & Deploy", "build"),
        ("Settings Hub", "settings"),
    ]

    def __init__(self):
        super().__init__()
        self._build_proc: Optional[QProcess] = None
        self.setWindowTitle("GAME XCLICKER — Mission Control")
        self.resize(1100, 720)
        self.setStyleSheet(f"background:{MC['bg']}; color:{MC['text']};")
        self._build_ui()
        self._start_status_timer()

    def _build_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        root = QHBoxLayout(central)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        # --- Sidebar ---
        sidebar = QWidget()
        sidebar.setObjectName("McSidebar")
        sidebar.setFixedWidth(200)
        sidebar.setStyleSheet(MC_SIDEBAR)
        sb_lay = QVBoxLayout(sidebar)
        sb_lay.setContentsMargins(0, 12, 0, 12)
        sb_lay.setSpacing(0)

        brand = QLabel("GAME X\nCLICKER")
        brand.setFont(QFont("Segoe UI", 14, QFont.Weight.Bold))
        brand.setStyleSheet(f"color:{MC['red_glow']}; padding:8px 16px; letter-spacing:2px;")
        sb_lay.addWidget(brand)

        sub = QLabel("Mission Control v3")
        sub.setStyleSheet(f"color:{MC['text_dim']}; font-size:9px; padding:0 16px 12px;")
        sb_lay.addWidget(sub)

        self._nav_group = QButtonGroup(self)
        for i, (label, key) in enumerate(self.NAV):
            btn = QPushButton(label)
            btn.setObjectName("navBtn")
            btn.setCheckable(True)
            btn.setProperty("nav_key", key)
            self._nav_group.addButton(btn, i)
            sb_lay.addWidget(btn)
            btn.clicked.connect(lambda checked, k=key: self._show_page(k))

        sb_lay.addStretch()

        self._status_panel = QFrame()
        self._status_panel.setStyleSheet(
            f"background:{MC['bg']}; border-top:1px solid {MC['border']}; margin:8px;"
        )
        sp_lay = QVBoxLayout(self._status_panel)
        sp_lay.setContentsMargins(12, 10, 12, 10)
        self._lbl_moteur = QLabel("MOTEUR: —")
        self._lbl_cpu = QLabel("CPU: —")
        self._lbl_ram = QLabel("RAM: —")
        self._lbl_desktop = QLabel("Bureau: —")
        for w in (self._lbl_moteur, self._lbl_cpu, self._lbl_ram, self._lbl_desktop):
            w.setStyleSheet(f"color:{MC['text_dim']}; font-size:10px;")
            sp_lay.addWidget(w)
        sb_lay.addWidget(self._status_panel)

        root.addWidget(sidebar)

        # --- Main ---
        main_col = QVBoxLayout()
        main_col.setSpacing(0)

        top = QHBoxLayout()
        self._page_title = QLabel("MISSION CONTROL")
        self._page_title.setFont(QFont("Segoe UI", 16, QFont.Weight.Bold))
        self._page_title.setStyleSheet(f"color:{MC['text']}; padding:16px 20px 8px;")
        top.addWidget(self._page_title)
        top.addStretch()
        main_col.addLayout(top)

        self._stack = QStackedWidget()
        self._stack.addWidget(self._page_hub())
        self._stack.addWidget(self._page_devices())
        self._stack.addWidget(self._page_action("native"))
        self._stack.addWidget(self._page_action("web"))
        self._stack.addWidget(self._page_build())
        self._stack.addWidget(self._page_settings())
        main_col.addWidget(self._stack, 1)

        self._device_bar = self._build_device_bar()
        main_col.addWidget(self._device_bar)

        main_wrap = QWidget()
        main_wrap.setLayout(main_col)
        root.addWidget(main_wrap, 1)

        self._nav_group.button(0).setChecked(True)

    def _page_hub(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(20, 8, 20, 16)

        hint = QLabel(
            "Choisissez comment lancer Game XClicker Elite. "
            "Tout est centralisé ici — un seul programme (GameXClicker.py / START.bat)."
        )
        hint.setWordWrap(True)
        hint.setStyleSheet(f"color:{MC['text_dim']}; font-size:11px; margin-bottom:12px;")
        lay.addWidget(hint)

        grid = QGridLayout()
        grid.setSpacing(14)
        cards = [
            ("INTERFACE NATIVE", "PyQt6 iCUE — macros, devices, contrôle complet", "🖥", self._launch_native),
            ("INTERFACE WEB", "Preview HTML dans navigateur / pywebview", "🌐", self._launch_web),
            ("BUILD .EXE", "Compile et copie sur le Bureau automatiquement", "📦", self._start_build),
            ("LANCER .EXE", "Ouvre la version Bureau (après build)", "▶", self._launch_desktop_exe),
        ]
        for i, (title, desc, badge, fn) in enumerate(cards):
            grid.addWidget(LaunchCard(title, desc, badge, fn), i // 2, i % 2)
        lay.addLayout(grid)
        lay.addStretch()
        return w

    def _page_devices(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(20, 8, 20, 16)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setStyleSheet("border:none;background:transparent;")
        host = QWidget()
        self._device_grid = QGridLayout(host)
        self._device_grid.setSpacing(10)
        scroll.setWidget(host)
        lay.addWidget(scroll)

        btn = QPushButton("Rescanner périphériques")
        btn.clicked.connect(self._fill_devices)
        lay.addWidget(btn)
        self._fill_devices()
        return w

    def _page_action(self, mode: str) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(40, 40, 40, 40)
        if mode == "native":
            title, desc, fn = (
                "Macro Studio",
                "Lance l'interface native PyQt6 avec les 6 macros et le panneau iCUE.",
                self._launch_native,
            )
        else:
            title, desc, fn = (
                "Interface Web",
                "Lance la version web (port 17840 / 5173) pour preview avant build.",
                self._launch_web,
            )
        lay.addWidget(QLabel(title))
        lay.addWidget(QLabel(desc))
        btn = QPushButton(f"Lancer {title}")
        btn.setObjectName("launchBtn")
        btn.setStyleSheet(MC_LAUNCH_BTN)
        btn.clicked.connect(fn)
        lay.addWidget(btn)
        lay.addStretch()
        return w

    def _page_build(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(20, 8, 20, 16)

        info = QLabel(
            "Le build compile l'application, la déploie dans dist\\, "
            "puis copie automatiquement sur votre Bureau (remplace l'ancien .exe)."
        )
        info.setWordWrap(True)
        info.setStyleSheet(f"color:{MC['text_dim']};")
        lay.addWidget(info)

        btn = QPushButton("BUILD & COPIER SUR LE BUREAU")
        btn.setObjectName("launchBtn")
        btn.setStyleSheet(MC_LAUNCH_BTN)
        btn.clicked.connect(self._start_build)
        lay.addWidget(btn)

        self.log = QTextEdit()
        self.log.setReadOnly(True)
        self.log.setStyleSheet(
            f"background:{MC['bg_panel']}; border:1px solid {MC['border']};"
            f"color:{MC['text_dim']}; font-family:Consolas; font-size:10px;"
        )
        lay.addWidget(self.log, 1)
        return w

    def _page_settings(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(20, 20, 20, 20)
        lay.addWidget(QLabel("Settings Hub"))
        lay.addWidget(QLabel(f"Projet: {ROOT}"))
        btn = QPushButton("REPARER (git pull)")
        btn.clicked.connect(self._run_repair)
        lay.addWidget(btn)
        lay.addStretch()
        return w

    def _build_device_bar(self) -> QFrame:
        bar = QFrame()
        bar.setFixedHeight(56)
        bar.setStyleSheet(
            f"background:{MC['bg_panel']}; border-top:1px solid {MC['border']};"
        )
        row = QHBoxLayout(bar)
        row.setContentsMargins(16, 8, 16, 8)
        icons = ["⌨", "🖱", "🎧", "⬡", "🌀", "▦", "💾"]
        for ic in icons:
            lbl = QLabel(ic)
            lbl.setStyleSheet(
                f"font-size:18px; padding:4px 12px; color:{MC['text_dim']};"
                f"border:1px solid {MC['border']}; border-radius:4px;"
            )
            row.addWidget(lbl)
        row.addStretch()
        return bar

    def _show_page(self, key: str):
        titles = {n[1]: n[0].upper() for n in self.NAV}
        self._page_title.setText(titles.get(key, key.upper()))
        idx = {"hub": 0, "devices": 1, "native": 2, "web": 3, "build": 4, "settings": 5}.get(key, 0)
        self._stack.setCurrentIndex(idx)
        if key == "devices":
            self._fill_devices()

    def _log(self, msg: str):
        if hasattr(self, "log"):
            self.log.append(msg)

    def _desktop_exe(self) -> str:
        desktop = os.path.join(os.path.expanduser("~"), "Desktop")
        return os.path.join(desktop, DESKTOP_FOLDER, EXE_NAME)

    def _dist_exe(self) -> str:
        return os.path.join(exe_dir(), EXE_DIST)

    def _fill_devices(self):
        while self._device_grid.count():
            item = self._device_grid.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
        devices = scan_devices()
        for i, d in enumerate(devices):
            card = LaunchCard(
                d.get("name", "?"),
                d.get("detail", ""),
                "⬡",
                lambda did=d.get("id"): self._on_device(did),
            )
            self._device_grid.addWidget(card, i // 3, i % 3)

    def _on_device(self, device_id: str):
        if device_id in ("commander", "mouse", "keyboard"):
            self._launch_native()

    def _start_status_timer(self):
        self._refresh_status()
        t = QTimer(self)
        t.timeout.connect(self._refresh_status)
        t.start(2000)

    def _refresh_status(self):
        try:
            import psutil

            cpu = psutil.cpu_percent(interval=0)
            mem = psutil.virtual_memory()
            self._lbl_cpu.setText(f"CPU: {cpu:.0f}%")
            self._lbl_ram.setText(f"RAM: {mem.percent:.0f}%")
        except Exception:
            self._lbl_cpu.setText("CPU: —")
            self._lbl_ram.setText("RAM: —")

        desk = self._desktop_exe()
        if os.path.isfile(desk):
            self._lbl_desktop.setText("Bureau: .exe prêt")
            self._lbl_desktop.setStyleSheet(f"color:{MC['success']}; font-size:10px;")
        else:
            self._lbl_desktop.setText("Bureau: pas encore build")
            self._lbl_desktop.setStyleSheet(f"color:{MC['text_dim']}; font-size:10px;")

        self._lbl_moteur.setText("MOTEUR: Hub actif")

    def _launch_native(self):
        self._log("→ Interface native PyQt6…")
        self.hide()
        from native_app import main as native_main

        code = native_main()
        self.show()
        self._refresh_status()
        if code != 0:
            self._log(f"Native code {code}")

    def _launch_web(self):
        self._log("→ Interface web…")
        os.environ["GX_BROWSER"] = "1"
        self.hide()
        from gxclicker import main as web_main

        code = web_main()
        self.show()
        if code != 0:
            self._log(f"Web code {code}")

    def _start_build(self):
        if self._build_proc and self._build_proc.state() != QProcess.ProcessState.NotRunning:
            self._log("Build en cours…")
            return
        self._log("→ Build + déploiement Bureau…")
        self._build_proc = QProcess(self)
        self._build_proc.setWorkingDirectory(ROOT)
        self._build_proc.setProcessChannelMode(QProcess.ProcessChannelMode.MergedChannels)
        self._build_proc.readyReadStandardOutput.connect(self._on_build_out)
        self._build_proc.finished.connect(self._on_build_done)
        script = os.path.join(ROOT, "scripts", "build_exe.py")
        self._build_proc.start(sys.executable, [script, "--desktop"])

    def _on_build_out(self):
        if not self._build_proc:
            return
        data = self._build_proc.readAllStandardOutput().data().decode("utf-8", errors="replace")
        for line in data.strip().splitlines():
            if line.strip():
                self._log(line.strip())

    def _on_build_done(self, code: int, _status):
        if code == 0:
            self._log("✓ Build OK — copié sur le Bureau")
        else:
            self._log(f"✗ Build échoué ({code})")
        self._refresh_status()

    def _launch_desktop_exe(self):
        path = self._desktop_exe()
        if not os.path.isfile(path):
            path = self._dist_exe()
        if not os.path.isfile(path):
            self._log("✗ .exe absent — faites BUILD d'abord")
            return
        self._log(f"→ {path}")
        try:
            if sys.platform == "win32":
                os.startfile(path)  # type: ignore[attr-defined]
            else:
                subprocess.Popen([path], cwd=os.path.dirname(path))
        except OSError as exc:
            self._log(f"✗ {exc}")

    def _run_repair(self):
        repair = os.path.join(ROOT, "REPARER.bat")
        if os.path.isfile(repair):
            subprocess.Popen(
                ["cmd", "/c", repair],
                cwd=ROOT,
                creationflags=getattr(subprocess, "CREATE_NEW_CONSOLE", 0),
            )


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName("Game XClicker Mission Control")
    win = MissionControlWindow()
    win.show()
    return app.exec()
