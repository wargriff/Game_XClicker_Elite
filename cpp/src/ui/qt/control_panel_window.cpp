#include "ui/qt/control_panel_window.hpp"

#include "app/bootstrap.hpp"
#include "config/paths.hpp"
#include "process_launcher.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace gx::ui {

static const char* kStyle = R"(
QMainWindow, QWidget { background-color: #121216; color: #e6e6eb; }
QPushButton {
  background-color: #1e1e26; border: 1px solid #3a3a44; border-radius: 6px;
  padding: 14px; font-size: 13px; text-align: left;
}
QPushButton:hover { border-color: #dc2626; }
QLabel#title { font-size: 20px; font-weight: bold; color: #f87171; }
QTextEdit { background: #16161c; border: 1px solid #3a3a44; color: #9ca3af; }
)";

ControlPanelWindow::ControlPanelWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("GAME XCLICKER — Control Panel (Qt C++)");
    resize(900, 640);
    setStyleSheet(kStyle);

    auto* central = new QWidget(this);
    auto* root = new QHBoxLayout(central);

    auto* sidebar = new QVBoxLayout();
    auto* brand = new QLabel("GAME X\nCLICKER");
    brand->setObjectName("title");
    sidebar->addWidget(brand);
    sidebar->addWidget(new QLabel("Control Panel Qt"));
    sidebar->addStretch();
    root->addLayout(sidebar, 1);

    auto* main = new QVBoxLayout();
    main->addWidget(new QLabel("Choisissez comment lancer Game XClicker Elite."));
    auto* grid = new QGridLayout();
    const struct {
        const char* title;
        const char* desc;
    } cards[] = {
        {"MOTEUR + API REST", "C++ port 17840 — macros natives"},
        {"INTERFACE NATIVE", "PyQt sanctuary (transition)"},
        {"INTERFACE WEB", "ui-web + navigateur"},
        {"BUILD .EXE", "PyInstaller desktop"},
        {"LANCER .EXE", "Version Bureau"},
        {"REPARER", "git + dependances"},
    };
    QPushButton* buttons[6];
    for (int i = 0; i < 6; ++i) {
        auto* b = new QPushButton(QString("%1\n%2").arg(cards[i].title, cards[i].desc));
        buttons[i] = b;
        grid->addWidget(b, i / 2, i % 2);
    }
    connect(buttons[0], &QPushButton::clicked, this, &ControlPanelWindow::onStartEngine);
    connect(buttons[1], &QPushButton::clicked, this, &ControlPanelWindow::onNativeUi);
    connect(buttons[2], &QPushButton::clicked, this, &ControlPanelWindow::onWebUi);
    connect(buttons[3], &QPushButton::clicked, this, &ControlPanelWindow::onBuild);
    connect(buttons[4], &QPushButton::clicked, this, &ControlPanelWindow::onLaunchExe);
    connect(buttons[5], &QPushButton::clicked, this, &ControlPanelWindow::onRepair);
    main->addLayout(grid);

    log_view_ = new QTextEdit();
    log_view_->setReadOnly(true);
    main->addWidget(log_view_, 1);
    root->addLayout(main, 4);

    setCentralWidget(central);
    log("Control Panel Qt pret.");
}

ControlPanelWindow::~ControlPanelWindow() {
    if (boot_) {
        boot_->sidecar->stop();
        boot_->proxy->stop();
    }
}

void ControlPanelWindow::log(const QString& msg) {
    if (log_view_) log_view_->append(msg);
}

void ControlPanelWindow::onStartEngine() {
    if (!boot_) {
        const auto root = project_root();
        boot_ = std::make_unique<gx::BootContext>(gx::bootstrap(root));
        log("Moteur C++ + API http://127.0.0.1:17840");
    } else {
        boot_->sidecar->stop();
        boot_->proxy->stop();
        boot_.reset();
        log("Moteur arrete.");
    }
}

void ControlPanelWindow::onNativeUi() {
    LaunchPythonMode(project_root(), L"--native");
    log("Lancement interface native Python...");
}

void ControlPanelWindow::onWebUi() {
    LaunchPythonMode(project_root(), L"--web");
    log("Lancement interface web...");
}

void ControlPanelWindow::onBuild() {
    LaunchPythonMode(project_root(), L"--build --desktop");
    log("Build lance...");
}

void ControlPanelWindow::onLaunchExe() {
    if (LaunchDesktopExe(project_root())) log(".exe ouvert.");
    else log(".exe introuvable.");
}

void ControlPanelWindow::onRepair() {
    LaunchRepair(project_root());
    log("REPARER lance.");
}

}  // namespace gx::ui
