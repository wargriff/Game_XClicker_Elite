@echo off
title Apres clone NEW - Game XClicker Elite
set "PARENT=C:\Users\wargriff\visual_studio_project"
set "NEW=%PARENT%\Game_XClicker_Elite_NEW"
cd /d "%NEW%"
if not exist "GameXClicker.py" (
    echo ERREUR: clone pas fini ou dossier absent: %NEW%
    pause
    exit /b 1
)

set "PY=%PARENT%\.venv\Scripts\python.exe"
if not exist "%PY%" (
    echo Creation venv...
    py -3.12 -m venv "%PARENT%\.venv"
)
if not exist "%PY%" set "PY=python"

echo pip install...
"%PY%" -m pip install --upgrade pip -q
"%PY%" -m pip install -r requirements.txt -q

echo.
echo Lancement Control Panel...
"%PY%" OUVRE_MOI.py

echo.
echo ========================================
echo Si tout marche:
echo   1) Fermez cette fenetre et PyCharm/VS
echo   2) Renommez Game_XClicker_Elite en _OLD
echo   3) Renommez Game_XClicker_Elite_NEW en Game_XClicker_Elite
echo ========================================
pause
