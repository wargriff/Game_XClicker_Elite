"""
Game XClicker Elite — Launcher bureau (interface JS iCUE + moteur Python).

Lance le moteur Win32, l'API Sidecar (17840), Node.js (5173) et ouvre
la fenêtre native avec l'interface web iCUE.
"""

import os
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT)

import utils.autopatch  # noqa: F401

from config.paths import resolve_icon
from services.bootstrap import bootstrap
from utils.debug import log

APP_TITLE = "Game XClicker Elite — SOURIS WARGRIFF"
UI_URL = "http://127.0.0.1:5173"


def _wait_node(node, timeout=15.0) -> bool:
    deadline = time.time() + timeout
    while time.time() < deadline:
        if node.online:
            return True
        time.sleep(0.2)
    return node.online


def _run_webview():
    import webview

    icon = resolve_icon()
    window = webview.create_window(
        APP_TITLE,
        UI_URL,
        width=1280,
        height=820,
        min_size=(1024, 640),
        background_color="#1a1a1a",
    )
    if icon and os.path.exists(icon):
        try:
            window.icon = icon
        except Exception:
            pass
    webview.start(debug=os.environ.get("XMACRO_DEBUG") == "1")


def _run_browser_fallback():
    import webbrowser
    webbrowser.open(UI_URL)
    print(f"[LAUNCHER] Navigateur → {UI_URL}")
    print("[LAUNCHER] Appuyez Ctrl+C pour quitter")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        pass


def main() -> int:
    print("[LAUNCHER] Game XClicker Elite v3.0 — interface JS iCUE")
    ctx = None
    try:
        ctx = bootstrap()
        if not _wait_node(ctx.node):
            log("LAUNCHER", "Node.js lent — ouverture quand même")

        mode = os.environ.get("XCLICKER_UI", "webview").lower()
        if mode == "browser":
            _run_browser_fallback()
        else:
            try:
                _run_webview()
            except ImportError:
                print("[LAUNCHER] pywebview absent — pip install pywebview")
                _run_browser_fallback()
    except Exception as exc:
        print(f"[LAUNCHER] Erreur: {exc}")
        return 1
    finally:
        if ctx:
            ctx.proxy.stop()
            ctx.sidecar.stop()
            if ctx.node:
                ctx.node.stop()
    return 0


if __name__ == "__main__":
    sys.exit(main())
