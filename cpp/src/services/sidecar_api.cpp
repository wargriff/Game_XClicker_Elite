#include "services/sidecar_api.hpp"

#include "config/paths.hpp"
#include "services/device_scanner.hpp"
#include "services/engine_proxy.hpp"
#include "services/profile_manager.hpp"

#include <httplib.h>

#include <windows.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

namespace {

using json = nlohmann::json;

std::string wide_to_utf8(const std::wstring& w) {
    if (w.empty()) return {};
    const int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string out(n - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, out.data(), n, nullptr, nullptr);
    return out;
}

std::wstring utf8_to_wide(const std::string& s) {
    if (s.empty()) return {};
    const int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring out(n - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, out.data(), n);
    return out;
}

void add_cors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

json macro_payload(gx::EngineProxy& engine, const std::string& key) {
    const auto* eng = engine.manager().mouse().button(key) ? &engine.manager().mouse()
                                                           : &engine.manager().keyboard();
    const gx::Btn* btn = eng->button(key);
    if (!btn) return json::object();
    return json{{"key", key},
                {"active", engine.is_active(key)},
                {"cps", engine.get_cps(key)},
                {"real_cps", engine.get_real_cps(key)},
                {"delay_ms", static_cast<int>(btn->delay * 1000)},
                {"burst", engine.get_burst_count(key)}};
}

const std::vector<std::string> kMacroKeys = {"left", "right", "1", "2", "3", "4"};

}  // namespace

