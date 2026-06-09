@echo off
REM Variables communes — inclus par les launchers
cd /d "%~dp0"

set "XCLICKER_NODE_PATH=C:\src\node.exe"
if exist "C:\src\node.exe" set "NODE=C:\src\node.exe"
if not defined NODE if exist "C:\src\node\node.exe" set "NODE=C:\src\node\node.exe"
if not defined NODE set "NODE=node"

set "PY=%~dp0..\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=%~dp0venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"

if exist "ui.py" if exist "ui\" (
    if exist "ui.py.bak" del /f "ui.py.bak" 2>nul
    ren "ui.py" "ui.py.bak" 2>nul
)

"%PY%" -c "import sys; sys.path.insert(0,r'%~dp0.'); from utils.bootstrap import ensure_project_ready; ensure_project_ready()" 2>nul
