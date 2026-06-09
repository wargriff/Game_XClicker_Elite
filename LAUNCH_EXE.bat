@echo off
title Game XClicker Elite — Lancer .exe
cd /d "%~dp0"

set "EXE=%~dp0dist\Game XClicker Elite\Game XClicker Elite.exe"

echo.
if not exist "%EXE%" (
    echo ========================================
    echo   .exe introuvable
    echo ========================================
    echo.
    echo Lancez d'abord BUILD.bat pour creer:
    echo   dist\Game XClicker Elite\Game XClicker Elite.exe
    echo.
    echo Ou preview sans build:
    echo   LAUNCH_NATIVE.bat  ^(PyQt6 .py^)
    echo   LAUNCH_WEB.bat     ^(interface web^)
    echo.
    pause
    exit /b 1
)

echo Lancement .exe...
start "" "%EXE%"
exit /b 0
