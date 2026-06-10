#include "services/profile_manager.hpp"

#include "config/paths.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace gx {

ProfileManager::ProfileManager(const std::wstring& profiles_dir) {
    profiles_dir_ = profiles_dir.empty() ? profiles_dir() : profiles_dir;
}

nlohmann::json ProfileManager::default_data() {
    return nlohmann::json{{"rgb", nlohmann::json::object()},
                          {"bindings", nlohmann::json::object()},
                          {"macro", nlohmann::json{{"buttons", nlohmann::json::object()}}}};
}

std::vector<std::string> ProfileManager::list_profiles() const {
    std::vector<std::string> names;
    if (!fs::exists(profiles_dir_)) return {"default"};
    for (const auto& e : fs::directory_iterator(profiles_dir_)) {
        if (e.path().extension() == L".json") {
            names.push_back(e.path().stem().string());
        }
    }
    if (names.empty()) names.push_back("default");
    return names;
}

std::wstring ProfileManager::path_for(const std::string& name) const {
    return join_path(profiles_dir_, std::wstring(name.begin(), name.end()) + L".json");
}

nlohmann::json ProfileManager::load(const std::string& name) {
    if (!name.empty()) current_name_ = name;
    const auto path = path_for(current_name_);
    if (!fs::exists(path)) {
        data_ = default_data();
        return data_;
    }
    std::ifstream in(path);
    in >> data_;
    return data_;
}

void ProfileManager::save(const std::string& name) {
    if (!name.empty()) current_name_ = name;
    fs::create_directories(profiles_dir_);
    const auto path = path_for(current_name_);
    std::ofstream out(path);
    out << data_.dump(2);
}

void ProfileManager::apply_to_engine(MacroManager& manager) const {
    const auto buttons = data_.value("macro", nlohmann::json::object())
                             .value("buttons", nlohmann::json::object());
    for (auto it = buttons.begin(); it != buttons.end(); ++it) {
        const std::string key = it.key();
        BaseEngine* eng = manager.mouse().button(key) ? &manager.mouse() : &manager.keyboard();
        if (!eng->button(key)) continue;
        const auto& cfg = it.value();
        if (cfg.contains("cps")) eng->set_cps(key, cfg["cps"].get<int>());
        if (cfg.contains("delay")) eng->set_delay(key, cfg["delay"].get<double>());
        if (cfg.contains("burst_count")) eng->set_burst_count(key, cfg["burst_count"].get<int>());
    }
}

void ProfileManager::apply_to_rgb(RgbEngine& rgb) const {
    const auto zones = data_.value("rgb", nlohmann::json::object());
    for (auto it = zones.begin(); it != zones.end(); ++it) {
        const std::string zone = it.key();
        const auto& cfg = it.value();
        if (cfg.contains("mode")) rgb.set_mode(zone, cfg["mode"].get<std::string>());
        if (cfg.contains("color") && cfg["color"].is_array() && cfg["color"].size() >= 3) {
            RgbColor c{cfg["color"][0].get<std::uint8_t>(), cfg["color"][1].get<std::uint8_t>(),
                       cfg["color"][2].get<std::uint8_t>()};
            rgb.set_color(zone, c);
        }
    }
}

void ProfileManager::capture_from_engine(const MacroManager& manager) {
    nlohmann::json buttons = nlohmann::json::object();
    for (const auto* eng : {&manager.mouse(), &manager.keyboard()}) {
        for (const auto& [key, btn] : eng->buttons()) {
            buttons[key] = {{"cps", btn.cps},
                            {"delay", btn.delay},
                            {"burst_count", btn.burst_count}};
        }
    }
    data_["macro"]["buttons"] = buttons;
}

void ProfileManager::capture_from_rgb(const RgbEngine& rgb) {
    nlohmann::json zones = nlohmann::json::object();
    for (const auto& [name, zone] : rgb.zones()) {
        (void)zone;
        const auto c = rgb.get_color(name);
        zones[name] = {{"mode", "static"}, {"color", {c.r, c.g, c.b}}};
    }
    data_["rgb"] = zones;
}

}  // namespace gx
