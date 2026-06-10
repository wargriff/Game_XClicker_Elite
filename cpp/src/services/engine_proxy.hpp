#pragma once

#include "core/engine.hpp"

#include <string>

namespace gx {

class EngineProxy {
public:
    explicit EngineProxy(MacroManager& manager);

    MacroManager& manager() { return manager_; }
    const MacroManager& manager() const { return manager_; }

    bool enabled() const;
    void toggle();
    void stop();

    void set_game_safe(bool state);
    void set_cps(const std::string& key, int value);
    void set_delay(const std::string& key, double value);
    void set_burst_count(const std::string& key, int value);
    int get_burst_count(const std::string& key) const;
    int get_cps(const std::string& key) const;
    int get_real_cps(const std::string& key) const;
    void set_active(const std::string& key, bool active);
    bool is_active(const std::string& key) const;
    int count_active_macros() const;
    int get_total_cps() const;

    bool has_macro(const std::string& key) const;

private:
    BaseEngine* engine_for(const std::string& key);
    const BaseEngine* engine_for(const std::string& key) const;

    MacroManager& manager_;
};

}  // namespace gx