namespace gx {

SidecarAPI::SidecarAPI(EngineProxy& engine, ProfileManager* profiles)
    : engine_(engine), profiles_(profiles) {}

SidecarAPI::~SidecarAPI() { stop(); }

void SidecarAPI::setup_routes() {
    auto& svr = *server_;

    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
        add_cors(res);
    });

    auto serve_file = [](const std::wstring& fp, const char* mime) -> httplib::Response {
        std::ifstream in(fp, std::ios::binary);
        std::string body((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        httplib::Response res;
        res.status = 200;
        res.set_content(body, mime);
        add_cors(res);
        return res;
    };

    svr.Get(R"(/.*)", [this, serve_file](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.path;
        if (!path.empty() && path.back() == '/') path.pop_back();
        if (path.empty()) path = "/";
        if (path.rfind("/v1/", 0) == 0) path = "/api" + path;

        auto try_static = [&](const std::wstring& base, const std::string& prefix) -> bool {
            if (path.rfind(prefix, 0) != 0) return false;
            const auto rel = utf8_to_wide(path.substr(1));  // drop leading /
            const auto fp = join_path(base, rel);
            if (!std::filesystem::exists(fp)) return false;
            const auto ext = std::filesystem::path(fp).extension().wstring();
            const char* mime = "application/octet-stream";
            if (ext == L".html") mime = "text/html; charset=utf-8";
            else if (ext == L".css") mime = "text/css; charset=utf-8";
            else if (ext == L".js") mime = "application/javascript";
            else if (ext == L".svg") mime = "image/svg+xml";
            else if (ext == L".png") mime = "image/png";
            res = serve_file(fp, mime);
            return true;
        };

        if (try_static(ui_web_dir(), "/css/") || try_static(ui_web_dir(), "/js/") ||
            try_static(brand_dir(), "/brand/") || try_static(devices_dir(), "/devices/")) {
            return;
        }

        if (path == "/" || path == "/app" || path == "/mission") {
            const auto html = mission_html();
            if (std::filesystem::exists(html)) {
                res = serve_file(html, "text/html; charset=utf-8");
            } else {
                res.set_content(R"({"status":"ok","ui":"ui-web missing"})", "application/json");
                add_cors(res);
            }
            return;
        }

        if (path == "/health" || path == "/api/health" || path == "/api/v1/health") {
            json body = {{"status", "ok"},
                         {"version", VERSION},
                         {"enabled", engine_.enabled()},
                         {"active_macros", engine_.count_active_macros()},
                         {"total_cps", engine_.get_total_cps()}};
            res.set_content(body.dump(), "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        if (path == "/api/v1/macros") {
            json macros = json::array();
            for (const auto& k : kMacroKeys) macros.push_back(macro_payload(engine_, k));
            res.set_content(json{{"macros", macros}}.dump(), "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        if (path.rfind("/api/v1/macros/", 0) == 0) {
            const std::string key = path.substr(std::string("/api/v1/macros/").size());
            if (std::find(kMacroKeys.begin(), kMacroKeys.end(), key) == kMacroKeys.end()) {
                res.status = 404;
                res.set_content(R"({"error":"macro not found"})", "application/json");
                add_cors(res);
                return;
            }
            res.set_content(macro_payload(engine_, key).dump(), "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        if (path == "/api/status") {
            json macros = json::object();
            for (const auto* eng : {&engine_.manager().mouse(), &engine_.manager().keyboard()}) {
                for (const auto& [k, btn] : eng->buttons()) {
                    (void)btn;
                    macros[k] = {{"active", engine_.is_active(k)}, {"cps", engine_.get_real_cps(k)}};
                }
            }
            res.set_content(json{{"engine", engine_.enabled() ? "active" : "stasis"}, {"macros", macros}}
                                .dump(),
                            "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        if (path == "/api/v1/profiles" && profiles_) {
            json names = json::array();
            for (const auto& n : profiles_->list_profiles()) names.push_back(n);
            res.set_content(json{{"profiles", names}}.dump(), "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        if (path == "/api/v1/devices") {
            const auto devices = scan_devices(&engine_);
            res.set_content(json{{"devices", devices}, {"count", devices.size()}}.dump(),
                            "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        if (path == "/api/v1/sensors") {
            res.set_content(json{{"sensors", scan_sensors()}}.dump(), "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        res.status = 404;
        res.set_content(R"({"error":"not found"})", "application/json");
        add_cors(res);
    });

    svr.Post(R"(/api/.*)", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.path;
        if (path.rfind("/v1/", 0) == 0) path = "/api" + path;
        json data;
        try {
            data = req.body.empty() ? json::object() : json::parse(req.body);
        } catch (...) {
            data = json::object();
        }

        if (path == "/api/v1/engine/toggle") {
            engine_.toggle();
            res.set_content(json{{"enabled", engine_.enabled()}}.dump(), "application/json");
            add_cors(res);
            return;
        }

        if (path == "/api/v1/engine/enable") {
            const bool state = data.value("enabled", true);
            if (state != engine_.enabled()) engine_.toggle();
            res.set_content(json{{"enabled", engine_.enabled()}}.dump(), "application/json");
            add_cors(res);
            return;
        }

        if (path.rfind("/api/v1/macros/", 0) == 0) {
            const std::string key = path.substr(std::string("/api/v1/macros/").size());
            if (std::find(kMacroKeys.begin(), kMacroKeys.end(), key) == kMacroKeys.end()) {
                res.status = 404;
                res.set_content(R"({"error":"macro not found"})", "application/json");
                add_cors(res);
                return;
            }
            if (data.contains("cps")) engine_.set_cps(key, data["cps"].get<int>());
            if (data.contains("delay_ms")) engine_.set_delay(key, data["delay_ms"].get<double>() / 1000.0);
            if (data.contains("burst")) engine_.set_burst_count(key, data["burst"].get<int>());
            if (data.contains("active")) engine_.set_active(key, data["active"].get<bool>());
            res.set_content(macro_payload(engine_, key).dump(), "application/json; charset=utf-8");
            add_cors(res);
            return;
        }

        res.status = 404;
        res.set_content(R"({"error":"not found"})", "application/json");
        add_cors(res);
    });
}

void SidecarAPI::start() {
    if (running_) return;
    server_ = std::make_unique<httplib::Server>();
    setup_routes();
    running_ = true;
    thread_ = std::thread([this]() {
        online_ = server_->listen("127.0.0.1", PORT);
        running_ = false;
        online_ = false;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void SidecarAPI::stop() {
    if (server_) server_->stop();
    if (thread_.joinable()) thread_.join();
    server_.reset();
    online_ = false;
    running_ = false;
}

}  // namespace gx
