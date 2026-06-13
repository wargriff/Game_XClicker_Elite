#include "AppPaths.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace
{
QString g_sourceRoot;

QString pickExisting(const QStringList& candidates)
{
    for (const QString& path : candidates)
    {
        if (path.isEmpty())
            continue;
        if (QFileInfo::exists(path))
            return QDir(path).canonicalPath();
    }
    return {};
}
}

void AppPaths::initialize(const QString& sourceRoot)
{
    if (!sourceRoot.isEmpty())
    {
        g_sourceRoot = QDir(sourceRoot).canonicalPath();
        return;
    }

#ifdef GMEL_SOURCE_DIR
    g_sourceRoot = QDir(QStringLiteral(GMEL_SOURCE_DIR)).canonicalPath();
#elif defined(GAMEX_SOURCE_DIR)
    g_sourceRoot = QDir(QStringLiteral(GAMEX_SOURCE_DIR)).canonicalPath();
#else
    const QString appDir = QCoreApplication::applicationDirPath();
    g_sourceRoot = pickExisting({
        QDir(appDir).absoluteFilePath(QStringLiteral("../..")),
        QDir(appDir).absoluteFilePath(QStringLiteral("..")),
        appDir
    });
#endif
}

QString AppPaths::sourceRoot()
{
    if (g_sourceRoot.isEmpty())
        initialize();
    return g_sourceRoot;
}

QString AppPaths::resourcesRoot()
{
    return QDir(sourceRoot()).filePath(QStringLiteral("resources"));
}

QString AppPaths::assetsRoot()
{
    return QDir(sourceRoot()).filePath(QStringLiteral("assets"));
}

QString AppPaths::stylesRoot()
{
    return QDir(sourceRoot()).filePath(QStringLiteral("styles"));
}

QString AppPaths::resolveResource(const QString& relativePath)
{
    const QString qrc = QStringLiteral(":/") + relativePath;
    if (QFileInfo::exists(qrc))
        return qrc;

    const QString inResources = QDir(resourcesRoot()).filePath(relativePath);
    if (QFileInfo::exists(inResources))
        return inResources;

    return qrc;
}

QString AppPaths::resolveAsset(const QString& relativePath)
{
    const QString qrcKey = QStringLiteral("assets/") + relativePath;
    const QString qrc = QStringLiteral(":/") + qrcKey;
    if (QFileInfo::exists(qrc))
        return qrc;

    const QString inResourcesAssets =
        QDir(resourcesRoot()).absoluteFilePath(QStringLiteral("assets/") + relativePath);
    if (QFileInfo::exists(inResourcesAssets))
        return inResourcesAssets;

    const QString inAssets = QDir(assetsRoot()).filePath(relativePath);
    if (QFileInfo::exists(inAssets))
        return inAssets;

    return inResourcesAssets;
}
