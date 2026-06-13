#include "Mouse3DWidget.h"
#include "../../core/AssetGenerator.h"
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QTransform>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

namespace
{
constexpr const char* kTopAsset = "assets/mouse/elite-m40-top.jpg";
constexpr const char* kSideAsset = "assets/mouse/elite-m40-side.jpg";

QVector3D rotatePoint(const QVector3D& p, float yawRad, float pitchRad)
{
    const float cy = std::cos(yawRad);
    const float sy = std::sin(yawRad);
    const float x1 = p.x() * cy - p.z() * sy;
    const float z1 = p.x() * sy + p.z() * cy;

    const float cp = std::cos(pitchRad);
    const float sp = std::sin(pitchRad);
    const float y2 = p.y() * cp - z1 * sp;
    const float z2 = p.y() * sp + z1 * cp;

    return QVector3D(x1, y2, z2);
}
}

Mouse3DWidget::Mouse3DWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("mouse3dWidget"));
    setMinimumSize(320, 280);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    loadTextures();
}

void Mouse3DWidget::loadTextures()
{
    m_topTexture = AssetGenerator::instance().loadPixmap(kTopAsset);
    m_sideTexture = AssetGenerator::instance().loadPixmap(kSideAsset);
}

void Mouse3DWidget::setRotation(float yawDegrees, float pitchDegrees)
{
    m_yaw = yawDegrees;
    m_pitch = qBound(-15.0f, pitchDegrees, 55.0f);
    update();
}

QPointF Mouse3DWidget::project(const QVector3D& v) const
{
    const float scale = m_zoom * qMin(width(), height()) / 420.0f;
    const float isoX = (v.x() - v.z()) * 0.8660254f;
    const float isoY = (v.x() + v.z()) * 0.5f - v.y();
    return QPointF(isoX * scale, isoY * scale);
}

