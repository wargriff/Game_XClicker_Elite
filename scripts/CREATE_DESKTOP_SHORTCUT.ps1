$WshShell = New-Object -ComObject WScript.Shell
$Desktop = [Environment]::GetFolderPath("Desktop")
$Exe = Join-Path $PSScriptRoot "..\dist\Game_XClicker_Elite.exe"
$Shortcut = Join-Path $Desktop "Game XClicker Elite.lnk"

if (-not (Test-Path $Exe)) {
    Write-Host "Compilez d'abord: scripts\BUILD_EXE.bat"
    exit 1
}

$Link = $WshShell.CreateShortcut($Shortcut)
$Link.TargetPath = $Exe
$Link.WorkingDirectory = Split-Path $Exe
$Link.IconLocation = (Join-Path $PSScriptRoot "..\assets\brand\favicon.ico")
$Link.Description = "Game XClicker Elite — SOURIS WARGRIFF"
$Link.Save()
Write-Host "Raccourci cree: $Shortcut"
