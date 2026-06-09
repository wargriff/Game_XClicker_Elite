@echo off
title Game XClicker Elite — SOURIS WARGRIFF v3.0
cd /d "%~dp0"

set XMACRO_DEBUG=0
set XCLICKER_UI=webview
set PYTHONSTARTUP=%~dp0utils\autopatch.py

echo ============================================
echo  Game XClicker Elite — SOURIS WARGRIFF
echo ============================================

REM Python: venv puis systeme
set PY=
if exist ".venv\Scripts\python.exe" set PY=.venv\Scripts\python.exe
if "%PY%"=="" if exist "venv\Scripts\python.exe" set PY=venv\Scripts\python.exe
if "%PY%"=="" set PY=python

REM Fix conflit ui.py (cause principale de l'erreur)
"%PY%" scripts\fix_ui_conflict.py

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

"%PY%" scripts\generate_icon.py 2>nul
"%PY%" CHECK_VERSION.py
if errorlevel 1 (
    echo.
    echo === MISE A JOUR REQUISE ===
    echo git fetch origin cursor/icue-web-launcher-9626
    echo git checkout cursor/icue-web-launcher-9626
    echo git pull origin cursor/icue-web-launcher-9626
    pause
    exit /b 1
)

echo.
echo Lancement interface iCUE...
"%PY%" run.py %*
if errorlevel 1 pause
