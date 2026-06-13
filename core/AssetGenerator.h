#pragma once

#include <QIcon>
#include <QMap>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStringList>

class AssetGenerator
{
public:
    static AssetGenerator& instance();

    /** Ensure resources/ tree exists and all manifest assets are present. */
    bool ensureAll(const QString& resourcesRoot = {});

    /** Resolve path: qrc first, then disk, generate if missing. */
    QString resolve(const QString& relativePath) const;

    /** Render SVG to PNG at given size (default 3840x2160). */
    bool renderSvgToPng(const QString& svgPath, const QString& pngPath,
                        int width = 3840, int height = 2160) const;

    QString resourcesRoot() const { return m_root; }

    QIcon icon(const QString& relativePath, const QSize& size = QSize(24, 24)) const;
    QPixmap pixmap(const QString& relativePath, const QSize& size) const;
    QPixmap loadPixmap(const QString& relativePath) const;

private:
    AssetGenerator();
    void buildManifest();
    bool ensureDirectory(const QString& path) const;
    bool writeIfMissing(const QString& relativePath, const QByteArray& content) const;
    bool generatePngPreviewIfMissing(const QString& svgRel, const QString& pngRel) const;
    QString diskPath(const QString& relativePath) const;
    static QByteArray svgHeader();
    static QByteArray svgFooter();

    QString m_root;
    QMap<QString, QByteArray> m_svgTemplates;
    QStringList m_manifest;
};
