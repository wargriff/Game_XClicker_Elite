/** Client API — Python Sidecar via proxy Node */

const BASE = "/api/v1";

export async function getHealth() {
  const r = await fetch(`${BASE}/health`);
  return r.json();
}

export async function getMacros() {
  const r = await fetch(`${BASE}/macros`);
  return r.json();
}

export async function getMacro(key) {
  const r = await fetch(`${BASE}/macros/${key}`);
  return r.json();
}

export async function updateMacro(key, data) {
  const r = await fetch(`${BASE}/macros/${key}`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  return r.json();
}

export async function toggleEngine() {
  const r = await fetch(`${BASE}/engine/toggle`, { method: "POST" });
  return r.json();
}

export async function getProfiles() {
  const r = await fetch(`${BASE}/profiles`);
  return r.json();
}
