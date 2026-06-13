#include "ThemeManager.h"
#include "../core/AppPaths.h"
#include <QApplication>
#include <QDir>
#include <QFile>

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {}

QString ThemeManager::readStyleSheet(const QString& fileName) const
{
    const QString qrcPath = QStringLiteral(":/styles/") + fileName;
    QFile f(qrcPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString::fromUtf8(f.readAll());

    const QString disk = QDir(AppPaths::stylesRoot()).filePath(fileName);
    f.setFileName(disk);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString::fromUtf8(f.readAll());

    return {};
}

void ThemeManager::loadTheme(const QString& themeName)
{
    Q_UNUSED(themeName);
}

void ThemeManager::applyGlobalStyle(QApplication* app)
{
    QString style = readStyleSheet(QStringLiteral("main.qss"));
    style += readStyleSheet(QStringLiteral("sidebar.qss"));
    style += readStyleSheet(QStringLiteral("cards.qss"));
    style += readStyleSheet(QStringLiteral("keyboard.qss"));
    style += readStyleSheet(QStringLiteral("dock.qss"));
    style += readStyleSheet(QStringLiteral("rgb.qss"));
    style += readStyleSheet(QStringLiteral("titlebar.qss"));
    app->setStyleSheet(style);
}
