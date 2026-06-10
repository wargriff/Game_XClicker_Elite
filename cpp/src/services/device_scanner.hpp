#pragma once

#include <nlohmann/json.hpp>

namespace gx {
class EngineProxy;
}

namespace gx {

nlohmann::json scan_devices(const EngineProxy* engine = nullptr);
nlohmann::json scan_sensors();

}  // namespace gx
