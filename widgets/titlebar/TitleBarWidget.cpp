#include "TitleBarWidget.h"
#include "WindowChromeButton.h"
#include "../../core/Constants.h"
#include "../../core/Enums.h"
#include "../../core/AppState.h"
#include "../../core/AssetGenerator.h"
#include "../../core/EventBus.h"
#include "../../services/ProfileService.h"
#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMouseEvent>
#include <QProgressBar>
#include <QStyle>
#include <QVBoxLayout>

TitleBarWidget::TitleBarWidget(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("titleBar"));
    setFixedHeight(Gx::Layout::TitleBarHeight);
    buildUi();

    connect(&EventBus::instance(), &EventBus::profileChanged, this, [this](int) { refresh(); });
    connect(&EventBus::instance(), &EventBus::sectionChanged, this, [this](NavSection) { refresh(); });
    connect(&EventBus::instance(), &EventBus::macroMasterChanged, this, [this](bool) { refresh(); });
}

void TitleBarWidget::buildUi()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* brand = new QFrame(this);
    brand->setObjectName(QStringLiteral("titleBarBrand"));
    auto* brandLayout = new QHBoxLayout(brand);
    brandLayout->setContentsMargins(14, 0, 16, 0);
    brandLayout->setSpacing(10);

    auto* logo = new QLabel(brand);
    logo->setObjectName(QStringLiteral("titleBarLogo"));
    logo->setFixedSize(28, 28);
    logo->setPixmap(AssetGenerator::instance().pixmap(QStringLiteral("assets/branding/app-mark.svg"), QSize(28, 28)));
    logo->setScaledContents(true);

    auto* titles = new QVBoxLayout();
    titles->setContentsMargins(0, 0, 0, 0);
    titles->setSpacing(0);

    m_appNameLabel = new QLabel(Gx::App::Name, brand);
    m_appNameLabel->setObjectName(QStringLiteral("titleBarAppName"));

    m_sectionLabel = new QLabel(brand);
    m_sectionLabel->setObjectName(QStringLiteral("titleBarSection"));

    titles->addWidget(m_appNameLabel);
    titles->addWidget(m_sectionLabel);
    brandLayout->addWidget(logo);
    brandLayout->addLayout(titles);
    layout->addWidget(brand);

    layout->addStretch(1);

    auto* strip = new QFrame(this);
    strip->setObjectName(QStringLiteral("titleBarStrip"));
    auto* stripLayout = new QHBoxLayout(strip);
    stripLayout->setContentsMargins(14, 6, 14, 6);
    stripLayout->setSpacing(14);

    auto* engineBlock = new QFrame(strip);
    engineBlock->setObjectName(QStringLiteral("titleBarEngine"));
    auto* engineLayout = new QHBoxLayout(engineBlock);
    engineLayout->setContentsMargins(10, 4, 10, 4);
    engineLayout->setSpacing(8);

    auto* engineTitle = new QLabel(QStringLiteral("MOTEUR"), engineBlock);
    engineTitle->setObjectName(QStringLiteral("titleBarEngineLabel"));
    m_engineBadge = new QLabel(QStringLiteral("PRET"), engineBlock);
    m_engineBadge->setObjectName(QStringLiteral("titleBarEngineBadge"));

    auto* meters = new QVBoxLayout();
    meters->setSpacing(3);
    auto addMeter = [&](const QString& label, QProgressBar*& bar, const char* accent) {
        auto* row = new QHBoxLayout();
        row->setSpacing(6);
        auto* lb = new QLabel(label, engineBlock);
        lb->setObjectName(QStringLiteral("titleBarMeterLabel"));
        bar = new QProgressBar(engineBlock);
        bar->setObjectName(QStringLiteral("titleBarMeterBar"));
        bar->setProperty("accent", accent);
        bar->setRange(0, 100);
        bar->setFixedSize(64, 4);
        bar->setTextVisible(false);
        row->addWidget(lb);
        row->addWidget(bar);
        meters->addLayout(row);
    };
    addMeter(QStringLiteral("CPU"), m_cpuBar, "cpu");
    addMeter(QStringLiteral("RAM"), m_ramBar, "ram");

    auto* ver = new QLabel(Gx::App::Version, engineBlock);
    ver->setObjectName(QStringLiteral("titleBarVersion"));

    engineLayout->addWidget(engineTitle);
    engineLayout->addWidget(m_engineBadge);
    engineLayout->addLayout(meters);
    engineLayout->addWidget(ver);
    stripLayout->addWidget(engineBlock);

    auto* sep = new QFrame(strip);
    sep->setObjectName(QStringLiteral("titleBarSep"));
    sep->setFixedWidth(1);
    sep->setFixedHeight(32);
    stripLayout->addWidget(sep);

    auto* profileBlock = new QHBoxLayout();
    profileBlock->setSpacing(8);
    auto* gameLabel = new QLabel(QStringLiteral("PROFIL"), strip);
    gameLabel->setObjectName(QStringLiteral("titleBarGameLabel"));
    m_gameCombo = new QComboBox(strip);
    m_gameCombo->setObjectName(QStringLiteral("titleBarGameCombo"));
    m_gameCombo->setMinimumWidth(240);
    m_gameCombo->setMinimumHeight(30);
    if (auto* view = qobject_cast<QListView*>(m_gameCombo->view()))
    {
        view->setTextElideMode(Qt::ElideNone);
        view->setMinimumWidth(300);
    }
    connect(m_gameCombo, QOverload<int>::of(&QComboBox::activated), this, [](int index) {
        ProfileService::instance().applyProfile(index);
    });
    profileBlock->addWidget(gameLabel);
    profileBlock->addWidget(m_gameCombo);
    stripLayout->addLayout(profileBlock);

    layout->addWidget(strip);

    auto* chrome = new QFrame(this);
    chrome->setObjectName(QStringLiteral("titleBarChrome"));
    auto* chromeLayout = new QHBoxLayout(chrome);
    chromeLayout->setContentsMargins(0, 0, 0, 0);
    chromeLayout->setSpacing(0);

    auto* minBtn = new WindowChromeButton(WindowChromeButton::Kind::Minimize, chrome);
    m_maxBtn = new WindowChromeButton(WindowChromeButton::Kind::Maximize, chrome);
    auto* closeBtn = new WindowChromeButton(WindowChromeButton::Kind::Close, chrome);

    minBtn->setToolTip(QStringLiteral("Reduire"));
    m_maxBtn->setToolTip(QStringLiteral("Plein ecran"));
    closeBtn->setToolTip(QStringLiteral("Fermer"));

    connect(minBtn, &QPushButton::clicked, this, &TitleBarWidget::minimizeClicked);
    connect(m_maxBtn, &QPushButton::clicked, this, &TitleBarWidget::maximizeClicked);
    connect(closeBtn, &QPushButton::clicked, this, &TitleBarWidget::closeClicked);

    chromeLayout->addWidget(minBtn);
    chromeLayout->addWidget(m_maxBtn);
    chromeLayout->addWidget(closeBtn);
    layout->addWidget(chrome);

    reloadGameCombo();
    refresh();
}

