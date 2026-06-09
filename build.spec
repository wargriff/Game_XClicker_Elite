# -*- mode: python ; coding: utf-8 -*-
# Build: scripts\BUILD_EXE.bat  →  dist\Game_XClicker_Elite.exe

block_cipher = None

a = Analysis(
    ['launcher/desktop_main.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('assets', 'assets'),
        ('ui-web', 'ui-web'),
        ('profiles', 'profiles'),
        ('nodejs', 'nodejs'),
        ('config', 'config'),
    ],
    hiddenimports=[
        'psutil',
        'webview',
        'clr_loader',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['PyQt6'],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name='Game_XClicker_Elite',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    icon='assets/brand/favicon.ico',
)
