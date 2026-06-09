/**
 * Node.js — sert l'interface iCUE (ui-web/) + proxy API Python
 */
const express = require("express");
const { createProxyMiddleware } = require("http-proxy-middleware");
const path = require("path");

const PORT = process.env.XCLICKER_NODE_PORT || 5173;
const PYTHON_API = process.env.XCLICKER_API_URL || "http://127.0.0.1:17840";
const UI_ROOT = path.join(__dirname, "..", "ui-web");
const BRAND_ROOT = path.join(__dirname, "..", "assets", "brand");

const app = express();
app.use(express.json());

app.use(
  "/api",
  createProxyMiddleware({
    target: PYTHON_API,
    changeOrigin: true,
    pathRewrite: (p) => (p.startsWith("/api") ? p : `/api${p}`),
    onError(err, _req, res) {
      console.error("[NODE] proxy error:", err.message);
      res.status(502).json({ status: "offline", error: err.message });
    },
  })
);

app.use("/brand", express.static(BRAND_ROOT));
app.use(express.static(UI_ROOT));

app.get("/health", (_req, res) => {
  res.json({ status: "ok", service: "xclicker-ui", port: PORT, python: PYTHON_API });
});

app.get("*", (_req, res) => {
  res.sendFile(path.join(UI_ROOT, "index.html"));
});

app.listen(PORT, "127.0.0.1", () => {
  console.log(`[NODE] iCUE UI → http://127.0.0.1:${PORT}`);
  console.log(`[NODE] API Python → ${PYTHON_API}`);
});
