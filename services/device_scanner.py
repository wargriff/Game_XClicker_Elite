"""Détection automatique périphériques + capteurs système (style iCUE)."""

from __future__ import annotations

import platform
import sys
from typing import Any

try:
    import psutil
except ImportError:  # pragma: no cover
    psutil = None  # type: ignore


def _cpu_name() -> str:
    name = platform.processor() or "CPU"
    if len(name) > 48:
        name = name[:45] + "..."
    return name


def scan_sensors() -> list[dict[str, Any]]:
    if not psutil:
        return [{"id": "load", "label": "Load", "value": "—", "unit": "", "icon": "chart"}]

    sensors: list[dict[str, Any]] = []

    load = psutil.cpu_percent(interval=0)
    sensors.append({
        "id": "load",
        "label": "Load",
        "value": f"{load:.0f}",
        "unit": "%",
        "icon": "chart",
        "detail": _cpu_name(),
    })

    freq = psutil.cpu_freq()
    if freq and freq.current:
        sensors.append({
            "id": "vcpu",
            "label": "VCPU",
            "value": f"{freq.current / 1000:.2f}",
            "unit": "GHz",
            "icon": "bolt",
            "detail": _cpu_name(),
        })

    mem = psutil.virtual_memory()
    sensors.append({
        "id": "ram",
        "label": "Memory",
        "value": f"{mem.percent:.0f}",
        "unit": "%",
        "icon": "ram",
        "detail": f"{mem.used // (1024 ** 3)} / {mem.total // (1024 ** 3)} GB",
    })

    temps_fn = getattr(psutil, "sensors_temperatures", None)
    if temps_fn:
        for name, entries in (temps_fn() or {}).items():
            if not entries:
                continue
            current = entries[0].current
            if current is None:
                continue
            label = "Package" if "cpu" in name.lower() or "core" in name.lower() else name
            sensors.append({
                "id": f"temp-{name}",
                "label": label[:12],
                "value": f"{current:.1f}",
                "unit": "°C",
                "icon": "temp",
                "detail": _cpu_name(),
            })
            if len([s for s in sensors if s["icon"] == "temp"]) >= 2:
                break

    return sensors[:8]


def scan_devices(engine=None) -> list[dict[str, Any]]:
    """Liste les périphériques détectés + hub XClicker."""
    active_macros = 0
    total_cps = 0
    engine_on = False
    if engine is not None:
        engine_on = bool(getattr(engine, "enabled", False))
        if hasattr(engine, "count_active_macros"):
            active_macros = engine.count_active_macros()
        if hasattr(engine, "get_total_cps"):
            total_cps = engine.get_total_cps()

    mouse_online = sys.platform == "win32"
    kb_online = sys.platform == "win32"
    hub_online = engine_on or active_macros > 0

    devices = [
        {
            "id": "commander",
            "name": "COMMANDER CORE XT",
            "type": "hub",
            "image": "/devices/hub.svg",
            "online": True,
            "detected": True,
            "status": "active" if hub_online else "idle",
            "detail": f"{active_macros} macro(s) · CPS Σ {total_cps}",
        },
        {
            "id": "mouse",
            "name": "SOURIS WARGRIFF",
            "type": "mouse",
            "image": "/devices/mouse.svg",
            "online": mouse_online,
            "detected": mouse_online,
            "status": "active" if active_macros else "idle",
            "detail": "Win32 macro engine",
        },
        {
            "id": "keyboard",
            "name": "K100 RGB",
            "type": "keyboard",
            "image": "/devices/keyboard.svg",
            "online": kb_online,
            "detected": kb_online,
            "status": "idle",
            "detail": "Touches 1-4 mappées",
        },
        {
            "id": "headset",
            "name": "HS80 RGB WIRELESS",
            "type": "headset",
            "image": "/devices/headset.svg",
            "online": False,
            "detected": False,
            "status": "offline",
            "detail": "Non détecté",
        },
        {
            "id": "fan",
            "name": "QL120 RGB",
            "type": "fan",
            "image": "/devices/fan.svg",
            "online": psutil is not None and psutil.cpu_count() > 0,
            "detected": True,
            "status": "idle",
            "detail": f"{psutil.cpu_count() if psutil else 0} threads CPU",
        },
        {
            "id": "ram",
            "name": "DOMINATOR RGB",
            "type": "ram",
            "image": "/devices/ram.svg",
            "online": psutil is not None,
            "detected": True,
            "status": "idle",
            "detail": _ram_detail(),
        },
    ]

    detected = [d for d in devices if d["detected"]]
    # Toujours afficher le hub + souris WARGRIFF (marque)
    core_ids = {"commander", "mouse", "keyboard"}
    core = [d for d in devices if d["id"] in core_ids]
    extra = [d for d in detected if d["id"] not in core_ids]
    return core + extra


def _ram_detail() -> str:
    if not psutil:
        return "—"
    mem = psutil.virtual_memory()
    return f"{mem.total // (1024 ** 3)} GB DDR"
