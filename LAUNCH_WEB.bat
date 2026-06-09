@echo off
title Game XClicker Elite — Interface WEB (.py)
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"

echo.
echo ========================================
echo   LAUNCH WEB — navigateur / pywebview
echo   Preview avant build .exe
echo ========================================
echo Python: %PY%
echo Node:   %NODE%
echo.

if not exist "gxclicker.py" (
    echo ERREUR: gxclicker.py absent — lancez REPARER.bat
    pause
    exit /b 1
)

"%PY%" --version 2>nul
if errorlevel 1 ( echo ERREUR Python & pause & exit /b 1 )

echo Installation dependances...
"%PY%" -m pip install -r requirements.txt -q 2>nul

if exist "nodejs\package.json" (
    if not exist "nodejs\node_modules" (
        echo npm install...
        pushd nodejs
        call npm install --silent 2>nul
        popd
    )
)

echo.
echo Lancement main.py --web
echo   URL: http://127.0.0.1:17840 ou http://127.0.0.1:5173
echo.
set GX_BROWSER=1
"%PY%" main.py --web
if errorlevel 1 pause
exit /b %ERRORLEVEL%
