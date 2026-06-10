#include "core/engine.hpp"

#include "core/win32_input.hpp"

#include <chrono>
#include <cmath>
#include <random>
#include <thread>

namespace {

double now_sec() {
    using clock = std::chrono::steady_clock;
    return std::chrono::duration<double>(clock::now().time_since_epoch()).count();
}

void sleep_sec(double s) {
    if (s > 0) std::this_thread::sleep_for(std::chrono::duration<double>(s));
}

}  // namespace

namespace gx {

constexpr int GAME_SAFE_MAX_CPS = 30;

BaseEngine::~BaseEngine() {
    stop();
    for (auto& t : threads_) {
        if (t.joinable()) t.join();
    }
}

void BaseEngine::set_on_toggle(ToggleCallback cb) { on_toggle_ = std::move(cb); }

int BaseEngine::get_cps(const std::string& key) const {
    auto it = buttons_.find(key);
    return it != buttons_.end() ? it->second.cps : 10;
}

void BaseEngine::set_cps(const std::string& key, int value) {
    auto it = buttons_.find(key);
    if (it == buttons_.end()) return;
    int max_cps = game_safe_ ? GAME_SAFE_MAX_CPS : 200;
    it->second.cps = std::max(1, std::min(max_cps, value));
}

void BaseEngine::set_delay(const std::string& key, double value) {
    auto it = buttons_.find(key);
    if (it != buttons_.end()) it->second.delay = std::max(0.0, value);
}

void BaseEngine::set_burst_count(const std::string& key, int value) {
    auto it = buttons_.find(key);
    if (it != buttons_.end()) it->second.burst_count = std::max(0, value);
}

int BaseEngine::get_burst_count(const std::string& key) const {
    auto it = buttons_.find(key);
    return it != buttons_.end() ? it->second.burst_count : 0;
}

int BaseEngine::get_real_cps(const std::string& key) const {
    auto it = stats_.find(key);
    return it != stats_.end() ? it->second.cps : 0;
}

void BaseEngine::stop() { running_ = false; }

void BaseEngine::toggle_global() { enabled_ = !enabled_; }

void BaseEngine::set_active(const std::string& key, bool active) {
    auto it = buttons_.find(key);
    if (it != buttons_.end()) it->second.active = active;
}

bool BaseEngine::is_active(const std::string& key) const {
    auto it = buttons_.find(key);
    return it != buttons_.end() && it->second.active;
}

void BaseEngine::set_game_safe(bool state) { game_safe_ = state; }

Btn* BaseEngine::button(const std::string& key) {
    auto it = buttons_.find(key);
    return it != buttons_.end() ? &it->second : nullptr;
}

const Btn* BaseEngine::button(const std::string& key) const {
    auto it = buttons_.find(key);
    return it != buttons_.end() ? &it->second : nullptr;
}

void BaseEngine::register_click(const std::string& key) {
    auto& s = stats_[key];
    const double now = now_sec();
    std::lock_guard<std::mutex> guard(s.lock);
    s.timestamps.push_back(now);
    while (!s.timestamps.empty() && now - s.timestamps.front() > 1.0) {
        s.timestamps.pop_front();
    }
    s.cps = static_cast<int>(s.timestamps.size());
}

double BaseEngine::effective_interval(const Btn& btn) const {
    int cps = btn.cps;
    if (game_safe_) cps = std::min(cps, GAME_SAFE_MAX_CPS);
    double interval = std::max(1.0 / cps, btn.delay);
    if (game_safe_) {
        static thread_local std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<double> dist(0.0, 0.008);
        interval += dist(rng);
    }
    return interval;
}

void BaseEngine::start_worker(const std::string& key,
                              std::function<void(const std::string&)> action) {
    threads_.emplace_back([this, key, action = std::move(action)]() {
        while (running_) {
            auto bit = buttons_.find(key);
            if (bit == buttons_.end()) break;
            if (!enabled_ || !bit->second.active) {
                sleep_sec(0.01);
                continue;
            }
            const double interval = effective_interval(bit->second);
            const double start = now_sec();
            action(key);
            register_click(key);
            sleep_sec(interval - (now_sec() - start));
        }
    });
}

void BaseEngine::notify_toggle(const std::string& key, bool active) {
    if (on_toggle_) on_toggle_(key, active);
}

MouseEngine::MouseEngine() {
    buttons_["left"] = Btn{};
    buttons_["right"] = Btn{.cps = 15, .delay = 0.019};
    for (const auto& k : {"left", "right"}) stats_[k] = Stats{};
    ignore_ms_["left"] = 0.05;
    ignore_ms_["right"] = 0.08;
    last_state_["left"] = last_state_["right"] = false;

    start_worker("left", [this](const std::string& k) { click(k); });
    start_worker("right", [this](const std::string& k) { click(k); });
    start_listeners();
}

MouseEngine::~MouseEngine() {
    stop();
    if (listener_.joinable()) listener_.join();
}

void MouseEngine::click(const std::string& key) {
    const double now = now_sec();
    ignore_until_[key] = now + ignore_ms_[key];
    if (key == "left") {
        send_mouse(MOUSEEVENTF_LEFTDOWN);
        send_mouse(MOUSEEVENTF_LEFTUP);
    } else {
        send_mouse(MOUSEEVENTF_RIGHTDOWN);
        send_mouse(MOUSEEVENTF_RIGHTUP);
    }
}

void MouseEngine::fire_burst(const std::string& key) {
    const int burst = buttons_[key].burst_count;
    if (burst <= 0) return;
    for (int i = 0; i < burst; ++i) {
        click(key);
        register_click(key);
        sleep_sec(0.01);
    }
}

void MouseEngine::toggle_button(const std::string& key) {
    auto& btn = buttons_[key];
    btn.active = !btn.active;
    if (btn.active) fire_burst(key);
    notify_toggle(key, btn.active);
}

void MouseEngine::start_listeners() {
    listener_ = std::thread([this]() {
        while (running_) {
            const double now = now_sec();
            const bool l = key_pressed(VK_LBUTTON);
            const bool r = key_pressed(VK_RBUTTON);
            for (auto [key, pressed] : {std::pair{"left", l}, {"right", r}}) {
                if (now <= ignore_until_[key]) continue;
                if (pressed && !last_state_[key]) press_time_[key] = now;
                if (!pressed && last_state_[key]) {
                    if (now - press_time_[key] > 0.02) toggle_button(key);
                }
                last_state_[key] = pressed;
            }
            sleep_sec(0.005);
        }
    });
}

KeyEngine::KeyEngine() {
    for (const auto& k : {"1", "2", "3", "4"}) {
        buttons_[k] = Btn{};
        stats_[k] = Stats{};
    }
    vk_["1"] = 0x31;
    vk_["2"] = 0x32;
    vk_["3"] = 0x33;
    vk_["4"] = 0x34;

    for (const auto& k : {"1", "2", "3", "4"}) {
        start_worker(k, [this](const std::string& key) { press(key); });
    }
    start_listeners();
}

KeyEngine::~KeyEngine() {
    stop();
    if (listener_.joinable()) listener_.join();
}

void KeyEngine::press(const std::string& key) {
    const auto vk = vk_[key];
    send_key(vk, true);
    send_key(vk, false);
}

void KeyEngine::start_listeners() {
    listener_ = std::thread([this]() {
        bool last = false;
        while (running_) {
            const bool p = key_pressed(VK_XBUTTON1);
            if (p && !last) {
                bool any = false;
                for (auto& [k, b] : buttons_) if (b.active) any = true;
                const bool active = !any;
                for (auto& [k, b] : buttons_) {
                    b.active = active;
                    notify_toggle(k, active);
                }
            }
            last = p;
            sleep_sec(0.02);
        }
    });
}

MacroManager::MacroManager() {
    mouse_ = std::make_unique<MouseEngine>();
    keyboard_ = std::make_unique<KeyEngine>();
    start_master_listener();
}

MacroManager::~MacroManager() {
    running_ = false;
    stop();
    if (master_listener_.joinable()) master_listener_.join();
}

void MacroManager::toggle_all() {
    mouse_->toggle_global();
    keyboard_->toggle_global();
}

void MacroManager::stop() {
    mouse_->stop();
    keyboard_->stop();
}

void MacroManager::set_on_toggle(BaseEngine::ToggleCallback cb) {
    mouse_->set_on_toggle(cb);
    keyboard_->set_on_toggle(cb);
}

void MacroManager::start_master_listener() {
    master_listener_ = std::thread([this]() {
        bool last = false;
        while (running_) {
            const bool p = key_pressed(VK_XBUTTON2);
            if (p && !last) toggle_all();
            last = p;
            sleep_sec(0.02);
        }
    });
}

}  // namespace gx
