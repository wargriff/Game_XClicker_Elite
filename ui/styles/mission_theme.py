"""Thème Mission Control — rouge gaming (référence UI hub)."""

MC = {
    "bg": "#0c0c0e",
    "bg_panel": "#141418",
    "bg_card": "#1a1a20",
    "bg_hover": "#222228",
    "border": "#2a2a32",
    "red": "#e53935",
    "red_dim": "#b71c1c",
    "red_glow": "#ff5252",
    "text": "#f0f0f2",
    "text_dim": "#8a8a96",
    "success": "#43a047",
    "warn": "#ffb300",
}

MC_SIDEBAR = f"""
QWidget#McSidebar {{
    background: {MC['bg_panel']};
    border-right: 1px solid {MC['border']};
}}
QPushButton#navBtn {{
    background: transparent;
    border: none;
    border-left: 3px solid transparent;
    color: {MC['text_dim']};
    text-align: left;
    padding: 10px 14px;
    font-size: 11px;
    font-weight: 600;
}}
QPushButton#navBtn:hover {{
    background: {MC['bg_hover']};
    color: {MC['text']};
}}
QPushButton#navBtn:checked {{
    background: {MC['bg_hover']};
    color: {MC['red_glow']};
    border-left: 3px solid {MC['red']};
}}
"""

MC_LAUNCH_BTN = f"""
QPushButton#launchBtn {{
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 {MC['red_glow']}, stop:1 {MC['red_dim']});
    border: none;
    border-radius: 4px;
    color: white;
    font-weight: 700;
    font-size: 12px;
    padding: 12px 20px;
    min-height: 44px;
}}
QPushButton#launchBtn:hover {{
    background: {MC['red_glow']};
}}
QPushButton#launchBtn:disabled {{
    background: {MC['border']};
    color: {MC['text_dim']};
}}
"""

MC_CARD = f"""
QFrame#mcCard {{
    background: {MC['bg_card']};
    border: 1px solid {MC['border']};
    border-radius: 6px;
}}
QFrame#mcCard:hover {{
    border-color: {MC['red_dim']};
}}
"""
