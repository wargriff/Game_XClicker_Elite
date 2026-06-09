import { TABS, SIDEBAR, DEVICES, LIGHTING_DEFAULTS, MACRO_KEYS, MACRO_LABELS, CH1_TYPES, CH1_QTY, CH2_TYPES, CH2_QTY, MURALS } from "./config.js";
import { getHealth, getMacros, updateMacro, toggleEngine, getDevices, getSensors } from "./api.js";

const state = {
  tab: "home",
  section: "channel2",
  device: "commander",
  macroKey: "left",
  engine: false,
  macros: {},
  devices: [],
  sensors: [],
  pollMs: 500,
};

const $ = (sel, root = document) => root.querySelector(sel);
const $$ = (sel, root = document) => [...root.querySelectorAll(sel)];

function buildShell() {
  const app = $("#app");
  app.innerHTML = `
    <header class="header">
      <div class="corsair-logo" title="XClicker Elite">⛵</div>
      ${TABS.map(t => `<button class="tab" data-tab="${t.id}">${t.label}</button>`).join("")}
      <div class="header-spacer"></div>
      <span class="status-pill" id="engineStatus">MOTEUR — STASE</span>
      <span class="cps-pill" id="cpsTotal">CPS Σ 0</span>
      <button class="btn-stasis" id="btnStasis" title="Toggle moteur">⏸</button>
      <div class="win-controls">
        <span class="win-btn">─</span>
        <span class="win-btn">□</span>
        <span class="win-btn win-close">✕</span>
      </div>
    </header>
    <div class="body">
      <aside class="sidebar" id="sidebar"></aside>
      <main class="main" id="main"></main>
    </div>
  `;
  buildSidebar();
  buildPages();
  wireHeader();
}

function buildSidebar() {
  const sb = $("#sidebar");
  sb.innerHTML = `
    <div class="profiles-block">
      <div class="profiles-head">
        <span class="profiles-label">PROFILES</span>
        <button class="prof-btn" title="Ajouter">+</button>
        <button class="prof-btn" title="Menu">☰</button>
      </div>
      <div class="profile-row active">
        <img src="/brand/favicon.svg" class="profile-icon" alt="" onerror="this.style.visibility='hidden'"/>
        <span>Default</span>
      </div>
    </div>
    <div id="navItems"></div>
  `;

  const nav = $("#navItems");
  SIDEBAR.forEach(item => {
    if (item.type === "header") {
      nav.insertAdjacentHTML("beforeend", `<div class="nav-header">${item.label}</div>`);
    } else {
      const cls = ["nav-item", item.child ? "nav-child" : ""].filter(Boolean).join(" ");
      const badge = item.badge ? `<span class="nav-badge">${item.badge}</span>` : "";
      const icon = item.icon ? `<span class="nav-icon">${item.icon}</span>` : "";
      nav.insertAdjacentHTML(
        "beforeend",
        `<button class="${cls}" data-section="${item.id}" data-key="${item.key || ""}">${icon}<span class="nav-text">${item.label}</span>${badge}</button>`
      );
    }
  });

  $$(".nav-item", sb).forEach(btn => {
    btn.addEventListener("click", () => selectSection(btn.dataset.section, btn.dataset.key));
  });
}

