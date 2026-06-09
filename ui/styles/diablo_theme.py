"""Diablo 4 inspired Sanctuary Edition theme."""

COLORS = {
    "bg_dark": "#0a0806",
    "bg_panel": "#12100e",
    "bg_card": "#181410",
    "border": "#3d2b1f",
    "border_gold": "#5c4a1f",
    "gold": "#c9a227",
    "gold_bright": "#ffd700",
    "gold_dim": "#8a7020",
    "parchment": "#e8dcc8",
    "parchment_dim": "#a89878",
    "blood": "#8b1a1a",
    "blood_bright": "#c62828",
    "ember": "#d4a017",
    "success": "#4caf50",
    "warning": "#ffb300",
    "danger": "#e53935",
}

WINDOW_TITLE = "Game XClicker Elite — iCUE Control"
WINDOW_SIZE = (1280, 780)

GLOBAL_STYLE = f"""
QMainWindow, QWidget {{
    background-color: transparent;
    color: {COLORS['parchment']};
    font-family: "Segoe UI", "Cinzel", Consolas, sans-serif;
    font-size: 13px;
}}
QScrollArea {{
    border: none;
    background: transparent;
}}
QScrollBar:vertical {{
    background: {COLORS['bg_dark']};
    width: 8px;
    border-radius: 4px;
}}
QScrollBar::handle:vertical {{
    background: {COLORS['border_gold']};
    border-radius: 4px;
    min-height: 24px;
}}
QGroupBox {{
    border: 1px solid {COLORS['border']};
    border-radius: 6px;
    margin-top: 14px;
    padding-top: 18px;
    font-weight: bold;
    color: {COLORS['gold']};
    background: rgba(18, 16, 14, 0.85);
}}
QGroupBox::title {{
    subcontrol-origin: margin;
    left: 12px;
    padding: 0 8px;
}}
QSlider::groove:horizontal {{
    height: 5px;
    background: #1a1410;
    border-radius: 3px;
    border: 1px solid {COLORS['border']};
}}
QSlider::handle:horizontal {{
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 {COLORS['gold_bright']}, stop:1 {COLORS['gold_dim']});
    width: 14px;
    margin: -6px 0;
    border-radius: 7px;
    border: 1px solid {COLORS['border_gold']};
}}
QPushButton {{
    background: rgba(24, 20, 16, 0.92);
    border: 1px solid {COLORS['border_gold']};
    border-radius: 4px;
    padding: 7px 14px;
    color: {COLORS['gold']};
    font-weight: 600;
}}
QPushButton:hover {{
    background: rgba(60, 45, 20, 0.95);
    color: {COLORS['gold_bright']};
    border-color: {COLORS['gold']};
}}
QPushButton:pressed {{
    background: rgba(40, 30, 15, 0.95);
}}
QPushButton:checked, QPushButton[active="true"] {{
    background: rgba(139, 26, 26, 0.45);
    border-color: {COLORS['blood_bright']};
    color: {COLORS['gold_bright']};
}}
QPushButton#danger {{
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #a02020, stop:1 #6b1010);
    border: 1px solid #ff4444;
    color: #ffe0e0;
    font-weight: bold;
}}
QPushButton#danger:hover {{
    background: #c62828;
}}
QPushButton#tab {{
    background: transparent;
    border: none;
    border-bottom: 2px solid transparent;
    border-radius: 0;
    padding: 10px 16px;
    color: {COLORS['parchment_dim']};
    font-weight: 600;
    letter-spacing: 1px;
}}
QPushButton#tab:hover {{
    color: {COLORS['gold']};
    background: rgba(30, 24, 16, 0.5);
}}
QPushButton#tab:checked {{
    color: {COLORS['gold_bright']};
    border-bottom: 2px solid {COLORS['gold']};
    background: rgba(40, 32, 16, 0.4);
}}
QComboBox {{
    background: rgba(18, 16, 14, 0.95);
    border: 1px solid {COLORS['border']};
    border-radius: 4px;
    padding: 5px 10px;
    color: {COLORS['parchment']};
}}
QComboBox:hover {{
    border-color: {COLORS['gold_dim']};
}}
QComboBox::drop-down {{
    border: none;
    width: 22px;
}}
QComboBox QAbstractItemView {{
    background: {COLORS['bg_panel']};
    border: 1px solid {COLORS['border_gold']};
    selection-background-color: {COLORS['blood']};
    selection-color: {COLORS['gold_bright']};
}}
QCheckBox {{
    color: {COLORS['parchment']};
    spacing: 8px;
}}
QCheckBox::indicator {{
    width: 16px;
    height: 16px;
    border: 1px solid {COLORS['border_gold']};
    border-radius: 3px;
    background: {COLORS['bg_dark']};
}}
QCheckBox::indicator:checked {{
    background: {COLORS['blood']};
    border-color: {COLORS['gold']};
}}
"""

CARD_STYLE = f"""
    background: rgba(18, 16, 14, 0.88);
    border: 1px solid {COLORS['border']};
    border-radius: 6px;
"""

TILE_STYLE = f"""
    DeviceTile {{
        background: rgba(20, 16, 12, 0.9);
        border: 1px solid {COLORS['border']};
        border-radius: 6px;
    }}
    DeviceTile:hover {{
        border-color: {COLORS['border_gold']};
    }}
"""

SIDEBAR_STYLE = f"""
    Sidebar {{
        background: #1e1e1e;
        border-right: 1px solid #3a3a3a;
    }}
"""

ICUE_SIDEBAR_STYLE = SIDEBAR_STYLE


HEADER_STYLE = f"""
    HeaderBar {{
        background: rgba(8, 6, 4, 0.95);
        border-bottom: 1px solid {COLORS['border_gold']};
    }}
"""

FOOTER_STYLE = f"""
    FooterBar {{
        background: rgba(8, 6, 4, 0.92);
        border-top: 1px solid {COLORS['border']};
        color: {COLORS['parchment_dim']};
        font-size: 11px;
    }}
"""
