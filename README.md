# Game_XClicker_Elite

<p align="center">
  <img src="assets/icon.png" width="140" alt="Game_XClicker_Elite Logo">
</p>

<h1 align="center">Game_XClicker_Elite</h1>

<p align="center">
  Macro avancée pour souris et clavier orientée gaming.<br>
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

## UI PyQt6

* interface moderne
* affichage CPS réel
* boutons interactifs
* état live des macros

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
│   ├── favicon/
│   ├── icon.png
│   ├── mouse.png
│   ├── razer_image.jpg
│   └── 6views.png
│
├── profiles/
│   └── default.json
│
├── tests/
│   └── test_ui.py
│
├── Xmacro_main.py
├── ui.py
├── config_ui.py
├── engine_win32.py
├── rgb_engine.py
├── .gitignore
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
pip install PyQt6
```

---

# Lancement

```bash
python Xmacro_main.py
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

## Installer PyInstaller

```bash
pip install pyinstaller
```

## Build

```bash
pyinstaller --onefile --windowed Xmacro_main.py
```

Le build sera généré dans :

```text
dist/
```

---

# Tests

```bash
pytest
```

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