function buildPages() {
  const main = $("#main");
  main.innerHTML = `
    <section class="page visible" id="page-home">
      <div class="home-layout">
        <div class="profile-rail" id="profileRail"></div>
        <div class="home-content">
          <div class="home-columns">
            <div class="home-left">
              <div class="widget-block">
                <div class="widget-head">MURALS</div>
                <div class="murals-grid" id="muralsGrid"></div>
              </div>
              <div class="widget-block">
                <div class="widget-head">SENSORS</div>
                <div class="sensors-grid" id="sensorsGrid"></div>
              </div>
            </div>
            <div class="home-right">
              <div class="widget-head devices-head">
                <span>DEVICES</span>
                <span class="detect-badge" id="detectCount">0 détectés</span>
              </div>
              <div class="device-grid" id="deviceGrid"></div>
            </div>
          </div>
        </div>
      </div>
    </section>
    <section class="page" id="page-dashboard">
      <div class="cards-grid" id="dashCards"></div>
    </section>
    <section class="page" id="page-devices">
      <div class="devices-bar">
        <span>DEVICES</span>
        <span class="devices-list-icon">☰</span>
      </div>
      <div class="device-strip" id="deviceStrip"></div>
      <div class="devices-body">
        <div class="device-center">
          <div class="device-name" id="deviceName">COMMANDER PRO</div>
          <div class="commander-wrap">
            <div class="commander-box">
              <div class="commander-led"></div>
            </div>
            <div class="ports">
              <div class="port"><small>LED</small><span>●</span></div>
              <div class="port"><small>TEMP</small><span>●</span></div>
              <div class="port"><small>USB</small><span>●</span></div>
              <div class="port"><small>FAN</small><span>●</span></div>
            </div>
          </div>
        </div>
        <div class="lighting-panel">
          <div class="lighting-header">
            <span class="lighting-title">✎ LIGHTING SETUP</span>
            <button class="btn-revert" id="btnRevert">Revert</button>
          </div>
          <div class="lighting-body">
            <div class="channel-row">
              <span class="channel-label">Lighting Channel 1</span>
              <select class="icue-select" id="ch1Type">${CH1_TYPES.map(o=>`<option>${o}</option>`).join("")}</select>
              <select class="icue-select" id="ch1Qty">${CH1_QTY.map(o=>`<option>${o}</option>`).join("")}</select>
            </div>
            <div class="channel-row">
              <span class="channel-label">Lighting Channel 2</span>
              <select class="icue-select" id="ch2Type">${CH2_TYPES.map(o=>`<option>${o}</option>`).join("")}</select>
              <select class="icue-select focus" id="ch2Qty">${CH2_QTY.map(o=>`<option>${o}</option>`).join("")}</select>
            </div>
          </div>
        </div>
      </div>
    </section>
    <section class="page" id="page-macros">
      <div class="macros-page">
        <h2 class="macros-title">MACROS — 6 CANAUX</h2>
        <div class="macro-grid" id="macroGrid"></div>
      </div>
    </section>
    <section class="page" id="page-settings">
      <div class="cards-grid">
        <div class="card"><h2>VERSION</h2><div class="val">3.0 Web iCUE</div></div>
        <div class="card"><h2>API</h2><div class="val" id="apiStatus">—</div></div>
        <div class="card"><h2>MOTEUR</h2><div class="val" id="settingsEngine">—</div></div>
      </div>
    </section>
    <section class="page" id="page-community">
      <div class="cards-grid">
        <div class="card"><h2>COMMUNAUTÉ</h2><div class="val" style="font-size:1rem">GitHub wargriff</div></div>
        <div class="card"><h2>SOURIS WARGRIFF</h2><div class="val" style="font-size:1rem">XClicker Elite</div></div>
      </div>
    </section>
  `;

  const strip = $("#deviceStrip");
  DEVICES.forEach((d, i) => {
    strip.insertAdjacentHTML(
      "beforeend",
      `<button class="device-icon${i === 7 ? " active" : ""}" data-device="${d.id}">${d.icon}</button>`
    );
  });
  $$(".device-icon", strip).forEach(btn => {
    btn.addEventListener("click", () => selectDevice(btn.dataset.device));
  });

  $("#btnRevert").addEventListener("click", revertLighting);
  buildMacroCards();
  buildHomeWidgets();
}

