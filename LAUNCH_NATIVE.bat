@echo off
title Game XClicker Elite — Interface NATIVE (.py)
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"

echo.
echo ========================================
echo   LAUNCH NATIVE — PyQt6 (.py)
echo   Fenetre Windows style iCUE
echo ========================================
echo Python: %PY%
echo.

if not exist "main.py" (
    echo ERREUR: main.py absent — lancez REPARER.bat
    pause
    exit /b 1
)

"%PY%" --version 2>nul
if errorlevel 1 ( echo ERREUR Python & pause & exit /b 1 )

echo Installation dependances...
"%PY%" -m pip install -r requirements.txt -q 2>nul

echo.
echo Lancement main.py (interface controle native)...
echo.
"%PY%" main.py
if errorlevel 1 pause
exit /b %ERRORLEVEL%
