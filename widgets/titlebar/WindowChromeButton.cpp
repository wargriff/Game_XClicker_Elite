#include "WindowChromeButton.h"
#include <QEnterEvent>
#include <QPainter>

WindowChromeButton::WindowChromeButton(Kind kind, QWidget* parent)
    : QPushButton(parent)
    , m_kind(kind)
{
    setObjectName(QStringLiteral("windowChromeBtn"));
    setProperty("chromeKind", kind == Kind::Close ? "close"
        : kind == Kind::Minimize ? "minimize"
        : kind == Kind::Maximize || kind == Kind::Restore ? "maximize" : "other");
    setCursor(Qt::PointingHandCursor);
    setFlat(true);
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(48, 52);
}

void WindowChromeButton::setKind(Kind kind)
{
    m_kind = kind;
    setProperty("chromeKind", kind == Kind::Close ? "close"
        : kind == Kind::Minimize ? "minimize" : "maximize");
    update();
}

void WindowChromeButton::enterEvent(QEnterEvent* event)
{
    m_hovered = true;
    update();
    QPushButton::enterEvent(event);
}

void WindowChromeButton::leaveEvent(QEvent* event)
{
    m_hovered = false;
    update();
    QPushButton::leaveEvent(event);
}

void WindowChromeButton::drawIcon(QPainter& p, const QRect& r) const
{
    p.setRenderHint(QPainter::Antialiasing);
    const QPoint c = r.center();
    const QColor icon = m_hovered ? QColor(240, 240, 248) : QColor(160, 165, 180);

    if (m_kind == Kind::Minimize)
    {
        p.setPen(QPen(icon, 1.6, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(c.x() - 6, c.y() + 4, c.x() + 6, c.y() + 4);
        return;
    }

    if (m_kind == Kind::Close)
    {
        p.setPen(QPen(m_hovered ? QColor(255, 255, 255) : icon, 1.6, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(c.x() - 5, c.y() - 5, c.x() + 5, c.y() + 5);
        p.drawLine(c.x() + 5, c.y() - 5, c.x() - 5, c.y() + 5);
        return;
    }

    p.setPen(QPen(icon, 1.5));
    p.setBrush(Qt::NoBrush);
    if (m_kind == Kind::Maximize)
    {
        p.drawRect(QRect(c.x() - 5, c.y() - 5, 10, 10));
        return;
    }

    p.drawRect(QRect(c.x() - 2, c.y() - 6, 8, 8));
    p.drawRect(QRect(c.x() - 6, c.y() - 2, 8, 8));
}

void WindowChromeButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), Qt::transparent);

    if (m_hovered)
    {
        const QColor bg = m_kind == Kind::Close ? QColor(224, 38, 38) : QColor(40, 44, 58);
        p.fillRect(rect(), bg);
    }

    drawIcon(p, rect());
}
