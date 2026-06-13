#pragma once

#include "Enums.h"
#include <QString>

struct AppState
{
    NavSection currentSection = NavSection::ProfileManager;
    DeviceTab deviceTab = DeviceTab::Keyboard;
    int selectedDeviceIndex = 2;
    int activeProfileIndex = 0;
    QString activeProfileName = QStringLiteral("Diablo IV - Main");
    QString selectedKeyLabel = QStringLiteral("W");
    QString activeGame = QStringLiteral("Diablo IV");
    bool engineActive = true;
    bool macroMasterEnabled = false;
    float cpuUsage = 0.32f;
    float ramUsage = 0.45f;
    float rgbSpeed = 0.65f;
    float rgbBrightness = 0.80f;
    RgbEffect rgbEffect = RgbEffect::Rainbow;
    int macroCount = 18;
    int functionCount = 5;
    int disabledCount = 37;
};

class AppStateStore
{
public:
    static AppStateStore& instance();
    AppState& state() { return m_state; }
    const AppState& state() const { return m_state; }

private:
    AppStateStore() = default;
    AppState m_state;
};
