#pragma once

#include "../core/Enums.h"
#include <QColor>
#include <QFrame>
#include <QRect>
#include <QWidget>

class QPropertyAnimation;
class QTimer;
class QPainter;
class QCloseEvent;
class QPaintEvent;
class QResizeEvent;
class QShowEvent;
class AnimatedStackedWidget;
class PageRegistry;

class NavigationManager;
class TitleBarWidget;
class SidebarWidget;
class DeviceService;
class DeviceCenterPage;
class DeviceDockWidget;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(NavigationManager* nav, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    void setupWindow();
    void setupLayout();
    void setupConnections();
    void setupAnimations();

    void applyStartupAnimation();
    void animateClose();
    void shutdown();

    void drawBackground(QPainter& painter);
    void drawGlow(QPainter& painter);
    void drawBorder(QPainter& painter);
    void drawDockAccent(QPainter& painter);

    void updateRgbTheme();

    void onSectionChanged(NavSection section);
    void onDockDeviceSelected(int index);
    void toggleMaximize();

    NavigationManager* m_nav = nullptr;
    PageRegistry* m_pages = nullptr;
    QWidget* m_shell = nullptr;
    SidebarWidget* m_sidebar = nullptr;
    TitleBarWidget* m_titleBar = nullptr;
    AnimatedStackedWidget* m_stack = nullptr;
    QFrame* m_dockHost = nullptr;
    DeviceDockWidget* m_dock = nullptr;
    DeviceService* m_deviceService = nullptr;

    QPropertyAnimation* m_startupAnimation = nullptr;
    QPropertyAnimation* m_closeAnimation = nullptr;
    QTimer* m_rgbTimer = nullptr;

    QColor m_accentColor = QColor(196, 30, 58);
    int m_rgbHue = 350;
    float m_glowPhase = 0.0f;

    bool m_startupAnimationPlayed = false;
    bool m_closing = false;
    bool m_shuttingDown = false;
    bool m_maximized = false;
    QRect m_restoreGeometry;
};
