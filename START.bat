@echo off
title Game XClicker Elite — Menu
cd /d "%~dp0"

echo.
echo  ========================================
echo    Game XClicker Elite — SOURIS WARGRIFF
echo  ========================================
echo.
echo    Choisissez un lanceur:
echo.
echo    1  LAUNCH_NATIVE.bat   Interface controle PyQt6 (.py)
echo    2  LAUNCH_WEB.bat        Interface web preview (.py)
echo    3  BUILD.bat             Creer le .exe
echo    4  LAUNCH_EXE.bat        Lancer le .exe (apres build)
echo    5  REPARER.bat           Sync GitHub + deps
echo.
echo    PyCharm: Run main.py ^(native^) ou main.py --web
echo.
set /p CHOICE="Votre choix (1-5): "

if "%CHOICE%"=="1" call "%~dp0LAUNCH_NATIVE.bat" & exit /b %ERRORLEVEL%
if "%CHOICE%"=="2" call "%~dp0LAUNCH_WEB.bat" & exit /b %ERRORLEVEL%
if "%CHOICE%"=="3" call "%~dp0BUILD.bat" & exit /b %ERRORLEVEL%
if "%CHOICE%"=="4" call "%~dp0LAUNCH_EXE.bat" & exit /b %ERRORLEVEL%
if "%CHOICE%"=="5" call "%~dp0REPARER.bat" & exit /b %ERRORLEVEL%

echo Choix invalide.
pause
exit /b 1
