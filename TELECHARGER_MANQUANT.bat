@echo off
title Telecharger fichiers manquants
cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0TELECHARGER_MANQUANT.ps1"
if errorlevel 1 pause
exit /b %ERRORLEVEL%