function buildHomeWidgets() {
  const rail = $("#profileRail");
  rail.innerHTML = `
    <button class="rail-toggle" title="Profils">›</button>
    <button class="hex-profile active" title="Default"><img src="/brand/favicon.svg" alt="" onerror="this.parentElement.textContent='D'"/></button>
    <button class="hex-profile" title="Gaming">G</button>
    <button class="hex-profile" title="Diablo">⚔</button>
    <button class="hex-add" title="Ajouter">+</button>
  `;

  $("#muralsGrid").innerHTML = MURALS.map(m => `
    <button class="mural-tile" data-mural="${m.id}" style="background:${m.gradient}" title="${m.label}">
      <span>${m.label}</span>
    </button>
  `).join("");

  renderSensors([]);
  renderDeviceGrid([]);
}

const SENSOR_ICONS = { chart: "📊", bolt: "⚡", ram: "▦", temp: "🌡" };

function renderSensors(list) {
  const grid = $("#sensorsGrid");
  if (!grid) return;
  if (!list.length) {
    grid.innerHTML = `<div class="sensor-empty">Scan capteurs…</div>`;
    return;
  }
  grid.innerHTML = list.map(s => `
    <article class="sensor-card">
      <div class="sensor-icon">${SENSOR_ICONS[s.icon] || "●"}</div>
      <div class="sensor-body">
        <div class="sensor-label">${s.label}</div>
        <div class="sensor-value">${s.value}<small>${s.unit || ""}</small></div>
        <div class="sensor-detail">${s.detail || ""}</div>
      </div>
    </article>
  `).join("");
}

function renderDeviceGrid(list) {
  const grid = $("#deviceGrid");
  const badge = $("#detectCount");
  if (!grid) return;
  const detected = list.filter(d => d.detected);
  if (badge) badge.textContent = `${detected.length} détecté${detected.length > 1 ? "s" : ""}`;

  if (!list.length) {
    grid.innerHTML = `<div class="device-empty">Détection automatique…</div>`;
    return;
  }

  grid.innerHTML = list.map(d => `
    <article class="device-card${d.id === state.device ? " selected" : ""}" data-device="${d.id}">
      <div class="device-card-head">
        <span class="device-card-name">${d.name}</span>
        <button class="device-gear" title="Configurer">⚙</button>
      </div>
      <div class="device-card-img">
        <img src="${d.image}" alt="${d.name}" onerror="this.src='/devices/mouse.svg'"/>
      </div>
      <div class="device-card-foot">
        <span class="status-dot ${d.online ? "online" : "offline"}"></span>
        <span class="device-detail">${d.detail || ""}</span>
      </div>
    </article>
  `).join("");

  $$(".device-card", grid).forEach(card => {
    card.addEventListener("click", (e) => {
      if (e.target.closest(".device-gear")) return;
      openDeviceDetail(card.dataset.device);
    });
  });
}

function openDeviceDetail(id) {
  selectDevice(id);
  selectTab("devices");
  if (id === "commander") selectSection("lighting");
  else if (id === "mouse") selectSection("macro1", "left");
}

function buildMacroCards() {
  const grid = $("#macroGrid");
  grid.innerHTML = MACRO_KEYS.map(key => `
    <article class="macro-card" data-macro="${key}" id="card-${key}">
      <h3>${MACRO_LABELS[key]}</h3>
      <div class="macro-stat" id="stat-${key}">CPS 0 / cible 0</div>
      <div class="slider-row">
        <label><span>CPS</span><span id="cpsVal-${key}">5</span></label>
        <input type="range" min="1" max="200" value="5" data-key="${key}" data-field="cps"/>
      </div>
      <div class="slider-row">
        <label><span>Délai ms</span><span id="delayVal-${key}">20</span></label>
        <input type="range" min="1" max="500" value="20" data-key="${key}" data-field="delay_ms"/>
      </div>
      <div class="slider-row">
        <label><span>Burst</span><span id="burstVal-${key}">0</span></label>
        <input type="range" min="0" max="20" value="0" data-key="${key}" data-field="burst"/>
      </div>
    </article>
  `).join("");

  $$("input[type=range]", grid).forEach(slider => {
    let timer;
    slider.addEventListener("input", () => {
      const key = slider.dataset.key;
      const field = slider.dataset.field;
      $(`#${field === "cps" ? "cpsVal" : field === "burst" ? "burstVal" : "delayVal"}-${key}`).textContent = slider.value;
      clearTimeout(timer);
      timer = setTimeout(() => pushMacro(key, { [field]: Number(slider.value) }), 80);
    });
  });
}

