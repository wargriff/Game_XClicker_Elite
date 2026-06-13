#include "MacroLibraryPage.h"

#include "../../core/AppState.h"
#include "../../core/AssetGenerator.h"
#include "../../core/EventBus.h"
#include "../../models/MacroModel.h"
#include "../../services/MacroService.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace
{
enum class MacroStatus { Running, Paused, Error };

MacroStatus computeStatus(const MacroEntry& m, const AppState& st)
{
    if (m.name.trimmed().isEmpty())
        return MacroStatus::Error;
    if (!m.toggle && m.cps <= 0.0)
        return MacroStatus::Error;

    if (!m.active)
        return MacroStatus::Paused;

    if (m.gatedByMaster && !st.macroMasterEnabled)
        return MacroStatus::Paused;

    if (m.toggle && m.keyLabel == QStringLiteral("L1") && !st.macroMasterEnabled)
        return MacroStatus::Paused;

    return MacroStatus::Running;
}

QString statusDotColor(MacroStatus s)
{
    switch (s)
    {
    case MacroStatus::Running: return QStringLiteral("#2ecc71");
    case MacroStatus::Paused:  return QStringLiteral("#ff8c2a");
    case MacroStatus::Error:   return QStringLiteral("#e02626");
    }
    return QStringLiteral("#9090a0");
}

QString statusLabel(MacroStatus s, const MacroEntry& m, const AppState& st)
{
    switch (s)
    {
    case MacroStatus::Running:
        if (m.toggle)
            return st.macroMasterEnabled ? QStringLiteral("Moteur ON") : QStringLiteral("Fonctionne");
        return QStringLiteral("Fonctionne");
    case MacroStatus::Paused:
        if (m.gatedByMaster && !st.macroMasterEnabled)
            return QStringLiteral("Pause (L1)");
        if (!m.active)
            return QStringLiteral("Pause");
        return QStringLiteral("En attente");
    case MacroStatus::Error:
        return QStringLiteral("Erreur / Bug");
    }
    return {};
}

QString makeStatusHtml(const MacroEntry& m)
{
    const auto& st = AppStateStore::instance().state();
    const MacroStatus status = computeStatus(m, st);
    const QString color = statusDotColor(status);
    const QString label = statusLabel(status, m, st);
    return QStringLiteral("<span style='color:%1;font-size:16px'>●</span> <span style='color:#d0d0dc'>%2</span>")
        .arg(color, label);
}
}

MacroLibraryPage::MacroLibraryPage(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("macroLibraryPage"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 16, 24, 16);
    root->setSpacing(16);

    auto* bannerRow = new QHBoxLayout();
    auto* banner = new QLabel(this);
    banner->setObjectName(QStringLiteral("pageBanner"));
    banner->setPixmap(AssetGenerator::instance().pixmap(
        QStringLiteral("assets/banners/banner-macros.svg"), QSize(900, 120)));
    banner->setScaledContents(true);
    banner->setFixedHeight(120);
    bannerRow->addWidget(banner, 1);

    auto* addBtn = new QPushButton(QStringLiteral("+ Ajouter"), this);
    addBtn->setObjectName(QStringLiteral("primaryButton"));
    connect(addBtn, &QPushButton::clicked, this, &MacroLibraryPage::onAddMacro);
    bannerRow->addWidget(addBtn, 0, Qt::AlignTop);
    root->addLayout(bannerRow);

    auto* subtitle = new QLabel(
        QStringLiteral("Cliquez une cellule pour modifier. Les changements sont enregistres automatiquement."),
        this);
    subtitle->setObjectName(QStringLiteral("pageSubtitle"));
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* legend = new QHBoxLayout();
    legend->setSpacing(20);
    auto addLeg = [&](const QString& color, const QString& text) {
        auto* lb = new QLabel(
            QStringLiteral("<span style='color:%1;font-size:15px'>●</span> %2").arg(color, text), this);
        lb->setTextFormat(Qt::RichText);
        lb->setObjectName(QStringLiteral("macroLegendItem"));
        legend->addWidget(lb);
    };
    addLeg(QStringLiteral("#2ecc71"), QStringLiteral("Fonctionne"));
    addLeg(QStringLiteral("#ff8c2a"), QStringLiteral("Pause"));
    addLeg(QStringLiteral("#e02626"), QStringLiteral("Bug / ne fonctionne pas"));
    legend->addStretch();
    root->addLayout(legend);

    m_table = new QTableWidget(this);
    m_table->setObjectName(QStringLiteral("macroTable"));
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("Nom"),
        QStringLiteral("Cible"),
        QStringLiteral("CPS"),
        QStringLiteral("Delai (ms)"),
        QStringLiteral("Type"),
        QStringLiteral("Etat")
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    connect(m_table, &QTableWidget::cellChanged, this, &MacroLibraryPage::onCellChanged);
    connect(&EventBus::instance(), &EventBus::profileChanged, this, &MacroLibraryPage::refreshList);
    connect(&EventBus::instance(), &EventBus::macroMasterChanged, this, &MacroLibraryPage::refreshList);

    MacroService::instance().syncMacroCount();
    refreshList();
}

