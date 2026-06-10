#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace gx {

struct RgbColor {
    std::uint8_t r = 0;
    std::uint8_t g = 255;
    std::uint8_t b = 120;
};

struct RgbZone {
    std::string mode = "static";
    RgbColor color{};
    double flash = 0.0;
};

class RgbEngine {
public:
    RgbEngine();

    void update();
    void set_mode(const std::string& zone, const std::string& mode);
    void set_color(const std::string& zone, const RgbColor& color);
    RgbColor get_color(const std::string& zone) const;
    void trigger_reactive(const std::string& zone);
    const std::unordered_map<std::string, RgbZone>& zones() const { return zones_; }

    double speed = 2.0;

private:
    RgbColor scale(const RgbColor& c, double factor) const;
    RgbColor rainbow() const;

    std::unordered_map<std::string, RgbZone> zones_;
    double t_ = 0.0;
    double last_ = 0.0;
};

}  // namespace gx
