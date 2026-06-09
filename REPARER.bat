@echo off
title REPARER — Game XClicker Elite
cd /d "%~dp0"

echo.
echo ============================================================
echo   REPARATION COMPLETE — Game XClicker Elite
echo   Dossier: %~dp0
echo ============================================================
echo.

set "PY=%~dp0..\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"

echo Python: %PY%
"%PY%" --version 2>nul
if errorlevel 1 (
    echo ERREUR: Python introuvable
    pause
    exit /b 1
)

if not exist ".git" (
    echo.
    echo ERREUR: ce dossier n'est pas un depot git.
    echo Solution: double-cliquez CLONE_FRESH.bat pour reinstaller proprement.
    pause
    exit /b 1
)

if not exist "_backup_local" mkdir "_backup_local"
echo Sauvegarde locale dans _backup_local\ ...
copy /Y "START.bat" "_backup_local\START.bat.bak" 2>nul
copy /Y "profiles\default.json" "_backup_local\default.json.bak" 2>nul
copy /Y "profiles\default_game.json" "_backup_local\default_game.json.bak" 2>nul

echo.
echo [1/6] git fetch origin cursor/icue-web-launcher-9626 ...
git fetch origin cursor/icue-web-launcher-9626
if errorlevel 1 (
    echo ERREUR git fetch — verifiez internet et git
    pause
    exit /b 1
)

echo [2/6] Bascule sur la branche corrigee (ecrase fichiers locaux) ...
git checkout -B cursor/icue-web-launcher-9626 origin/cursor/icue-web-launcher-9626
if errorlevel 1 (
    echo checkout echoue — nettoyage ...
    git clean -fd
    git checkout -B cursor/icue-web-launcher-9626 origin/cursor/icue-web-launcher-9626
)
git reset --hard origin/cursor/icue-web-launcher-9626

echo [3/6] Verification fichiers essentiels ...
set "MISSING=0"
if not exist "gxclicker.py" ( echo   MANQUE: gxclicker.py & set "MISSING=1" )
if not exist "main.py" ( echo   MANQUE: main.py & set "MISSING=1" )
if not exist "START.bat" ( echo   MANQUE: START.bat & set "MISSING=1" )
if not exist "services\bootstrap.py" ( echo   MANQUE: services\bootstrap.py & set "MISSING=1" )
if not exist "ui-web\index.html" ( echo   MANQUE: ui-web\index.html & set "MISSING=1" )
if "%MISSING%"=="1" (
    echo.
    echo ERREUR: fichiers toujours absents apres git pull.
    echo Essayez CLONE_FRESH.bat ou contactez support.
    pause
    exit /b 1
)
echo   OK gxclicker.py main.py START.bat ui-web

echo [4/6] Fix conflit ui.py ...
if exist "ui.py" if exist "ui\" (
    if exist "ui.py.bak" del /f "ui.py.bak"
    ren "ui.py" "ui.py.bak"
    echo   ui.py -^> ui.py.bak
)

echo [5/6] Dependances Python ...
"%PY%" -m pip install -r requirements.txt -q

echo [6/6] Node.js C:\src ...
set "XCLICKER_NODE_PATH=C:\src\node.exe"
if exist "nodejs\package.json" (
    pushd nodejs
    if exist "C:\src\node.exe" (
        "C:\src\node.exe" --version
        if not exist node_modules call "C:\src\node.exe" "C:\src\node_modules\npm\bin\npm-cli.js" install 2>nul
        if not exist node_modules call npm install --silent 2>nul
    ) else (
        echo   WARN: C:\src\node.exe absent — UI via port 17840
    )
    popd
)

echo.
"%PY%" CHECK_VERSION.py
echo.
echo ============================================================
echo   REPARATION OK — lancement START.bat ...
echo   PyCharm script: main.py  ou  gxclicker.py
echo ============================================================
echo.
call "%~dp0START.bat"
exit /b %ERRORLEVEL%
