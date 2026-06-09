import signal
import sys
import traceback

from PyQt6.QtCore import QTimer
from PyQt6.QtWidgets import QApplication

from core.engine import MacroManager
from services.engine_proxy import EngineProxy
from ui.main_window import MainWindow


_original_excepthook = sys.excepthook
_handling_fatal = False


def handle_exception(exc_type, exc_value, exc_tb):
    global _handling_fatal
    if _handling_fatal:
        _original_excepthook(exc_type, exc_value, exc_tb)
        return
    _handling_fatal = True
    try:
        print(f"[FATAL] {exc_type.__name__}: {exc_value}")
        traceback.print_exception(exc_type, exc_value, exc_tb)
    except Exception:
        _original_excepthook(exc_type, exc_value, exc_tb)
    finally:
        _handling_fatal = False


sys.excepthook = handle_exception


def safe_stop(engine):
    if not engine:
        return
    try:
        engine.stop()
    except Exception:
        pass


def main():
    print("[XMACRO] Booting...")

    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app = QApplication(sys.argv)

    timer = QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    proxy = None
    exit_code = 0

    try:
        manager = MacroManager()
        proxy = EngineProxy(manager)
        window = MainWindow(proxy, "assets/mouse.svg")
        window.show()

        def shutdown():
            print("[XMACRO] Shutting down...")
            safe_stop(proxy)

        app.aboutToQuit.connect(shutdown)
        exit_code = app.exec()

    except Exception:
        traceback.print_exc()
        exit_code = 1

    finally:
        safe_stop(proxy)
        print("[XMACRO] Exit complete")

    return exit_code


if __name__ == "__main__":
    sys.exit(main())
