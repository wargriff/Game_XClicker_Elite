@echo off
setlocal EnableDelayedExpansion
call "%~dp0build_env.bat"

if not defined VCVARS (
  echo [ERREUR] Visual Studio C++ introuvable.
  exit /b 1
)
if not defined QT (
  echo [ERREUR] Qt6 introuvable. Definissez QT_DIR.
  exit /b 1
)

call "%VCVARS%" >nul
echo Qt6: %QT%

if not exist "%BUILD%" mkdir "%BUILD%"

if not exist "%BUILD%\CMakeCache.txt" goto :configure
findstr /C:"CMAKE_GENERATOR:INTERNAL=Ninja Multi-Config" "%BUILD%\CMakeCache.txt" >nul 2>&1
if errorlevel 1 goto :configure
goto :build

:configure
call "%~dp0scripts\migrate_build_layout.bat" "%BUILD%"
powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%\scripts\download_mouse_assets.ps1" 2>nul
echo Configuration CMake...
"%CMAKE%" -S "%ROOT%" -B "%BUILD%" -G "Ninja Multi-Config" -DCMAKE_PREFIX_PATH="%QT%" -DGMEL_QT_ROOT="%QT%"
if errorlevel 1 exit /b 1

:build
echo Compilation %GMEL_CONFIG%...
"%CMAKE%" --build "%BUILD%" --config %GMEL_CONFIG%
if errorlevel 1 (
  echo [ERREUR] Compilation %GMEL_CONFIG% echouee.
  exit /b 1
)

if not exist "%GMEL_EXE%" (
  echo [ERREUR] Executable introuvable: %GMEL_EXE%
  exit /b 1
)

echo OK: %GMEL_EXE%
exit /b 0
