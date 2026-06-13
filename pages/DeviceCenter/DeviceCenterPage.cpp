#include "DeviceCenterPage.h"

#include "../../widgets/keyboard/KeyboardWidget.h"
#include "../../widgets/mouse/MouseWidget.h"
#include "../../core/AssetGenerator.h"
#include "../../services/MacroEngine.h"
#include "../../services/MacroService.h"
#include "../../core/AppState.h"
#include "../../core/EventBus.h"
#include "../../core/Enums.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>

DeviceCenterPage::DeviceCenterPage(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("deviceCenterPage"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 16, 24, 16);
    root->setSpacing(14);

    auto* banner = new QLabel(this);
    banner->setObjectName(QStringLiteral("pageBanner"));
    banner->setPixmap(AssetGenerator::instance().pixmap(
        QStringLiteral("assets/banners/banner-devices.svg"), QSize(900, 120)));
    banner->setScaledContents(true);
    banner->setFixedHeight(120);
    root->addWidget(banner);

    auto* deviceStack = new QStackedWidget(this);

    auto* mousePage = new QWidget(this);
    auto* mouseLayout = new QVBoxLayout(mousePage);
    mouseLayout->setContentsMargins(0, 0, 0, 0);

    auto* mouseWidget = new MouseWidget(mousePage);
    m_mouseWidget = mouseWidget;
    mouseLayout->addWidget(mouseWidget, 1);

    auto* kbPage = new QWidget(this);
    auto* kbLayout = new QVBoxLayout(kbPage);
    kbLayout->setContentsMargins(0, 0, 0, 0);
    auto* keyboard = new KeyboardWidget(kbPage);
    kbLayout->addWidget(keyboard, 1);

    deviceStack->addWidget(mousePage);
    deviceStack->addWidget(kbPage);

    auto* header = new QHBoxLayout();
    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName(QStringLiteral("deviceTabs"));
    m_tabs->addTab(new QWidget(), QStringLiteral("Souris"));
    m_tabs->addTab(new QWidget(), QStringLiteral("Clavier"));
    m_tabs->setCurrentIndex(0);
    connect(m_tabs, &QTabWidget::currentChanged, deviceStack, &QStackedWidget::setCurrentIndex);
    header->addWidget(m_tabs, 1);

    auto* deviceCombo = new QComboBox(this);
    deviceCombo->setMinimumWidth(220);
    deviceCombo->addItem(QStringLiteral("60% Gaming Keyboard"));
    deviceCombo->addItem(QStringLiteral("TKL Gaming Keyboard"));
    deviceCombo->addItem(QStringLiteral("Elite M40"));
    header->addWidget(deviceCombo);
    root->addLayout(header);

    auto* bodySplitter = new QSplitter(Qt::Horizontal, this);
    bodySplitter->setObjectName(QStringLiteral("deviceCenterSplitter"));
    bodySplitter->setChildrenCollapsible(false);
    bodySplitter->setHandleWidth(10);

    auto* leftHost = new QWidget(bodySplitter);
    auto* left = new QVBoxLayout(leftHost);
    left->setContentsMargins(0, 0, 0, 0);
    left->addWidget(deviceStack, 1);

    auto* legend = new QHBoxLayout();
    legend->setSpacing(20);
    auto addLeg = [&](const QString& html) {
        auto* lb = new QLabel(html, leftHost);
        lb->setTextFormat(Qt::RichText);
        lb->setStyleSheet(QStringLiteral("color: #b0b0bc; font-size: 13px;"));
        legend->addWidget(lb);
    };
    addLeg(QStringLiteral("<span style='color:#3498db;font-size:14px'>●</span> Contour bleu"));
    addLeg(QStringLiteral("<span style='color:#e02626;font-size:14px'>●</span> Macro"));
    addLeg(QStringLiteral("<span style='color:#2ecc71;font-size:14px'>●</span> L2 ON / OFF macros"));
    legend->addStretch();
    left->addLayout(legend);

    auto* right = new QFrame(bodySplitter);
    right->setObjectName(QStringLiteral("keyConfigPanel"));
    right->setMinimumWidth(360);
    auto* panelLayout = new QVBoxLayout(right);
    panelLayout->setContentsMargins(20, 18, 20, 18);
    panelLayout->setSpacing(0);

    m_panelHeader = new QLabel(QStringLiteral("TOUCHE W"), right);
    m_panelHeader->setObjectName(QStringLiteral("panelHeader"));
    panelLayout->addWidget(m_panelHeader);
    panelLayout->addSpacing(16);

    auto* scroll = new QScrollArea(right);
    scroll->setObjectName(QStringLiteral("keyConfigScroll"));
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* formHost = new QWidget(scroll);
    formHost->setObjectName(QStringLiteral("keyConfigForm"));
    auto* form = new QVBoxLayout(formHost);
    form->setContentsMargins(0, 0, 4, 0);
    form->setSpacing(16);

    auto addFieldBlock = [&](const QString& labelText, QWidget* field) {
        auto* block = new QVBoxLayout();
        block->setSpacing(6);
        auto* label = new QLabel(labelText, formHost);
        label->setObjectName(QStringLiteral("keyConfigLabel"));
        block->addWidget(label);
        block->addWidget(field);
        form->addLayout(block);
    };

    m_targetLabel = new QLabel(QStringLiteral("Touche W"), formHost);
    m_targetLabel->setObjectName(QStringLiteral("keyConfigValue"));
    m_targetLabel->setWordWrap(true);
    addFieldBlock(QStringLiteral("Cible"), m_targetLabel);

    m_macroCombo = new QComboBox(formHost);
    m_macroCombo->setObjectName(QStringLiteral("macroAssignCombo"));
    m_macroCombo->setMinimumHeight(38);
    m_macroCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    if (auto* view = qobject_cast<QListView*>(m_macroCombo->view()))
    {
        view->setTextElideMode(Qt::ElideNone);
        view->setMinimumWidth(440);
        view->setSpacing(2);
    }
    addFieldBlock(QStringLiteral("Macro assignee"), m_macroCombo);

    m_cpsSpin = new QDoubleSpinBox(formHost);
    m_cpsSpin->setObjectName(QStringLiteral("keyConfigSpin"));
    m_cpsSpin->setRange(0.0, 50.0);
    m_cpsSpin->setSingleStep(0.5);
    m_cpsSpin->setDecimals(1);
    m_cpsSpin->setSuffix(QStringLiteral(" CPS"));
    m_cpsSpin->setMinimumHeight(36);
    addFieldBlock(QStringLiteral("Vitesse CPS"), m_cpsSpin);

    m_delaySpin = new QSpinBox(formHost);
    m_delaySpin->setObjectName(QStringLiteral("keyConfigSpin"));
    m_delaySpin->setRange(0, 2000);
    m_delaySpin->setSuffix(QStringLiteral(" ms"));
    m_delaySpin->setMinimumHeight(36);
    addFieldBlock(QStringLiteral("Delai"), m_delaySpin);

    m_toggleHint = new QLabel(
        QStringLiteral("Bouton lateral L2 (XButton1 ou XButton2) : clic souris ou bouton L2 a l'ecran"),
        formHost);
    m_toggleHint->setObjectName(QStringLiteral("toggleHint"));
    m_toggleHint->setWordWrap(true);
    m_toggleHint->hide();
    form->addWidget(m_toggleHint);

    m_l2LaunchBtn = new QPushButton(QStringLiteral("L2 — MACROS OFF"), formHost);
    m_l2LaunchBtn->setObjectName(QStringLiteral("l2LaunchBtn"));
    m_l2LaunchBtn->setMinimumHeight(52);
    m_l2LaunchBtn->setCursor(Qt::PointingHandCursor);
    m_l2LaunchBtn->hide();
    form->addWidget(m_l2LaunchBtn);

    auto* activeBlock = new QVBoxLayout();
    activeBlock->setSpacing(6);
    auto* activeLabel = new QLabel(QStringLiteral("Actif"), formHost);
    activeLabel->setObjectName(QStringLiteral("keyConfigLabel"));
    activeBlock->addWidget(activeLabel);

    auto* activeRow = new QHBoxLayout();
    activeRow->setContentsMargins(0, 0, 0, 0);
    m_activeToggle = new QCheckBox(formHost);
    m_activeToggle->setObjectName(QStringLiteral("keyConfigToggle"));
    m_activeToggle->setChecked(true);
    activeRow->addStretch();
    activeRow->addWidget(m_activeToggle);
    activeBlock->addLayout(activeRow);
    form->addLayout(activeBlock);

    form->addStretch();
    scroll->setWidget(formHost);
    panelLayout->addWidget(scroll, 1);

    auto* btns = new QHBoxLayout();
    btns->setContentsMargins(0, 14, 0, 0);
    btns->setSpacing(12);
    auto* save = new QPushButton(QStringLiteral("Enregistrer"), right);
    save->setObjectName(QStringLiteral("primaryButton"));
    save->setMinimumWidth(160);
    auto* test = new QPushButton(QStringLiteral("Tester"), right);
    test->setObjectName(QStringLiteral("secondaryButton"));
    test->setMinimumWidth(140);
    btns->addWidget(test, 1);
    btns->addWidget(save, 1);
    panelLayout->addLayout(btns);

    bodySplitter->addWidget(leftHost);
    bodySplitter->addWidget(right);
    bodySplitter->setStretchFactor(0, 2);
    bodySplitter->setStretchFactor(1, 1);
    bodySplitter->setSizes({780, 480});

    root->addWidget(bodySplitter, 1);

    connect(m_macroCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeviceCenterPage::syncPanelFromMacro);
    connect(m_cpsSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &DeviceCenterPage::onCpsChanged);
    connect(m_delaySpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &DeviceCenterPage::onDelayChanged);
    connect(m_activeToggle, &QCheckBox::toggled, this, [this](bool on) {
        auto& macros = MacroService::instance().activeMacros();
        const int idx = m_macroCombo->currentIndex();
        if (idx < 0 || idx >= macros.size()) return;
        macros[idx].active = on;
        if (isMasterToggle(macros[idx].keyLabel, macros[idx].toggle))
            MacroEngine::instance().setMasterEnabled(on);
    });
    connect(test, &QPushButton::clicked, this, [this]() {
        auto& macros = MacroService::instance().activeMacros();
        const int idx = m_macroCombo->currentIndex();
        if (idx < 0 || idx >= macros.size()) return;
        if (isMasterToggle(macros[idx].keyLabel, macros[idx].toggle))
            toggleMacroMaster();
    });
    connect(save, &QPushButton::clicked, this, [this]() {
        auto& macros = MacroService::instance().activeMacros();
        const int idx = m_macroCombo->currentIndex();
        if (idx < 0 || idx >= macros.size()) return;
        auto& m = macros[idx];
        m.cps = m_cpsSpin->value();
        m.delayMs = m_delaySpin->value();
        m.active = m_activeToggle->isChecked();
        if (m_mouseMode)
        {
            m.device = QStringLiteral("mouse");
            m.keyLabel = m_mouseButton;
        }
        else
        {
            m.device = QStringLiteral("keyboard");
            m.keyLabel = m_keyLabel;
        }
    });

    connect(keyboard, &KeyboardWidget::keySelected, this, [this, deviceCombo](const QString& key) {
        m_mouseMode = false;
        m_keyLabel = key;
        deviceCombo->setCurrentIndex(0);
        updatePanelHeader();
        const auto& macros = MacroService::instance().activeMacros();
        for (int i = 0; i < macros.size(); ++i)
        {
            if (macros[i].device == QStringLiteral("keyboard") &&
                macros[i].keyLabel.compare(key, Qt::CaseInsensitive) == 0)
            {
                reloadMacroList(i);
                return;
            }
        }
    });

    connect(m_l2LaunchBtn, &QPushButton::clicked, this, &DeviceCenterPage::toggleMacroMaster);

    connect(m_mouseWidget, &MouseWidget::macroToggleRequested, this, &DeviceCenterPage::toggleMacroMaster);

    connect(m_mouseWidget, &MouseWidget::buttonSelected, this, [this, deviceCombo](const QString& btn) {
        m_mouseMode = true;
        m_mouseButton = btn;
        deviceCombo->setCurrentText(QStringLiteral("Elite M40"));
        updatePanelHeader();
        if (m_l2LaunchBtn)
            m_l2LaunchBtn->setVisible(btn.compare(QStringLiteral("L2"), Qt::CaseInsensitive) == 0);
        updateL2LaunchButton();
        const auto& macros = MacroService::instance().activeMacros();
        for (int i = 0; i < macros.size(); ++i)
        {
            if (macros[i].device == QStringLiteral("mouse") &&
                macros[i].keyLabel.compare(btn, Qt::CaseInsensitive) == 0)
            {
                reloadMacroList(i);
                return;
            }
        }
    });

    connect(m_tabs, &QTabWidget::currentChanged, this, [this, deviceCombo](int idx) {
        m_mouseMode = (idx == 0);
        if (idx == 0)
            deviceCombo->setCurrentText(QStringLiteral("Elite M40"));
        else
            deviceCombo->setCurrentIndex(0);
        updatePanelHeader();
    });

    connect(&EventBus::instance(), &EventBus::profileChanged, this, &DeviceCenterPage::refreshProfile);
    connect(&EventBus::instance(), &EventBus::macroMasterChanged, this, [this](bool on) {
        if (m_activeToggle)
            m_activeToggle->setChecked(on);
        if (m_toggleHint)
            m_toggleHint->setVisible(m_mouseMode && m_mouseButton == QStringLiteral("L2"));
        if (m_mouseWidget)
            m_mouseWidget->setMacroMasterEnabled(on);
        updateL2LaunchButton();
    });

    m_mouseWidget->selectButtonByLabel(QStringLiteral("L2"));
    m_mouseMode = true;
    m_mouseButton = QStringLiteral("L2");
    deviceCombo->setCurrentText(QStringLiteral("Elite M40"));
    updatePanelHeader();
    m_l2LaunchBtn->setVisible(true);
    m_toggleHint->setVisible(true);
    updateL2LaunchButton();

    reloadMacroList(0);
    const auto& macros = MacroService::instance().activeMacros();
    for (int i = 0; i < macros.size(); ++i)
    {
        if (macros[i].device == QStringLiteral("mouse") &&
            macros[i].keyLabel == QStringLiteral("L2") && macros[i].toggle)
        {
            reloadMacroList(i);
            break;
        }
    }
}

