#include "services/device_scanner.hpp"

#include "services/engine_proxy.hpp"

namespace gx {

nlohmann::json scan_devices(const EngineProxy* /*engine*/) {
    return nlohmann::json::array({
        {{"id", "commander"}, {"name", "iCUE LINK Commander"}, {"type", "hub"}, {"detail", "C++ sidecar"}},
        {{"id", "mouse"}, {"name", "SCIMITAR ELITE"}, {"type", "mouse"}, {"detail", "15 boutons"}},
        {{"id", "keyboard"}, {"name", "K100 RGB"}, {"type", "keyboard"}, {"detail", "MX switches"}},
    });
}

nlohmann::json scan_sensors() {
    return nlohmann::json::array({
        {{"id", "load"}, {"label", "Load"}, {"value", "12"}, {"unit", "%"}, {"icon", "chart"}},
        {{"id", "ram"}, {"label", "Memory"}, {"value", "48"}, {"unit", "%"}, {"icon", "ram"}},
    });
}

}  // namespace gx