void MacroLibraryPage::updateStatusCell(int row)
{
    if (!m_table) return;
    const auto& macros = MacroService::instance().activeMacros();
    if (row < 0 || row >= macros.size()) return;

    auto* statusLabel = new QLabel(makeStatusHtml(macros.at(row)), m_table);
    statusLabel->setTextFormat(Qt::RichText);
    statusLabel->setObjectName(QStringLiteral("macroStatusCell"));
    statusLabel->setContentsMargins(10, 0, 10, 0);
    m_table->setCellWidget(row, 5, statusLabel);
}

void MacroLibraryPage::refreshList()
{
    if (!m_table) return;

    m_blockTableSignals = true;

    const auto& macros = MacroService::instance().activeMacros();

    m_table->setRowCount(macros.size());
    for (int i = 0; i < macros.size(); ++i)
    {
        const MacroEntry& m = macros.at(i);

        auto* nameItem = new QTableWidgetItem(m.name);
        m_table->setItem(i, 0, nameItem);

        const QString target = m.device == QStringLiteral("mouse")
            ? QStringLiteral("Souris %1").arg(m.keyLabel)
            : QStringLiteral("Touche %1").arg(m.keyLabel);
        m_table->setItem(i, 1, new QTableWidgetItem(target));

        auto* cpsItem = new QTableWidgetItem(
            m.toggle ? QStringLiteral("—") : QString::number(m.cps, 'f', 1));
        if (m.toggle) cpsItem->setFlags(cpsItem->flags() & ~Qt::ItemIsEditable);
        m_table->setItem(i, 2, cpsItem);

        auto* delayItem = new QTableWidgetItem(
            m.toggle ? QStringLiteral("—") : QString::number(m.delayMs));
        if (m.toggle) delayItem->setFlags(delayItem->flags() & ~Qt::ItemIsEditable);
        m_table->setItem(i, 3, delayItem);

        QString type;
        if (m.toggle)
            type = QStringLiteral("Marche/Arret L1");
        else if (m.gatedByMaster)
            type = QStringLiteral("Autoclick (L1)");
        else
            type = QStringLiteral("Autoclick");

        auto* typeItem = new QTableWidgetItem(type);
        typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);
        m_table->setItem(i, 4, typeItem);

        updateStatusCell(i);
    }

    m_blockTableSignals = false;
}

void MacroLibraryPage::onCellChanged(int row, int column)
{
    if (m_blockTableSignals || !m_table) return;

    auto& macros = MacroService::instance().activeMacros();
    if (row < 0 || row >= macros.size()) return;

    MacroEntry& m = macros[row];
    const QTableWidgetItem* item = m_table->item(row, column);
    if (!item) return;

    m_blockTableSignals = true;

    switch (column)
    {
    case 0:
        m.name = item->text().trimmed();
        break;
    case 1:
    {
        const QString text = item->text().trimmed();
        if (text.startsWith(QStringLiteral("Souris"), Qt::CaseInsensitive))
        {
            m.device = QStringLiteral("mouse");
            m.keyLabel = text.mid(6).trimmed();
        }
        else if (text.startsWith(QStringLiteral("Touche"), Qt::CaseInsensitive))
        {
            m.device = QStringLiteral("keyboard");
            m.keyLabel = text.mid(6).trimmed();
        }
        else
        {
            m.keyLabel = text;
        }
        break;
    }
    case 2:
        if (!m.toggle)
        {
            m.cps = item->text().toDouble();
            if (m.cps > 0.0)
                m.delayMs = qMax(1, int(1000.0 / m.cps));
        }
        break;
    case 3:
        if (!m.toggle)
        {
            m.delayMs = item->text().toInt();
            if (m.delayMs > 0)
                m.cps = 1000.0 / m.delayMs;
        }
        break;
    default:
        break;
    }

    updateStatusCell(row);

    m_blockTableSignals = false;
}

void MacroLibraryPage::onAddMacro()
{
    MacroEntry entry;
    entry.name = QStringLiteral("Nouvelle macro");
    entry.keyLabel = QStringLiteral("1");
    entry.device = QStringLiteral("keyboard");
    entry.cps = 10.0;
    entry.delayMs = 100;
    entry.active = true;
    entry.gatedByMaster = true;
    MacroService::instance().addMacro(entry);
    refreshList();
}
