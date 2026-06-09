"""Tests détection périphériques / capteurs."""

import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if ROOT not in sys.path:
    sys.path.insert(0, ROOT)


def test_scan_devices_returns_commander():
    from services.device_scanner import scan_devices

    devices = scan_devices()
    ids = {d["id"] for d in devices}
    assert "commander" in ids
    assert "mouse" in ids
    assert all("name" in d and "detected" in d for d in devices)


def test_scan_sensors_returns_load():
    from services.device_scanner import scan_sensors

    sensors = scan_sensors()
    assert len(sensors) >= 1
    assert sensors[0]["label"]
