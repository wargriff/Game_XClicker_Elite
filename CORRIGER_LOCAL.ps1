# Force la mise a jour des fichiers lanceur depuis GitHub
# Clic droit > Executer avec PowerShell  (dans le dossier Game_XClicker_Elite)

$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
Set-Location $Root

$Base = "https://raw.githubusercontent.com/wargriff/Game_XClicker_Elite/main"
$Files = @(
    "launcher.py",
    "OUVRE_MOI.py",
    "OUVRE_MOI.pyw",
    "GameXClicker.py",
    "scripts/__init__.py",
    "scripts/setup.py",
    "CORRIGER_LOCAL.ps1"
)

if (-not (Test-Path (Join-Path $Root "GameXClicker.py"))) {
    Write-Host "ERREUR: lancez ce script DANS le dossier Game_XClicker_Elite" -ForegroundColor Red
    Write-Host "Dossier actuel: $Root"
    Read-Host "Entree"
    exit 1
}

# Supprime launcher.py errone dans le dossier parent (cause ModuleNotFoundError)
$WrongLauncher = Join-Path (Split-Path $Root -Parent) "launcher.py"
if (Test-Path $WrongLauncher) {
    Write-Host "Suppression launcher.py errone: $WrongLauncher" -ForegroundColor Yellow
    Remove-Item $WrongLauncher -Force
}

Write-Host "Correction des lanceurs dans: $Root" -ForegroundColor Cyan

foreach ($f in $Files) {
    $url = "$Base/$f"
    $dest = Join-Path $Root ($f -replace "/", "\")
    $dir = Split-Path $dest -Parent
    if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }
    Write-Host "  -> $f"
    Invoke-WebRequest -Uri $url -OutFile $dest -UseBasicParsing
}

$Py = Join-Path (Split-Path $Root -Parent) ".venv\Scripts\python.exe"
if (-not (Test-Path $Py)) { $Py = "python" }

Write-Host ""
Write-Host "Verification OUVRE_MOI.py (5 premieres lignes):" -ForegroundColor Yellow
Get-Content (Join-Path $Root "OUVRE_MOI.py") -TotalCount 8

Write-Host ""
Write-Host "pip install..." -ForegroundColor Cyan
& $Py -m pip install -r requirements.txt -q

Write-Host "Lancement..." -ForegroundColor Green
& $Py (Join-Path $Root "OUVRE_MOI.py")