void DeviceCenterPage::focusDevice(const QString& deviceId)
{
    if (!m_tabs) return;
    if (deviceId == QStringLiteral("ms"))
    {
        m_tabs->setCurrentIndex(0);
        m_mouseMode = true;
    }
    else if (deviceId == QStringLiteral("kb"))
    {
        m_tabs->setCurrentIndex(1);
        m_mouseMode = false;
    }
    updatePanelHeader();
}

void DeviceCenterPage::refreshProfile()
{
    reloadMacroList(0);
    updatePanelHeader();
}

void DeviceCenterPage::reloadMacroList(int selectIndex)
{
    if (!m_macroCombo) return;

    m_macroCombo->blockSignals(true);
    m_macroCombo->clear();

    const auto& macros = MacroService::instance().activeMacros();
    for (const auto& m : macros)
        m_macroCombo->addItem(m.name);

    const int idx = selectIndex >= 0 ? qBound(0, selectIndex, macros.size() - 1) : 0;
    m_macroCombo->setCurrentIndex(macros.isEmpty() ? -1 : idx);
    m_macroCombo->blockSignals(false);

    if (!macros.isEmpty())
        syncPanelFromMacro(idx);
}

void DeviceCenterPage::syncPanelFromMacro(int index)
{
    const auto& macros = MacroService::instance().activeMacros();
    if (index < 0 || index >= macros.size()) return;

    const MacroEntry& m = macros.at(index);
    MacroService::instance().model().setSelectedIndex(index);

    m_cpsSpin->blockSignals(true);
    m_delaySpin->blockSignals(true);
    m_cpsSpin->setValue(m.cps);
    m_delaySpin->setValue(m.delayMs);
    m_cpsSpin->setEnabled(!m.toggle);
    m_cpsSpin->blockSignals(false);
    m_delaySpin->blockSignals(false);

    m_activeToggle->setChecked(m.toggle ? AppStateStore::instance().state().macroMasterEnabled : m.active);
    m_toggleHint->setVisible(m.toggle && isMasterToggle(m.keyLabel, m.toggle));
    if (m.toggle && isMasterToggle(m.keyLabel, m.toggle))
        m_toggleHint->setText(QStringLiteral("Bouton lateral L2 (XButton2) : cliquer sur L2 souris pour ON / OFF"));

    const bool showL2 = m_mouseMode && m_mouseButton.compare(QStringLiteral("L2"), Qt::CaseInsensitive) == 0;
    if (m_l2LaunchBtn)
        m_l2LaunchBtn->setVisible(showL2);
    updateL2LaunchButton();

    if (m.device == QStringLiteral("mouse"))
    {
        m_mouseMode = true;
        m_mouseButton = m.keyLabel;
    }
    else
    {
        m_mouseMode = false;
        m_keyLabel = m.keyLabel;
    }
    updatePanelHeader();
}

