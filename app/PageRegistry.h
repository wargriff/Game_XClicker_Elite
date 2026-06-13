#pragma once

#include "../core/Enums.h"
#include "../widgets/stack/AnimatedStackedWidget.h"
#include <QHash>
#include <QObject>

class QWidget;
class DeviceCenterPage;

class PageRegistry : public QObject
{
    Q_OBJECT
public:
    PageRegistry(QWidget* pageParent, AnimatedStackedWidget* stack, QObject* parent = nullptr);

    int indexFor(NavSection section) const;
    AnimatedStackedWidget::Transition transitionFor(NavSection section) const;
    DeviceCenterPage* deviceCenter() const { return m_deviceCenter; }

private:
    void registerPage(NavSection section, QWidget* page);
    void registerAlias(NavSection section, NavSection target);

    AnimatedStackedWidget* m_stack = nullptr;
    DeviceCenterPage* m_deviceCenter = nullptr;
    QHash<NavSection, int> m_indices;
};
