@echo off
title Game XClicker Elite
cd /d "%~dp0"

echo ========================================
echo   Game XClicker Elite — SOURIS WARGRIFF
echo ========================================

REM --- Python (venv parent ou local) ---
set "PY=%~dp0..\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"

echo Python: %PY%
"%PY%" --version 2>nul
if errorlevel 1 (
    echo ERREUR: Python introuvable. Installez Python 3.12+
    pause
    exit /b 1
)

REM --- Fix ui.py (conflit package ui/) ---
if exist "ui.py" if exist "ui\" (
    echo [FIX] Renommage ui.py...
    if exist "ui.py.bak" del /f "ui.py.bak"
    ren "ui.py" "ui.py.bak"
)

if not exist "gxclicker.py" (
    echo ERREUR: gxclicker.py manquant — git pull origin cursor/icue-web-launcher-9626
    pause
    exit /b 1
)

if /I "%~1"=="build" goto build
if /I "%~1"=="browser" goto runbrowser

REM --- .exe si compile ---
set "EXE=%~dp0dist\Game XClicker Elite\Game XClicker Elite.exe"
if exist "%EXE%" (
    echo Lancement .exe...
    start "" "%EXE%"
    exit /b 0
)

REM --- Mode dev ---
echo Installation dependances...
"%PY%" -m pip install -r requirements.txt -q 2>nul
if exist "nodejs\package.json" (
    where node >nul 2>&1
    if not errorlevel 1 (
        pushd nodejs
        if not exist node_modules call npm install --silent
        popd
    )
)

echo Lancement...
"%PY%" gxclicker.py
if errorlevel 1 pause
exit /b %ERRORLEVEL%

:runbrowser
"%PY%" -m pip install -r requirements.txt -q 2>nul
set GX_BROWSER=1
"%PY%" gxclicker.py
if errorlevel 1 pause
exit /b %ERRORLEVEL%

:build
echo Compilation .exe...
"%PY%" -m pip install -r requirements.txt pyinstaller -q
"%PY%" scripts\generate_icon.py 2>nul
"%PY%" -m PyInstaller build.spec --noconfirm
if errorlevel 1 ( echo BUILD ECHEC & pause & exit /b 1 )
powershell -Command "Get-ChildItem 'dist\Game XClicker Elite' -Recurse -ErrorAction SilentlyContinue | Unblock-File" 2>nul
echo.
echo OK: dist\Game XClicker Elite\Game XClicker Elite.exe
echo Puis: START.bat
pause
exit /b 0
