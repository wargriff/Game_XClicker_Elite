@echo off
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"
"%PY%" GameXClicker.py --native
exit /b %ERRORLEVEL%
