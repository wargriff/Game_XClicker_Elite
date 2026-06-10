@echo off
cd /d "%~dp0"
set "EXE=%USERPROFILE%\Desktop\Game XClicker Elite\Game XClicker Elite.exe"
if not exist "%EXE%" set "EXE=%~dp0dist\Game XClicker Elite\Game XClicker Elite.exe"
if not exist "%EXE%" (
    echo .exe absent — lancez BUILD.bat depuis Mission Control
    pause
    exit /b 1
)
start "" "%EXE%"
exit /b 0
