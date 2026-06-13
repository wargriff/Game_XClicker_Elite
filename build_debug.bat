@echo off
setlocal
set "GMEL_CONFIG=Debug"
set "GMEL_EXE=%~dp0build\Debug\Game_macro_elite.exe"
call "%~dp0scripts\build_core.bat"
if errorlevel 1 exit /b 1
echo Lancement Debug...
start "" "%GMEL_EXE%"
