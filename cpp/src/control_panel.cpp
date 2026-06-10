#include "control_panel.h"

#include "app/bootstrap.hpp"
#include "config/paths.hpp"
#include "process_launcher.h"

#include <commctrl.h>
#include <string>

namespace {

enum ButtonId : int {
    BTN_ENGINE = 1000,
    BTN_PANEL = 1001,
    BTN_NATIVE = 1002,
    BTN_WEB = 1003,
    BTN_BUILD = 1004,
    BTN_EXE = 1005,
    BTN_REPAIR = 1006,
};

constexpr COLORREF kBg = RGB(18, 18, 22);
constexpr COLORREF kRed = RGB(220, 38, 38);
constexpr COLORREF kText = RGB(230, 230, 235);

HFONT MakeFont(int size, bool bold = false) {
    return CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

HWND MakeButton(HWND parent, const wchar_t* text, int id, int x, int y, int w, int h) {
    return CreateWindowW(L"BUTTON", text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                         x, y, w, h, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                         nullptr, nullptr);
}

}  // namespace

namespace gx {

bool ControlPanel::Create(HINSTANCE instance) {
    instance_ = instance;
    projectRoot_ = GetProjectRoot();

    const wchar_t* cls = L"GXControlPanel";
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.lpszClassName = cls;
    wc.hbrBackground = CreateSolidBrush(kBg);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    hwnd_ = CreateWindowW(cls, L"GAME XCLICKER — Control Panel (C++)",
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                          CW_USEDEFAULT, CW_USEDEFAULT, 520, 500,
                          nullptr, nullptr, instance, this);
    return hwnd_ != nullptr;
}

int ControlPanel::Run() {
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

void ControlPanel::OnCreate(HWND hwnd) {
    hwnd_ = hwnd;
    HFONT titleFont = MakeFont(22, true);
    HFONT btnFont = MakeFont(14);

    HWND title = CreateWindowW(L"STATIC", L"GAME XCLICKER ELITE",
                               WS_CHILD | WS_VISIBLE | SS_CENTER,
                               20, 16, 460, 32, hwnd, nullptr, instance_, nullptr);
    SendMessageW(title, WM_SETFONT, reinterpret_cast<WPARAM>(titleFont), TRUE);

    HWND sub = CreateWindowW(L"STATIC", L"C++ natif — moteur macros + lanceur",
                             WS_CHILD | WS_VISIBLE | SS_CENTER,
                             20, 48, 460, 20, hwnd, nullptr, instance_, nullptr);

    int y = 82;
    const int w = 460;
    const int h = 40;
    const int x = 20;

    HWND b0 = MakeButton(hwnd, L"DEMARRER MOTEUR + API :17840", BTN_ENGINE, x, y, w, h);
    y += h + 8;
    HWND b1 = MakeButton(hwnd, L"CONTROL PANEL (Python UI)", BTN_PANEL, x, y, w, h);
    y += h + 10;
    HWND b2 = MakeButton(hwnd, L"INTERFACE NATIVE", BTN_NATIVE, x, y, w, h);
    y += h + 10;
    HWND b3 = MakeButton(hwnd, L"INTERFACE WEB", BTN_WEB, x, y, w, h);
    y += h + 10;
    HWND b4 = MakeButton(hwnd, L"BUILD .EXE", BTN_BUILD, x, y, w, h);
    y += h + 10;
    HWND b5 = MakeButton(hwnd, L"LANCER .EXE BUREAU", BTN_EXE, x, y, w, h);
    y += h + 10;
    HWND b6 = MakeButton(hwnd, L"REPARER (git + deps)", BTN_REPAIR, x, y, w, h);

    for (HWND b : {b0, b1, b2, b3, b4, b5, b6}) {
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(btnFont), TRUE);
    }

    status_ = CreateWindowW(L"STATIC", L"Pret.",
                            WS_CHILD | WS_VISIBLE | SS_LEFT,
                            20, 440, 460, 20, hwnd, nullptr, instance_, nullptr);

    std::wstring st = L"Projet: " + projectRoot_;
    SetStatus(st.c_str());
}

void ControlPanel::SetStatus(const wchar_t* text) {
    if (status_) SetWindowTextW(status_, text);
}

void ControlPanel::OnClose() {
    if (boot_) {
        boot_->sidecar->stop();
        boot_->proxy->stop();
        boot_.reset();
    }
}

void ControlPanel::OnCommand(int id) {
    bool ok = false;
    switch (id) {
        case BTN_ENGINE:
            if (!boot_) {
                set_project_root(projectRoot_);
                boot_ = std::make_unique<BootContext>(bootstrap(projectRoot_));
                SetStatus(L"Moteur + API REST C++ actif -> http://127.0.0.1:17840");
            } else {
                boot_->sidecar->stop();
                boot_->proxy->stop();
                boot_.reset();
                SetStatus(L"Moteur + API arretes.");
            }
            break;
        case BTN_PANEL:
            ok = LaunchPythonMode(projectRoot_, L"");
            SetStatus(ok ? L"Control Panel Python lance." : L"Echec lancement.");
            break;
        case BTN_NATIVE:
            ok = LaunchPythonMode(projectRoot_, L"--native");
            SetStatus(ok ? L"Interface native lancee." : L"Echec native.");
            break;
        case BTN_WEB:
            ok = LaunchPythonMode(projectRoot_, L"--web");
            SetStatus(ok ? L"Interface web lancee." : L"Echec web.");
            break;
        case BTN_BUILD:
            ok = LaunchPythonMode(projectRoot_, L"--build --desktop");
            SetStatus(ok ? L"Build lance (console)." : L"Echec build.");
            break;
        case BTN_EXE:
            ok = LaunchDesktopExe(projectRoot_);
            SetStatus(ok ? L".exe Bureau ouvert." : L".exe introuvable — build d'abord.");
            break;
        case BTN_REPAIR:
            ok = LaunchRepair(projectRoot_);
            SetStatus(ok ? L"REPARER lance." : L"Echec REPARER.");
            break;
        default:
            break;
    }
}

LRESULT CALLBACK ControlPanel::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ControlPanel* self = nullptr;
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = static_cast<ControlPanel*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<ControlPanel*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    switch (msg) {
        case WM_CREATE:
            if (self) self->OnCreate(hwnd);
            return 0;
        case WM_COMMAND:
            if (self && HIWORD(wParam) == BN_CLICKED) {
                self->OnCommand(LOWORD(wParam));
            }
            return 0;
        case WM_CTLCOLORSTATIC: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetTextColor(hdc, kText);
            SetBkColor(hdc, kBg);
            return reinterpret_cast<LRESULT>(CreateSolidBrush(kBg));
        }
        case WM_CLOSE:
            if (self) self->OnClose();
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            if (self) self->OnClose();
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

}  // namespace gx
