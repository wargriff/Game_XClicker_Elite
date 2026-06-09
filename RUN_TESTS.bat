@echo off
cd /d "%~dp0"
pip install -r requirements.txt -q
pytest tests/ -v
pause
