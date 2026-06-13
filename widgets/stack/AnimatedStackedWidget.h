#pragma once

#include <QStackedWidget>
#include <QWidget>

class QPropertyAnimation;

class AnimatedStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    enum Transition
    {
        Fade = 0,
        Slide = 1,
        Zoom = 2
    };

    explicit AnimatedStackedWidget(QWidget* parent = nullptr);

    void animateToIndex(int index, Transition transition = Fade);
    void setCurrentIndexImmediate(int index);
    bool isAnimating() const { return m_animating; }

private:
    void finishTransition(int index);
    void runFadeTransition(int index);
    void runSlideTransition(int index);
    void runZoomTransition(int index);

    bool m_animating = false;
    int m_pendingIndex = -1;
    QPropertyAnimation* m_activeAnimation = nullptr;
};
