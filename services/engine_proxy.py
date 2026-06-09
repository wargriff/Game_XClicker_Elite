from core.engine import MacroManager


class EngineProxy:
    def __init__(self, manager: MacroManager):
        self.manager = manager
        self.buttons = {
            **manager.mouse.buttons,
            **manager.keyboard.buttons,
        }

    @property
    def enabled(self):
        return self.manager.mouse.enabled

    @property
    def running(self):
        return self.manager.mouse.running and self.manager.keyboard.running

    @running.setter
    def running(self, state: bool):
        self.manager.mouse.running = state
        self.manager.keyboard.running = state

    def toggle(self):
        self.manager.toggle_all()

    def stop(self):
        self.manager.stop()

    def set_game_safe(self, state: bool):
        self.manager.mouse.game_safe = state
        self.manager.keyboard.game_safe = state

    def set_on_toggle(self, callback):
        self.manager.set_on_toggle(callback)

    def _get_engine(self, key):
        if key in self.manager.mouse.buttons:
            return self.manager.mouse
        return self.manager.keyboard

    def set_cps(self, key, value):
        self._get_engine(key).set_cps(key, value)

    def set_delay(self, key, value):
        self._get_engine(key).set_delay(key, value)

    def set_burst_count(self, key, value):
        self._get_engine(key).set_burst_count(key, value)

    def get_burst_count(self, key):
        return self._get_engine(key).get_burst_count(key)

    def get_cps(self, key):
        return self._get_engine(key).get_cps(key)

    def get_real_cps(self, key):
        return self._get_engine(key).get_real_cps(key)

    def set_active(self, key, state: bool):
        engine = self._get_engine(key)
        if key in engine.buttons:
            engine.buttons[key].active = state

    def is_active(self, key) -> bool:
        engine = self._get_engine(key)
        if key in engine.buttons:
            return engine.buttons[key].active
        return False
