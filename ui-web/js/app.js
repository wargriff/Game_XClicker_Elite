import { TABS, SIDEBAR, DEVICES, LIGHTING_DEFAULTS, MACRO_KEYS, MACRO_LABELS } from "./config.js";
import { getHealth, getMacros, updateMacro, toggleEngine } from "./api.js";

const state = {
  tab: "devices",
  section: "channel2",
  device: "commander",
  macroKey: "left",
  engine: false,
  macros: {},
  pollMs: 500,
};

const $ = (sel, root = document) => root.querySelector(sel);
const $$ = (sel, root = document) => [...root.querySelectorAll(sel)];

function buildShell() {
  const app = $("#app");
  app.innerHTML = `
    <header class="header">
      <img class="brand-icon" src="/brand/favicon.svg" alt="SOURIS WARGRIFF" onerror="this.style.display='none'"/>
      ${TABS.map(t => `<button class="tab" data-tab="${t.id}">${t.label}</button>`).join("")}
      <div class="header-spacer"></div>
      <span class="status-pill" id="engineStatus">MOTEUR — STASE</span>
      <span class="cps-pill" id="cpsTotal">CPS Σ 0</span>
      <button class="btn-stasis" id="btnStasis">SCEAU DE STASE</button>
    </header>
    <div class="body">
      <aside class="sidebar" id="sidebar"></aside>
      <main class="main" id="main"></main>
    </div>
    <footer class="footer">Latéral 2 = pause globale · XButton1 = clavier · 6 macros actives</footer>
  `;
  buildSidebar();
  buildPages();
  wireHeader();
}

function buildSidebar() {
  const sb = $("#sidebar");
  sb.innerHTML = `
    <div class="sidebar-brand">
      <img src="/brand/favicon.svg" alt="" onerror="this.src='data:image/svg+xml,<svg xmlns=%22http://www.w3.org/2000/svg%22/>'"/>
      <h1>XCLICKER ELITE<br/>SOURIS WARGRIFF</h1>
    </div>
    <div class="profiles-label">PROFILES</div>
    <select class="profile-select" id="profileSelect"><option>Default</option></select>
    <div id="navItems"></div>
  `;

  const nav = $("#navItems");
  SIDEBAR.forEach(item => {
    if (item.type === "header") {
      nav.insertAdjacentHTML("beforeend", `<div class="nav-header">${item.label}</div>`);
    } else {
      nav.insertAdjacentHTML(
        "beforeend",
        `<button class="nav-item" data-section="${item.id}" data-key="${item.key || ""}">${item.label}</button>`
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
    <section class="page" id="page-home">
      <div class="cards-grid" id="homeCards"></div>
    </section>
    <section class="page" id="page-dashboard">
      <div class="cards-grid" id="dashCards"></div>
    </section>
    <section class="page visible" id="page-devices">
      <div class="devices-bar">DEVICES</div>
      <div class="device-strip" id="deviceStrip"></div>
      <div class="device-center">
        <div class="device-name" id="deviceName">COMMANDER PRO</div>
        <div class="commander-box"></div>
        <div class="ports">
          <div class="port">TEMP<span>●</span></div>
          <div class="port">USB<span>●</span></div>
          <div class="port">LED<span>●</span></div>
          <div class="port">FAN<span>●</span></div>
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
            <select class="icue-select" id="ch1Type">
              <option>RGB Light Strip</option><option>Commander Core</option>
            </select>
            <select class="icue-select" id="ch1Qty">
              <option>4 Strips are connected</option><option>2 Strips are connected</option>
            </select>
          </div>
          <div class="channel-row">
            <span class="channel-label">Lighting Channel 2</span>
            <select class="icue-select" id="ch2Type">
              <option>LL Fan Hub</option><option>QL Fan Hub</option>
            </select>
            <select class="icue-select focus" id="ch2Qty">
              <option>4 Fans are connected</option><option>2 Fans are connected</option>
            </select>
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
        <div class="card"><h2>VERSION</h2><div class="val">3.0 Web</div></div>
        <div class="card"><h2>API</h2><div class="val" id="apiStatus">—</div></div>
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
  const pageId = tab === "devices" ? "devices" : tab;
  $(`#page-${pageId}`)?.classList.add("visible");

  if (tab === "devices") selectSection(state.section || "channel2");
  if (tab === "home" || tab === "dashboard") fillStatusCards(tab);
}

function selectSection(section, key = "") {
  state.section = section;
  $$(".nav-item").forEach(b => b.classList.toggle("active", b.dataset.section === section));

  if (section.startsWith("macro") || key) {
    state.macroKey = key || state.macroKey;
    selectTab("devices");
    showMacroPanel();
    return;
  }

  if (["performance", "graphing"].includes(section)) {
    selectTab("dashboard");
    return;
  }

  if (["lighting", "channel1", "channel2"].includes(section)) {
    selectTab("devices");
    focusChannel(section === "channel1" ? 1 : section === "channel2" ? 2 : 0);
  }
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
  ["ch1Qty", "ch2Qty"].forEach(id => $(`#${id}`)?.classList.remove("focus"));
  if (n === 1) $("#ch1Qty")?.classList.add("focus");
  if (n === 2) $("#ch2Qty")?.classList.add("focus");
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

    const { macros } = await getMacros();
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
  selectTab("devices");
  selectSection("channel2");
  poll();
  setInterval(poll, state.pollMs);
}

document.addEventListener("DOMContentLoaded", initApp);
