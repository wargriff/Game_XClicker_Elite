@echo off
setlocal
set "GMEL_CONFIG=Release"
set "GMEL_EXE=%~dp0build\Release\Game_macro_elite.exe"
call "%~dp0scripts\build_core.bat"
if errorlevel 1 exit /b 1
echo Lancement Release...
start "" "%GMEL_EXE%"
