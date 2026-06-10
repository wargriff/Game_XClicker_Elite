#include "config/paths.hpp"

#include <windows.h>

#include <filesystem>

namespace fs = std::filesystem;

namespace gx {

static std::wstring g_root;

void set_project_root(const std::wstring& root) { g_root = root; }

std::wstring project_root() {
    if (!g_root.empty()) return g_root;
    wchar_t buf[MAX_PATH]{};
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    fs::path p = buf;
    for (int i = 0; i < 6; ++i) {
        if (fs::exists(p / L"GameXClicker.py") || fs::exists(p / L"launcher.py")) {
            g_root = p.wstring();
            return g_root;
        }
        if (!p.has_parent_path()) break;
        p = p.parent_path();
    }
    g_root = fs::path(buf).parent_path().wstring();
    return g_root;
}

std::wstring join_path(const std::wstring& a, const std::wstring& b) {
    return (fs::path(a) / b).wstring();
}

std::wstring ui_web_dir() { return join_path(project_root(), L"ui-web"); }
std::wstring brand_dir() { return join_path(project_root(), L"assets/brand"); }
std::wstring devices_dir() { return join_path(project_root(), L"assets/devices"); }
std::wstring profiles_dir() { return join_path(project_root(), L"profiles"); }
std::wstring mission_html() { return join_path(ui_web_dir(), L"index.html"); }

}  // namespace gx
