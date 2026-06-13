#include "PageRegistry.h"
#include "../pages/DeviceCenter/DeviceCenterPage.h"
#include "../pages/LightingEngine/LightingPage.h"
#include "../pages/MacroLibrary/MacroLibraryPage.h"
#include "../pages/ProfileManager/ProfileManagerPage.h"

PageRegistry::PageRegistry(QWidget* pageParent, AnimatedStackedWidget* stack, QObject* parent)
    : QObject(parent)
    , m_stack(stack)
{
    registerPage(NavSection::ProfileManager, new ProfileManagerPage(pageParent));
    m_deviceCenter = new DeviceCenterPage(pageParent);
    registerPage(NavSection::DeviceCenter, m_deviceCenter);
    registerPage(NavSection::MacroLibrary, new MacroLibraryPage(pageParent));
    registerPage(NavSection::LightingEngine, new LightingPage(pageParent));

    registerAlias(NavSection::MissionControl, NavSection::DeviceCenter);
    registerAlias(NavSection::MacroStudio, NavSection::MacroLibrary);
    registerAlias(NavSection::ActivityMonitor, NavSection::DeviceCenter);
    registerAlias(NavSection::AnalyticsCenter, NavSection::DeviceCenter);
    registerAlias(NavSection::MobileCommand, NavSection::DeviceCenter);
    registerAlias(NavSection::SettingsHub, NavSection::DeviceCenter);
}

void PageRegistry::registerPage(NavSection section, QWidget* page)
{
    if (!m_stack || !page)
        return;
    m_indices.insert(section, m_stack->count());
    m_stack->addWidget(page);
}

void PageRegistry::registerAlias(NavSection section, NavSection target)
{
    if (m_indices.contains(target))
        m_indices.insert(section, m_indices.value(target));
}

int PageRegistry::indexFor(NavSection section) const
{
    return m_indices.value(section, m_indices.value(NavSection::DeviceCenter, 0));
}

AnimatedStackedWidget::Transition PageRegistry::transitionFor(NavSection section) const
{
    if (section == NavSection::DeviceCenter || section == NavSection::LightingEngine)
        return AnimatedStackedWidget::Zoom;
    if (section == NavSection::MacroLibrary)
        return AnimatedStackedWidget::Slide;
    return AnimatedStackedWidget::Fade;
}
