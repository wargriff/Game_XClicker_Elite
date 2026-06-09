@echo off
title Game XClicker Elite — Sanctuary Edition v3.0
cd /d "%~dp0"

set XMACRO_DEBUG=0
set XCLICKER_UI=webview
set PYTHONSTARTUP=%~dp0utils\autopatch.py

echo ============================================
echo  Game XClicker Elite — SOURIS WARGRIFF
echo  Interface JS iCUE + moteur Python Win32
echo ============================================

if exist "nodejs\package.json" (
    where node >nul 2>&1
    if not errorlevel 1 (
        pushd nodejs
        if not exist "node_modules\" call npm install --silent
        popd
    ) else (
        echo [START] Node.js requis — https://nodejs.org
    )
)

python scripts\generate_icon.py 2>nul

echo.
echo Mode interface:
echo   [1] Bureau JS iCUE (recommande) — launcher\desktop_main.py
echo   [2] PyQt Sanctuary (legacy)      — Xmacro_main.py
echo.
set /p MODE="Choix [1/2] (defaut 1): "
if "%MODE%"=="" set MODE=1
if "%MODE%"=="2" goto PYQT

if exist ".venv\Scripts\python.exe" (
    ".venv\Scripts\python.exe" launcher\desktop_main.py
) else (
    python launcher\desktop_main.py
)
goto END

:PYQT
if exist ".venv\Scripts\python.exe" (
    ".venv\Scripts\python.exe" Xmacro_main.py
) else (
    python Xmacro_main.py
)

:END
if errorlevel 1 pause
