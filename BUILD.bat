@echo off
title Game XClicker Elite — BUILD .exe
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"

echo.
echo ========================================
echo   BUILD — Game XClicker Elite.exe
echo   Interface native PyQt6 (Windows)
echo ========================================
echo Python: %PY%
echo.

if not exist "build.spec" (
    echo ERREUR: build.spec absent
    pause
    exit /b 1
)

echo [1/3] Dependances + PyInstaller...
"%PY%" -m pip install -r requirements.txt pyinstaller -q
if errorlevel 1 ( echo pip echoue & pause & exit /b 1 )

echo [2/3] Icone...
"%PY%" scripts\generate_icon.py 2>nul

echo [3/3] Compilation (2-5 min)...
"%PY%" -m PyInstaller build.spec --noconfirm
if errorlevel 1 (
    echo.
    echo BUILD ECHEC — voir erreurs ci-dessus
    pause
    exit /b 1
)

powershell -Command "Get-ChildItem 'dist\Game XClicker Elite' -Recurse -ErrorAction SilentlyContinue | Unblock-File" 2>nul

echo.
echo ========================================
echo   OK — .exe cree:
echo   dist\Game XClicker Elite\Game XClicker Elite.exe
echo.
echo   Test: double-clic LAUNCH_EXE.bat
echo ========================================
pause
exit /b 0
