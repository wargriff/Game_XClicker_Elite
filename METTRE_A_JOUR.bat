@echo off
title Mise a jour — Game XClicker Elite
cd /d "%~dp0"
call "%~dp0scripts\_env.bat"
"%PY%" METTRE_A_JOUR.py
if errorlevel 1 pause
exit /b %ERRORLEVEL%
