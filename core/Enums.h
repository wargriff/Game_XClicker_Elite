#pragma once

#include <QMap>
#include <QString>

enum class NavSection
{
    MissionControl,
    DeviceCenter,
    MacroStudio,
    MacroLibrary,
    ProfileManager,
    ActivityMonitor,
    AnalyticsCenter,
    MobileCommand,
    LightingEngine,
    SettingsHub
};

enum class DeviceTab
{
    Mouse,
    Keyboard
};

enum class KeyAssignment
{
    Normal,
    Macro,
    Function,
    Disabled
};

enum class RgbEffect
{
    Rainbow,
    Wave,
    Breathing,
    Ripple,
    Reactive,
    Static
};

inline QString navSectionLabel(NavSection s)
{
    switch (s)
    {
    case NavSection::ProfileManager:  return QStringLiteral("Profils");
    case NavSection::DeviceCenter:    return QStringLiteral("Peripheriques");
    case NavSection::MacroLibrary:    return QStringLiteral("Macros");
    case NavSection::LightingEngine:  return QStringLiteral("Eclairage");
    case NavSection::MissionControl:
    case NavSection::SettingsHub:
    case NavSection::ActivityMonitor: return QStringLiteral("Peripheriques");
    case NavSection::MacroStudio:     return QStringLiteral("Macros");
    case NavSection::AnalyticsCenter:
    case NavSection::MobileCommand:     return QStringLiteral("Peripheriques");
    }
    return QStringLiteral("Peripheriques");
}

inline QString navSectionIconAsset(NavSection s)
{
    switch (s)
    {
    case NavSection::ProfileManager:  return QStringLiteral("icons/nav-profiles.svg");
    case NavSection::DeviceCenter:    return QStringLiteral("icons/nav-devices.svg");
    case NavSection::MacroLibrary:
    case NavSection::MacroStudio:     return QStringLiteral("icons/nav-macros.svg");
    case NavSection::LightingEngine:  return QStringLiteral("icons/nav-lighting.svg");
    case NavSection::MissionControl:
    case NavSection::SettingsHub:
    case NavSection::ActivityMonitor:
    case NavSection::AnalyticsCenter:
    case NavSection::MobileCommand:   return QStringLiteral("icons/nav-devices.svg");
    }
    return QStringLiteral("icons/nav-devices.svg");
}

inline QString deviceIconAsset(const QString& deviceId)
{
    static const QMap<QString, QString> map = {
        { QStringLiteral("mb"),  QStringLiteral("devices/dock-motherboard.svg") },
        { QStringLiteral("gpu"), QStringLiteral("devices/dock-gpu.svg") },
        { QStringLiteral("kb"),  QStringLiteral("devices/dock-keyboard.svg") },
        { QStringLiteral("ms"),  QStringLiteral("devices/dock-mouse-elite-m40.svg") },
        { QStringLiteral("hs"),  QStringLiteral("devices/dock-headset.svg") },
        { QStringLiteral("aio"), QStringLiteral("devices/dock-aio.svg") },
        { QStringLiteral("ssd"), QStringLiteral("devices/dock-ssd.svg") },
        { QStringLiteral("usb"), QStringLiteral("devices/dock-usb.svg") }
    };
    return map.value(deviceId, QStringLiteral("devices/cpu.svg"));
}