async function pushMacro(key, data) {
  try {
    const res = await updateMacro(key, data);
    state.macros[key] = res;
    refreshMacroCard(key, res);
  } catch (e) { /* offline */ }
}

function refreshMacroCard(key, m) {
  const card = $(`#card-${key}`);
  if (!card || !m) return;
  card.classList.toggle("active-card", m.active);
  $(`#stat-${key}`).textContent = `${m.active ? "ACTIF" : "STASE"} · CPS ${m.real_cps} / cible ${m.cps}`;
}

function wireHeader() {
  $$(".tab").forEach(btn => {
    btn.addEventListener("click", () => selectTab(btn.dataset.tab));
  });
  $("#btnStasis").addEventListener("click", async () => {
    try {
      const r = await toggleEngine();
      state.engine = r.enabled;
      updateEngineUI();
    } catch (e) { /* offline */ }
  });
}

function selectTab(tab) {
  state.tab = tab;
  $$(".tab").forEach(b => b.classList.toggle("active", b.dataset.tab === tab));
  $$(".page").forEach(p => p.classList.remove("visible"));

  if (tab === "home") {
    $("#page-home")?.classList.add("visible");
    renderDeviceGrid(state.devices);
    renderSensors(state.sensors);
    return;
  }

  if (tab === "devices") {
    $("#page-devices")?.classList.add("visible");
    selectSection(state.section || "lighting", "", false);
    return;
  }

  $(`#page-${tab}`)?.classList.add("visible");
  if (tab === "home" || tab === "dashboard") fillStatusCards(tab);
}

function selectSection(section, key = "", syncTab = true) {
  state.section = section;
  $$(".nav-item").forEach(b => {
    b.classList.toggle("active", b.dataset.section === section);
  });

  if (section.startsWith("macro") || key) {
    state.macroKey = key || macroKeyFromSection(section) || state.macroKey;
    if (syncTab) {
      $$(".tab").forEach(b => b.classList.toggle("active", b.dataset.tab === "devices"));
    }
    showMacroPanel();
    return;
  }

  if (["performance", "graphing"].includes(section)) {
    if (syncTab) selectTab("dashboard");
    return;
  }

  if (["lighting", "channel1", "channel2"].includes(section)) {
    if (syncTab) {
      $$(".tab").forEach(b => b.classList.toggle("active", b.dataset.tab === "devices"));
      $$(".page").forEach(p => p.classList.remove("visible"));
      $("#page-devices")?.classList.add("visible");
    }
    const ch = section === "channel1" ? 1 : section === "channel2" ? 2 : 0;
    focusChannel(ch);
  }
}

function macroKeyFromSection(section) {
  const map = { macro1: "left", macro2: "right", macro3: "1", macro4: "2", macro5: "3", macro6: "4" };
  return map[section] || "";
}

function showMacroPanel() {
  $$(".page").forEach(p => p.classList.remove("visible"));
  $("#page-macros").classList.add("visible");
  $$(".tab").forEach(b => b.classList.remove("active"));
  highlightMacroCard(state.macroKey);
}

function highlightMacroCard(key) {
  $$(".macro-card").forEach(c => c.classList.toggle("active-card", c.dataset.macro === key));
  const card = $(`#card-${key}`);
  card?.scrollIntoView({ behavior: "smooth", block: "nearest" });
}

function focusChannel(n) {
  ["ch1Type", "ch1Qty", "ch2Type", "ch2Qty"].forEach(id => $(`#${id}`)?.classList.remove("focus"));
  if (n === 1) { $("#ch1Qty")?.classList.add("focus"); $("#ch1Type")?.classList.add("focus"); }
  if (n === 2) { $("#ch2Qty")?.classList.add("focus"); $("#ch2Type")?.classList.add("focus"); }
}

