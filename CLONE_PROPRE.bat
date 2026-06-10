@echo off
title Clone propre Game XClicker Elite
set "PARENT=C:\Users\wargriff\visual_studio_project"
set "NEW=%PARENT%\Game_XClicker_Elite_NEW"

echo.
echo Clone vers: %NEW%
echo Fermez PyCharm et Visual Studio avant de continuer.
echo.
pause

if not exist "%PARENT%" mkdir "%PARENT%"
cd /d "%PARENT%"

if exist "%NEW%" (
    echo Suppression ancien NEW...
    rmdir /s /q "%NEW%"
)

git clone https://github.com/wargriff/Game_XClicker_Elite.git "%NEW%"
if errorlevel 1 (
    echo ERREUR git clone. Installez Git: https://git-scm.com
    pause
    exit /b 1
)

cd /d "%NEW%"

set "PY=%PARENT%\.venv\Scripts\python.exe"
if not exist "%PY%" (
    echo Creation venv...
    py -3.12 -m venv "%PARENT%\.venv"
)
if not exist "%PY%" set "PY=python"

"%PY%" -m pip install -r requirements.txt -q
"%PY%" OUVRE_MOI.py

echo.
echo Si OK: renommez Game_XClicker_Elite en _OLD puis NEW en Game_XClicker_Elite
pause
