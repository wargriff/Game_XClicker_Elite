@echo off
title RESTAURATION TOUS LES PROGRAMMES
color 0A
echo.
echo  ============================================================
echo    RESTAURATION COMPLETE — Pycharm_Project_v 3.12
echo  ============================================================
echo.
echo  GitHub (auto): Game_XClicker, Diablo_Translator,
echo                 App_Manager_Pro, Pac-Man
echo.
echo  Manuel (PyCharm Local History): ManaCodex, chess_app, game_2048
echo.
echo  NE PAS interrompre...
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0RESTORE_ALL_PROJECTS.ps1"

pause
