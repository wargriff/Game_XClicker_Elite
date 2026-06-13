#include "MouseWidget.h"
#include "../../core/AssetGenerator.h"
#include "../../core/AppState.h"
#include "../../core/EventBus.h"
#include "../../core/Logger.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QSvgRenderer>
#include <QtMath>

namespace
{
constexpr const char* kHeroPng = "previews/mouse-elite-m40-4k.png";
constexpr const char* kHeroSvg = "assets/mouse/elite-m40-hero.svg";
constexpr const char* kTopPhotoAsset = "assets/mouse/elite-m40-top.jpg";
constexpr QColor kBlue(52, 152, 219);
constexpr QColor kBlueBright(96, 180, 255);
constexpr QColor kOrange(255, 140, 42);
constexpr QColor kRedMacro(224, 38, 38);
constexpr QColor kGreenOn(46, 204, 113);

void drawButtonLabel(QPainter& p, const QRectF& r, const QString& text, const QColor& color, int pxSize)
{
    QFont f = p.font();
    f.setPixelSize(pxSize);
    f.setBold(true);
    p.setFont(f);
    p.setPen(color);
    p.drawText(r, Qt::AlignCenter, text);
}

void drawOnOffBadge(QPainter& p, const QRectF& zone, bool on)
{
    const QRectF badge(zone.center().x() - zone.width() * 0.32,
                       zone.center().y() - zone.height() * 0.12,
                       zone.width() * 0.64,
                       zone.height() * 0.48);

    const QColor bg = on ? kGreenOn : QColor(48, 48, 62);
    const QColor fg = on ? QColor(6, 22, 10) : QColor(200, 200, 210);

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawRoundedRect(badge, 7, 7);

    QFont f = p.font();
    f.setPixelSize(qMax(12, int(badge.height() * 0.52)));
    f.setBold(true);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 1.2);
    p.setFont(f);
    p.setPen(fg);
    p.drawText(badge, Qt::AlignCenter, on ? QStringLiteral("ON") : QStringLiteral("OFF"));
}
}

MouseWidget::MouseWidget(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("mouseWidget"));
    setMinimumSize(640, 720);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);

    m_macroMasterEnabled = AppStateStore::instance().state().macroMasterEnabled;

    m_zones = {
        { QRectF(0.34, 0.24, 0.18, 0.28), QStringLiteral("Gauche"), kBlue, false },
        { QRectF(0.51, 0.24, 0.18, 0.28), QStringLiteral("Droit"), kBlue, false },
        { QRectF(0.46, 0.20, 0.08, 0.34), QStringLiteral("Molette"), kBlueBright, false },
        { QRectF(0.26, 0.36, 0.08, 0.11), QStringLiteral("L1"), kOrange, true },
        { QRectF(0.26, 0.48, 0.08, 0.11), QStringLiteral("L2"), kOrange, true },
        { QRectF(0.42, 0.58, 0.16, 0.12), QStringLiteral("DPI"), kBlueBright, false }
    };

    loadPhoto();

    m_pulseAnim = new QPropertyAnimation(this, "pulsePhase", this);
    m_pulseAnim->setDuration(900);
    m_pulseAnim->setStartValue(0.0);
    m_pulseAnim->setEndValue(1.0);
    m_pulseAnim->setLoopCount(-1);
    m_pulseAnim->start();

    connect(&EventBus::instance(), &EventBus::macroMasterChanged, this, [this](bool on) {
        setMacroMasterEnabled(on);
    });
}

void MouseWidget::loadPhoto()
{
    m_topPhoto = AssetGenerator::instance().loadPixmap(kHeroPng);
    if (m_topPhoto.isNull())
        m_topPhoto = AssetGenerator::instance().pixmap(kHeroSvg, QSize(1920, 1080));
    if (m_topPhoto.isNull())
        m_topPhoto = AssetGenerator::instance().loadPixmap(kTopPhotoAsset);
}

void MouseWidget::setMacroMasterEnabled(bool enabled)
{
    m_macroMasterEnabled = enabled;
    update();
}

void MouseWidget::setPulsePhase(qreal v)
{
    m_pulsePhase = v;
    update();
}

QRectF MouseWidget::imageRect() const
{
    const float margin = 20.f;
    const float bottomReserve = 48.f;

    if (m_topPhoto.isNull())
    {
        const float w = 420.f;
        const float h = 580.f;
        const float sx = (width() - margin * 2.f) / w;
        const float sy = (height() - bottomReserve - margin) / h;
        const float s = std::min(sx, sy) * 1.08f;
        return QRectF((width() - w * s) * 0.5f, (height() - h * s) * 0.5f - 12.f, w * s, h * s);
    }

    const float sx = (width() - margin * 2.f) / m_topPhoto.width();
    const float sy = (height() - bottomReserve - margin) / m_topPhoto.height();
    const float s = std::min(sx, sy);
    const float pw = m_topPhoto.width() * s;
    const float ph = m_topPhoto.height() * s;
    return QRectF((width() - pw) * 0.5f, margin, pw, ph);
}

QRectF MouseWidget::zoneRect(int index, const QRectF& imageArea) const
{
    const BtnZone& z = m_zones[index];
    return QRectF(imageArea.x() + z.normRect.x() * imageArea.width(),
                  imageArea.y() + z.normRect.y() * imageArea.height(),
                  z.normRect.width() * imageArea.width(),
                  z.normRect.height() * imageArea.height());
}

