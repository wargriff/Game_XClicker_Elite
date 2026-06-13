#include "ClickSoundFilter.h"
#include "../core/UiSound.h"
#include "../widgets/titlebar/WindowChromeButton.h"
#include <QAbstractButton>
#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QMouseEvent>
#include <QTabBar>

ClickSoundFilter::ClickSoundFilter(QObject* parent)
    : QObject(parent)
{
}

bool ClickSoundFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() != QEvent::MouseButtonRelease)
        return QObject::eventFilter(watched, event);

    auto* mouse = static_cast<QMouseEvent*>(event);
    if (mouse->button() != Qt::LeftButton)
        return QObject::eventFilter(watched, event);

    if (auto* chrome = qobject_cast<WindowChromeButton*>(watched))
    {
        if (chrome->kind() == WindowChromeButton::Kind::Close)
            UiSound::close();
        else
            UiSound::click();
        return QObject::eventFilter(watched, event);
    }

    if (qobject_cast<QCheckBox*>(watched))
    {
        UiSound::toggle();
        return QObject::eventFilter(watched, event);
    }

    if (qobject_cast<QAbstractButton*>(watched) || qobject_cast<QTabBar*>(watched))
    {
        UiSound::click();
        return QObject::eventFilter(watched, event);
    }

    return QObject::eventFilter(watched, event);
}
