#include "MainWindow.h"
#include "../app/PageRegistry.h"
#include "../widgets/stack/AnimatedStackedWidget.h"
#include "../widgets/sidebar/SidebarWidget.h"
#include "../widgets/titlebar/TitleBarWidget.h"
#include "../widgets/dock/DeviceDockWidget.h"
#include "../services/DeviceService.h"
#include "../app/NavigationManager.h"
#include "../core/Constants.h"
#include "../core/AppState.h"
#include "../pages/DeviceCenter/DeviceCenterPage.h"
#include "../services/MacroEngine.h"
#include <QCloseEvent>
#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QPropertyAnimation>
#include <QRadialGradient>
#include <QScreen>
#include <QShowEvent>
#include <QEasingCurve>
#include <QTimer>
#include <QVBoxLayout>
#include <cmath>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <windowsx.h>
#endif

namespace
{
constexpr int kWindowRadius = 12;
constexpr int kShellMargin = 4;
}

MainWindow::MainWindow(NavigationManager* nav, QWidget* parent)
    : QWidget(parent)
    , m_nav(nav)
{
    setObjectName(QStringLiteral("mainWindow"));
    m_deviceService = new DeviceService(this);

    setupWindow();
    setupLayout();
    setupConnections();
    setupAnimations();

    onSectionChanged(NavSection::ProfileManager);
}

void MainWindow::setupWindow()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);

    setMinimumSize(Gx::Layout::WindowMinWidth, Gx::Layout::WindowMinHeight);
    resize(Gx::Layout::WindowWidth, Gx::Layout::WindowHeight);
    setWindowTitle(Gx::App::Name);
    setWindowOpacity(0.0);

    m_accentColor = QColor::fromHsv(m_rgbHue, 210, 235);
}

void MainWindow::setupLayout()
{
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(kShellMargin, kShellMargin, kShellMargin, kShellMargin);
    outer->setSpacing(0);

    m_shell = new QWidget(this);
    m_shell->setObjectName(QStringLiteral("mainShell"));
    m_shell->setAttribute(Qt::WA_StyledBackground, true);
    outer->addWidget(m_shell);

    auto* root = new QHBoxLayout(m_shell);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_sidebar = new SidebarWidget(m_nav, m_shell);
    root->addWidget(m_sidebar);

    auto* mainColumnHost = new QWidget(m_shell);
    mainColumnHost->setObjectName(QStringLiteral("mainColumnHost"));
    auto* mainCol = new QVBoxLayout(mainColumnHost);
    mainCol->setContentsMargins(0, 0, 0, 0);
    mainCol->setSpacing(0);

    m_titleBar = new TitleBarWidget(mainColumnHost);
    mainCol->addWidget(m_titleBar);

    m_stack = new AnimatedStackedWidget(mainColumnHost);
    mainCol->addWidget(m_stack, 1);

    m_pages = new PageRegistry(m_shell, m_stack, this);

    m_dockHost = new QFrame(mainColumnHost);
    m_dockHost->setObjectName(QStringLiteral("floatingDock"));
    m_dockHost->setAttribute(Qt::WA_StyledBackground, true);
    auto* dockLayout = new QVBoxLayout(m_dockHost);
    dockLayout->setContentsMargins(16, 10, 16, 14);
    dockLayout->setSpacing(0);

    m_dock = new DeviceDockWidget(m_deviceService, m_dockHost);
    m_dock->setObjectName(QStringLiteral("deviceDockPremium"));
    dockLayout->addWidget(m_dock);

    mainCol->addWidget(m_dockHost);
    root->addWidget(mainColumnHost, 1);
}

void MainWindow::setupConnections()
{
    connect(m_nav, &NavigationManager::sectionChanged, this, &MainWindow::onSectionChanged);
    connect(m_dock, &DeviceDockWidget::deviceSelected, this, &MainWindow::onDockDeviceSelected);
    connect(m_titleBar, &TitleBarWidget::closeClicked, this, &MainWindow::animateClose);
    connect(m_titleBar, &TitleBarWidget::minimizeClicked, this, &QWidget::showMinimized);
    connect(m_titleBar, &TitleBarWidget::maximizeClicked, this, &MainWindow::toggleMaximize);
}

void MainWindow::setupAnimations()
{
    m_rgbTimer = new QTimer(this);
    m_rgbTimer->setInterval(50);
    connect(m_rgbTimer, &QTimer::timeout, this, &MainWindow::updateRgbTheme);
    m_rgbTimer->start();

    m_startupAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_startupAnimation->setDuration(700);
    m_startupAnimation->setStartValue(0.0);
    m_startupAnimation->setEndValue(1.0);
    m_startupAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_closeAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_closeAnimation->setDuration(250);
    m_closeAnimation->setStartValue(windowOpacity());
    m_closeAnimation->setEndValue(0.0);
    m_closeAnimation->setEasingCurve(QEasingCurve::InCubic);
    connect(m_closeAnimation, &QPropertyAnimation::finished, this, [this]() {
        shutdown();
        QApplication::quit();
    });
}

void MainWindow::shutdown()
{
    if (m_shuttingDown)
        return;
    m_shuttingDown = true;
    m_closing = true;

    if (m_rgbTimer)
        m_rgbTimer->stop();
    if (m_closeAnimation)
        m_closeAnimation->stop();
    if (m_startupAnimation)
        m_startupAnimation->stop();

    MacroEngine::instance().stop();
}

void MainWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!m_startupAnimationPlayed)
    {
        m_startupAnimationPlayed = true;
        applyStartupAnimation();
    }
}

void MainWindow::applyStartupAnimation()
{
    if (!m_startupAnimation)
        return;

    setWindowOpacity(0.0);
    m_startupAnimation->stop();
    m_startupAnimation->start();
}

