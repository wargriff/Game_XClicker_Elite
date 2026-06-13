#pragma once

#include <QString>

namespace AppPaths
{
void initialize(const QString& sourceRoot = {});

QString sourceRoot();
QString resourcesRoot();
QString assetsRoot();
QString stylesRoot();

QString resolveResource(const QString& relativePath);
QString resolveAsset(const QString& relativePath);
}
