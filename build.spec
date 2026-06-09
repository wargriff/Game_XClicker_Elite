# -*- mode: python ; coding: utf-8 -*-

block_cipher = None

hidden = [
    'psutil', 'webview', 'clr_loader',
    'config', 'config.paths', 'config.asset_system', 'config.runtime',
    'services.bootstrap', 'services.sidecar_api', 'services.engine_proxy',
    'services.profile_manager', 'services.node_bridge', 'services.device_scanner',
    'core.engine', 'core.models', 'core.win32_input',
]

a = Analysis(
    ['gxclicker.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('assets', 'assets'),
        ('ui-web', 'ui-web'),
        ('profiles', 'profiles'),
        ('config', 'config'),
        ('services', 'services'),
        ('core', 'core'),
    ],
    hiddenimports=hidden,
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['PyQt6', 'pytest', 'ui'],
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='Game XClicker Elite',
    debug=False,
    strip=False,
    upx=False,
    console=False,
    icon='assets/brand/favicon.ico',
)

coll = COLLECT(
    exe, a.binaries, a.zipfiles, a.datas,
    name='Game XClicker Elite',
)
