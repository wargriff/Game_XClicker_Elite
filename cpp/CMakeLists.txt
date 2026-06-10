cmake_minimum_required(VERSION 3.16)
project(GameXClicker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(httplib json)

set(GX_CORE_SOURCES
    src/config/paths.cpp
    src/core/win32_input.cpp
    src/core/engine.cpp
    src/core/rgb_engine.cpp
    src/services/engine_proxy.cpp
    src/services/profile_manager.cpp
    src/services/device_scanner.cpp
    src/services/sidecar_api.cpp
    src/app/bootstrap.cpp
)

set(GX_WIN32_SOURCES
    src/main.cpp
    src/control_panel.cpp
    src/process_launcher.cpp
    ${GX_CORE_SOURCES}
)

add_executable(GameXClicker WIN32 ${GX_WIN32_SOURCES})
target_include_directories(GameXClicker PRIVATE src)
target_link_libraries(GameXClicker PRIVATE httplib::httplib nlohmann_json::nlohmann_json user32 gdi32 shell32 comctl32)

if(MSVC)
    target_compile_options(GameXClicker PRIVATE /W4 /utf-8)
endif()

add_custom_command(TARGET GameXClicker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GameXClicker> ${CMAKE_SOURCE_DIR}/../GameXClicker.exe
    COMMENT "Copy GameXClicker.exe to project root"
)

option(GX_BUILD_QT "Build Qt6 Control Panel" ON)
if(GX_BUILD_QT)
    find_package(Qt6 QUIET COMPONENTS Widgets)
    if(Qt6_FOUND)
        set(CMAKE_AUTOMOC ON)
        qt_add_executable(GameXClickerQt
            src/main_qt.cpp
            src/process_launcher.cpp
            src/ui/qt/control_panel_window.cpp
            ${GX_CORE_SOURCES}
        )
        target_include_directories(GameXClickerQt PRIVATE src)
        target_link_libraries(GameXClickerQt PRIVATE
            Qt6::Widgets
            httplib::httplib
            nlohmann_json::nlohmann_json
            user32 shell32
        )
        if(MSVC)
            target_compile_options(GameXClickerQt PRIVATE /W4 /utf-8)
        endif()
        add_custom_command(TARGET GameXClickerQt POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GameXClickerQt> ${CMAKE_SOURCE_DIR}/../GameXClickerQt.exe
        )
        message(STATUS "Qt6 found — building GameXClickerQt.exe")
    else()
        message(STATUS "Qt6 not found — skip GameXClickerQt (install Qt 6 Widgets)")
    endif()
endif()
