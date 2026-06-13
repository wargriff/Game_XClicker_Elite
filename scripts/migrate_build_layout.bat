@echo off
setlocal
set "BUILD=%~1"
if not defined BUILD set "BUILD=%~dp0..\build"

echo Migration build\x64 vers build\Debug et build\Release...

if exist "%BUILD%\x64\Debug" (
  if not exist "%BUILD%\Debug" mkdir "%BUILD%\Debug"
  xcopy /E /Y /I "%BUILD%\x64\Debug\*" "%BUILD%\Debug\" >nul 2>&1
)

if exist "%BUILD%\x64\Release" (
  if not exist "%BUILD%\Release" mkdir "%BUILD%\Release"
  xcopy /E /Y /I "%BUILD%\x64\Release\*" "%BUILD%\Release\" >nul 2>&1
)

if exist "%BUILD%\x64" rmdir /s /q "%BUILD%\x64"

if exist "%BUILD%\Game_macro_elite.exe" (
  if not exist "%BUILD%\Release" mkdir "%BUILD%\Release"
  move /Y "%BUILD%\Game_macro_elite.exe" "%BUILD%\Release\" >nul 2>&1
)

echo Suppression ZERO_CHECK / ALL_BUILD...
del /f /q "%BUILD%\*.slnx" 2>nul
del /f /q "%BUILD%\ZERO_CHECK.vcxproj" 2>nul
del /f /q "%BUILD%\ALL_BUILD.vcxproj" 2>nul
del /f /q "%BUILD%\ZERO_CHECK.vcxproj.filters" 2>nul
del /f /q "%BUILD%\ALL_BUILD.vcxproj.filters" 2>nul
del /f /q "%BUILD%\ZERO_CHECK.vcxproj.user" 2>nul
del /f /q "%BUILD%\ALL_BUILD.vcxproj.user" 2>nul

echo Migration terminee.
