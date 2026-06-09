@echo off
title Game XClicker Elite — PyCharm
cd /d "%~dp0"

echo.
echo ============================================================
echo   Game XClicker Elite — Configuration PyCharm
echo ============================================================
echo.
echo Script PyCharm : %~dp0main.py
echo Interface NATIVE PyQt6 iCUE (fenetre Windows — pas web, pas pygame)
echo Mode web optionnel : python main.py --web
echo.
echo 1. Run - Edit Configurations
echo 2. Supprimez les configs run.py / Xmacro_main.py
echo 3. Nouvelle config Python :
echo    Script path : main.py
echo    Working directory : %~dp0
echo.
echo    Import : pycharm\Game_XClicker_Elite.run.xml
echo.
echo 4. Si gxclicker.py introuvable : REPARER.bat
echo.
pause
