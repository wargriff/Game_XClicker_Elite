#!/usr/bin/env python3
"""Build .exe + copie automatique sur le Bureau Windows."""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE_NAME = "Game XClicker Elite.exe"
DIST_DIR = os.path.join(ROOT, "dist", "Game XClicker Elite")
DESKTOP = os.path.join(os.path.expanduser("~"), "Desktop")
DESKTOP_APP = os.path.join(DESKTOP, "Game XClicker Elite")


def _run_build() -> None:
    py = sys.executable
    subprocess.check_call([py, "-m", "pip", "install", "-r", "requirements.txt", "pyinstaller", "-q"])
    icon_script = os.path.join(ROOT, "scripts", "generate_icon.py")
    if os.path.isfile(icon_script):
        subprocess.call([py, icon_script])
    subprocess.check_call([py, "-m", "PyInstaller", "build.spec", "--noconfirm"])
    if sys.platform == "win32":
        ps = (
            "Get-ChildItem 'dist\\Game XClicker Elite' -Recurse -ErrorAction SilentlyContinue "
            "| Unblock-File"
        )
        subprocess.call(["powershell", "-Command", ps], cwd=ROOT)


def publish_desktop() -> str:
    """Copie dist vers Bureau et supprime anciens .exe."""
    src_exe = os.path.join(DIST_DIR, EXE_NAME)
    if not os.path.isfile(src_exe):
        raise FileNotFoundError(f"Build introuvable: {src_exe}")

    # Supprimer anciens fichiers Bureau
    for old_name in (
        "Game XClicker Elite.exe",
        "Game_XClicker_Elite.exe",
        "gxclicker.exe",
    ):
        old = os.path.join(DESKTOP, old_name)
        if os.path.isfile(old):
            try:
                os.remove(old)
                print(f"[desktop] supprimé: {old_name}")
            except OSError as exc:
                print(f"[desktop] warn: {exc}")

    old_lnk = os.path.join(DESKTOP, "Game XClicker Elite.lnk")
    if os.path.isfile(old_lnk):
        try:
            os.remove(old_lnk)
        except OSError:
            pass

    if os.path.isdir(DESKTOP_APP):
        shutil.rmtree(DESKTOP_APP)
    shutil.copytree(DIST_DIR, DESKTOP_APP)
    print(f"[desktop] copié → {DESKTOP_APP}")

    dest_exe = os.path.join(DESKTOP_APP, EXE_NAME)
    if sys.platform == "win32":
        _create_shortcut(dest_exe)
    return dest_exe


def _create_shortcut(exe_path: str) -> None:
    lnk = os.path.join(DESKTOP, "Game XClicker Elite.lnk")
    ico = os.path.join(ROOT, "assets", "brand", "favicon.ico")
    ps = f"""
$Wsh = New-Object -ComObject WScript.Shell
$Sc = $Wsh.CreateShortcut('{lnk.replace("'", "''")}')
$Sc.TargetPath = '{exe_path.replace("'", "''")}'
$Sc.WorkingDirectory = '{os.path.dirname(exe_path).replace("'", "''")}'
$Sc.IconLocation = '{ico.replace("'", "''")}'
$Sc.Description = 'Game XClicker Elite — Mission Control'
$Sc.Save()
"""
    subprocess.call(["powershell", "-Command", ps], cwd=ROOT)
    print(f"[desktop] raccourci → {lnk}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--desktop", action="store_true", help="Copie sur le Bureau après build")
    args = parser.parse_args()
    os.chdir(ROOT)
    _run_build()
    print(f"OK: {DIST_DIR}\\{EXE_NAME}")
    if args.desktop:
        publish_desktop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