int MouseWidget::indexForLabel(const QString& label) const
{
    for (int i = 0; i < m_zones.size(); ++i)
    {
        if (m_zones[i].label.compare(label, Qt::CaseInsensitive) == 0)
            return i;
    }
    return -1;
}

void MouseWidget::selectButtonByLabel(const QString& label)
{
    const int idx = indexForLabel(label);
    if (idx >= 0)
        applySelection(idx);
}

int MouseWidget::hitTest(const QPoint& p) const
{
    const QRectF ir = imageRect();
    for (int i = m_zones.size() - 1; i >= 0; --i)
    {
        if (zoneRect(i, ir).contains(p))
            return i;
    }
    return -1;
}

void MouseWidget::applySelection(int index)
{
    if (index < 0 || index >= m_zones.size())
        return;

    m_selectedIndex = index;
    m_selectedButton = m_zones[index].label;
    Logger::debug(QStringLiteral("Elite M40 — bouton: %1").arg(m_selectedButton));
    emit buttonSelected(m_selectedButton);
    update();
}

void MouseWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0.0, QColor(14, 16, 24));
    bg.setColorAt(1.0, QColor(8, 9, 14));
    p.fillRect(rect(), bg);

    const QRectF ir = imageRect();
    const float scaleHint = ir.width() / 420.f;

    p.setPen(QPen(QColor(224, 38, 38, 80), 1.5));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(ir.adjusted(-8, -8, 8, 8), 18, 18);

    if (!m_topPhoto.isNull())
    {
        p.drawPixmap(ir.toRect(), m_topPhoto);
    }
    else
    {
        const QString path = AssetGenerator::instance().resolve(QStringLiteral("devices/mouse-elite-m40.svg"));
        QSvgRenderer renderer(path);
        if (renderer.isValid())
            renderer.render(&p, ir);
    }

    const qreal pulse = 0.5 + 0.5 * std::sin(m_pulsePhase * 6.28318);

    for (int i = 0; i < m_zones.size(); ++i)
    {
        const BtnZone& z = m_zones[i];
        const QRectF r = zoneRect(i, ir);
        const bool selected = (i == m_selectedIndex);
        const bool hovered = (i == m_hoveredIndex);
        const bool isL2 = z.label == QStringLiteral("L2");
        const bool l2Running = isL2 && m_macroMasterEnabled;

        QColor accent = z.macroSide ? kOrange : z.accent;
        if (l2Running)
            accent = kGreenOn;
        else if (selected && z.macroSide)
            accent = kRedMacro;
        else if (selected || hovered)
            accent = z.macroSide ? kOrange : kBlueBright;

        const bool highlight = selected || hovered || l2Running;
        const int fillAlpha = l2Running ? int(100 + 40 * pulse) : (highlight ? int(50 + 20 * pulse) : 18);
        const int penW = l2Running ? 4 : (highlight ? 3 : 1);

        p.setPen(QPen(accent, penW));
        p.setBrush(QColor(accent.red(), accent.green(), accent.blue(), fillAlpha));
        p.drawRoundedRect(r.adjusted(1, 1, -1, -1), 10, 10);

        if (highlight)
        {
            p.setPen(QPen(accent, 1, Qt::DotLine));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(r.adjusted(-5, -5, 5, 5), 12, 12);
        }

        if (isL2)
        {
            drawOnOffBadge(p, r, m_macroMasterEnabled);
            drawButtonLabel(p, r.adjusted(0, r.height() * 0.62, 0, 0), QStringLiteral("L2"),
                            l2Running ? kGreenOn.lighter(130) : kOrange, qMax(10, int(11 * scaleHint)));
        }
        else if (z.label == QStringLiteral("L1"))
        {
            drawButtonLabel(p, r, QStringLiteral("L1"), kOrange.lighter(120), qMax(10, int(12 * scaleHint)));
        }
        else if (selected || hovered)
        {
            const int labelPx = qMax(9, int(10 * scaleHint));
            drawButtonLabel(p, r, z.label, QColor(220, 230, 245), labelPx);
        }
    }

    QFont statusFont = p.font();
    statusFont.setPixelSize(14);
    statusFont.setBold(true);
    p.setFont(statusFont);

    const BtnZone& sel = m_zones[m_selectedIndex];
    QString status;
    if (sel.label == QStringLiteral("L2"))
    {
        status = m_macroMasterEnabled
            ? QStringLiteral("L2 ACTIF — autoclicks 1·2·3·4 en cours (clic = OFF)")
            : QStringLiteral("L2 — cliquez pour activer les macros");
    }
    else if (sel.macroSide)
    {
        status = QStringLiteral("Bouton %1 — assignation macro").arg(sel.label);
    }
    else
    {
        status = QStringLiteral("Bouton %1 — Elite M40").arg(sel.label);
    }

    p.setPen(QColor(160, 170, 190));
    p.drawText(QRectF(12, height() - 40, width() - 24, 28), Qt::AlignCenter | Qt::TextWordWrap, status);
}

void MouseWidget::mousePressEvent(QMouseEvent* e)
{
    const int idx = hitTest(e->pos());
    if (idx < 0)
        return;

    const bool isL2 = m_zones[idx].label == QStringLiteral("L2");
    applySelection(idx);

    if (isL2)
        emit macroToggleRequested();
}

void MouseWidget::mouseMoveEvent(QMouseEvent* e)
{
    const int idx = hitTest(e->pos());
    if (idx != m_hoveredIndex)
    {
        m_hoveredIndex = idx;
        setCursor(idx >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void MouseWidget::leaveEvent(QEvent*)
{
    m_hoveredIndex = -1;
    unsetCursor();
    update();
}
