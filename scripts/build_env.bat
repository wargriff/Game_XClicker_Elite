@echo off
rem Variables partagees : ROOT, BUILD, CMAKE, VCVARS, QT
set "ROOT=%~dp0.."
set "ROOT=%ROOT:~0,-1%"
set "BUILD=%ROOT%\build"

set "CMAKE=cmake"
where cmake >nul 2>&1
if errorlevel 1 set "CMAKE=C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not exist "%CMAKE%" set "CMAKE=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

set "VCVARS="
if exist "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat" (
  set "VCVARS=C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
  set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
  set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
)

set "QT="
if defined QT_DIR if exist "%QT_DIR%\lib\cmake\Qt6\Qt6Config.cmake" set "QT=%QT_DIR%"
if not defined QT (
  for /f "delims=" %%V in ('dir /b /ad "C:\Qt\6.*" 2^>nul') do (
    if exist "C:\Qt\%%V\msvc2022_64\lib\cmake\Qt6\Qt6Config.cmake" (
      set "QT=C:\Qt\%%V\msvc2022_64"
      goto :gmel_qt_ok
    )
  )
)
:gmel_qt_ok
