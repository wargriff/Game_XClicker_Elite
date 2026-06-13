#pragma once

#include <QPoint>
#include <QWidget>

class QLabel;
class QComboBox;
class QProgressBar;
class WindowChromeButton;

class TitleBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBarWidget(QWidget* parent = nullptr);
    void setMaximizedState(bool maximized);

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void buildUi();
    void refresh();
    void reloadGameCombo();
    bool isInteractiveTarget(QWidget* target) const;

    QLabel* m_appNameLabel = nullptr;
    QLabel* m_sectionLabel = nullptr;
    QComboBox* m_gameCombo = nullptr;
    QLabel* m_engineBadge = nullptr;
    QProgressBar* m_cpuBar = nullptr;
    QProgressBar* m_ramBar = nullptr;
    WindowChromeButton* m_maxBtn = nullptr;
    bool m_dragging = false;
    bool m_maximized = false;
    QPoint m_dragOffset;
};
