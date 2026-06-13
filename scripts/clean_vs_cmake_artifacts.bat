@echo off
setlocal
set "BUILD=%~1"
if not defined BUILD set "BUILD=%~dp0..\build"

echo Nettoyage : Visual Studio, ZERO_CHECK, ALL_BUILD, x64...

del /f /q "%BUILD%\CMakeCache.txt" 2>nul
del /f /q "%BUILD%\*.slnx" 2>nul
del /f /q "%BUILD%\*.vcxproj" 2>nul
del /f /q "%BUILD%\*.vcxproj.filters" 2>nul
del /f /q "%BUILD%\*.vcxproj.user" 2>nul
del /f /q "%BUILD%\cmake_install.cmake" 2>nul
del /f /q "%BUILD%\build.ninja" 2>nul
del /f /q "%BUILD%\build-*.ninja" 2>nul

for /d %%D in ("%BUILD%\*.dir") do rmdir /s /q "%%D" 2>nul
for /d %%D in ("%BUILD%\*_autogen") do rmdir /s /q "%%D" 2>nul
if exist "%BUILD%\CMakeFiles" rmdir /s /q "%BUILD%\CMakeFiles" 2>nul
if exist "%BUILD%\x64" rmdir /s /q "%BUILD%\x64" 2>nul

echo OK - structure build/Debug et build/Release uniquement.
