@echo off
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"
"%PY%" GameXClicker.py --build --desktop
pause
exit /b %ERRORLEVEL%
