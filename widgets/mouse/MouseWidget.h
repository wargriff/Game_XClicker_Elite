#pragma once

#include <QColor>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QRectF>
#include <QString>
#include <QVector>
#include <QWidget>

class MouseWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal pulsePhase READ pulsePhase WRITE setPulsePhase)

public:
    explicit MouseWidget(QWidget* parent = nullptr);

    QString selectedButton() const { return m_selectedButton; }
    void selectButtonByLabel(const QString& label);
    void setMacroMasterEnabled(bool enabled);

signals:
    void buttonSelected(const QString& button);
    void macroToggleRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    qreal pulsePhase() const { return m_pulsePhase; }
    void setPulsePhase(qreal v);

    int hitTest(const QPoint& p) const;
    int indexForLabel(const QString& label) const;
    QRectF imageRect() const;
    QRectF zoneRect(int index, const QRectF& imageArea) const;
    void loadPhoto();
    void applySelection(int index);

    QString m_selectedButton = QStringLiteral("L2");
    int m_selectedIndex = 4;
    int m_hoveredIndex = -1;
    bool m_macroMasterEnabled = false;
    qreal m_pulsePhase = 0.0;
    QPropertyAnimation* m_pulseAnim = nullptr;
    QPixmap m_topPhoto;

    struct BtnZone {
        QRectF normRect;
        QString label;
        QColor accent;
        bool macroSide = false;
    };
    QVector<BtnZone> m_zones;
};
