#include "ProfileService.h"
#include "../core/AppState.h"
#include "../core/EventBus.h"
#include "../services/MacroService.h"

ProfileService& ProfileService::instance()
{
    static ProfileService svc;
    return svc;
}

int ProfileService::activeIndex() const
{
    return AppStateStore::instance().state().activeProfileIndex;
}

void ProfileService::applyProfile(int index)
{
    const auto& profiles = m_model.profiles();
    if (index < 0 || index >= profiles.size()) return;

    auto& st = AppStateStore::instance().state();
    if (st.activeProfileIndex == index) return;

    st.activeProfileIndex = index;
    const auto& profile = profiles.at(index);
    st.activeGame = profile.game;
    st.activeProfileName = profile.name;
    st.macroMasterEnabled = false;

    MacroService::instance().syncMacroCount();

    emit EventBus::instance().profileChanged(index);
    emit EventBus::instance().macroMasterChanged(false);
}
