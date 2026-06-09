@echo off
title Game XClicker Elite — Launcher Bureau
cd /d "%~dp0.."

set XMACRO_DEBUG=0
set XCLICKER_UI=webview
set PYTHONSTARTUP=%~dp0..\utils\autopatch.py

echo === Game XClicker Elite v3.0 — SOURIS WARGRIFF ===

if exist "nodejs\package.json" (
    where node >nul 2>&1
    if not errorlevel 1 (
        pushd nodejs
        if not exist "node_modules\" call npm install --silent
        popd
    )
)

python scripts\generate_icon.py 2>nul

if exist ".venv\Scripts\python.exe" (
    ".venv\Scripts\python.exe" launcher\desktop_main.py
) else if exist "venv\Scripts\python.exe" (
    "venv\Scripts\python.exe" launcher\desktop_main.py
) else (
    python launcher\desktop_main.py
)

if errorlevel 1 pause
