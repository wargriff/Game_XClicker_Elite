import random
import threading
import time
from typing import Callable, Dict, Optional

from core.models import Btn, Stats
from core.win32_input import (
    MOUSEEVENTF_LEFTDOWN,
    MOUSEEVENTF_LEFTUP,
    MOUSEEVENTF_RIGHTDOWN,
    MOUSEEVENTF_RIGHTUP,
    VK_LBUTTON,
    VK_RBUTTON,
    VK_XBUTTON1,
    VK_XBUTTON2,
    send_key,
    send_mouse,
    user32,
)

GAME_SAFE_MAX_CPS = 30
IGNORE_LEFT_MS = 0.05
IGNORE_RIGHT_MS = 0.15
TOGGLE_COOLDOWN_MS = 0.30
MIN_PRESS_MS = 0.05


class BaseEngine:
    def __init__(self):
        self.enabled = True
        self.running = True
        self.game_safe = False
        self.buttons: Dict[str, Btn] = {}
        self.stats: Dict[str, Stats] = {}
        self._threads = []
        self._on_toggle: Optional[Callable[[str, bool], None]] = None

    def set_on_toggle(self, callback: Callable[[str, bool], None]):
        self._on_toggle = callback

    def get_cps(self, key):
        return self.buttons.get(key, Btn()).cps

    def set_cps(self, key, value):
        if key in self.buttons:
            max_cps = GAME_SAFE_MAX_CPS if self.game_safe else 200
            self.buttons[key].cps = max(1, min(max_cps, value))

    def set_delay(self, key, value):
        if key in self.buttons:
            self.buttons[key].delay = max(0.0, value)

    def set_burst_count(self, key, value):
        if key in self.buttons:
            self.buttons[key].burst_count = max(0, value)

    def get_burst_count(self, key):
        return self.buttons.get(key, Btn()).burst_count

    def get_real_cps(self, key):
        return self.stats.get(key, Stats()).cps

    def stop(self):
        self.running = False

    def toggle_global(self):
        self.enabled = not self.enabled

    def _register(self, key):
        s = self.stats[key]
        now = time.perf_counter()
        with s.lock:
            s.timestamps.append(now)
            while s.timestamps and now - s.timestamps[0] > 1:
                s.timestamps.popleft()
            s.cps = len(s.timestamps)

    def _effective_interval(self, btn: Btn) -> float:
        cps = btn.cps
        if self.game_safe:
            cps = min(cps, GAME_SAFE_MAX_CPS)
        interval = max(1 / cps, btn.delay)
        if self.game_safe:
            interval += random.uniform(0, 0.008)
        return interval

    def _loop(self, key, action):
        while self.running:
            if not self.enabled or not self.buttons[key].active:
                time.sleep(0.01)
                continue

            interval = self._effective_interval(self.buttons[key])
            start = time.perf_counter()
            action(key)
            self._register(key)

            sleep = interval - (time.perf_counter() - start)
            if sleep > 0:
                time.sleep(sleep)

    def _start_worker(self, key, action):
        t = threading.Thread(target=self._loop, args=(key, action), daemon=True)
        t.start()
        self._threads.append(t)

    def _notify_toggle(self, key: str, active: bool):
        if self._on_toggle:
            self._on_toggle(key, active)


