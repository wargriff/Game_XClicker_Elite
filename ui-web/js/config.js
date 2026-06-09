/** Configuration partagée — 6 macros + navigation iCUE */

export const ICUE = {
  bgMain: "#1a1a1a",
  bgPanel: "#242424",
  bgRow: "#2d2d2d",
  bgInput: "#1e1e1e",
  border: "#3a3a3a",
  yellow: "#f5c518",
  text: "#e0e0e0",
  textDim: "#9a9a9a",
};

export const TABS = [
  { id: "home", label: "HOME" },
  { id: "dashboard", label: "DASHBOARD" },
  { id: "devices", label: "INSTANT LIGHTING" },
  { id: "settings", label: "SETTINGS" },
  { id: "community", label: "COMMUNITY" },
];

export const SIDEBAR = [
  { type: "item", id: "performance", label: "PERFORMANCE", badge: "3" },
  { type: "item", id: "graphing", label: "GRAPHING" },
  { type: "item", id: "lighting", label: "LIGHTING SETUP", icon: "⚡" },
  { type: "item", id: "channel1", label: "LIGHTING CHANNEL 1", child: true, badge: "1", icon: "⚡" },
  { type: "item", id: "channel2", label: "LIGHTING CHANNEL 2", child: true, badge: "1", icon: "⚡" },
  { type: "header", label: "MACROS" },
  { type: "item", id: "macro1", label: "MACRO 1 — Clic gauche", key: "left" },
  { type: "item", id: "macro2", label: "MACRO 2 — Clic droit", key: "right" },
  { type: "item", id: "macro3", label: "MACRO 3 — Touche 1", key: "1" },
  { type: "item", id: "macro4", label: "MACRO 4 — Touche 2", key: "2" },
  { type: "item", id: "macro5", label: "MACRO 5 — Touche 3", key: "3" },
  { type: "item", id: "macro6", label: "MACRO 6 — Touche 4", key: "4" },
];

export const DEVICES = [
  { id: "keyboard", icon: "⌨" },
  { id: "mouse", icon: "🖱" },
  { id: "headset", icon: "🎧" },
  { id: "stand", icon: "🎤" },
  { id: "pad", icon: "▭" },
  { id: "fan", icon: "🌀" },
  { id: "ram", icon: "▦" },
  { id: "commander", icon: "⬡" },
];

export const CH1_TYPES = ["RGB Light Strip", "Commander Core", "Lighting Node PRO"];
export const CH1_QTY = ["1 Strip is connected", "2 Strips are connected", "3 Strips are connected", "4 Strips are connected"];
export const CH2_TYPES = ["LL Fan Hub", "QL Fan Hub", "RGB Light Strip"];
export const CH2_QTY = ["1 Fan is connected", "2 Fans are connected", "3 Fans are connected", "4 Fans are connected"];

export const LIGHTING_DEFAULTS = {
  ch1_type: "RGB Light Strip",
  ch1_qty: "4 Strips are connected",
  ch2_type: "LL Fan Hub",
  ch2_qty: "4 Fans are connected",
};

export const MACRO_KEYS = ["left", "right", "1", "2", "3", "4"];

export const MACRO_LABELS = {
  left: "Macro 1 — Clic gauche",
  right: "Macro 2 — Clic droit",
  1: "Macro 3 — Touche 1",
  2: "Macro 4 — Touche 2",
  3: "Macro 5 — Touche 3",
  4: "Macro 6 — Touche 4",
};

export const MURALS = [
  { id: "yellow", label: "CORSAIR Yellow", gradient: "linear-gradient(135deg,#f5c518,#8a6d00)" },
  { id: "rainbow", label: "Rainbow", gradient: "linear-gradient(90deg,#f00,#ff0,#0f0,#0ff,#00f,#f0f)" },
  { id: "abstract", label: "Abstract", gradient: "linear-gradient(160deg,#1a1a2e,#16213e,#0f3460)" },
  { id: "fire", label: "Inferno", gradient: "linear-gradient(135deg,#ff4500,#8b0000,#1a1a1a)" },
  { id: "ice", label: "Arctic", gradient: "linear-gradient(135deg,#a8d8ff,#004e89,#001233)" },
  { id: "neon", label: "Neon", gradient: "linear-gradient(135deg,#00ff88,#0088ff,#8800ff)" },
];
