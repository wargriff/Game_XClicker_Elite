@echo off
title Game XClicker Elite — Mission Control
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"

echo.
echo  Mission Control — un seul programme
echo.

if not exist "GameXClicker.py" (
    echo ERREUR: GameXClicker.py absent — REPARER.bat
    pause
    exit /b 1
)

"%PY%" -m pip install -r requirements.txt -q 2>nul
"%PY%" GameXClicker.py
if errorlevel 1 pause
exit /b %ERRORLEVEL%
