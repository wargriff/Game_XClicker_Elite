# =============================================================================
# RESTAURATION COMPLETE — tous les projets Pycharm_Project_v 3.12
# Double-clic ou: powershell -ExecutionPolicy Bypass -File RESTORE_ALL_PROJECTS.ps1
# =============================================================================

$Root = "C:\Users\wargriff\Pycharm_Project_v 3.12"
$LogFile = Join-Path $Root "_restore_log_$(Get-Date -Format 'yyyyMMdd_HHmmss').txt"

function Log($msg, [string]$Color = "White") {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $msg"
    Write-Host $line -ForegroundColor $Color
    New-Item -ItemType Directory -Force -Path $Root | Out-Null
    Add-Content -Path $LogFile -Value $line -ErrorAction SilentlyContinue
}

if (-not (Test-Path $Root)) {
    Write-Host "Creation du dossier $Root" -ForegroundColor Yellow
    New-Item -ItemType Directory -Force -Path $Root | Out-Null
}

Set-Location $Root
Log "=== RESTAURATION COMPLETE ===" "Cyan"
Log "Dossier: $Root"
Log "Log: $LogFile"

# -----------------------------------------------------------------------------
# DEPOTS GITHUB wargriff (recuperables par clone)
# -----------------------------------------------------------------------------
$GitHubRepos = @(
    @{
        Name   = "Game_XClicker_Elite_Sanctuary"
        Url    = "https://github.com/wargriff/Game_XClicker_Elite.git"
        Branch = "cursor/sanctuary-diablo-ui-9626"
        Post   = {
            param($dir)
            if (Test-Path "$dir\requirements.txt") {
                pip install -r "$dir\requirements.txt" 2>&1 | Out-Null
            }
            if (Test-Path "$dir\nodejs\package.json") {
                Push-Location "$dir\nodejs"; npm install 2>&1 | Out-Null; Pop-Location
            }
        }
    },
    @{
        Name   = "Diablo_Translator"
        Url    = "https://github.com/wargriff/diablo_translator.git"
        Branch = "main"
        Post   = {
            param($dir)
            if (Test-Path "$dir\requirements.txt") {
                pip install -r "$dir\requirements.txt" 2>&1 | Out-Null
            }
        }
    },
    @{
        Name   = "App_Manager_Pro"
        Url    = "https://github.com/wargriff/App_Manager_Pro.git"
        Branch = "main"
        Post   = { param($dir) }
    },
    @{
        Name   = "Pac-Man"
        Url    = "https://github.com/wargriff/Pac-Man.git"
        Branch = "main"
        Post   = { param($dir) }
    }
)

Log ""
Log "=== ETAPE 1: Clone GitHub (4 projets wargriff) ===" "Cyan"

foreach ($repo in $GitHubRepos) {
    $target = Join-Path $Root $repo.Name
    Log "Projet: $($repo.Name)"

    if (Test-Path (Join-Path $target ".git")) {
        Log "  Existe — git pull..."
        Push-Location $target
        git fetch origin 2>&1 | ForEach-Object { Log "  $_" }
        git checkout $repo.Branch 2>&1 | Out-Null
        git reset --hard "origin/$($repo.Branch)" 2>&1 | ForEach-Object { Log "  $_" }
        if (-not $?) { git reset --hard HEAD 2>&1 | Out-Null }
        Pop-Location
    } else {
        Log "  Clone $($repo.Url) ..."
        if (Test-Path $target) {
            Log "  Dossier sans .git — suppression et re-clone" "Yellow"
            Remove-Item -Recurse -Force $target -ErrorAction SilentlyContinue
        }
        git clone -b $repo.Branch $repo.Url $target 2>&1 | ForEach-Object { Log "  $_" }
        if (-not $?) {
            Log "  Branche $($repo.Branch) absente — clone main/master"
            git clone $repo.Url $target 2>&1 | ForEach-Object { Log "  $_" }
        }
    }

    if (Test-Path $target) {
        & $repo.Post $target
        Log "  OK" "Green"
    } else {
        Log "  ECHEC clone" "Red"
    }
}

# -----------------------------------------------------------------------------
# ETAPE 2 — Restaurer Game_XClicker_Elite original si .git reste
# -----------------------------------------------------------------------------
Log ""
Log "=== ETAPE 2: Game_XClicker_Elite (ancien dossier) ===" "Cyan"

$GxOld = Join-Path $Root "Game_XClicker_Elite"
if (Test-Path (Join-Path $GxOld ".git")) {
    Push-Location $GxOld
    git remote set-url origin https://github.com/wargriff/Game_XClicker_Elite.git 2>$null
    git fetch origin 2>&1 | ForEach-Object { Log $_ }
    git reset --hard HEAD 2>&1 | ForEach-Object { Log $_ }
    git checkout -B cursor/sanctuary-diablo-ui-9626 origin/cursor/sanctuary-diablo-ui-9626 2>&1 |
        ForEach-Object { Log $_ }
    Pop-Location
    Log "Game_XClicker_Elite restaure depuis Git" "Green"
} else {
    Log "Pas de .git dans Game_XClicker_Elite — utilise Game_XClicker_Elite_Sanctuary" "Yellow"
}

