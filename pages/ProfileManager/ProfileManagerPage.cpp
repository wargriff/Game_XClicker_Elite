#include "ProfileManagerPage.h"

#include "../../core/AssetGenerator.h"
#include "../../core/EventBus.h"
#include "../../services/MacroService.h"
#include "../../models/ProfileModel.h"
#include "../../services/ProfileService.h"

#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace
{
QString avatarForGame(const QString& game)
{
    if (game.contains(QStringLiteral("Diablo IV"), Qt::CaseInsensitive))
        return QStringLiteral("avatars/diablo-iv.svg");
    return QStringLiteral("avatars/default-game.svg");
}
}

ProfileManagerPage::ProfileManagerPage(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("profileManagerPage"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 16, 24, 16);
    root->setSpacing(18);

    auto* hero = new QFrame(this);
    hero->setObjectName(QStringLiteral("profileHero"));
    auto* heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(0, 0, 0, 0);

    auto* banner = new QLabel(hero);
    banner->setObjectName(QStringLiteral("pageBanner"));
    banner->setPixmap(AssetGenerator::instance().pixmap(
        QStringLiteral("assets/banners/banner-profiles.svg"), QSize(900, 120)));
    banner->setScaledContents(true);
    banner->setFixedHeight(120);
    heroLayout->addWidget(banner);

    auto* heroActions = new QHBoxLayout();
    heroActions->setContentsMargins(24, 0, 24, 12);
    heroActions->addStretch();
    auto* newBtn = new QPushButton(QStringLiteral("+ Nouveau profil"), hero);
    newBtn->setObjectName(QStringLiteral("primaryButton"));
    heroActions->addWidget(newBtn, 0, Qt::AlignTop);
    heroLayout->addLayout(heroActions);
    root->addWidget(hero);

    const ProfileModel model = ProfileService::instance().model();
    const auto& profiles = model.profiles();
    int activeIdx = ProfileService::instance().activeIndex();

    auto* body = new QHBoxLayout();
    body->setSpacing(20);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setObjectName(QStringLiteral("profileScroll"));
    auto* listHost = new QWidget(scroll);
    auto* grid = new QGridLayout(listHost);
    grid->setSpacing(12);

    for (int i = 0; i < profiles.size(); ++i)
    {
        const auto& p = profiles.at(i);
        auto* card = new QPushButton(listHost);
        card->setObjectName(i == activeIdx ? QStringLiteral("profileCardActive") : QStringLiteral("profileCard"));
        card->setCursor(Qt::PointingHandCursor);
        card->setMinimumHeight(88);

        auto* cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(14, 12, 14, 12);

        auto* av = new QLabel(card);
        av->setFixedSize(44, 44);
        av->setPixmap(AssetGenerator::instance().pixmap(avatarForGame(p.game), QSize(44, 44)));
        av->setScaledContents(true);
        av->setAttribute(Qt::WA_TransparentForMouseEvents);
        cardLayout->addWidget(av);

        auto* col = new QVBoxLayout();
        auto* nameLb = new QLabel(p.name, card);
        nameLb->setObjectName(QStringLiteral("profileCardName"));
        nameLb->setAttribute(Qt::WA_TransparentForMouseEvents);
        auto* gameLb = new QLabel(p.game, card);
        gameLb->setObjectName(QStringLiteral("profileCardGame"));
        gameLb->setAttribute(Qt::WA_TransparentForMouseEvents);
        col->addWidget(nameLb);
        col->addWidget(gameLb);
        cardLayout->addLayout(col, 1);

        if (i == activeIdx)
        {
            auto* badge = new QLabel(QStringLiteral("ACTIF"), card);
            badge->setObjectName(QStringLiteral("profileActiveBadge"));
            badge->setAttribute(Qt::WA_TransparentForMouseEvents);
            cardLayout->addWidget(badge);
        }

        grid->addWidget(card, i / 2, i % 2);

        connect(card, &QPushButton::clicked, this, [this, i]() {
            ProfileService::instance().applyProfile(i);
        });
    }

    scroll->setWidget(listHost);
    body->addWidget(scroll, 3);

    const auto& active = profiles.at(qBound(0, activeIdx, profiles.size() - 1));

    auto* detailPanel = new QFrame(this);
    detailPanel->setObjectName(QStringLiteral("profileDetailHero"));
    auto* detailLayout = new QVBoxLayout(detailPanel);
    detailLayout->setContentsMargins(24, 24, 24, 24);
    detailLayout->setSpacing(16);

    auto* detailHeader = new QHBoxLayout();
    auto* bigAvatar = new QLabel(detailPanel);
    bigAvatar->setFixedSize(72, 72);
    bigAvatar->setPixmap(AssetGenerator::instance().pixmap(avatarForGame(active.game), QSize(72, 72)));
    bigAvatar->setScaledContents(true);
    detailHeader->addWidget(bigAvatar);

    auto* headerText = new QVBoxLayout();
    auto* activeName = new QLabel(active.name, detailPanel);
    activeName->setObjectName(QStringLiteral("profileDetailName"));
    auto* activeGame = new QLabel(active.game, detailPanel);
    activeGame->setObjectName(QStringLiteral("profileDetailGame"));
    headerText->addWidget(activeName);
    headerText->addWidget(activeGame);
    detailHeader->addLayout(headerText, 1);
    detailLayout->addLayout(detailHeader);

    auto* stats = new QGridLayout();
    auto addStat = [&](int row, int col, const QString& label, const QString& value) {
        auto* box = new QFrame(detailPanel);
        box->setObjectName(QStringLiteral("profileStatBox"));
        auto* bl = new QVBoxLayout(box);
        bl->setContentsMargins(12, 10, 12, 10);
        auto* vl = new QLabel(value, box);
        vl->setObjectName(QStringLiteral("profileStatValue"));
        auto* ll = new QLabel(label, box);
        ll->setObjectName(QStringLiteral("profileStatLabel"));
        bl->addWidget(vl);
        bl->addWidget(ll);
        stats->addWidget(box, row, col);
    };

    addStat(0, 0, QStringLiteral("Macros"), QString::number(MacroService::instance().activeMacros().size()));
    addStat(0, 1, QStringLiteral("Peripheriques"), QStringLiteral("3"));
    addStat(1, 0, QStringLiteral("Sync cloud"), active.cloud ? QStringLiteral("ON") : QStringLiteral("OFF"));
    addStat(1, 1, QStringLiteral("Auto-detect"), active.autoDetect ? QStringLiteral("ON") : QStringLiteral("OFF"));
    detailLayout->addLayout(stats);

    auto* cloudToggle = new QCheckBox(QStringLiteral("Synchronisation cloud"), detailPanel);
    cloudToggle->setChecked(active.cloud);
    detailLayout->addWidget(cloudToggle);

    auto* autoToggle = new QCheckBox(QStringLiteral("Detection automatique du jeu"), detailPanel);
    autoToggle->setChecked(active.autoDetect);
    detailLayout->addWidget(autoToggle);

    detailLayout->addStretch();

    auto* applyBtn = new QPushButton(QStringLiteral("Appliquer le profil"), detailPanel);
    applyBtn->setObjectName(QStringLiteral("primaryButton"));
    detailLayout->addWidget(applyBtn);

    body->addWidget(detailPanel, 2);
    root->addLayout(body, 1);
}
