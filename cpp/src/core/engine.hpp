#pragma once

#include "core/models.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace gx {

class BaseEngine {
public:
    using ToggleCallback = std::function<void(const std::string&, bool)>;

    virtual ~BaseEngine();

    void set_on_toggle(ToggleCallback cb);
    int get_cps(const std::string& key) const;
    void set_cps(const std::string& key, int value);
    void set_delay(const std::string& key, double value);
    void set_burst_count(const std::string& key, int value);
    int get_burst_count(const std::string& key) const;
    int get_real_cps(const std::string& key) const;
    void stop();
    void toggle_global();
    void set_active(const std::string& key, bool active);
    bool is_active(const std::string& key) const;
    void set_game_safe(bool state);
    bool game_safe() const { return game_safe_; }

    Btn* button(const std::string& key);
    const Btn* button(const std::string& key) const;
    bool enabled() const { return enabled_; }
    const std::unordered_map<std::string, Btn>& buttons() const { return buttons_; }

protected:
    std::atomic<bool> enabled_{true};
    std::atomic<bool> running_{true};
    bool game_safe_ = false;
    std::unordered_map<std::string, Btn> buttons_;
    std::unordered_map<std::string, Stats> stats_;
    ToggleCallback on_toggle_;
    std::vector<std::thread> threads_;

    void register_click(const std::string& key);
    double effective_interval(const Btn& btn) const;
    void start_worker(const std::string& key, std::function<void(const std::string&)> action);
    void notify_toggle(const std::string& key, bool active);
};

class MouseEngine : public BaseEngine {
public:
    MouseEngine();
    ~MouseEngine() override;

private:
    void click(const std::string& key);
    void fire_burst(const std::string& key);
    void toggle_button(const std::string& key);
    void start_listeners();

    std::unordered_map<std::string, bool> last_state_;
    std::unordered_map<std::string, double> press_time_;
    std::unordered_map<std::string, double> ignore_until_;
    std::unordered_map<std::string, double> ignore_ms_;
    std::thread listener_;
};

class KeyEngine : public BaseEngine {
public:
    KeyEngine();
    ~KeyEngine() override;

private:
    void press(const std::string& key);
    void start_listeners();

    std::unordered_map<std::string, std::uint16_t> vk_;
    std::thread listener_;
};

class MacroManager {
public:
    MacroManager();
    ~MacroManager();

    void toggle_all();
    void stop();
    void set_on_toggle(BaseEngine::ToggleCallback cb);

    MouseEngine& mouse() { return *mouse_; }
    KeyEngine& keyboard() { return *keyboard_; }

private:
    void start_master_listener();

    std::unique_ptr<MouseEngine> mouse_;
    std::unique_ptr<KeyEngine> keyboard_;
    std::thread master_listener_;
    std::atomic<bool> running_{true};
};

}  // namespace gx