class MouseEngine(BaseEngine):
    def __init__(self):
        super().__init__()
        self.buttons = {"left": Btn(), "right": Btn(cps=15, delay=0.019)}
        self.stats = {k: Stats() for k in self.buttons}
        self._last_state = {"left": False, "right": False}
        self._press_time = {"left": 0.0, "right": 0.0}
        self._ignore_until = {"left": 0.0, "right": 0.0}
        self._ignore_ms = {"left": IGNORE_LEFT_MS, "right": IGNORE_RIGHT_MS}
        self._toggle_cooldown_until = {"left": 0.0, "right": 0.0}
        self._start()
        self._listener()

    def _click(self, key):
        now = time.perf_counter()
        self._ignore_until[key] = now + self._ignore_ms[key]
        if key == "left":
            send_mouse(MOUSEEVENTF_LEFTDOWN)
            send_mouse(MOUSEEVENTF_LEFTUP)
        else:
            send_mouse(MOUSEEVENTF_RIGHTDOWN)
            send_mouse(MOUSEEVENTF_RIGHTUP)

    def _fire_burst(self, key: str):
        burst = self.buttons[key].burst_count
        if burst <= 0:
            return
        threading.Thread(
            target=self._run_burst, args=(key, burst), daemon=True
        ).start()

    def _run_burst(self, key: str, burst: int):
        for _ in range(burst):
            if not self.running:
                return
            self._click(key)
            self._register(key)
            time.sleep(0.02)

    def _toggle_button(self, key: str):
        now = time.perf_counter()
        if now < self._toggle_cooldown_until[key]:
            return

        btn = self.buttons[key]
        btn.active = not btn.active
        self._toggle_cooldown_until[key] = now + TOGGLE_COOLDOWN_MS
        self._ignore_until[key] = now + self._ignore_ms[key]

        if btn.active:
            self._fire_burst(key)
        self._notify_toggle(key, btn.active)

    def _start(self):
        for k in self.buttons:
            self._start_worker(k, self._click)

    def _listener(self):
        def loop():
            while self.running:
                now = time.perf_counter()
                l = bool(user32.GetAsyncKeyState(VK_LBUTTON) & 0x8000)
                r = bool(user32.GetAsyncKeyState(VK_RBUTTON) & 0x8000)

                for key, pressed in (("left", l), ("right", r)):
                    if now <= self._ignore_until[key]:
                        self._last_state[key] = pressed
                        continue
                    if pressed and not self._last_state[key]:
                        self._press_time[key] = now
                    if not pressed and self._last_state[key]:
                        hold = now - self._press_time[key]
                        if hold >= MIN_PRESS_MS:
                            self._toggle_button(key)

                self._last_state["left"] = l
                self._last_state["right"] = r
                time.sleep(0.005)

        threading.Thread(target=loop, daemon=True).start()


class KeyEngine(BaseEngine):
    def __init__(self):
        super().__init__()
        self.buttons = {"1": Btn(), "2": Btn(), "3": Btn(), "4": Btn()}
        self.stats = {k: Stats() for k in self.buttons}
        self.vk = {"1": 0x31, "2": 0x32, "3": 0x33, "4": 0x34}
        self._start()
        self._listener()

    def _press(self, key):
        vk = self.vk[key]
        send_key(vk, True)
        send_key(vk, False)

    def _start(self):
        for k in self.buttons:
            self._start_worker(k, self._press)

    def _listener(self):
        def loop():
            last = False
            while self.running:
                p = user32.GetAsyncKeyState(VK_XBUTTON1) & 0x8000 != 0
                if p and not last:
                    active = not any(b.active for b in self.buttons.values())
                    for k, b in self.buttons.items():
                        b.active = active
                        self._notify_toggle(k, active)
                last = p
                time.sleep(0.02)

        threading.Thread(target=loop, daemon=True).start()


class MacroManager:
    def __init__(self):
        self.mouse = MouseEngine()
        self.keyboard = KeyEngine()
        self._listener()

    def toggle_all(self):
        self.mouse.toggle_global()
        self.keyboard.toggle_global()

    def stop(self):
        self.mouse.stop()
        self.keyboard.stop()

    def set_on_toggle(self, callback: Callable[[str, bool], None]):
        self.mouse.set_on_toggle(callback)
        self.keyboard.set_on_toggle(callback)

    def _listener(self):
        def loop():
            last = False
            while self.mouse.running:
                p = user32.GetAsyncKeyState(VK_XBUTTON2) & 0x8000 != 0
                if p and not last:
                    self.toggle_all()
                last = p
                time.sleep(0.02)

        threading.Thread(target=loop, daemon=True).start()
