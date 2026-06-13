#include "SidebarWidget.h"
#include "SidebarButton.h"
#include "../../app/NavigationManager.h"
#include "../../core/AssetGenerator.h"
#include "../../core/Constants.h"
#include "../../core/AppState.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

SidebarWidget::SidebarWidget(NavigationManager* nav, QWidget* parent)
    : QWidget(parent)
    , m_nav(nav)
{
    setObjectName(QStringLiteral("sidebar"));
    setFixedWidth(Gx::Layout::SidebarWidth);
    buildUi();

    if (m_nav)
        connect(m_nav, &NavigationManager::sectionChanged, this, [this](NavSection s) {
            for (SidebarButton* b : m_buttons)
                b->setActive(b->section() == s);
        });
}

void SidebarWidget::buildUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 16, 12, 16);
    layout->setSpacing(4);

    auto* logoRow = new QHBoxLayout();
    auto* logoIcon = new QLabel(this);
    logoIcon->setFixedSize(32, 32);
    logoIcon->setPixmap(AssetGenerator::instance().pixmap(QStringLiteral("assets/branding/app-mark.svg"), QSize(32, 32)));
    logoIcon->setScaledContents(true);
    logoRow->addWidget(logoIcon);

    auto* logo = new QLabel(QStringLiteral("Game<span style='color:#3498db'>_</span>macro_elite"), this);
    logo->setObjectName(QStringLiteral("sidebarLogo"));
    logo->setTextFormat(Qt::RichText);
    logoRow->addWidget(logo, 1);
    layout->addLayout(logoRow);
    layout->addSpacing(12);

    const NavSection sections[] = {
        NavSection::ProfileManager,
        NavSection::DeviceCenter,
        NavSection::MacroLibrary,
        NavSection::LightingEngine
    };

    for (NavSection s : sections)
    {
        auto* btn = new SidebarButton(s, navSectionIconAsset(s), navSectionLabel(s), this);
        btn->setActive(s == AppStateStore::instance().state().currentSection);
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            for (SidebarButton* b : m_buttons) b->setActive(b == btn);
            if (m_nav) m_nav->navigateTo(btn->section());
            emit navigateRequested(btn->section());
        });
        m_buttons.push_back(btn);
        layout->addWidget(btn);
    }

    layout->addStretch();
}
