#!/usr/bin/env python3
"""Game XClicker Elite — START.bat / .exe"""

from __future__ import annotations

import os
import sys
import time
import traceback
import webbrowser


def _root() -> str:
    if getattr(sys, "frozen", False):
        return sys._MEIPASS  # type: ignore[attr-defined]
    return os.path.dirname(os.path.abspath(__file__))


def _dev_dir() -> str:
    if getattr(sys, "frozen", False):
        return os.path.dirname(sys.executable)
    return os.path.dirname(os.path.abspath(__file__))


def _prepare() -> None:
    root = _root()
    dev = _dev_dir()
    if root not in sys.path:
        sys.path.insert(0, root)

    ui_py = os.path.join(dev, "ui.py")
    ui_dir = os.path.join(dev, "ui")
    if os.path.isfile(ui_py) and os.path.isdir(ui_dir):
        bak = ui_py + ".bak"
        n = 0
        while os.path.exists(bak):
            n += 1
            bak = ui_py + f".bak{n}"
        try:
            os.rename(ui_py, bak)
            print(f"[GX] ui.py renomme -> {os.path.basename(bak)}")
        except OSError as exc:
            print(f"[GX] Fermez PyCharm puis: ren ui.py ui_old.py ({exc})")


def _pick_url(ctx) -> str:
    url = "http://127.0.0.1:17840"
    if ctx.node:
        deadline = time.time() + 8
        while time.time() < deadline and not ctx.node.online:
            time.sleep(0.2)
        if ctx.node.online:
            return "http://127.0.0.1:5173"
    print("[GX] Node.js absent — UI via Python port 17840")
    return url


def _run_browser(url: str, ctx) -> int:
    print(f"[GX] Ouverture navigateur → {url}")
    webbrowser.open(url)
    print("[GX] App active — fermez cette fenetre ou Ctrl+C pour quitter")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    return 0


def _run_webview(url: str) -> None:
    import webview
    webview.create_window(
        "Game XClicker Elite — SOURIS WARGRIFF",
        url,
        width=1280,
        height=820,
        min_size=(1024, 640),
        background_color="#1a1a1a",
    )
    webview.start()


def main() -> int:
    print("[GX] Demarrage Game XClicker Elite...")
    _prepare()

    ctx = None
    url = "http://127.0.0.1:17840"
    try:
        from services.bootstrap import bootstrap
        print("[GX] Moteur Win32 + API...")
        ctx = bootstrap()
        url = _pick_url(ctx)
        print(f"[GX] Interface → {url}")

        force_browser = "--browser" in sys.argv or os.environ.get("GX_BROWSER") == "1"
        if force_browser:
            return _run_browser(url, ctx)

        try:
            _run_webview(url)
        except ImportError:
            print("[GX] pywebview manquant — pip install pywebview")
            return _run_browser(url, ctx)
        except Exception as exc:
            print(f"[GX] Fenetre native impossible ({exc}) — fallback navigateur")
            return _run_browser(url, ctx)

    except Exception:
        traceback.print_exc()
        input("\n[GX] ERREUR — Entree pour fermer...")
        return 1
    finally:
        if ctx:
            try:
                ctx.proxy.stop()
                ctx.sidecar.stop()
                if ctx.node:
                    ctx.node.stop()
            except Exception:
                pass
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
