#include "app/bootstrap.hpp"

#include "config/paths.hpp"
#include "core/engine.hpp"
#include "core/rgb_engine.hpp"
#include "services/engine_proxy.hpp"
#include "services/profile_manager.hpp"
#include "services/sidecar_api.hpp"

namespace gx {

BootContext bootstrap(const std::wstring& root) {
    if (!root.empty()) set_project_root(root);

    BootContext ctx;
    ctx.manager = std::make_unique<MacroManager>();
    ctx.rgb = std::make_unique<RgbEngine>();
    ctx.proxy = std::make_unique<EngineProxy>(*ctx.manager);
    ctx.profiles = std::make_unique<ProfileManager>();
    ctx.profiles->load("default");
    ctx.profiles->apply_to_engine(*ctx.manager);
    ctx.profiles->apply_to_rgb(*ctx.rgb);
    ctx.sidecar = std::make_unique<SidecarAPI>(*ctx.proxy, ctx.profiles.get());
    ctx.sidecar->start();
    return ctx;
}

}  // namespace gx
