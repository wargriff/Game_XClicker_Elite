# -*- mode: python ; coding: utf-8 -*-

block_cipher = None

hidden = [
    'psutil', 'clr_loader',
    'PyQt6', 'PyQt6.QtCore', 'PyQt6.QtGui', 'PyQt6.QtWidgets',
    'native_app', 'gxclicker',
    'config', 'config.paths', 'config.asset_system', 'config.runtime',
    'services.bootstrap', 'services.sidecar_api', 'services.engine_proxy',
    'services.profile_manager', 'services.node_bridge', 'services.device_scanner',
    'services.api_monitor',
    'core.engine', 'core.models', 'core.win32_input',
    'ui.control_center', 'ui.mission_control', 'ui.sanctuary_window', 'ui.splash_screen', 'ui.pages.home_page',
    'rgb_engine',
]

a = Analysis(
    ['GameXClicker.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('assets', 'assets'),
        ('profiles', 'profiles'),
        ('config', 'config'),
        ('services', 'services'),
        ('core', 'core'),
        ('ui', 'ui'),
    ],
    hiddenimports=hidden,
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['pytest', 'webview'],
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
