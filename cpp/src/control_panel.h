#pragma once

#include <memory>
#include <windows.h>
#include <string>

namespace gx {
class BootContext;
}

namespace gx {

class ControlPanel {
public:
    bool Create(HINSTANCE instance);
    int Run();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void OnCreate(HWND hwnd);
    void OnCommand(int id);
    void OnClose();
    void SetStatus(const wchar_t* text);

    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    HWND status_ = nullptr;
    std::wstring projectRoot_;
    std::unique_ptr<BootContext> boot_;
};

}  // namespace gx
