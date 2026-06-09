# Icônes SOURIS WARGRIFF (copiez vos PNG depuis PyCharm ici)

Placez vos fichiers dans ce dossier :

- `favicon.ico` — icône du .exe et raccourci bureau
- `favicon-96x96.png`
- `web-app-manifest-192x192.png`
- `web-app-manifest-512x512.png`
- `apple-touch-icon.png`

Génération automatique du .ico :

```powershell
python scripts/generate_icon.py
```

Si seul `favicon.svg` est présent, le script crée un `favicon.ico` basique.