function revertLighting() {
  $("#ch1Type").value = LIGHTING_DEFAULTS.ch1_type;
  $("#ch1Qty").value = LIGHTING_DEFAULTS.ch1_qty;
  $("#ch2Type").value = LIGHTING_DEFAULTS.ch2_type;
  $("#ch2Qty").value = LIGHTING_DEFAULTS.ch2_qty;
  focusChannel(0);
}

const DEVICE_NAMES = {
  keyboard: "K70 RGB PRO",
  mouse: "M65 RGB ELITE",
  headset: "HS80 RGB WIRELESS",
  commander: "COMMANDER PRO",
  fan: "QL120 RGB",
  ram: "DOMINATOR RGB",
};

function selectDevice(id) {
  state.device = id;
  $$(".device-icon").forEach(b => b.classList.toggle("active", b.dataset.device === id));
  $("#deviceName").textContent = DEVICE_NAMES[id] || "Périphérique";
}

function updateEngineUI() {
  const el = $("#engineStatus");
  el.textContent = state.engine ? "MOTEUR — ACTIF" : "MOTEUR — STASE";
  el.classList.toggle("active", state.engine);
}

function fillStatusCards(which) {
  const target = which === "home" ? "#homeCards" : "#dashCards";
  $(target).innerHTML = `
    <div class="card"><h2>MOTEUR</h2><div class="val">${state.engine ? "ACTIF" : "STASE"}</div></div>
    <div class="card"><h2>MACROS ACTIVES</h2><div class="val">${Object.values(state.macros).filter(m=>m.active).length}</div></div>
    <div class="card"><h2>CPS TOTAL</h2><div class="val" id="cardCps">${$("#cpsTotal")?.textContent.replace("CPS Σ ","")||0}</div></div>
    <div class="card"><h2>CANAUX</h2><div class="val">6</div></div>
  `;
}

async function poll() {
  try {
    const health = await getHealth();
    state.engine = health.enabled;
    $("#cpsTotal").textContent = `CPS Σ ${health.total_cps}`;
    updateEngineUI();
    $("#apiStatus") && ($("#apiStatus").textContent = "OK");

    const [{ macros }, devRes, sensRes] = await Promise.all([
      getMacros(),
      getDevices().catch(() => ({ devices: [] })),
      getSensors().catch(() => ({ sensors: [] })),
    ]);

    state.devices = devRes.devices || [];
    state.sensors = sensRes.sensors || [];
    if (state.tab === "home") {
      renderDeviceGrid(state.devices);
      renderSensors(state.sensors);
    }

    macros.forEach(m => {
      state.macros[m.key] = m;
      refreshMacroCard(m.key, m);
      const cpsSl = $(`input[data-key="${m.key}"][data-field="cps"]`);
      const dSl = $(`input[data-key="${m.key}"][data-field="delay_ms"]`);
      const bSl = $(`input[data-key="${m.key}"][data-field="burst"]`);
      if (cpsSl && document.activeElement !== cpsSl) {
        cpsSl.value = m.cps;
        $(`#cpsVal-${m.key}`).textContent = m.cps;
      }
      if (dSl && document.activeElement !== dSl) {
        dSl.value = m.delay_ms;
        $(`#delayVal-${m.key}`).textContent = m.delay_ms;
      }
      if (bSl && document.activeElement !== bSl) {
        bSl.value = m.burst;
        $(`#burstVal-${m.key}`).textContent = m.burst;
      }
    });
  } catch (e) {
    $("#apiStatus") && ($("#apiStatus").textContent = "OFFLINE");
  }
}

export function initApp() {
  buildShell();
  selectTab("home");
  poll();
  setInterval(poll, state.pollMs);
}

document.addEventListener("DOMContentLoaded", initApp);
