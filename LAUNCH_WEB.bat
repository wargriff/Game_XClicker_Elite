@echo off
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"
set GX_BROWSER=1
"%PY%" GameXClicker.py --web
exit /b %ERRORLEVEL%
