@echo off
title Fix rgb_engine
cd /d "%~dp0"
set "BASE=https://raw.githubusercontent.com/wargriff/Game_XClicker_Elite/main"
set "PY=%~dp0..\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"

echo Telechargement rgb_engine + sanctuary_window...
if not exist "core" mkdir core

curl -fsSL "%BASE%/rgb_engine.py" -o rgb_engine.py
curl -fsSL "%BASE%/core/rgb_engine.py" -o core\rgb_engine.py
curl -fsSL "%BASE%/core/__init__.py" -o core\__init__.py
curl -fsSL "%BASE%/ui/sanctuary_window.py" -o ui\sanctuary_window.py

echo OK. Relancez OUVRE_MOI.pyw
"%PY%" OUVRE_MOI.py
pause
