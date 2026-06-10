#include "services/engine_proxy.hpp"

namespace gx {

EngineProxy::EngineProxy(MacroManager& manager) : manager_(manager) {}

bool EngineProxy::enabled() const { return manager_.mouse().enabled(); }

void EngineProxy::toggle() { manager_.toggle_all(); }

void EngineProxy::stop() { manager_.stop(); }

void EngineProxy::set_game_safe(bool state) {
    manager_.mouse().set_game_safe(state);
    manager_.keyboard().set_game_safe(state);
}

BaseEngine* EngineProxy::engine_for(const std::string& key) {
    if (manager_.mouse().button(key)) return &manager_.mouse();
    return &manager_.keyboard();
}

const BaseEngine* EngineProxy::engine_for(const std::string& key) const {
    if (manager_.mouse().button(key)) return &manager_.mouse();
    return &manager_.keyboard();
}

void EngineProxy::set_cps(const std::string& key, int value) {
    engine_for(key)->set_cps(key, value);
}

void EngineProxy::set_delay(const std::string& key, double value) {
    engine_for(key)->set_delay(key, value);
}

void EngineProxy::set_burst_count(const std::string& key, int value) {
    engine_for(key)->set_burst_count(key, value);
}

int EngineProxy::get_burst_count(const std::string& key) const {
    return engine_for(key)->get_burst_count(key);
}

int EngineProxy::get_cps(const std::string& key) const {
    return engine_for(key)->get_cps(key);
}

int EngineProxy::get_real_cps(const std::string& key) const {
    return engine_for(key)->get_real_cps(key);
}

void EngineProxy::set_active(const std::string& key, bool active) {
    engine_for(key)->set_active(key, active);
}

bool EngineProxy::is_active(const std::string& key) const {
    return engine_for(key)->is_active(key);
}

bool EngineProxy::has_macro(const std::string& key) const {
    return manager_.mouse().button(key) || manager_.keyboard().button(key);
}

int EngineProxy::count_active_macros() const {
    int n = 0;
    for (const auto* eng : {&manager_.mouse(), &manager_.keyboard()}) {
        for (const auto& [k, btn] : eng->buttons()) {
            (void)k;
            if (btn.active) ++n;
        }
    }
    return n;
}

int EngineProxy::get_total_cps() const {
    int total = 0;
    for (const auto* eng : {&manager_.mouse(), &manager_.keyboard()}) {
        for (const auto& [key, btn] : eng->buttons()) {
            if (btn.active) total += eng->get_real_cps(key);
        }
    }
    return total;
}

}  // namespace gx
