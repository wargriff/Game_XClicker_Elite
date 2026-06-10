#pragma once

#include <string>

namespace gx {

std::wstring project_root();
void set_project_root(const std::wstring& root);

std::wstring join_path(const std::wstring& a, const std::wstring& b);
std::wstring ui_web_dir();
std::wstring brand_dir();
std::wstring devices_dir();
std::wstring profiles_dir();
std::wstring mission_html();

}  // namespace gx
