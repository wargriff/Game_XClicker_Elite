#pragma once

#include <atomic>
#include <memory>
#include <thread>

namespace httplib {
class Server;
}

namespace gx {
class EngineProxy;
class ProfileManager;
}

namespace gx {

class SidecarAPI {
public:
    static constexpr int PORT = 17840;
    static constexpr const char* VERSION = "4.0.0-cpp";

    SidecarAPI(EngineProxy& engine, ProfileManager* profiles = nullptr);
    ~SidecarAPI();

    void start();
    void stop();

    bool online() const { return online_; }

private:
    void setup_routes();

    EngineProxy& engine_;
    ProfileManager* profiles_;
    std::unique_ptr<httplib::Server> server_;
    std::thread thread_;
    std::atomic<bool> online_{false};
    std::atomic<bool> running_{false};
};

}  // namespace gx
