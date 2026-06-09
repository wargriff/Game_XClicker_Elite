@echo off
title Build Game XClicker Elite.exe
cd /d "%~dp0.."

echo === Generation icone .exe ===
python scripts\generate_icon.py
if not exist "assets\brand\favicon.ico" (
    echo Copiez favicon.ico dans assets\brand\ ou installez Pillow
)

echo === npm install ===
pushd nodejs
call npm install --silent
popd

echo === PyInstaller ===
pip install pyinstaller pywebview pillow -q

pyinstaller build.spec --noconfirm

echo.
echo === Termine ===
echo Executable: dist\Game_XClicker_Elite.exe
echo.
echo Raccourci bureau:
echo   Clic droit dist\Game_XClicker_Elite.exe ^> Envoyer vers ^> Bureau
pause
