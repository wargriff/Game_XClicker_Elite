@echo off
title Game XClicker Elite
cd /d "%~dp0"

echo ========================================
echo   Game XClicker Elite — SOURIS WARGRIFF
echo   Entree: main.py / gxclicker.py
echo ========================================

REM --- Node.js C:\src ---
set "XCLICKER_NODE_PATH=C:\src\node.exe"
if exist "C:\src\node.exe" set "NODE=C:\src\node.exe"
if not defined NODE if exist "C:\src\node\node.exe" set "NODE=C:\src\node\node.exe"
if not defined NODE set "NODE=node"

REM --- Python venv ---
set "PY=%~dp0..\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"

echo Python: %PY%
echo Node:   %NODE%
"%PY%" --version 2>nul
if errorlevel 1 ( echo ERREUR Python & pause & exit /b 1 )

REM --- Fix ui.py ---
if exist "ui.py" if exist "ui\" (
    echo [FIX] ui.py -^> ui.py.bak
    if exist "ui.py.bak" del /f "ui.py.bak"
    ren "ui.py" "ui.py.bak"
)

if not exist "gxclicker.py" (
    echo.
    echo ERREUR: gxclicker.py absent — projet pas a jour.
    echo Lancez REPARER.bat une fois, puis relancez START.bat
    echo.
    pause
    exit /b 1
)

if not exist "main.py" (
    echo Creation main.py ...
    "%PY%" -c "open('main.py','w',encoding='utf-8').write('from gxclicker import main\nimport sys\nsys.exit(main())\n')"
)

REM --- Fix conflit ui.py / dossier ui/ ---
"%PY%" -c "import sys; sys.path.insert(0,r'%~dp0.'); from utils.bootstrap import ensure_project_ready; ensure_project_ready()" 2>nul

if /I "%~1"=="build" goto build
if /I "%~1"=="browser" goto runbrowser
if /I "%~1"=="repair" goto repair

set "EXE=%~dp0dist\Game XClicker Elite\Game XClicker Elite.exe"
if exist "%EXE%" (
    echo Lancement .exe...
    start "" "%EXE%"
    exit /b 0
)

echo Installation dependances Python...
"%PY%" -m pip install -r requirements.txt -q 2>nul

if exist "nodejs\package.json" (
    echo Installation Node.js ...
    pushd nodejs
    if not exist node_modules (
        "%NODE%" --version 2>nul
        if errorlevel 1 (
            echo WARN: Node introuvable — UI via port 17840
        ) else (
            call npm install --silent 2>nul
        )
    )
    popd
)

echo Lancement interface native PyQt6 (main.py) ...
echo   Mode web: START.bat browser
"%PY%" main.py
if errorlevel 1 pause
exit /b %ERRORLEVEL%

:repair
call "%~dp0REPARER.bat"
exit /b %ERRORLEVEL%

:runbrowser
set GX_BROWSER=1
"%PY%" main.py --web
if errorlevel 1 pause
exit /b %ERRORLEVEL%

:build
"%PY%" -m pip install -r requirements.txt pyinstaller -q
"%PY%" scripts\generate_icon.py 2>nul
"%PY%" -m PyInstaller build.spec --noconfirm
if errorlevel 1 ( echo BUILD ECHEC & pause & exit /b 1 )
powershell -Command "Get-ChildItem 'dist\Game XClicker Elite' -Recurse -ErrorAction SilentlyContinue | Unblock-File" 2>nul
echo OK: dist\Game XClicker Elite\Game XClicker Elite.exe
pause
exit /b 0
