#pragma once

#include <memory>
#include <string>

namespace gx {
class EngineProxy;
class MacroManager;
class ProfileManager;
class RgbEngine;
class SidecarAPI;
}

namespace gx {

struct BootContext {
    std::unique_ptr<MacroManager> manager;
    std::unique_ptr<RgbEngine> rgb;
    std::unique_ptr<EngineProxy> proxy;
    std::unique_ptr<ProfileManager> profiles;
    std::unique_ptr<SidecarAPI> sidecar;
};

BootContext bootstrap(const std::wstring& root = L"");

}  // namespace gx
