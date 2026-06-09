import os
import signal
import sys
import traceback

# Correctif global AVANT PyQt / UI (master_combo + name_edit + Sanctuary Bot)
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# autopatch = legacy_patch + diagnostic bot (aussi via PYTHONSTARTUP)
import utils.autopatch  # noqa: F401, E402
import utils.legacy_patch

print("[XMACRO] launcher sanctuary v2.2 + Sanctuary Bot — OK si vous voyez ce message", flush=True)

from PyQt6.QtCore import QTimer
from PyQt6.QtWidgets import QApplication

from services.bootstrap import bootstrap
from services.api_monitor import ApiMonitor
from ui.main_window import MainWindow
from ui.pages.macros_page import MACRO_KEYS
from ui.splash_screen import SplashScreen
from utils.debug import log

_original_excepthook = sys.excepthook
_handling_fatal = False


def handle_exception(exc_type, exc_value, exc_tb):
    """Fallback si autopatch n'a pas pris la main."""
    global _handling_fatal
    if _handling_fatal:
        return
    _handling_fatal = True
    try:
        from utils.diagnostic_bot import explain_exception
        explain_exception(exc_type, exc_value, exc_tb)
        _original_excepthook(exc_type, exc_value, exc_tb)
    except Exception:
        pass
    finally:
        _handling_fatal = False


sys.excepthook = handle_exception


def _make_api_timer(app, callback):
    from PyQt6.QtCore import QTimer

    t = QTimer()
    t.timeout.connect(callback)
    return t


def _ensure_legacy_hooks(window):
    """Injecte master_combo/name_edit si MainWindow custom local sans ces attrs."""
    if "master_combo" in getattr(window, "__dict__", {}) and "name_edit" in getattr(
        window, "__dict__", {}
    ):
        return
    if getattr(window, "master_combo", None) and getattr(window, "name_edit", None):
        return

    from PyQt6.QtWidgets import QComboBox, QLineEdit

    combo = QComboBox()
    for _, label in MACRO_KEYS:
        combo.addItem(label)
    edit = QLineEdit("default")
    edit.setReadOnly(True)
    window.master_combo = combo
    window.name_edit = edit
    log("XMACRO", "⚠ hooks legacy injectés (MainWindow custom détecté)")


def safe_stop(ctx):
    if not ctx:
        return
    try:
        ctx.proxy.stop()
        ctx.sidecar.stop()
        if ctx.node:
            ctx.node.stop()
    except Exception:
        pass


def _print_build_info():
    root = os.path.dirname(os.path.abspath(__file__))
    has_node = os.path.isdir(os.path.join(root, "nodejs"))
    has_debug = os.path.isfile(os.path.join(root, "utils", "debug.py"))
    print(f"[XMACRO] Build check — nodejs/={has_node} utils/debug.py={has_debug}")
    if not has_node or not has_debug:
        print(
            "[XMACRO] ⚠ Version ancienne détectée !\n"
            "[XMACRO]   git fetch origin\n"
            "[XMACRO]   git checkout cursor/sanctuary-diablo-ui-9626\n"
            "[XMACRO]   git pull origin cursor/sanctuary-diablo-ui-9626"
        )


def main():
    _print_build_info()
    log("XMACRO", "Booting unified launcher…")
    print("[XMACRO] Debug logs actifs — XMACRO_DEBUG=0 pour désactiver")

    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app = QApplication(sys.argv)
    app.setApplicationName("Game XClicker Elite")

    timer = QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    splash = SplashScreen()
    splash.show()
    app.processEvents()

    ctx = None
    exit_code = 0
    api_monitor = None

    try:
        def on_progress(percent: int, message: str):
            splash.set_progress(percent, message)
            app.processEvents()

        ctx = bootstrap(on_progress)
        splash.set_progress(95, "Ouverture interface iCUE Sanctuary…")
        app.processEvents()

        window = MainWindow(ctx.proxy, boot=ctx)
        utils.legacy_patch.hook_mainwindow_import()
        utils.legacy_patch.ensure_instance_legacy(window)
        _ensure_legacy_hooks(window)
        if hasattr(window, "_bind_macro_attrs"):
            window._bind_macro_attrs()
        log("XMACRO", f"MainWindow={type(window).__module__}.{type(window).__name__}")
        log("XMACRO", f"master_combo={window.master_combo is not None}")
        log("XMACRO", f"name_edit={window.name_edit is not None}")

        from utils import diagnostic_bot
        diagnostic_bot.say("Fenêtre principale OK — master_combo et name_edit présents", "OK")

        api_monitor = ApiMonitor(window)
        api_monitor.start(lambda cb: _make_api_timer(app, cb))
        window.show()
        splash.close()

        def shutdown():
            print("[XMACRO] Shutting down…")
            if api_monitor:
                api_monitor.stop()
            safe_stop(ctx)

        app.aboutToQuit.connect(shutdown)
        exit_code = app.exec()

    except Exception:
        traceback.print_exc()
        splash.set_progress(0, "Erreur au démarrage — voir la console")
        exit_code = 1

    finally:
        try:
            if api_monitor:
                api_monitor.stop()
        except Exception:
            pass
        safe_stop(ctx)
        print("[XMACRO] Exit complete")

    return exit_code


if __name__ == "__main__":
    sys.exit(main())
