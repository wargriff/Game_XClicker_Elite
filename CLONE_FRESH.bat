@echo off
title CLONE FRESH — Game XClicker Elite
set "PARENT=C:\Users\wargriff\Pycharm_Project_v 3.12"
set "TARGET=%PARENT%\Game_XClicker_Elite"

echo.
echo ============================================================
echo   Installation propre Game XClicker Elite
echo   Cible: %TARGET%
echo ============================================================
echo.

if exist "%TARGET%\.git" (
    echo Le dossier existe deja avec git.
    echo Ouvrez-le et lancez REPARER.bat
    pause
    exit /b 1
)

if exist "%TARGET%" (
    echo Le dossier existe sans git — renommez-le d'abord.
    pause
    exit /b 1
)

echo Clone branche cursor/icue-web-launcher-9626 ...
git clone -b cursor/icue-web-launcher-9626 https://github.com/wargriff/Game_XClicker_Elite.git "%TARGET%"
if errorlevel 1 (
    echo Clone echoue.
    pause
    exit /b 1
)

cd /d "%TARGET%"

set "PY=%PARENT%\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"

echo Installation Python ...
"%PY%" -m pip install -r requirements.txt

echo.
echo ============================================================
echo   OK — Ouvrez PyCharm sur:
echo   %TARGET%
echo.
echo   Script PyCharm: main.py
echo   Lancement: double-clic START.bat
echo ============================================================
echo.
call REPARER.bat
pause