# -----------------------------------------------------------------------------
# ETAPE 3 — Autres depots .git restants (scan profond)
# -----------------------------------------------------------------------------
Log ""
Log "=== ETAPE 3: Autres depots Git locaux ===" "Cyan"

$seen = @{}
Get-ChildItem -Path $Root -Directory -Recurse -Filter ".git" -ErrorAction SilentlyContinue |
    ForEach-Object {
        $repoDir = $_.Parent.FullName
        if ($repoDir -match "node_modules|\.venv|venv|site-packages|_restore_tools") { return }
        if ($seen[$repoDir]) { return }
        $seen[$repoDir] = $true

        Log "Depot: $repoDir"
        Push-Location $repoDir
        git reset --hard HEAD 2>&1 | ForEach-Object { Log "  $_" }
        $remote = git remote get-url origin 2>$null
        if ($remote) {
            git fetch origin 2>&1 | Out-Null
            $b = git rev-parse --abbrev-ref HEAD 2>$null
            git pull origin $b 2>&1 | ForEach-Object { Log "  $_" }
        }
        Pop-Location
    }

# -----------------------------------------------------------------------------
# ETAPE 4 — Projets SANS GitHub (ManaCodex, chess_app, game_2048...)
# -----------------------------------------------------------------------------
Log ""
Log "=== ETAPE 4: Projets locaux SANS GitHub ===" "Yellow"
Log "Ces projets ne sont PAS sur github.com/wargriff :"
Log "  - ManaCodex"
Log "  - chess_app"
Log "  - game_2048"
Log ""
Log "Recuperation possible UNIQUEMENT via:"
Log "  A) PyCharm > clic droit dossier > Local History > Show History"
Log "  B) Dossier JetBrains:"
Log "     C:\Users\wargriff\AppData\Local\JetBrains\PyCharm*\LocalHistory"
Log "  C) Windows File Recovery (winfr) ou Recuva"

$LocalOnly = @("ManaCodex", "chess_app", "game_2048")
foreach ($name in $LocalOnly) {
    $p = Join-Path $Root $name
    if (-not (Test-Path $p)) {
        New-Item -ItemType Directory -Force -Path $p | Out-Null
        Set-Content -Path (Join-Path $p "LISEZMOI_RECUPERATION.txt") -Value @"
Projet supprime par 'git clean -fd' depuis le dossier parent.

RECUPERATION:
1. PyCharm > Local History sur ce dossier
2. JetBrains LocalHistory (voir RESTORE_ALL_PROJECTS.ps1)
3. winfr / Recuva

NE PAS lancer git clean depuis Pycharm_Project_v 3.12 !
"@
        Log "  Cree placeholder: $name (a restaurer via PyCharm Local History)" "Yellow"
    }
}

# -----------------------------------------------------------------------------
# ETAPE 5 — .venv parent
# -----------------------------------------------------------------------------
Log ""
Log "=== ETAPE 5: Environnement Python .venv ===" "Cyan"

$Venv = Join-Path $Root ".venv"
if (-not (Test-Path "$Venv\Scripts\python.exe")) {
    Log "Creation .venv..."
    python -m venv $Venv
    & "$Venv\Scripts\python.exe" -m pip install --upgrade pip 2>&1 | Out-Null
    Log ".venv cree" "Green"
} else {
    Log ".venv deja present" "Green"
}

# -----------------------------------------------------------------------------
# RESUME
# -----------------------------------------------------------------------------
Log ""
Log "=== TERMINE ===" "Green"
Log ""
Log "Projets restaures depuis GitHub:" "Green"
Log "  Game_XClicker_Elite_Sanctuary  -> python Xmacro_main.py"
Log "  Diablo_Translator              -> github.com/wargriff/diablo_translator"
Log "  App_Manager_Pro"
Log "  Pac-Man"
Log ""
Log "A restaurer manuellement (Local History PyCharm):" "Yellow"
Log "  ManaCodex, chess_app, game_2048"
Log ""
Log "PyCharm — ouvrir:" "Cyan"
Log "  $Root\Game_XClicker_Elite_Sanctuary"
Log "  Script: Xmacro_main.py"
Log "  Env: PYTHONSTARTUP=$Root\Game_XClicker_Elite_Sanctuary\utils\autopatch.py"
Log ""
Log "INTERDIT: git clean -fd depuis $Root" "Red"

Write-Host ""
Write-Host "Log: $LogFile" -ForegroundColor Cyan
Read-Host "Appuie Entree pour fermer"
