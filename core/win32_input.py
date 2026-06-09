import ctypes
import sys

MOUSEEVENTF_LEFTDOWN = 0x0002
MOUSEEVENTF_LEFTUP = 0x0004
MOUSEEVENTF_RIGHTDOWN = 0x0008
MOUSEEVENTF_RIGHTUP = 0x0010

VK_LBUTTON = 0x01
VK_RBUTTON = 0x02
VK_XBUTTON1 = 0x05
VK_XBUTTON2 = 0x06

KEYEVENTF_KEYUP = 0x0002
INPUT_MOUSE = 0
INPUT_KEYBOARD = 1

ULONG_PTR = ctypes.c_ulonglong


class _Win32Stub:
    def GetAsyncKeyState(self, _vk):
        return 0

    def SendInput(self, *_args, **_kwargs):
        return 1


if sys.platform == "win32":
    user32 = ctypes.windll.user32
else:
    user32 = _Win32Stub()


class MOUSEINPUT(ctypes.Structure):
    _fields_ = [
        ("dx", ctypes.c_long),
        ("dy", ctypes.c_long),
        ("mouseData", ctypes.c_ulong),
        ("dwFlags", ctypes.c_ulong),
        ("time", ctypes.c_ulong),
        ("dwExtraInfo", ULONG_PTR),
    ]


class KEYBDINPUT(ctypes.Structure):
    _fields_ = [
        ("wVk", ctypes.c_ushort),
        ("wScan", ctypes.c_ushort),
        ("dwFlags", ctypes.c_ulong),
        ("time", ctypes.c_ulong),
        ("dwExtraInfo", ULONG_PTR),
    ]


class INPUT_UNION(ctypes.Union):
    _fields_ = [("mi", MOUSEINPUT), ("ki", KEYBDINPUT)]


class INPUT(ctypes.Structure):
    _anonymous_ = ("u",)
    _fields_ = [("type", ctypes.c_ulong), ("u", INPUT_UNION)]


def send_mouse(flag):
    inp = INPUT(type=INPUT_MOUSE)
    inp.mi = MOUSEINPUT(0, 0, 0, flag, 0, ULONG_PTR(0))
    user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))


def send_key(vk, down=True):
    inp = INPUT(type=INPUT_KEYBOARD)
    inp.ki = KEYBDINPUT(vk, 0, 0 if down else KEYEVENTF_KEYUP, 0, ULONG_PTR(0))
    user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))
