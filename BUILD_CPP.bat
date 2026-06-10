@echo off
title Build C++ Control Panel — Game XClicker Elite
cd /d "%~dp0"

if not exist "cpp\CMakeLists.txt" (
    echo ERREUR: dossier cpp\ absent
    pause
    exit /b 1
)

where cmake >nul 2>&1
if errorlevel 1 (
    echo.
    echo CMake introuvable. Installez-le ou ouvrez cpp\ dans Visual Studio 2022
    echo   Fichier ^> Ouvrir ^> CMake... ^> cpp\CMakeLists.txt
    echo.
    pause
    exit /b 1
)

cmake -S cpp -B cpp\build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo Essayez: cmake -S cpp -B cpp\build
    pause
    exit /b 1
)

cmake --build cpp\build --config Release
if errorlevel 1 pause
echo.
echo Binaires:
echo   GameXClicker.exe    Win32 Control Panel
echo   GameXClickerQt.exe  Qt Control Panel (si Qt6 installe)
exit /b %ERRORLEVEL%
