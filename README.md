# Game_XClicker_Elite

<p align="center">
  <img src="assets/favicon/favicon.svg" width="80" alt="Game_XClicker_Elite Logo">
</p>

<h1 align="center">Game XClicker Elite — Sanctuary Edition</h1>

<p align="center">
  Macro avancée pour souris et clavier orientée gaming.<br>
  Interface pro style iCUE avec thème Diablo 4.<br>
  Optimisée pour des jeux comme <strong>Diablo IV</strong>.
</p>

---

# Aperçu

Game_XClicker_Elite est une application desktop Python avec interface PyQt6 permettant de :

* automatiser les clics souris
* automatiser certaines touches clavier
* gérer plusieurs profils
* contrôler les CPS (Clicks Per Second)
* appliquer des délais personnalisés
* activer/désactiver des macros en temps réel
* utiliser un mode "Game Safe"
* surveiller les performances en direct

L'application utilise une architecture séparée :

* moteur Win32 bas niveau
* interface PyQt6 moderne
* système de profils JSON
* moteur RGB personnalisable

---

# Interface

## Vue principale

<p align="center">
  <img src="assets/6views.png" width="900" alt="Main UI">
</p>

---

## Icône de l'application

<p align="center">
  <img src="assets/icon.png" width="180" alt="Application Icon">
</p>

---

## Preview Gaming

<p align="center">
  <img src="assets/razer_image.jpg" width="800" alt="Gaming Preview">
</p>

---

# Fonctionnalités

## Mouse Macro

* Auto-click gauche/droite
* CPS personnalisable
* délai personnalisable
* activation indépendante par bouton
* gestion temps réel

---

## Keyboard Macro

* support multi-touch
* simulation Win32 native
* dispatch séparé clavier/souris

---

## UI Sanctuary Edition (PyQt6)

* interface pro style Corsair iCUE
* thème Diablo 4 (or, sang, fond gothique)
* grille de tuiles devices / macros
* sidebar profils + navigation
* capteurs CPU / RAM / CPS en temps réel
* API Sidecar REST locale (port 17840)
* onglets HOME, DASHBOARD, DEVICES, MACROS, SETTINGS

---

## Profils

* sauvegarde JSON
* chargement automatique
* profils multiples

---

## Mode Game Safe

Réduit certains comportements agressifs pour améliorer la stabilité dans les jeux.

---

## RGB Engine

Système RGB intégré pour effets visuels et feedback utilisateur.

---

# Technologies utilisées

* Python 3.12+
* PyQt6
* Win32 API
* JSON
* Threading Python

---

# Structure du projet

```text
Game_XClicker_Elite/
│
├── assets/
│   ├── bg/diablo_bg.svg
│   ├── favicon/
│   ├── icons/
│   └── mouse.svg
│
├── core/
│   ├── engine.py
│   ├── models.py
│   └── win32_input.py
│
├── services/
│   ├── engine_proxy.py
│   ├── profile_manager.py
│   └── sidecar_api.py
│
├── ui/
│   ├── sanctuary_window.py
│   ├── pages/
│   ├── widgets/
│   └── styles/diablo_theme.py
│
├── profiles/
│   └── default.json
│
├── tests/
│   └── test_ui.py
│
├── Xmacro_main.py
├── build.spec
├── requirements.txt
└── README.md
```

---

# Installation

## 1. Cloner le projet

```bash
git clone https://github.com/wargriff/Game_XClicker_Elite.git
cd Game_XClicker_Elite
```

---

## 2. Créer un environnement virtuel

### Windows

```bash
python -m venv .venv
```

Activation :

```bash
.venv\Scripts\activate
```

---

## 3. Installer les dépendances

```bash
pip install -r requirements.txt
```

---

# Lancement

**Un seul lanceur** — double-cliquez `START.bat` ou :

```bash
python Xmacro_main.py
```

Le splash Control Center charge automatiquement :
1. Moteur Win32
2. Profils
3. API Sidecar + Mission Control (port 17840)
4. Interface iCUE Sanctuary

Plus besoin de lancer séparément Sidecar, React (5173) ou iCUE.

---

# Lancement (développement)

```bash
python Xmacro_main.py
```

---

## Mission Control Web

Intégré dans l'API Sidecar — **pas de Node.js requis** au runtime.

- URL : `http://127.0.0.1:17840/mission`
- Clic sur tuile **Mission Control** ou **Sidecar API** dans l'UI

Dev optionnel (Node.js dans `C:\src`) :

```bash
cd web/mission-control
npm run dev
```

---

# Configuration

Les profils sont stockés dans :

```text
profiles/default.json
```

Vous pouvez :

* modifier les CPS
* changer les délais
* personnaliser les touches

---

# Compilation EXE

Sur **Windows** uniquement (API Win32).

## Installer PyInstaller

```bash
pip install pyinstaller
```

## Build (recommandé)

```bash
pyinstaller build.spec
```

Le build sera généré dans :

```text
dist/Game_XClicker_Elite.exe
```

## Build rapide

```bash
pyinstaller --onefile --windowed --name Game_XClicker_Elite ^
  --add-data "assets;assets" --add-data "profiles;profiles" Xmacro_main.py
```

---

# Tests

```bash
pip install -r requirements.txt
pytest tests/ -v
```

Sur Windows (avec écran), tous les tests UI s'exécutent. Sur Linux CI, les tests moteur/API passent ; les tests PyQt6 sont ignorés si l'affichage n'est pas disponible.

Tests couverts :
- Moteur macro + proxy
- Profils JSON
- API Sidecar
- Onglets header (HOME, DASHBOARD, DEVICES, **MACROS**, SETTINGS)
- Sidebar (PERFORMANCE, MACRO 1/2, etc.)
- Boutons burst sans récursion
- Attributs `master_combo` / `name_edit`

---

# Roadmap

* [ ] support multi-profils avancé
* [ ] overlay in-game
* [ ] hotkeys configurables
* [ ] thèmes UI
* [ ] statistiques avancées
* [ ] plugin system

---

# Sécurité

Ce projet est fourni à des fins éducatives et expérimentales.

L'utilisation de macros dans certains jeux peut enfreindre leurs conditions d'utilisation.

Utilisez ce logiciel sous votre propre responsabilité.

---

# Licence

MIT License

---

# Auteur

## Wargriff

GitHub :
https://github.com/wargriff

---

# Contributions

Les pull requests et suggestions sont les bienvenues.

## Fork

```bash
git fork
```

## Branch

```bash
git checkout -b feature/awesome-feature
```

## Commit

```bash
git commit -m "Add awesome feature"
```

## Push

```bash
git push origin feature/awesome-feature
```
