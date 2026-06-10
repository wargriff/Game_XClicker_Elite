#pragma once

#include "core/engine.hpp"
#include "core/rgb_engine.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace gx {

class ProfileManager {
public:
    explicit ProfileManager(const std::wstring& profiles_dir = L"");

    std::vector<std::string> list_profiles() const;
    std::wstring path_for(const std::string& name) const;

    nlohmann::json& data() { return data_; }
    const nlohmann::json& data() const { return data_; }

    nlohmann::json load(const std::string& name = "");
    void save(const std::string& name = "");

    void apply_to_engine(MacroManager& manager) const;
    void apply_to_rgb(RgbEngine& rgb) const;
    void capture_from_engine(const MacroManager& manager);
    void capture_from_rgb(const RgbEngine& rgb);

    const std::string& current_name() const { return current_name_; }

private:
    static nlohmann::json default_data();

    std::wstring profiles_dir_;
    std::string current_name_ = "default";
    nlohmann::json data_ = default_data();
};

}  // namespace gx
