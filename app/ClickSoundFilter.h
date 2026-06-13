#pragma once

#include <QObject>

class ClickSoundFilter : public QObject
{
    Q_OBJECT
public:
    explicit ClickSoundFilter(QObject* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};