void Mouse3DWidget::drawTexturedFace(QPainter& painter, const QPolygonF& quad, const QPixmap& texture) const
{
    if (texture.isNull() || quad.size() != 4)
    {
        painter.setPen(QPen(QColor(80, 40, 48, 180), 1.2));
        painter.setBrush(QColor(18, 16, 22, 220));
        painter.drawPolygon(quad);
        return;
    }

    QTransform transform;
    if (QTransform::quadToQuad(QRectF(0, 0, texture.width(), texture.height()), quad, transform))
    {
        painter.save();
        painter.setTransform(transform, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(0, 0, texture);
        painter.restore();
    }

    painter.setPen(QPen(QColor(224, 38, 38, 120), 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(quad);
}

void Mouse3DWidget::drawFloorGrid(QPainter& painter) const
{
    const QPointF center(width() * 0.5, height() * 0.72);
    painter.translate(center);

    QPen gridPen(QColor(52, 152, 219, 35), 1.0);
    painter.setPen(gridPen);

    const int lines = 8;
    const float step = qMin(width(), height()) * 0.055f * m_zoom;
    for (int i = -lines; i <= lines; ++i)
    {
        const float o = i * step;
        painter.drawLine(QPointF(-lines * step, o * 0.5f), QPointF(lines * step, o * 0.5f));
        painter.drawLine(QPointF(o * 0.866f, -lines * step * 0.5f), QPointF(o * 0.866f, lines * step * 0.5f));
    }

    QRadialGradient glow(0, 0, step * 4.0f);
    glow.setColorAt(0.0, QColor(224, 38, 38, 45));
    glow.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(glow);
    painter.drawEllipse(QPointF(0, 0), step * 4.0f, step * 2.0f);
}

void Mouse3DWidget::drawMouseSolid(QPainter& painter) const
{
    const float hx = kLengthMm * 0.5f;
    const float hy = kHeightMm * 0.5f;
    const float hz = kWidthMm * 0.5f;

    const float yawRad = qDegreesToRadians(m_yaw);
    const float pitchRad = qDegreesToRadians(m_pitch);

    const QVector3D corners[8] = {
        { -hx, -hy, -hz }, {  hx, -hy, -hz }, {  hx, -hy,  hz }, { -hx, -hy,  hz },
        { -hx,  hy, -hz }, {  hx,  hy, -hz }, {  hx,  hy,  hz }, { -hx,  hy,  hz }
    };

    QPointF screen[8];
    float depth[8];
    for (int i = 0; i < 8; ++i)
    {
        const QVector3D r = rotatePoint(corners[i], yawRad, pitchRad);
        screen[i] = project(r);
        depth[i] = r.z();
    }

    struct Face {
        int i0, i1, i2, i3;
        const QPixmap* tex;
        float avgDepth;
    };

    Face faces[] = {
        { 4, 5, 6, 7, &m_topTexture, 0 },
        { 0, 1, 5, 4, &m_sideTexture, 0 },
        { 1, 2, 6, 5, nullptr, 0 },
        { 2, 3, 7, 6, nullptr, 0 },
        { 3, 0, 4, 7, nullptr, 0 },
        { 0, 3, 2, 1, nullptr, 0 }
    };

    for (Face& f : faces)
    {
        f.avgDepth = (depth[f.i0] + depth[f.i1] + depth[f.i2] + depth[f.i3]) * 0.25f;
    }

    std::sort(std::begin(faces), std::end(faces), [](const Face& a, const Face& b) {
        return a.avgDepth < b.avgDepth;
    });

    for (const Face& f : faces)
    {
        QPolygonF quad;
        quad << screen[f.i0] << screen[f.i1] << screen[f.i2] << screen[f.i3];

        if (f.tex)
        {
            drawTexturedFace(painter, quad, *f.tex);
        }
        else
        {
            const float shade = 0.35f + 0.15f * (f.avgDepth / hz + 1.0f);
            const int g = int(28 * shade);
            painter.setPen(QPen(QColor(60, 30, 36, 140), 1.0));
            painter.setBrush(QColor(g, g - 4, g + 2, 230));
            painter.drawPolygon(quad);
        }
    }

    QPen edgePen(QColor(224, 38, 38, 160), 1.4);
    painter.setPen(edgePen);
    painter.setBrush(Qt::NoBrush);
    const int edges[][2] = {
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {0, 4}, {1, 5}, {2, 6}, {3, 7 }
    };
    for (const auto& e : edges)
        painter.drawLine(screen[e[0]], screen[e[1]]);

    if (!m_topTexture.isNull())
    {
        const QPointF skullCenter = (screen[4] + screen[5] + screen[6] + screen[7]) * 0.25;
        QRadialGradient skullGlow(skullCenter, 36.0 * m_zoom);
        skullGlow.setColorAt(0.0, QColor(224, 38, 38, 70));
        skullGlow.setColorAt(1.0, QColor(0, 0, 0, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(skullGlow);
        painter.drawEllipse(skullCenter, 36.0 * m_zoom, 18.0 * m_zoom);
    }
}

void Mouse3DWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0.0, QColor(10, 12, 18));
    bg.setColorAt(1.0, QColor(6, 7, 10));
    painter.fillRect(rect(), bg);

    painter.save();
    drawFloorGrid(painter);
    painter.translate(0, -height() * 0.08);
    drawMouseSolid(painter);
    painter.restore();

    QFont label = painter.font();
    label.setPixelSize(12);
    label.setBold(true);
    painter.setFont(label);
    painter.setPen(QColor(140, 150, 170));
    painter.drawText(QRect(12, 10, width() - 24, 20), Qt::AlignLeft,
                     QStringLiteral("PLAN 3D — Elite M40 · glisser pour pivoter · molette = zoom"));

    QFont dim = painter.font();
    dim.setPixelSize(11);
    dim.setBold(false);
    painter.setFont(dim);
    painter.setPen(QColor(100, 110, 130));
    painter.drawText(QRect(12, height() - 28, width() - 24, 20), Qt::AlignRight,
                     QStringLiteral("130 × 72 × 42 mm"));
}

void Mouse3DWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;
        m_lastMousePos = event->pos();
    }
}

void Mouse3DWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_dragging)
        return;

    const QPoint delta = event->pos() - m_lastMousePos;
    m_lastMousePos = event->pos();
    setRotation(m_yaw + delta.x() * 0.45f, m_pitch - delta.y() * 0.35f);
}

void Mouse3DWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
}

void Mouse3DWidget::wheelEvent(QWheelEvent* event)
{
    const float step = event->angleDelta().y() > 0 ? 0.06f : -0.06f;
    m_zoom = qBound(0.65f, m_zoom + step, 1.45f);
    update();
}
