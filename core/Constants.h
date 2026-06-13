#pragma once

#include <QString>

namespace Gx
{
namespace Colors
{
    inline const char* Background      = "#0a0a0c";
    inline const char* BackgroundPanel = "#121218";
    inline const char* BackgroundCard  = "#181820";
    inline const char* Border          = "#2a2a34";
    inline const char* Accent          = "#e02626";
    inline const char* AccentHover     = "#ff3b3b";
    inline const char* AccentDark      = "#a01818";
    inline const char* TextPrimary     = "#f0f0f5";
    inline const char* TextSecondary   = "#9090a0";
    inline const char* TextMuted       = "#606070";
    inline const char* Success         = "#2ecc71";
    inline const char* Orange          = "#ff8c2a";
    inline const char* Disabled        = "#505058";
}

namespace Layout
{
    constexpr int WindowWidth    = 1440;
    constexpr int WindowHeight   = 900;
    constexpr int WindowMinWidth = 1100;
    constexpr int WindowMinHeight = 680;
    constexpr int ResizeBorder   = 8;
    constexpr int SidebarWidth   = 220;
    constexpr int TitleBarHeight = 52;
    constexpr int DockHeight     = 88;
    constexpr int Radius         = 8;
}

namespace App
{
    inline const QString Name    = QStringLiteral("Game_macro_elite");
    inline const QString Version = QStringLiteral("v1.2.0");
}
}