void TitleBarWidget::setMaximizedState(bool maximized)
{
    m_maximized = maximized;
    if (m_maxBtn)
    {
        m_maxBtn->setKind(maximized ? WindowChromeButton::Kind::Restore
                                    : WindowChromeButton::Kind::Maximize);
        m_maxBtn->setToolTip(maximized ? QStringLiteral("Restaurer")
                                       : QStringLiteral("Plein ecran"));
    }
}

bool TitleBarWidget::isInteractiveTarget(QWidget* target) const
{
    while (target && target != this)
    {
        if (qobject_cast<QPushButton*>(target) || qobject_cast<QComboBox*>(target))
            return true;
        target = target->parentWidget();
    }
    return false;
}

void TitleBarWidget::reloadGameCombo()
{
    if (!m_gameCombo)
        return;

    m_gameCombo->blockSignals(true);
    m_gameCombo->clear();

    const auto& profiles = ProfileService::instance().model().profiles();
    for (const auto& profile : profiles)
        m_gameCombo->addItem(QStringLiteral("%1  ·  %2").arg(profile.game, profile.name), profile.name);

    const int active = ProfileService::instance().activeIndex();
    m_gameCombo->setCurrentIndex(active >= 0 ? qBound(0, active, profiles.size() - 1) : 0);
    m_gameCombo->blockSignals(false);
}

void TitleBarWidget::refresh()
{
    const auto& st = AppStateStore::instance().state();
    if (m_sectionLabel)
        m_sectionLabel->setText(navSectionLabel(st.currentSection));
    if (m_gameCombo)
    {
        m_gameCombo->blockSignals(true);
        reloadGameCombo();
        m_gameCombo->blockSignals(false);
    }
    if (m_engineBadge)
    {
        const bool on = st.engineActive && st.macroMasterEnabled;
        m_engineBadge->setText(on ? QStringLiteral("MACROS ON") : QStringLiteral("PRET"));
        m_engineBadge->setProperty("running", on);
        m_engineBadge->style()->unpolish(m_engineBadge);
        m_engineBadge->style()->polish(m_engineBadge);
    }
    if (m_cpuBar)
        m_cpuBar->setValue(int(st.cpuUsage * 100));
    if (m_ramBar)
        m_ramBar->setValue(int(st.ramUsage * 100));
}

void TitleBarWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !isInteractiveTarget(childAt(event->pos())))
    {
        m_dragging = true;
        m_dragOffset = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void TitleBarWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton))
    {
        if (QWidget* w = window())
            w->move(event->globalPosition().toPoint() - m_dragOffset);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void TitleBarWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
    QWidget::mouseReleaseEvent(event);
}

void TitleBarWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (!isInteractiveTarget(childAt(event->pos())))
    {
        emit maximizeClicked();
        event->accept();
        return;
    }
    QWidget::mouseDoubleClickEvent(event);
}
