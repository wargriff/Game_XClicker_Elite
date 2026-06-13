#include "AnimatedStackedWidget.h"
#include <QAbstractAnimation>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QWidget>

AnimatedStackedWidget::AnimatedStackedWidget(QWidget* parent)
    : QStackedWidget(parent)
{
    setObjectName(QStringLiteral("animatedStack"));
    setAttribute(Qt::WA_StyledBackground, true);
}

void AnimatedStackedWidget::setCurrentIndexImmediate(int index)
{
    if (index < 0 || index >= count())
        return;

    m_animating = false;
    m_pendingIndex = -1;
    m_activeAnimation = nullptr;

    if (QWidget* w = currentWidget())
        w->setGraphicsEffect(nullptr);

    QStackedWidget::setCurrentIndex(index);

    if (QWidget* w = widget(index))
        w->setGraphicsEffect(nullptr);
}

void AnimatedStackedWidget::animateToIndex(int index, Transition transition)
{
    if (index < 0 || index >= count())
        return;
    if (index == currentIndex() && !m_animating)
        return;

    if (m_animating)
    {
        m_pendingIndex = index;
        return;
    }

    Q_UNUSED(transition);
    runFadeTransition(index);
}

void AnimatedStackedWidget::finishTransition(int index)
{
    m_animating = false;
    m_activeAnimation = nullptr;

    if (QWidget* w = widget(index))
        w->setGraphicsEffect(nullptr);

    if (m_pendingIndex >= 0 && m_pendingIndex != index)
    {
        const int next = m_pendingIndex;
        m_pendingIndex = -1;
        animateToIndex(next, Fade);
    }
}

void AnimatedStackedWidget::runFadeTransition(int index)
{
    m_animating = true;
    QStackedWidget::setCurrentIndex(index);

    QWidget* page = widget(index);
    if (!page)
    {
        finishTransition(index);
        return;
    }

    auto* effect = new QGraphicsOpacityEffect(page);
    effect->setOpacity(0.0);
    page->setGraphicsEffect(effect);

    auto* anim = new QPropertyAnimation(effect, "opacity", this);
    m_activeAnimation = anim;
    anim->setDuration(200);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QPropertyAnimation::finished, this, [this, index, page]() {
        page->setGraphicsEffect(nullptr);
        finishTransition(index);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimatedStackedWidget::runSlideTransition(int index)
{
    runFadeTransition(index);
}

void AnimatedStackedWidget::runZoomTransition(int index)
{
    runFadeTransition(index);
}
