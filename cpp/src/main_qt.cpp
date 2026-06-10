#include "ui/qt/control_panel_window.hpp"

#include "config/paths.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Game XClicker Control Panel");
    gx::set_project_root(gx::project_root());
    gx::ui::ControlPanelWindow window;
    window.show();
    return app.exec();
}
