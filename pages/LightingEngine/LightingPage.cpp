#include "LightingPage.h"

#include "../../core/AssetGenerator.h"
#include "../../services/RGBService.h"
#include "../../widgets/rgb/RGBPreviewWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

namespace
{
QFrame* makeDeviceLightCard(const QString& iconAsset, const QString& name, const QString& status, QWidget* parent)
{
    auto* card = new QFrame(parent);
    card->setObjectName(QStringLiteral("lightDeviceCard"));
    auto* layout = new QHBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(12);

    auto* icon = new QLabel(card);
    icon->setFixedSize(44, 44);
    icon->setPixmap(AssetGenerator::instance().pixmap(iconAsset, QSize(44, 44)));
    icon->setScaledContents(true);
    layout->addWidget(icon);

    auto* col = new QVBoxLayout();
    col->setSpacing(2);
    auto* title = new QLabel(name, card);
    title->setObjectName(QStringLiteral("lightDeviceName"));
    auto* st = new QLabel(status, card);
    st->setObjectName(QStringLiteral("lightDeviceStatus"));
    col->addWidget(title);
    col->addWidget(st);
    layout->addLayout(col, 1);

    auto* toggle = new QCheckBox(card);
    toggle->setChecked(true);
    toggle->setObjectName(QStringLiteral("lightDeviceToggle"));
    layout->addWidget(toggle);

    return card;
}
}

LightingPage::LightingPage(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("lightingPage"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 16, 24, 16);
    root->setSpacing(16);

    auto* banner = new QLabel(this);
    banner->setObjectName(QStringLiteral("pageBanner"));
    banner->setPixmap(AssetGenerator::instance().pixmap(
        QStringLiteral("assets/banners/banner-lighting.svg"), QSize(900, 120)));
    banner->setScaledContents(true);
    banner->setFixedHeight(120);
    root->addWidget(banner);

    auto* subtitle = new QLabel(
        QStringLiteral("Controlez l'eclairage RGB de tous vos peripheriques — effets, couleurs et synchronisation."),
        this);
    subtitle->setObjectName(QStringLiteral("pageSubtitle"));
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto* body = new QHBoxLayout();
    body->setSpacing(20);

    auto* left = new QVBoxLayout();
    left->setSpacing(12);

    auto* devicesTitle = new QLabel(QStringLiteral("Peripheriques"), this);
    devicesTitle->setObjectName(QStringLiteral("panelHeader"));
    left->addWidget(devicesTitle);

    left->addWidget(makeDeviceLightCard(QStringLiteral("devices/dock-keyboard.svg"),
        QStringLiteral("60% Gaming Keyboard"), QStringLiteral("Contour bleu — Statique"), this));
    left->addWidget(makeDeviceLightCard(QStringLiteral("devices/dock-mouse-elite-m40.svg"),
        QStringLiteral("Elite M40"), QStringLiteral("Bandeau bleu — Statique"), this));
    left->addWidget(makeDeviceLightCard(QStringLiteral("devices/dock-headset.svg"),
        QStringLiteral("HS80 RGB"), QStringLiteral("Arc-en-ciel — 65%"), this));
    left->addWidget(makeDeviceLightCard(QStringLiteral("devices/dock-aio.svg"),
        QStringLiteral("H150i Elite"), QStringLiteral("Temp 28° — Bleu froid"), this));
    left->addStretch();

    body->addLayout(left, 2);

    auto* controlPanel = new QFrame(this);
    controlPanel->setObjectName(QStringLiteral("lightControlPanel"));
    auto* controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setContentsMargins(20, 20, 20, 20);
    controlLayout->setSpacing(14);

    auto* ctrlTitle = new QLabel(QStringLiteral("Effet global"), controlPanel);
    ctrlTitle->setObjectName(QStringLiteral("panelHeader"));
    controlLayout->addWidget(ctrlTitle);

    static RGBService rgb;

    auto* presetRow = new QHBoxLayout();
    auto* preset = new QComboBox(controlPanel);
    preset->addItems({
        QStringLiteral("Statique"),
        QStringLiteral("Arc-en-ciel"),
        QStringLiteral("Vague"),
        QStringLiteral("Respiration"),
        QStringLiteral("Reactive")
    });
    preset->setCurrentIndex(1);
    presetRow->addWidget(new QLabel(QStringLiteral("Preset"), controlPanel));
    presetRow->addWidget(preset, 1);
    controlLayout->addLayout(presetRow);

    auto* zone = new QComboBox(controlPanel);
    zone->addItems({
        QStringLiteral("Tous les peripheriques"),
        QStringLiteral("Clavier uniquement"),
        QStringLiteral("Elite M40"),
        QStringLiteral("Casque + AIO")
    });
    controlLayout->addWidget(new QLabel(QStringLiteral("Zone"), controlPanel));
    controlLayout->addWidget(zone);

    auto* speed = new QSlider(Qt::Horizontal, controlPanel);
    speed->setRange(0, 100);
    speed->setValue(int(rgb.speed() * 100));
    controlLayout->addWidget(new QLabel(QStringLiteral("Vitesse"), controlPanel));
    controlLayout->addWidget(speed);

    auto* bright = new QSlider(Qt::Horizontal, controlPanel);
    bright->setRange(0, 100);
    bright->setValue(int(rgb.brightness() * 100));
    controlLayout->addWidget(new QLabel(QStringLiteral("Luminosite"), controlPanel));
    controlLayout->addWidget(bright);

    auto* colorRow = new QHBoxLayout();
    const char* colors[] = { "#3498db", "#e02626", "#2ecc71", "#9b59b6", "#ff8c2a", "#ffffff" };
    for (const char* hex : colors)
    {
        auto* swatch = new QPushButton(controlPanel);
        swatch->setFixedSize(28, 28);
        swatch->setStyleSheet(QStringLiteral("background:%1; border:2px solid #2a2a34; border-radius:14px;").arg(QString::fromLatin1(hex)));
        colorRow->addWidget(swatch);
    }
    colorRow->addStretch();
    controlLayout->addWidget(new QLabel(QStringLiteral("Couleur"), controlPanel));
    controlLayout->addLayout(colorRow);

    auto* sync = new QPushButton(QStringLiteral("Synchroniser tous les peripheriques"), controlPanel);
    sync->setObjectName(QStringLiteral("primaryButton"));
    controlLayout->addWidget(sync);
    controlLayout->addStretch();

    body->addWidget(controlPanel, 2);

    auto* previewPanel = new QFrame(this);
    previewPanel->setObjectName(QStringLiteral("lightPreviewPanel"));
    auto* previewLayout = new QVBoxLayout(previewPanel);
    previewLayout->setContentsMargins(16, 16, 16, 16);
    previewLayout->addWidget(new QLabel(QStringLiteral("Apercu"), previewPanel));
    previewLayout->addWidget(new RGBPreviewWidget(previewPanel), 1);

    body->addWidget(previewPanel, 2);
    root->addLayout(body, 1);
}
