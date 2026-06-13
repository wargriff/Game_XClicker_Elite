#include "MacroModel.h"

namespace
{
QVector<MacroEntry> diabloIvMacros()
{
    return {
        { 1, QStringLiteral("Combo Attack"), QStringLiteral("W"), QStringLiteral("keyboard"), 8.5, 118, true, false, false },
        { 2, QStringLiteral("Loot rapide"), QStringLiteral("F"), QStringLiteral("keyboard"), 15.0, 67, true, false, false },
        { 3, QStringLiteral("Evade"), QStringLiteral("Space"), QStringLiteral("keyboard"), 6.0, 167, true, false, false },
        { 4, QStringLiteral("Potion"), QStringLiteral("Q"), QStringLiteral("keyboard"), 4.0, 250, true, false, false },
        { 5, QStringLiteral("Ultimate"), QStringLiteral("R"), QStringLiteral("keyboard"), 3.0, 333, true, false, false },
        { 6, QStringLiteral("Marche/Arret autoclicks (L2)"), QStringLiteral("L2"), QStringLiteral("mouse"), 0.0, 0, true, true, false },
        { 7, QStringLiteral("Autoclick — Touche 1"), QStringLiteral("1"), QStringLiteral("keyboard"), 12.0, 83, true, false, true },
        { 8, QStringLiteral("Autoclick — Touche 2"), QStringLiteral("2"), QStringLiteral("keyboard"), 12.0, 83, true, false, true },
        { 9, QStringLiteral("Autoclick — Touche 3"), QStringLiteral("3"), QStringLiteral("keyboard"), 12.0, 83, true, false, true },
        { 10, QStringLiteral("Autoclick — Touche 4"), QStringLiteral("4"), QStringLiteral("keyboard"), 12.0, 83, true, false, true }
    };
}

QVector<MacroEntry> diabloIiiMacros()
{
    return {
        { 1, QStringLiteral("Autoclick — Touche 1"), QStringLiteral("1"), QStringLiteral("keyboard"), 12.0, 83, true, false, true },
        { 2, QStringLiteral("Autoclick — Touche 2"), QStringLiteral("2"), QStringLiteral("keyboard"), 12.0, 83, true, false, true },
        { 3, QStringLiteral("Autoclick — Touche 3"), QStringLiteral("3"), QStringLiteral("keyboard"), 12.0, 83, true, false, true },
        { 4, QStringLiteral("Autoclick — Touche 4"), QStringLiteral("4"), QStringLiteral("keyboard"), 12.0, 83, true, false, true },
        { 5, QStringLiteral("D3 — Marche/Arret (L2 souris)"), QStringLiteral("L2"), QStringLiteral("mouse"), 0.0, 0, true, true, false },
        { 6, QStringLiteral("D3 — Action L1"), QStringLiteral("L1"), QStringLiteral("mouse"), 8.0, 125, true, false, false }
    };
}

QVector<MacroEntry> genericMacros(const QString& prefix)
{
    return {
        { 1, prefix + QStringLiteral(" — Skill 1"), QStringLiteral("1"), QStringLiteral("keyboard"), 10.0, 100, true, false, false },
        { 2, prefix + QStringLiteral(" — Skill 2"), QStringLiteral("2"), QStringLiteral("keyboard"), 10.0, 100, true, false, false },
        { 3, prefix + QStringLiteral(" — Skill 3"), QStringLiteral("3"), QStringLiteral("keyboard"), 10.0, 100, true, false, false },
        { 4, prefix + QStringLiteral(" — Skill 4"), QStringLiteral("4"), QStringLiteral("keyboard"), 10.0, 100, true, false, false },
        { 5, prefix + QStringLiteral(" — Toggle L2"), QStringLiteral("L2"), QStringLiteral("mouse"), 0.0, 0, true, true, false },
        { 6, QStringLiteral("Elite M40 — L1"), QStringLiteral("L1"), QStringLiteral("mouse"), 10.0, 100, true, false, false }
    };
}
}

MacroModel::MacroModel()
{
    seedDefaults();
}

void MacroModel::seedDefaults()
{
    m_byProfile[0] = diabloIvMacros();
    m_byProfile[1] = diabloIiiMacros();
    m_byProfile[2] = genericMacros(QStringLiteral("WoW"));
    m_byProfile[3] = genericMacros(QStringLiteral("Valorant"));
}

const QVector<MacroEntry>& MacroModel::macrosForProfile(int profileIndex) const
{
    static const QVector<MacroEntry> kEmpty;
    auto it = m_byProfile.constFind(profileIndex);
    if (it == m_byProfile.constEnd())
        return kEmpty;
    return it.value();
}

QVector<MacroEntry>& MacroModel::macrosForProfile(int profileIndex)
{
    if (!m_byProfile.contains(profileIndex))
        m_byProfile[profileIndex] = genericMacros(QStringLiteral("Profil"));
    return m_byProfile[profileIndex];
}

MacroEntry& MacroModel::selectedMacro(int profileIndex)
{
    auto& list = macrosForProfile(profileIndex);
    if (list.isEmpty())
    {
        static MacroEntry fallback;
        return fallback;
    }
    m_selectedIndex = qBound(0, m_selectedIndex, list.size() - 1);
    return list[m_selectedIndex];
}

const MacroEntry& MacroModel::selectedMacro(int profileIndex) const
{
    return const_cast<MacroModel*>(this)->selectedMacro(profileIndex);
}

void MacroModel::addMacro(int profileIndex, const MacroEntry& entry)
{
    auto& list = macrosForProfile(profileIndex);
    MacroEntry m = entry;
    m.id = list.isEmpty() ? 1 : list.last().id + 1;
    list.push_back(m);
}
