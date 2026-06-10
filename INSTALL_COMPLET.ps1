# Installation complete Game XClicker Elite — Visual Studio
# Clic droit > Executer avec PowerShell
# OU dans PowerShell admin :
#   Set-ExecutionPolicy Bypass -Scope Process -Force
#   & "C:\Users\wargriff\visual_studio_project\INSTALL_COMPLET.ps1"

$ErrorActionPreference = "Stop"

$Parent = "C:\Users\wargriff\visual_studio_project"
$Project = Join-Path $Parent "Game_XClicker_Elite"
$Repo = "https://github.com/wargriff/Game_XClicker_Elite.git"
$VenvPy = Join-Path $Parent ".venv\Scripts\python.exe"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  INSTALL COMPLET — Game XClicker Elite" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

New-Item -ItemType Directory -Path $Parent -Force | Out-Null

# --- 1. Git : clone ou mise a jour ---
if (Test-Path (Join-Path $Project ".git")) {
    Write-Host "[1/5] git pull..." -ForegroundColor Yellow
    Set-Location $Project
    $env:GIT_MERGE_AUTOEDIT = "no"
    git remote set-url origin $Repo
    git fetch origin main
    git reset --hard origin/main
} elseif (Test-Path $Project) {
    Write-Host "[1/5] Dossier sans git — sauvegarde + clone propre..." -ForegroundColor Yellow
    $Backup = Join-Path $Parent ("Game_XClicker_Elite_backup_" + (Get-Date -Format "yyyyMMdd_HHmmss"))
    try {
        Rename-Item $Project $Backup -ErrorAction Stop
        Write-Host "      Ancien dossier -> $Backup"
        git clone $Repo $Project
        Set-Location $Project
    } catch {
        Write-Host "      Dossier en cours d'utilisation — clone vers Game_XClicker_Elite_NEW" -ForegroundColor Yellow
        $NewProject = Join-Path $Parent "Game_XClicker_Elite_NEW"
        if (Test-Path $NewProject) { Remove-Item $NewProject -Recurse -Force }
        git clone $Repo $NewProject
        Set-Location $NewProject
        $Project = $NewProject
        Write-Host ""
        Write-Host "  Fermez PyCharm/VS, puis renommez:" -ForegroundColor Green
        Write-Host "    Game_XClicker_Elite     -> Game_XClicker_Elite_OLD"
        Write-Host "    Game_XClicker_Elite_NEW -> Game_XClicker_Elite"
        Write-Host ""
    }
} else {
    Write-Host "[1/5] git clone..." -ForegroundColor Yellow
    git clone $Repo $Project
    Set-Location $Project
}

# --- 2. Python venv ---
Write-Host "[2/5] Python venv..." -ForegroundColor Yellow
if (-not (Test-Path $VenvPy)) {
    $sysPy = $null
    foreach ($c in @("py -3.12", "py -3", "python", "python3")) {
        try {
            $v = Invoke-Expression "$c --version 2>&1"
            if ($LASTEXITCODE -eq 0) { $sysPy = $c; break }
        } catch {}
    }
    if (-not $sysPy) {
        Write-Host "ERREUR: Python introuvable. Installez Python 3.12 depuis python.org" -ForegroundColor Red
        Read-Host "Entree"
        exit 1
    }
    Invoke-Expression "$sysPy -m venv `"$(Join-Path $Parent '.venv')`""
}
& $VenvPy --version

# --- 3. pip ---
Write-Host "[3/5] pip install..." -ForegroundColor Yellow
& $VenvPy -m pip install --upgrade pip -q
& $VenvPy -m pip install -r requirements.txt -q

# --- 4. Nettoyage launcher errone parent ---
$Wrong = Join-Path $Parent "launcher.py"
if (Test-Path $Wrong) {
    Remove-Item $Wrong -Force
    Write-Host "      Supprime launcher.py errone dans $Parent"
}

# --- 5. Build C++ (optionnel) ---
Write-Host "[4/5] Build C++ (si CMake disponible)..." -ForegroundColor Yellow
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmake -and (Test-Path "BUILD_CPP.bat")) {
    cmd /c "BUILD_CPP.bat"
} else {
    Write-Host "      CMake absent — saute build C++. Utilisez OUVRE_MOI.pyw" -ForegroundColor DarkYellow
}

Write-Host "[5/5] Verification..." -ForegroundColor Yellow
& $VenvPy CHECK_VERSION.py

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  INSTALLATION TERMINEE" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "  Dossier : $Project"
Write-Host "  C++     : GameXClicker.exe  (si build OK)"
Write-Host "  Python  : double-clic OUVRE_MOI.pyw"
Write-Host ""
Write-Host "Lancement Python maintenant..."
& $VenvPy OUVRE_MOI.py
