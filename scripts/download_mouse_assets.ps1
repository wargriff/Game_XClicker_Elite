$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$dest = Join-Path $root "assets\mouse"
New-Item -ItemType Directory -Force -Path $dest | Out-Null

$files = @{
    "elite-m40-top.jpg"  = "https://media.ldlc.com/r80/ld/products/00/06/19/59/LD0006195932.jpg"
    "elite-m40-side.jpg" = "https://media.ldlc.com/r1600/ld/products/00/06/19/59/LD0006195933.jpg"
}

$placeholder = "/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/wAARCAABAAEDASIAAhEBAxEB/8QAFQABAQAAAAAAAAAAAAAAAAAAAAn/xAAUEAEAAAAAAAAAAAAAAAAAAAAA/8QAFQEBAQAAAAAAAAAAAAAAAAAAAAX/xAAUEQEAAAAAAAAAAAAAAAAAAAAA/9oADAMBAAIRAxEAPwCwAA8A/9k="

function Write-Placeholder($path) {
    [IO.File]::WriteAllBytes($path, [Convert]::FromBase64String($placeholder))
}

foreach ($entry in $files.GetEnumerator()) {
    $out = Join-Path $dest $entry.Key
    try {
        Write-Host "Telechargement $($entry.Key)..."
        Invoke-WebRequest -Uri $entry.Value -OutFile $out -UseBasicParsing -TimeoutSec 30
        if ((Get-Item $out).Length -lt 1024) { throw "Fichier trop petit" }
        Write-Host "OK $($entry.Key)"
    }
    catch {
        Write-Warning "Echec $($entry.Key) : $_ - placeholder utilise"
        Write-Placeholder $out
    }
}

Write-Host "Assets souris pret dans $dest"
