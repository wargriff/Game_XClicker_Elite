# Telecharge les fichiers essentiels depuis GitHub (sans git)
# powershell -ExecutionPolicy Bypass -File TELECHARGER_MANQUANT.ps1

$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
if (-not $Root) { $Root = (Get-Location).Path }
Set-Location $Root

$Base = "https://raw.githubusercontent.com/wargriff/Game_XClicker_Elite/main"
$Files = @(
    "launcher.py",
    "OUVRE_MOI.py",
    "OUVRE_MOI.pyw",
    "GameXClicker.py",
    "rgb_engine.py",
    "core/rgb_engine.py",
    "core/__init__.py",
    "native_app.py",
    "scripts/__init__.py",
    "scripts/setup.py",
    "ui/control_panel.py",
    "ui/mission_control.py",
    "ui/sanctuary_window.py",
    "ui/widgets/mural_panel.py",
    "ui/tabs/rgb_tab.py",
    "ui/pages/devices_page.py",
    "config/runtime.py",
    "services/profile_manager.py"
)

Write-Host "Telechargement dans: $Root" -ForegroundColor Cyan
foreach ($f in $Files) {
    $url = "$Base/$f"
    $dest = Join-Path $Root ($f -replace "/", "\")
    $dir = Split-Path $dest -Parent
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
    Write-Host "  $f"
    Invoke-WebRequest -Uri $url -OutFile $dest -UseBasicParsing
}

$Parent = Split-Path $Root -Parent
$VenvPy = Join-Path $Parent ".venv\Scripts\python.exe"
if (-not (Test-Path $VenvPy)) {
    $VenvPy = Join-Path $Root ".venv\Scripts\python.exe"
}
if (-not (Test-Path $VenvPy)) {
    Write-Host "Python venv absent. Creez-le:" -ForegroundColor Yellow
    Write-Host "  py -3.12 -m venv $Parent\.venv"
} else {
    & $VenvPy -m pip install -r requirements.txt -q
    Write-Host "Lancement..." -ForegroundColor Green
    & $VenvPy OUVRE_MOI.py
}
