#include "MacroEngine.h"
#include "MacroService.h"
#include "../core/AppState.h"
#include "../core/EventBus.h"
#include "../core/Logger.h"
#include "../models/MacroModel.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QMetaObject>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifdef Q_OS_WIN
namespace
{
HHOOK g_mouseHook = nullptr;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_XBUTTONDOWN)
    {
        const auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        const WORD xbtn = HIWORD(info->mouseData);
        int btnIndex = 0;
        if (xbtn == XBUTTON1)
            btnIndex = 1;
        else if (xbtn == XBUTTON2)
            btnIndex = 2;

        if (btnIndex != 0)
        {
            QMetaObject::invokeMethod(
                &MacroEngine::instance(),
                "handleSideButton",
                Qt::QueuedConnection,
                Q_ARG(int, btnIndex));
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}
}
#endif

MacroEngine& MacroEngine::instance()
{
    static MacroEngine engine;
    return engine;
}

MacroEngine::MacroEngine(QObject* parent) : QObject(parent)
{
    m_timer.setInterval(16);
    connect(&m_timer, &QTimer::timeout, this, &MacroEngine::onTick);
    connect(&EventBus::instance(), &EventBus::macroMasterChanged, this, &MacroEngine::onMasterChanged);
}

void MacroEngine::installGlobalMouseHook()
{
#ifdef Q_OS_WIN
    if (g_mouseHook)
        return;
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandleW(nullptr), 0);
    if (g_mouseHook)
        Logger::info(QStringLiteral("Hook souris global actif (L1/L2 partout, meme en jeu)"));
    else
        Logger::info(QStringLiteral("Echec hook souris global — erreur %1").arg(GetLastError()));
#endif
}

void MacroEngine::removeGlobalMouseHook()
{
#ifdef Q_OS_WIN
    if (g_mouseHook)
    {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
#endif
}

void MacroEngine::start()
{
    if (m_running)
        return;

    installGlobalMouseHook();
    m_timer.start();
    m_running = true;
    Logger::info(QStringLiteral("MacroEngine actif — L2 bascule ON/OFF, autoclicks touches 1-2-3-4"));
}

void MacroEngine::stop()
{
    m_timer.stop();
    removeGlobalMouseHook();
    if (!m_running)
        return;
    m_running = false;
    m_lastFireMs.clear();
}

void MacroEngine::handleSideButton(int buttonIndex)
{
    onMouseSideButton(buttonIndex);
}

void MacroEngine::onMasterChanged(bool enabled)
{
    Logger::info(enabled ? QStringLiteral("Macros : ON")
                         : QStringLiteral("Macros : OFF"));
}

void MacroEngine::toggleMaster()
{
    setMasterEnabled(!AppStateStore::instance().state().macroMasterEnabled);
}

void MacroEngine::setMasterEnabled(bool enabled)
{
    auto& st = AppStateStore::instance().state();
    if (st.macroMasterEnabled == enabled)
        return;

    st.macroMasterEnabled = enabled;

    auto& macros = MacroService::instance().activeMacros();
    for (auto& macro : macros)
    {
        if (macro.toggle && macro.device == QStringLiteral("mouse") &&
            (macro.keyLabel == QStringLiteral("L2") || macro.keyLabel == QStringLiteral("L1")))
        {
            macro.active = true;
        }
    }

    emit EventBus::instance().macroMasterChanged(enabled);
    emit masterToggled(enabled);
}

bool MacroEngine::handleSideLabel(const QString& sideLabel)
{
    const auto& macros = MacroService::instance().activeMacros();

    for (const MacroEntry& entry : macros)
    {
        if (entry.device != QStringLiteral("mouse"))
            continue;
        if (entry.keyLabel.compare(sideLabel, Qt::CaseInsensitive) != 0)
            continue;

        if (entry.toggle)
        {
            toggleMaster();
            Logger::info(QStringLiteral("Bouton %1 — macros %2")
                             .arg(sideLabel,
                                  AppStateStore::instance().state().macroMasterEnabled
                                      ? QStringLiteral("ON")
                                      : QStringLiteral("OFF")));
            return true;
        }

        if (!entry.active)
            continue;

        simulateKeyLabel(entry.keyLabel);
        emit macroTriggered(entry.keyLabel);
        return true;
    }
    return false;
}

void MacroEngine::onMouseSideButton(int buttonIndex)
{
    const QString primary = buttonIndex == 1 ? QStringLiteral("L1") : QStringLiteral("L2");
    const QString fallback = buttonIndex == 1 ? QStringLiteral("L2") : QStringLiteral("L1");

    if (handleSideLabel(primary))
        return;
    handleSideLabel(fallback);
}

quint16 MacroEngine::virtualKeyForLabel(const QString& label) const
{
    const QString k = label.trimmed();
    if (k.size() == 1)
    {
        const QChar c = k.at(0).toUpper();
        if (c.isDigit() || c.isLetter())
            return static_cast<quint16>(c.unicode());
    }

    static const QHash<QString, quint16> map = {
        { QStringLiteral("Space"), VK_SPACE },
        { QStringLiteral("Enter"), VK_RETURN },
        { QStringLiteral("Tab"), VK_TAB },
        { QStringLiteral("Esc"), VK_ESCAPE },
        { QStringLiteral("Shift"), VK_SHIFT },
        { QStringLiteral("Ctrl"), VK_CONTROL },
        { QStringLiteral("Alt"), VK_MENU },
        { QStringLiteral("F"), 'F' },
        { QStringLiteral("Q"), 'Q' },
        { QStringLiteral("W"), 'W' },
        { QStringLiteral("R"), 'R' },
    };

    return map.value(k, 0);
}

void MacroEngine::simulateKeyLabel(const QString& label)
{
#ifdef Q_OS_WIN
    const quint16 vk = virtualKeyForLabel(label);
    if (!vk)
        return;

    const UINT scan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    if (!scan)
        return;

    INPUT down {};
    down.type = INPUT_KEYBOARD;
    down.ki.wScan = static_cast<WORD>(scan);
    down.ki.dwFlags = KEYEVENTF_SCANCODE;

    INPUT up {};
    up.type = INPUT_KEYBOARD;
    up.ki.wScan = static_cast<WORD>(scan);
    up.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

    INPUT seq[2] = { down, up };
    SendInput(2, seq, sizeof(INPUT));
#else
    Q_UNUSED(label);
#endif
}

bool MacroEngine::shouldRunMacro(const MacroEntry& entry) const
{
    if (!entry.active || entry.toggle)
        return false;
    if (entry.gatedByMaster)
        return AppStateStore::instance().state().macroMasterEnabled;
    return true;
}

void MacroEngine::onTick()
{
    if (!AppStateStore::instance().state().engineActive)
        return;
    if (!AppStateStore::instance().state().macroMasterEnabled)
        return;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const auto& macros = MacroService::instance().activeMacros();

    for (const MacroEntry& entry : macros)
    {
        if (!shouldRunMacro(entry))
            continue;

        const int interval = qMax(1, entry.delayMs);
        const qint64 last = m_lastFireMs.value(entry.id, 0);
        if (now - last < interval)
            continue;

        simulateKeyLabel(entry.keyLabel);
        m_lastFireMs[entry.id] = now;
    }
}
