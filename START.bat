@echo off
title Game XClicker Elite — Sanctuary Edition
cd /d "%~dp0"

set XMACRO_DEBUG=1

if exist "nodejs\package.json" (
    echo [START] Verification dependances Node.js...
    where node >nul 2>&1
    if not errorlevel 1 (
        pushd nodejs
        if not exist "node_modules\" (
            echo [START] npm install dans nodejs\
            call npm install
        )
        popd
    ) else (
        echo [START] Node.js introuvable — installez-le ou definissez XCLICKER_NODE_PATH
    )
)

echo === Verification version ===
if exist ".venv\Scripts\python.exe" (
    ".venv\Scripts\python.exe" CHECK_VERSION.py
) else if exist "venv\Scripts\python.exe" (
    "venv\Scripts\python.exe" CHECK_VERSION.py
) else (
    python CHECK_VERSION.py
)
if errorlevel 1 (
    echo.
    echo CORRIGEZ avec: git pull origin cursor/sanctuary-diablo-ui-9626
    pause
    exit /b 1
)

echo === Lancement ===
if exist ".venv\Scripts\python.exe" (
    ".venv\Scripts\python.exe" Xmacro_main.py
) else if exist "venv\Scripts\python.exe" (
    "venv\Scripts\python.exe" Xmacro_main.py
) else (
    python Xmacro_main.py
)

if errorlevel 1 pause
