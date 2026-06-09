@echo off
title Fix Git Pull — Game XClicker Elite
cd /d "%~dp0"
echo Redirection vers REPARER.bat ...
call "%~dp0REPARER.bat"
exit /b %ERRORLEVEL%
