cmake_minimum_required(VERSION 3.16)

# Generateur : Ninja Multi-Config uniquement (pas de ALL_BUILD / ZERO_CHECK / dossier x64).
# Binaires : build/Debug/ et build/Release/
# Visual Studio : Fichier > Ouvrir > Dossier (racine), preset "ninja-multi".

project(Game_macro_elite VERSION 1.2.0 LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/OutputLayout.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 — auto-detection Windows + QT_DIR / -DGMEL_QT_ROOT=
set(GMEL_QT_ROOT "" CACHE PATH "Chemin vers Qt6 (ex: C:/Qt/6.8.0/msvc2022_64)")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SetupQt.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DeployQt.cmake)
gmel_resolve_qt_prefix(GMEL_QT_PREFIX)

if(GMEL_QT_PREFIX)
    list(PREPEND CMAKE_PREFIX_PATH "${GMEL_QT_PREFIX}")
    message(STATUS "Game_macro_elite — Qt6 prefix: ${GMEL_QT_PREFIX}")
else()
    message(WARNING "Qt6 prefix not auto-detected. Set -DGMEL_QT_ROOT=C:/Qt/6.x.x/msvc2022_64 or QT_DIR")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

set(GMEL_SOURCES
    main.cpp
    app/Application.cpp
    app/ThemeManager.cpp
    app/SettingsManager.cpp
    app/NavigationManager.cpp
    app/PageRegistry.cpp
    app/ClickSoundFilter.cpp
    core/Logger.cpp
    core/DebugManager.cpp
    core/EventBus.cpp
    core/AppState.cpp
    core/AppPaths.cpp
    core/AssetGenerator.cpp
    core/UiSound.cpp
    windows/MainWindow.cpp
    windows/SplashScreen.cpp
    windows/AboutWindow.cpp
    windows/SettingsWindow.cpp
    pages/DeviceCenter/DeviceCenterPage.cpp
    pages/MacroLibrary/MacroLibraryPage.cpp
    pages/ProfileManager/ProfileManagerPage.cpp
    pages/LightingEngine/LightingPage.cpp
    widgets/sidebar/SidebarWidget.cpp
    widgets/sidebar/SidebarButton.cpp
    widgets/titlebar/TitleBarWidget.cpp
    widgets/titlebar/WindowChromeButton.cpp
    widgets/cards/InfoCard.cpp
    widgets/cards/StatsCard.cpp
    widgets/cards/DeviceCard.cpp
    widgets/keyboard/KeyboardWidget.cpp
    widgets/keyboard/KeyWidget.cpp
    widgets/keyboard/KeyAnimator.cpp
    widgets/mouse/MouseWidget.cpp
    widgets/mouse/MouseButtonWidget.cpp
    widgets/rgb/RGBPanelWidget.cpp
    widgets/rgb/RGBPreviewWidget.cpp
    widgets/rgb/ColorPickerWidget.cpp
    widgets/stats/StatsPanelWidget.cpp
    widgets/stats/CounterWidget.cpp
    widgets/dock/DeviceDockWidget.cpp
    widgets/dock/DockDeviceItem.cpp
    widgets/stack/AnimatedStackedWidget.cpp
    widgets/charts/CpuChartWidget.cpp
    widgets/charts/RamChartWidget.cpp
    animations/FadeAnimation.cpp
    animations/GlowAnimation.cpp
    animations/HoverAnimation.cpp
    graphics/GlowEffect.cpp
    graphics/ShadowEffect.cpp
    graphics/BlurEffect.cpp
    models/DeviceModel.cpp
    models/MacroModel.cpp
    models/ProfileModel.cpp
    services/DeviceService.cpp
    services/MacroService.cpp
    services/MacroEngine.cpp
    services/ProfileService.cpp
    services/RGBService.cpp
    resources/gamex.qrc
)

file(GLOB_RECURSE GMEL_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/windows/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/pages/*/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/widgets/*/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/widgets/*/*/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/services/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/animations/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.h"
)

add_executable(Game_macro_elite ${GMEL_SOURCES} ${GMEL_HEADERS})

target_include_directories(Game_macro_elite PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_link_libraries(Game_macro_elite PRIVATE Qt6::Widgets Qt6::Svg)

target_compile_definitions(Game_macro_elite PRIVATE GMEL_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_compile_definitions(Game_macro_elite PRIVATE $<$<CONFIG:Debug>:GMEL_DEBUG>)

if(WIN32)
    set_target_properties(Game_macro_elite PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(Game_macro_elite PRIVATE winmm)
    gmel_deploy_qt(Game_macro_elite)
endif()

set_target_properties(Game_macro_elite PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)

add_custom_command(TARGET Game_macro_elite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:Game_macro_elite>/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Game_macro_elite>/assets"
    COMMENT "Sync resources and assets to build output"
)

option(GMEL_BUILD_TESTS "Build Game_macro_elite unit tests" OFF)
find_package(Qt6 COMPONENTS Test QUIET)
if(GMEL_BUILD_TESTS AND Qt6Test_FOUND)
    enable_testing()
    add_executable(Game_macro_elite_tests tests/WidgetTests.cpp tests/ThemeTests.cpp tests/DeviceTests.cpp)
    target_link_libraries(Game_macro_elite_tests PRIVATE Qt6::Widgets Qt6::Test)
    add_test(NAME Game_macro_elite_tests COMMAND Game_macro_elite_tests)
endif()