void DeviceCenterPage::onCpsChanged(double cps)
{
    auto& macros = MacroService::instance().activeMacros();
    const int idx = m_macroCombo->currentIndex();
    if (idx < 0 || idx >= macros.size() || macros[idx].toggle) return;
    macros[idx].cps = cps;
    if (cps > 0.0)
    {
        const int ms = qMax(1, int(1000.0 / cps));
        m_delaySpin->blockSignals(true);
        m_delaySpin->setValue(ms);
        macros[idx].delayMs = ms;
        m_delaySpin->blockSignals(false);
    }
}

void DeviceCenterPage::onDelayChanged(int ms)
{
    auto& macros = MacroService::instance().activeMacros();
    const int idx = m_macroCombo->currentIndex();
    if (idx < 0 || idx >= macros.size() || macros[idx].toggle) return;
    macros[idx].delayMs = ms;
    if (ms > 0)
    {
        const double cps = 1000.0 / ms;
        m_cpsSpin->blockSignals(true);
        m_cpsSpin->setValue(cps);
        macros[idx].cps = cps;
        m_cpsSpin->blockSignals(false);
    }
}

void DeviceCenterPage::updatePanelHeader()
{
    if (!m_panelHeader || !m_targetLabel) return;

    if (m_mouseMode)
    {
        m_panelHeader->setText(QStringLiteral("BOUTON SOURIS"));
        m_targetLabel->setText(QStringLiteral("Elite M40 — %1").arg(m_mouseButton));
    }
    else
    {
        m_panelHeader->setText(QStringLiteral("TOUCHE %1").arg(m_keyLabel.toUpper()));
        m_targetLabel->setText(QStringLiteral("Touche %1").arg(m_keyLabel.toUpper()));
    }

    if (m_l2LaunchBtn && m_mouseMode)
        m_l2LaunchBtn->setVisible(m_mouseButton.compare(QStringLiteral("L2"), Qt::CaseInsensitive) == 0);
}

bool DeviceCenterPage::isMasterToggle(const QString& keyLabel, bool toggleFlag)
{
    return toggleFlag && keyLabel.compare(QStringLiteral("L2"), Qt::CaseInsensitive) == 0;
}

void DeviceCenterPage::toggleMacroMaster()
{
    MacroEngine::instance().toggleMaster();
}

void DeviceCenterPage::updateL2LaunchButton()
{
    if (!m_l2LaunchBtn)
        return;

    const bool on = AppStateStore::instance().state().macroMasterEnabled;
    m_l2LaunchBtn->setText(on ? QStringLiteral("L2 — MACROS ON")
                              : QStringLiteral("L2 — MACROS OFF"));
    m_l2LaunchBtn->setProperty("macroOn", on);
    m_l2LaunchBtn->style()->unpolish(m_l2LaunchBtn);
    m_l2LaunchBtn->style()->polish(m_l2LaunchBtn);
}
