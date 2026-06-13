#include "Application.h"
#include "ThemeManager.h"
#include "SettingsManager.h"
#include "NavigationManager.h"
#include "ClickSoundFilter.h"
#include "../windows/MainWindow.h"
#include "../core/AppPaths.h"
#include "../core/Enums.h"
#include "../core/Logger.h"
#include "../core/AssetGenerator.h"
#include "../services/MacroEngine.h"
#include "../core/DebugManager.h"
#include <QApplication>
#include <QDir>

Application::Application(int& argc, char** argv)
{
    m_app = new QApplication(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Game_macro_elite"));
    QApplication::setApplicationDisplayName(QStringLiteral("Game_macro_elite"));
    QApplication::setOrganizationName(QStringLiteral("GameX"));
    QApplication::setQuitOnLastWindowClosed(true);

    connect(m_app, &QApplication::aboutToQuit, []() {
        MacroEngine::instance().stop();
    });

    DebugManager::instance().initialize(argc, argv);

    AppPaths::initialize();
    m_theme = std::make_unique<ThemeManager>();
    m_settings = std::make_unique<SettingsManager>();
    m_navigation = std::make_unique<NavigationManager>();

    m_app->installEventFilter(new ClickSoundFilter(m_app));
}

Application::~Application()
{
    MacroEngine::instance().stop();
    delete m_app;
}

int Application::run()
{
    Logger::info(QStringLiteral("Demarrage Game_macro_elite"));

    AssetGenerator::instance().ensureAll(AppPaths::resourcesRoot());
    m_theme->applyGlobalStyle(m_app);

    m_mainWindow = std::make_unique<MainWindow>(m_navigation.get());
    MacroEngine::instance().start();
    m_mainWindow->show();

    return m_app->exec();
}
