#pragma once

#include <QPixmap>
#include <QPointF>
#include <QVector3D>
#include <QWidget>

class QMouseEvent;
class QPaintEvent;
class QWheelEvent;

class Mouse3DWidget : public QWidget
{
public:
    explicit Mouse3DWidget(QWidget* parent = nullptr);

    void setRotation(float yawDegrees, float pitchDegrees);
    float yaw() const { return m_yaw; }
    float pitch() const { return m_pitch; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void loadTextures();
    QPointF project(const QVector3D& v) const;
    void drawTexturedFace(QPainter& painter, const QPolygonF& quad, const QPixmap& texture) const;
    void drawMouseSolid(QPainter& painter) const;
    void drawFloorGrid(QPainter& painter) const;

    QPixmap m_topTexture;
    QPixmap m_sideTexture;
    float m_yaw = 35.0f;
    float m_pitch = 22.0f;
    float m_zoom = 1.0f;
    bool m_dragging = false;
    QPoint m_lastMousePos;

    static constexpr float kLengthMm = 130.0f;
    static constexpr float kWidthMm = 72.0f;
    static constexpr float kHeightMm = 42.0f;
};
