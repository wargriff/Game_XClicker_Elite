@echo off
setlocal EnableDelayedExpansion
call "%~dp0scripts\build_env.bat"

if not defined VCVARS exit /b 1
if not defined QT exit /b 1
call "%VCVARS%" >nul

call "%ROOT%\scripts\migrate_build_layout.bat" "%BUILD%"
powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%\scripts\download_mouse_assets.ps1" 2>nul

echo === Configure ===
"%CMAKE%" -S "%ROOT%" -B "%BUILD%" -G "Ninja Multi-Config" -DCMAKE_PREFIX_PATH="%QT%" -DGMEL_QT_ROOT="%QT%"
if errorlevel 1 exit /b 1

echo === Release ===
"%CMAKE%" --build "%BUILD%" --config Release
if errorlevel 1 exit /b 1

echo === Debug ===
"%CMAKE%" --build "%BUILD%" --config Debug
if errorlevel 1 exit /b 1

echo.
echo Release : %BUILD%\Release\Game_macro_elite.exe
echo Debug   : %BUILD%\Debug\Game_macro_elite.exe
