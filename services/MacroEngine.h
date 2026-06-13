#pragma once

#include "../models/MacroModel.h"
#include <QObject>
#include <QHash>
#include <QTimer>

class MacroEngine : public QObject
{
    Q_OBJECT
public:
    static MacroEngine& instance();

    void start();
    void stop();
    bool isRunning() const { return m_running; }

    void onMouseSideButton(int buttonIndex);
    void toggleMaster();
    void setMasterEnabled(bool enabled);
    void simulateKeyLabel(const QString& label);

signals:
    void masterToggled(bool enabled);
    void macroTriggered(const QString& keyLabel);

public slots:
    void handleSideButton(int buttonIndex);

private slots:
    void onTick();
    void onMasterChanged(bool enabled);

private:
    explicit MacroEngine(QObject* parent = nullptr);
    bool handleSideLabel(const QString& sideLabel);
    bool shouldRunMacro(const MacroEntry& entry) const;
    quint16 virtualKeyForLabel(const QString& label) const;
    void installGlobalMouseHook();
    void removeGlobalMouseHook();

    QTimer m_timer;
    QHash<int, qint64> m_lastFireMs;
    bool m_running = false;
};
