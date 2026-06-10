#pragma once

#include <QMainWindow>
#include <memory>

namespace gx {
class BootContext;
}

namespace gx::ui {

class ControlPanelWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ControlPanelWindow(QWidget* parent = nullptr);
    ~ControlPanelWindow() override;

private slots:
    void onStartEngine();
    void onNativeUi();
    void onWebUi();
    void onBuild();
    void onLaunchExe();
    void onRepair();

private:
    void log(const QString& msg);

    std::unique_ptr<gx::BootContext> boot_;
    class QTextEdit* log_view_ = nullptr;
};

}  // namespace gx::ui
