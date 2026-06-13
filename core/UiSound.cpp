#include "UiSound.h"

#include <QtGlobal>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace
{
bool g_enabled = true;

void playAlias(const wchar_t* alias)
{
    if (!g_enabled)
        return;
#ifdef Q_OS_WIN
    PlaySoundW(alias, nullptr, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
#else
    Q_UNUSED(alias);
#endif
}
}

void UiSound::setEnabled(bool enabled)
{
    g_enabled = enabled;
}

bool UiSound::isEnabled()
{
    return g_enabled;
}

void UiSound::click()
{
    playAlias(L"MenuCommand");
}

void UiSound::toggle()
{
    playAlias(L"DeviceConnect");
}

void UiSound::success()
{
    playAlias(L"MailBeep");
}

void UiSound::close()
{
    playAlias(L"Close");
}
