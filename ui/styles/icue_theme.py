"""Couleurs style Corsair iCUE."""

ICUE = {
    "bg_main": "#1a1a1a",
    "bg_panel": "#242424",
    "bg_row": "#2d2d2d",
    "bg_input": "#1e1e1e",
    "border": "#3a3a3a",
    "border_focus": "#f5c518",
    "text": "#e0e0e0",
    "text_dim": "#9a9a9a",
    "yellow": "#f5c518",
    "yellow_dim": "#c9a000",
    "header_bar": "#2a2a2a",
}

ICUE_DEVICE_STRIP = f"""
QWidget#DeviceStrip {{
    background: {ICUE['bg_panel']};
    border-bottom: 1px solid {ICUE['border']};
}}
QPushButton#deviceIcon {{
    background: transparent;
    border: none;
    border-bottom: 3px solid transparent;
    border-radius: 0;
    padding: 10px 18px;
    color: {ICUE['text_dim']};
    font-size: 22px;
}}
QPushButton#deviceIcon:hover {{
    background: rgba(255,255,255,0.05);
    color: {ICUE['text']};
}}
QPushButton#deviceIcon:checked {{
    color: {ICUE['yellow']};
    border-bottom: 3px solid {ICUE['yellow']};
    background: rgba(245, 197, 24, 0.08);
}}
"""

ICUE_LIGHTING_PANEL = f"""
QWidget#LightingSetupPanel {{
    background: {ICUE['bg_row']};
    border-top: 1px solid {ICUE['border']};
}}
QLabel#panelTitle {{
    color: {ICUE['text']};
    font-size: 13px;
    font-weight: 600;
}}
QLabel#channelLabel {{
    color: {ICUE['text']};
    font-size: 12px;
    min-width: 130px;
}}
QPushButton#revertBtn {{
    background: transparent;
    border: 1px solid {ICUE['border']};
    border-radius: 3px;
    padding: 4px 14px;
    color: {ICUE['text_dim']};
    font-size: 11px;
}}
QPushButton#revertBtn:hover {{
    border-color: {ICUE['yellow_dim']};
    color: {ICUE['text']};
}}
QComboBox#icueCombo {{
    background: {ICUE['bg_input']};
    border: 1px solid {ICUE['border']};
    border-radius: 3px;
    padding: 6px 10px;
    color: {ICUE['text']};
    min-width: 180px;
}}
QComboBox#icueCombo:hover {{
    border-color: #555;
}}
QComboBox#icueCombo:focus {{
    border-color: {ICUE['border_focus']};
}}
QComboBox#icueCombo::drop-down {{
    border: none;
    width: 24px;
}}
QComboBox#icueCombo QAbstractItemView {{
    background: {ICUE['bg_panel']};
    border: 1px solid {ICUE['border']};
    selection-background-color: {ICUE['yellow_dim']};
    selection-color: #000;
}}
"""

ICUE_DEVICE_VIEW = f"""
QWidget#DeviceCenterView {{
    background: {ICUE['bg_main']};
    border: none;
}}
QLabel#deviceTitle {{
    color: {ICUE['text_dim']};
    font-size: 11px;
    letter-spacing: 1px;
}}
QLabel#deviceName {{
    color: {ICUE['text']};
    font-size: 14px;
    font-weight: 600;
}}
"""
