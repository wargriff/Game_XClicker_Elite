"""REST API Sidecar — moteur macros + contrôle UI web."""

import json
import os
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer
from typing import Optional
from urllib.parse import parse_qs, urlparse

from config.paths import UI_WEB_DIR

MISSION_HTML = os.path.join(UI_WEB_DIR, "index.html")


class SidecarAPI:
    PORT = 17840
    VERSION = "3.0.0"
    MISSION_URL = f"http://127.0.0.1:{PORT}/"
    MACRO_KEYS = ("left", "right", "1", "2", "3", "4")

    def __init__(self, engine_proxy, profiles=None):
        self.engine = engine_proxy
        self.profiles = profiles
        self._server: Optional[HTTPServer] = None
        self._thread: Optional[threading.Thread] = None
        self.online = False

    def start(self):
        engine = self.engine
        profiles = self.profiles
        version = self.VERSION
        sidecar = self
        macro_keys = self.MACRO_KEYS

        class Handler(BaseHTTPRequestHandler):
            def log_message(self, *_args):
                pass

            def _cors(self):
                self.send_header("Access-Control-Allow-Origin", "*")
                self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
                self.send_header("Access-Control-Allow-Headers", "Content-Type")

            def _json(self, code: int, data: dict):
                body = json.dumps(data, ensure_ascii=False).encode("utf-8")
                self.send_response(code)
                self.send_header("Content-Type", "application/json; charset=utf-8")
                self._cors()
                self.send_header("Content-Length", str(len(body)))
                self.end_headers()
                self.wfile.write(body)

            def _read_json(self) -> dict:
                length = int(self.headers.get("Content-Length", 0))
                if length <= 0:
                    return {}
                raw = self.rfile.read(length).decode("utf-8")
                return json.loads(raw or "{}")

            def _macro_payload(self, key: str) -> dict:
                btn = engine.buttons.get(key)
                if not btn:
                    return {}
                return {
                    "key": key,
                    "active": engine.is_active(key),
                    "cps": engine.get_cps(key),
                    "real_cps": engine.get_real_cps(key),
                    "delay_ms": int(engine._get_engine(key).buttons[key].delay * 1000),
                    "burst": engine.get_burst_count(key),
                }

            def do_OPTIONS(self):
                self.send_response(204)
                self._cors()
                self.end_headers()

            def do_GET(self):
                path = urlparse(self.path).path.rstrip("/") or "/"

                if path in ("/", "/app", "/mission"):
                    if os.path.exists(MISSION_HTML):
                        with open(MISSION_HTML, encoding="utf-8") as f:
                            body = f.read().encode("utf-8")
                        self.send_response(200)
                        self.send_header("Content-Type", "text/html; charset=utf-8")
                        self.send_header("Content-Length", str(len(body)))
                        self.end_headers()
                        self.wfile.write(body)
                    else:
                        self._json(200, {"status": "ok", "ui": "ui-web missing"})
                    return

                if path in ("/health", "/api/health", "/api/v1/health"):
                    self._json(200, {
                        "status": "ok",
                        "version": version,
                        "enabled": engine.enabled,
                        "active_macros": engine.count_active_macros(),
                        "total_cps": engine.get_total_cps(),
                    })
                    return

                if path == "/api/v1/macros":
                    self._json(200, {
                        "macros": [self._macro_payload(k) for k in macro_keys],
                    })
                    return

                if path.startswith("/api/v1/macros/"):
                    key = path.split("/")[-1]
                    if key not in macro_keys:
                        self._json(404, {"error": "macro not found"})
                        return
                    self._json(200, self._macro_payload(key))
                    return

                if path == "/api/status":
                    self._json(200, {
                        "engine": "active" if engine.enabled else "stasis",
                        "macros": {
                            k: {
                                "active": engine.is_active(k),
                                "cps": engine.get_real_cps(k),
                            }
                            for k in engine.buttons
                        },
                    })
                    return

                if path == "/api/v1/profiles" and profiles:
                    self._json(200, {"profiles": profiles.list_profiles()})
                    return

                self._json(404, {"error": "not found"})

            def do_POST(self):
                path = urlparse(self.path).path.rstrip("/")
                data = self._read_json()

                if path == "/api/v1/engine/toggle":
                    engine.toggle()
                    self._json(200, {"enabled": engine.enabled})
                    return

                if path == "/api/v1/engine/enable":
                    state = bool(data.get("enabled", True))
                    if state != engine.enabled:
                        engine.toggle()
                    self._json(200, {"enabled": engine.enabled})
                    return

                if path.startswith("/api/v1/macros/"):
                    key = path.split("/")[-1]
                    if key not in macro_keys:
                        self._json(404, {"error": "macro not found"})
                        return
                    if "cps" in data:
                        engine.set_cps(key, int(data["cps"]))
                    if "delay_ms" in data:
                        engine.set_delay(key, float(data["delay_ms"]) / 1000.0)
                    if "burst" in data:
                        engine.set_burst_count(key, int(data["burst"]))
                    if "active" in data:
                        engine.set_active(key, bool(data["active"]))
                    self._json(200, self._macro_payload(key))
                    return

                self._json(404, {"error": "not found"})

        try:
            self._server = HTTPServer(("127.0.0.1", self.PORT), Handler)
            self._thread = threading.Thread(target=self._server.serve_forever, daemon=True)
            self._thread.start()
            self.online = True
            print(f"[SIDECAR] API v{version} → http://127.0.0.1:{self.PORT}")
        except OSError as exc:
            self.online = False
            print(f"[SIDECAR] API hors ligne: {exc}")

    def stop(self):
        if self._server:
            self._server.shutdown()
            self.online = False
