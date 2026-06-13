#pragma once

#include <QPushButton>

class WindowChromeButton : public QPushButton
{
    Q_OBJECT
public:
    enum class Kind { Minimize, Maximize, Restore, Close };

    explicit WindowChromeButton(Kind kind, QWidget* parent = nullptr);

    void setKind(Kind kind);
    Kind kind() const { return m_kind; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void drawIcon(QPainter& p, const QRect& r) const;

    Kind m_kind = Kind::Minimize;
    bool m_hovered = false;
};
