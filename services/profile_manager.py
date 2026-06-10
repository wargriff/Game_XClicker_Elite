import json
import os
from typing import TYPE_CHECKING, Any, Dict, List, Optional

from core.engine import MacroManager

if TYPE_CHECKING:
    from core.rgb_engine import RGBEngine


class ProfileManager:
    def __init__(self, profiles_dir: str = "profiles"):
        base = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        self.profiles_dir = profiles_dir if os.path.isabs(profiles_dir) else os.path.join(base, profiles_dir)
        self.current_name = "default"
        self._data: Dict[str, Any] = {}

    def list_profiles(self) -> List[str]:
        if not os.path.isdir(self.profiles_dir):
            return ["default"]
        names = [
            f[:-5] for f in os.listdir(self.profiles_dir)
            if f.endswith(".json")
        ]
        return names or ["default"]

    def path_for(self, name: str) -> str:
        return os.path.join(self.profiles_dir, f"{name}.json")

    def load(self, name: Optional[str] = None) -> Dict[str, Any]:
        self.current_name = name or self.current_name
        path = self.path_for(self.current_name)
        if not os.path.exists(path):
            self._data = self._default_data()
            return self._data
        with open(path, "r", encoding="utf-8") as f:
            self._data = json.load(f)
        return self._data

    def save(self, name: Optional[str] = None) -> None:
        self.current_name = name or self.current_name
        os.makedirs(self.profiles_dir, exist_ok=True)
        path = self.path_for(self.current_name)
        with open(path, "w", encoding="utf-8") as f:
            json.dump(self._data, f, indent=2)

    def apply_to_engine(self, manager: MacroManager) -> None:
        macro = self._data.get("macro", {}).get("buttons", {})
        for key, cfg in macro.items():
            engine = manager.mouse if key in manager.mouse.buttons else manager.keyboard
            if key not in engine.buttons:
                continue
            if "cps" in cfg:
                engine.set_cps(key, cfg["cps"])
            if "delay" in cfg:
                engine.set_delay(key, cfg["delay"])
            if "burst_count" in cfg:
                engine.set_burst_count(key, cfg["burst_count"])

    def apply_to_rgb(self, rgb: "RGBEngine") -> None:
        from PyQt6.QtGui import QColor

        zones = self._data.get("rgb", {})
        for zone, cfg in zones.items():
            if zone not in rgb.zones:
                continue
            if "mode" in cfg:
                rgb.set_mode(zone, cfg["mode"])
            if "color" in cfg:
                r, g, b = cfg["color"]
                rgb.set_color(zone, QColor(r, g, b))

    def capture_from_engine(self, manager: MacroManager) -> None:
        buttons = {}
        for eng in (manager.mouse, manager.keyboard):
            for key, btn in eng.buttons.items():
                buttons[key] = {
                    "cps": btn.cps,
                    "delay": btn.delay,
                    "burst_count": btn.burst_count,
                }
        self._data.setdefault("macro", {})["buttons"] = buttons

    def capture_from_rgb(self, rgb: "RGBEngine") -> None:
        zones = {}
        for name, zone in rgb.zones.items():
            c = zone.color
            zones[name] = {
                "mode": zone.mode,
                "color": [c.red(), c.green(), c.blue()],
            }
        self._data["rgb"] = zones

    def _default_data(self) -> Dict[str, Any]:
        return {
            "rgb": {},
            "bindings": {},
            "macro": {"buttons": {}},
        }