void MainWindow::animateClose()
{
    if (m_closing || !m_closeAnimation)
        return;

    if (m_rgbTimer)
        m_rgbTimer->stop();

    m_closeAnimation->setStartValue(windowOpacity());
    m_closeAnimation->stop();
    m_closeAnimation->start();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (!m_closing)
    {
        event->ignore();
        animateClose();
        return;
    }

    shutdown();
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}

void MainWindow::updateRgbTheme()
{
    m_rgbHue = (m_rgbHue + 1) % 360;
    m_glowPhase += 0.03f;
    if (m_glowPhase > 6.28318f)
        m_glowPhase -= 6.28318f;

    m_accentColor.setHsv(m_rgbHue, 210, 235);
    update();
}

void MainWindow::drawBackground(QPainter& painter)
{
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0.0, QColor(5, 5, 5));
    gradient.setColorAt(0.45, QColor(16, 16, 16));
    gradient.setColorAt(1.0, QColor(24, 24, 24));
    painter.fillRect(rect(), gradient);

    QLinearGradient vignette(width() * 0.5, 0, width() * 0.5, height());
    vignette.setColorAt(0.0, QColor(0, 0, 0, 0));
    vignette.setColorAt(1.0, QColor(0, 0, 0, 140));
    painter.fillRect(rect(), vignette);
}

void MainWindow::drawGlow(QPainter& painter)
{
    const float pulse = 0.65f + 0.35f * std::sin(m_glowPhase);
    const QPointF center(width() * 0.72, height() * 0.22);
    const qreal radius = qMax(width(), height()) * (0.42 + 0.04 * pulse);

    QRadialGradient halo(center, radius);
    QColor core = m_accentColor;
    core.setAlpha(int(90 * pulse));
    QColor mid = m_accentColor;
    mid.setAlpha(int(38 * pulse));
    halo.setColorAt(0.0, core);
    halo.setColorAt(0.35, mid);
    halo.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(rect(), halo);
}

void MainWindow::drawBorder(QPainter& painter)
{
    const QRectF frame = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    QColor border = m_accentColor;
    border.setAlpha(180);

    painter.setPen(QPen(border, 2.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(frame, kWindowRadius, kWindowRadius);
}

void MainWindow::drawDockAccent(QPainter& painter)
{
    if (!m_dockHost || !m_dockHost->isVisible())
        return;

    const QRectF dock = m_dockHost->geometry();
    const QPointF topLeft = m_shell->mapTo(this, dock.topLeft());
    const QRectF frame = QRectF(topLeft, dock.size()).adjusted(-1, -1, 1, 1);

    QColor border = m_accentColor;
    border.setAlpha(140);
    painter.setPen(QPen(border, 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(frame, 14, 14);
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawGlow(painter);
    drawBorder(painter);
    drawDockAccent(painter);
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        auto* msg = static_cast<MSG*>(message);
        if (msg->message == WM_NCHITTEST)
        {
            const LONG border = Gx::Layout::ResizeBorder;
            const POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };

            RECT winRect;
            GetWindowRect(reinterpret_cast<HWND>(winId()), &winRect);

            const bool left   = pt.x >= winRect.left && pt.x < winRect.left + border;
            const bool right  = pt.x < winRect.right && pt.x >= winRect.right - border;
            const bool top    = pt.y >= winRect.top && pt.y < winRect.top + border;
            const bool bottom = pt.y < winRect.bottom && pt.y >= winRect.bottom - border;

            if (top && left)   { *result = HTTOPLEFT;  return true; }
            if (top && right)  { *result = HTTOPRIGHT; return true; }
            if (bottom && left)  { *result = HTBOTTOMLEFT;  return true; }
            if (bottom && right) { *result = HTBOTTOMRIGHT; return true; }
            if (left)   { *result = HTLEFT;   return true; }
            if (right)  { *result = HTRIGHT;  return true; }
            if (top)    { *result = HTTOP;    return true; }
            if (bottom) { *result = HTBOTTOM; return true; }
        }
    }
#else
    Q_UNUSED(eventType)
    Q_UNUSED(message)
    Q_UNUSED(result)
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void MainWindow::toggleMaximize()
{
    if (m_maximized)
    {
        setGeometry(m_restoreGeometry);
        m_maximized = false;
    }
    else
    {
        m_restoreGeometry = geometry();
        if (QScreen* scr = screen())
            setGeometry(scr->availableGeometry());
        m_maximized = true;
    }
    if (m_titleBar)
        m_titleBar->setMaximizedState(m_maximized);
}

void MainWindow::onSectionChanged(NavSection section)
{
    AppStateStore::instance().state().currentSection = section;
    if (!m_stack || !m_pages)
        return;

    const int index = m_pages->indexFor(section);
    const auto transition = m_pages->transitionFor(section);
    m_stack->animateToIndex(index, transition);
}

void MainWindow::onDockDeviceSelected(int index)
{
    if (!m_deviceService || !m_dock || !m_nav || !m_pages)
        return;

    AppStateStore::instance().state().selectedDeviceIndex = index;
    const auto& device = m_deviceService->model().deviceAt(index);
    const QString& id = device.id;

    if (id == QStringLiteral("kb") || id == QStringLiteral("ms"))
    {
        m_nav->navigateTo(NavSection::DeviceCenter);
        if (DeviceCenterPage* dc = m_pages->deviceCenter())
            dc->focusDevice(id);
    }
    else if (id == QStringLiteral("hs") || id == QStringLiteral("aio"))
    {
        m_nav->navigateTo(NavSection::LightingEngine);
    }
    else
    {
        m_nav->navigateTo(NavSection::DeviceCenter);
    }
}
