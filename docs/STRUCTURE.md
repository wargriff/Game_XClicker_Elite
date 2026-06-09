# Structure du projet v3.0

```
Game_XClicker_Elite/
├── launcher/              # Launcher bureau (.exe)
│   ├── desktop_main.py    # Fenêtre native + interface JS
│   └── LAUNCH_DESKTOP.bat
├── ui-web/                # Interface iCUE (HTML/CSS/JS)
│   ├── index.html
│   ├── css/
│   └── js/
├── assets/
│   ├── brand/             # Icône .exe, favicon SOURIS WARGRIFF
│   ├── ui/icons/          # Icônes navigation
│   ├── ui/backgrounds/
│   └── devices/
├── core/                  # Moteur Win32 (Python)
├── services/              # API Sidecar, Node bridge, profils
├── nodejs/                # Serveur UI (port 5173)
├── profiles/              # Profils JSON
└── scripts/
    ├── BUILD_EXE.bat      # Compile Game_XClicker_Elite.exe
    └── CREATE_DESKTOP_SHORTCUT.ps1
```

## Lancement rapide (Windows)

```powershell
START.bat                    # Interface JS iCUE (recommandé)
launcher\LAUNCH_DESKTOP.bat  # Idem
START.bat → choix 2          # PyQt legacy
```

## .exe bureau avec icône SOURIS WARGRIFF

1. Copiez vos PNG dans `assets/brand/` (favicon-96x96.png, etc.)
2. `python scripts/generate_icon.py`
3. `scripts\BUILD_EXE.bat`
4. `scripts\CREATE_DESKTOP_SHORTCUT.ps1`

## 6 macros (sidebar sous LIGHTING CHANNEL 2)

| Sidebar | Touche |
|---------|--------|
| MACRO 1 — Clic gauche | left |
| MACRO 2 — Clic droit | right |
| MACRO 3 — Touche 1 | 1 |
| MACRO 4 — Touche 2 | 2 |
| MACRO 5 — Touche 3 | 3 |
| MACRO 6 — Touche 4 | 4 |
