#!/usr/bin/env python3
"""Interface native PyQt6 style iCUE — fenêtre Windows (pas web)."""

from __future__ import annotations

import os
import signal
import sys
import traceback

ROOT = os.path.dirname(os.path.abspath(__file__))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)

from utils.bootstrap import ensure_project_ready

ensure_project_ready()


def main() -> int:
    from PyQt6.QtCore import QTimer
    from PyQt6.QtWidgets import QApplication

    from services.api_monitor import ApiMonitor
    from services.bootstrap import bootstrap
    from ui.sanctuary_window import MainWindow
    from ui.splash_screen import SplashScreen

    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app = QApplication(sys.argv)
    app.setApplicationName("Game XClicker Elite")

    heartbeat = QTimer()
    heartbeat.start(500)
    heartbeat.timeout.connect(lambda: None)

    splash = SplashScreen()
    splash.show()
    app.processEvents()

    ctx = None
    api_monitor = None
    exit_code = 0

    try:
        def on_progress(percent: int, message: str):
            splash.set_progress(percent, message)
            app.processEvents()

        ctx = bootstrap(on_progress)
        splash.set_progress(95, "Interface iCUE native…")
        app.processEvents()

        window = MainWindow(ctx.proxy, boot=ctx)
        window._bind_macro_attrs()

        api_monitor = ApiMonitor(window)

        def _timer_factory(callback):
            t = QTimer()
            t.timeout.connect(callback)
            return t

        api_monitor.start(_timer_factory)
        window.show()
        splash.close()

        def shutdown():
            if api_monitor:
                api_monitor.stop()
            if ctx:
                try:
                    ctx.proxy.stop()
                    ctx.sidecar.stop()
                    if ctx.node:
                        ctx.node.stop()
                except Exception:
                    pass

        app.aboutToQuit.connect(shutdown)
        exit_code = app.exec()

    except Exception:
        traceback.print_exc()
        splash.set_progress(0, "Erreur — voir console")
        exit_code = 1
    finally:
        if api_monitor:
            try:
                api_monitor.stop()
            except Exception:
                pass
        if ctx:
            try:
                ctx.proxy.stop()
                ctx.sidecar.stop()
                if ctx.node:
                    ctx.node.stop()
            except Exception:
                pass

    return exit_code


if __name__ == "__main__":
    raise SystemExit(main())
