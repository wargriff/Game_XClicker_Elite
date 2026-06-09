@echo off
title Fix + Mise a jour Game XClicker Elite
cd /d "%~dp0"

echo === 1. Mise a jour GitHub ===
git fetch origin cursor/icue-web-launcher-9626
git checkout cursor/icue-web-launcher-9626 2>nul
git pull origin cursor/icue-web-launcher-9626

echo.
echo === 2. Fix conflit ui.py ===
python scripts\fix_ui_conflict.py

echo.
echo === 3. Dependances ===
pip install -r requirements.txt -q
pushd nodejs && npm install --silent && popd

echo.
echo === 4. Verification ===
python CHECK_VERSION.py

echo.
echo === 5. Lancement ===
call START.bat
