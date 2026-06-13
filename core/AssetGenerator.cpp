#include "AssetGenerator.h"
#include "AppPaths.h"
#include "Logger.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QStandardPaths>

namespace
{
const char* kSubDirs[] = {
    "icons", "devices", "previews", "wallpapers",
    "logos", "avatars", "themes", "animations",
    "assets/branding", "assets/status", "assets/ui",
    "assets/games", "assets/macro", "assets/badges",
    "assets/empty", "assets/mouse", "assets/lighting",
    "assets/keyboard", "assets/banners"
};

QByteArray placeholderSvg(const QString& title, const QString& subtitle)
{
    return QString(
        R"SVG(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256">
  <defs>
    <linearGradient id="bg" x1="0" y1="0" x2="1" y2="1">
      <stop offset="0%" stop-color="#121218"/>
      <stop offset="100%" stop-color="#0a0a0c"/>
    </linearGradient>
    <filter id="glow"><feGaussianBlur stdDeviation="3"/><feMerge><feMergeNode/><feMergeNode in="SourceGraphic"/></feMerge></filter>
  </defs>
  <rect width="256" height="256" rx="16" fill="url(#bg)" stroke="#e02626" stroke-width="2"/>
  <rect x="16" y="16" width="224" height="224" rx="12" fill="none" stroke="#e02626" stroke-width="1" opacity="0.3" filter="url(#glow)"/>
  <text x="128" y="120" text-anchor="middle" font-family="Segoe UI,sans-serif" font-size="22" font-weight="700" fill="#f0f0f5">%1</text>
  <text x="128" y="152" text-anchor="middle" font-family="Segoe UI,sans-serif" font-size="12" fill="#9090a0">%2</text>
</svg>)SVG")
        .arg(title, subtitle)
        .toUtf8();
}
}

AssetGenerator& AssetGenerator::instance()
{
    static AssetGenerator gen;
    return gen;
}

AssetGenerator::AssetGenerator()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString devRoot = QDir(appDir).absoluteFilePath(QStringLiteral("../resources"));
    const QString srcRoot = QDir(appDir).absoluteFilePath(QStringLiteral("../../resources"));

    if (QDir(devRoot).exists())
        m_root = QDir(devRoot).canonicalPath();
    else if (QDir(srcRoot).exists())
        m_root = QDir(srcRoot).canonicalPath();
    else
        m_root = QDir(appDir).absoluteFilePath(QStringLiteral("resources"));

    buildManifest();
}

void AssetGenerator::buildManifest()
{
    m_manifest = {
        QStringLiteral("icons/mission-control.svg"),
        QStringLiteral("icons/device-center.svg"),
        QStringLiteral("icons/macro-studio.svg"),
        QStringLiteral("icons/macro-library.svg"),
        QStringLiteral("icons/profile-manager.svg"),
        QStringLiteral("icons/activity-monitor.svg"),
        QStringLiteral("icons/analytics-center.svg"),
        QStringLiteral("icons/mobile-command.svg"),
        QStringLiteral("icons/lighting-engine.svg"),
        QStringLiteral("icons/settings-hub.svg"),
        QStringLiteral("icons/mobile-phone.svg"),
        QStringLiteral("icons/mobile-sync.svg"),
        QStringLiteral("icons/mobile-remote.svg"),
        QStringLiteral("icons/mobile-rgb.svg"),
        QStringLiteral("logos/gamex-logo.svg"),
        QStringLiteral("logos/gamex-mark.svg"),
        QStringLiteral("devices/keyboard-60.svg"),
        QStringLiteral("devices/keyboard-tkl.svg"),
        QStringLiteral("devices/mouse.svg"),
        QStringLiteral("devices/mouse-elite-m40.svg"),
        QStringLiteral("assets/elite-m40.svg"),
        QStringLiteral("devices/mouse-interactive.svg"),
        QStringLiteral("devices/dock-mouse-elite-m40.svg"),
        QStringLiteral("devices/headset.svg"),
        QStringLiteral("devices/monitor.svg"),
        QStringLiteral("devices/gpu.svg"),
        QStringLiteral("devices/motherboard.svg"),
        QStringLiteral("devices/cpu.svg"),
        QStringLiteral("devices/ram.svg"),
        QStringLiteral("devices/fan-rgb.svg"),
        QStringLiteral("devices/dock-motherboard.svg"),
        QStringLiteral("devices/dock-gpu.svg"),
        QStringLiteral("devices/dock-keyboard.svg"),
        QStringLiteral("devices/dock-mouse.svg"),
        QStringLiteral("devices/dock-headset.svg"),
        QStringLiteral("devices/dock-aio.svg"),
        QStringLiteral("devices/dock-ssd.svg"),
        QStringLiteral("devices/dock-usb.svg"),
        QStringLiteral("wallpapers/gamex-dark.svg"),
        QStringLiteral("avatars/diablo-iv.svg"),
        QStringLiteral("avatars/default-game.svg"),
        QStringLiteral("themes/gamex-dark.json"),
        QStringLiteral("animations/glow-pulse.svg"),
        QStringLiteral("previews/keyboard-rgb-preview.svg"),
        QStringLiteral("previews/mouse-rgb-preview.svg"),
        QStringLiteral("assets/branding/app-logo.svg"),
        QStringLiteral("assets/branding/app-mark.svg"),
        QStringLiteral("assets/status/dot-green.svg"),
        QStringLiteral("assets/status/dot-orange.svg"),
        QStringLiteral("assets/status/dot-red.svg"),
        QStringLiteral("assets/ui/btn-add.svg"),
        QStringLiteral("assets/ui/btn-edit.svg"),
        QStringLiteral("assets/ui/btn-delete.svg"),
        QStringLiteral("assets/ui/btn-save.svg"),
        QStringLiteral("assets/ui/btn-test.svg"),
        QStringLiteral("assets/games/diablo-iv.svg"),
        QStringLiteral("assets/games/diablo-iii.svg"),
        QStringLiteral("assets/games/wow.svg"),
        QStringLiteral("assets/games/valorant.svg"),
        QStringLiteral("assets/macro/autoclick.svg"),
        QStringLiteral("assets/macro/toggle-l1.svg"),
        QStringLiteral("assets/macro/timeline.svg"),
        QStringLiteral("assets/badges/cloud.svg"),
        QStringLiteral("assets/badges/engine-on.svg"),
        QStringLiteral("assets/badges/engine-off.svg"),
        QStringLiteral("assets/badges/profile-active.svg"),
        QStringLiteral("assets/empty/no-macros.svg"),
        QStringLiteral("assets/empty/no-devices.svg"),
        QStringLiteral("assets/empty/no-profiles.svg"),
        QStringLiteral("assets/mouse/elite-m40-top.svg"),
        QStringLiteral("assets/mouse/elite-m40-side.svg"),
        QStringLiteral("assets/mouse/elite-m40-hero.svg"),
        QStringLiteral("assets/banners/banner-profiles.svg"),
        QStringLiteral("assets/banners/banner-devices.svg"),
        QStringLiteral("assets/banners/banner-macros.svg"),
        QStringLiteral("assets/banners/banner-lighting.svg"),
        QStringLiteral("icons/nav-profiles.svg"),
        QStringLiteral("icons/nav-devices.svg"),
        QStringLiteral("icons/nav-macros.svg"),
        QStringLiteral("icons/nav-lighting.svg"),
        QStringLiteral("assets/lighting/rgb-wave.svg"),
        QStringLiteral("assets/lighting/rgb-static.svg"),
        QStringLiteral("assets/keyboard/tkl-blue.svg")
    };

    m_svgTemplates[QStringLiteral("icons/mission-control.svg")] = placeholderSvg(QStringLiteral("Mission"), QStringLiteral("Control"));
    m_svgTemplates[QStringLiteral("icons/device-center.svg")] = placeholderSvg(QStringLiteral("Device"), QStringLiteral("Center"));
    m_svgTemplates[QStringLiteral("icons/macro-studio.svg")] = placeholderSvg(QStringLiteral("Macro"), QStringLiteral("Studio"));
    m_svgTemplates[QStringLiteral("icons/macro-library.svg")] = placeholderSvg(QStringLiteral("Macro"), QStringLiteral("Library"));
    m_svgTemplates[QStringLiteral("icons/profile-manager.svg")] = placeholderSvg(QStringLiteral("Profile"), QStringLiteral("Manager"));
    m_svgTemplates[QStringLiteral("icons/activity-monitor.svg")] = placeholderSvg(QStringLiteral("Activity"), QStringLiteral("Monitor"));
    m_svgTemplates[QStringLiteral("icons/analytics-center.svg")] = placeholderSvg(QStringLiteral("Analytics"), QStringLiteral("Center"));
    m_svgTemplates[QStringLiteral("icons/mobile-command.svg")] = placeholderSvg(QStringLiteral("Mobile"), QStringLiteral("Command"));
    m_svgTemplates[QStringLiteral("icons/lighting-engine.svg")] = placeholderSvg(QStringLiteral("Lighting"), QStringLiteral("Engine"));
    m_svgTemplates[QStringLiteral("icons/settings-hub.svg")] = placeholderSvg(QStringLiteral("Settings"), QStringLiteral("Hub"));
    m_svgTemplates[QStringLiteral("logos/gamex-logo.svg")] = placeholderSvg(QStringLiteral("GAMEX"), QStringLiteral("CLICKER"));
    m_svgTemplates[QStringLiteral("logos/gamex-mark.svg")] = placeholderSvg(QStringLiteral("G"), QStringLiteral("Mark"));
    m_svgTemplates[QStringLiteral("devices/keyboard-60.svg")] = placeholderSvg(QStringLiteral("60%"), QStringLiteral("Keyboard"));
    m_svgTemplates[QStringLiteral("devices/keyboard-tkl.svg")] = placeholderSvg(QStringLiteral("TKL"), QStringLiteral("Keyboard"));
    m_svgTemplates[QStringLiteral("devices/mouse.svg")] = placeholderSvg(QStringLiteral("Gaming"), QStringLiteral("Mouse"));
    m_svgTemplates[QStringLiteral("devices/headset.svg")] = placeholderSvg(QStringLiteral("Gaming"), QStringLiteral("Headset"));
    m_svgTemplates[QStringLiteral("devices/monitor.svg")] = placeholderSvg(QStringLiteral("Gaming"), QStringLiteral("Monitor"));
    m_svgTemplates[QStringLiteral("devices/gpu.svg")] = placeholderSvg(QStringLiteral("GPU"), QStringLiteral("Graphics"));
    m_svgTemplates[QStringLiteral("devices/motherboard.svg")] = placeholderSvg(QStringLiteral("MB"), QStringLiteral("Motherboard"));
    m_svgTemplates[QStringLiteral("devices/cpu.svg")] = placeholderSvg(QStringLiteral("CPU"), QStringLiteral("Processor"));
    m_svgTemplates[QStringLiteral("devices/ram.svg")] = placeholderSvg(QStringLiteral("RAM"), QStringLiteral("Memory"));
    m_svgTemplates[QStringLiteral("devices/fan-rgb.svg")] = placeholderSvg(QStringLiteral("RGB"), QStringLiteral("Fan"));
}

QString AssetGenerator::diskPath(const QString& relativePath) const
{
    return QDir(m_root).absoluteFilePath(relativePath);
}

bool AssetGenerator::ensureDirectory(const QString& path) const
{
    QDir dir(path);
    if (dir.exists()) return true;
    return dir.mkpath(QStringLiteral("."));
}

bool AssetGenerator::writeIfMissing(const QString& relativePath, const QByteArray& content) const
{
    const QString full = diskPath(relativePath);
    if (QFileInfo::exists(full))
        return true;

    QFileInfo fi(full);
    ensureDirectory(fi.absolutePath());

    QFile f(full);
    if (!f.open(QIODevice::WriteOnly))
    {
        Logger::warn(QStringLiteral("AssetGenerator: impossible d'ecrire %1").arg(full));
        return false;
    }
    f.write(content);
    Logger::info(QStringLiteral("AssetGenerator: genere %1").arg(relativePath));
    return true;
}

bool AssetGenerator::renderSvgToPng(const QString& svgPath, const QString& pngPath,
                                      int width, int height) const
{
    QSvgRenderer renderer(svgPath);
    if (!renderer.isValid())
        return false;

    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    renderer.render(&painter, QRectF(0, 0, width, height));
    painter.end();

    ensureDirectory(QFileInfo(pngPath).absolutePath());
    return image.save(pngPath, "PNG");
}

bool AssetGenerator::generatePngPreviewIfMissing(const QString& svgRel, const QString& pngRel) const
{
    const QString pngFull = diskPath(pngRel);
    if (QFileInfo::exists(pngFull))
        return true;

    QString svgFull = diskPath(svgRel);
    if (!QFileInfo::exists(svgFull))
    {
        const QString qrc = QStringLiteral(":/") + svgRel;
        if (QFileInfo::exists(qrc))
            svgFull = qrc;
        else
            return false;
    }

    return renderSvgToPng(svgFull, pngFull);
}

bool AssetGenerator::ensureAll(const QString& resourcesRoot)
{
    if (!resourcesRoot.isEmpty())
        m_root = resourcesRoot;

    ensureDirectory(m_root);
    for (const char* sub : kSubDirs)
        ensureDirectory(QDir(m_root).absoluteFilePath(QString::fromLatin1(sub)));

    for (const QString& rel : m_manifest)
    {
        if (rel.endsWith(QStringLiteral(".json")))
        {
            if (!QFileInfo::exists(diskPath(rel)))
            {
                const QByteArray theme = R"({
  "name": "GAMEX Dark",
  "colors": { "accent": "#e02626", "background": "#0a0a0c" }
})";
                writeIfMissing(rel, theme);
            }
            continue;
        }

        if (rel.endsWith(QStringLiteral(".svg")))
        {
            if (!QFileInfo::exists(diskPath(rel)))
            {
                const QByteArray tpl = m_svgTemplates.value(rel);
                if (!tpl.isEmpty())
                    writeIfMissing(rel, tpl);
            }
        }
    }

    struct PreviewPair { const char* svg; const char* png; };
    const PreviewPair previews[] = {
        { "devices/keyboard-60.svg", "previews/keyboard-60-4k.png" },
        { "devices/keyboard-tkl.svg", "previews/keyboard-tkl-4k.png" },
        { "devices/mouse.svg", "previews/mouse-4k.png" },
        { "devices/mouse-interactive.svg", "previews/mouse-interactive-4k.png" },
        { "devices/headset.svg", "previews/headset-4k.png" },
        { "devices/gpu.svg", "previews/gpu-4k.png" },
        { "devices/motherboard.svg", "previews/motherboard-4k.png" },
        { "devices/monitor.svg", "previews/monitor-4k.png" },
        { "previews/keyboard-rgb-preview.svg", "previews/keyboard-rgb-preview-4k.png" },
        { "previews/mouse-rgb-preview.svg", "previews/mouse-rgb-preview-4k.png" },
        { "assets/mouse/elite-m40-hero.svg", "previews/mouse-elite-m40-4k.png" },
        { "wallpapers/gamex-dark.svg", "wallpapers/gamex-dark-4k.png" }
    };

    for (const auto& p : previews)
        generatePngPreviewIfMissing(QString::fromLatin1(p.svg), QString::fromLatin1(p.png));

    Logger::info(QStringLiteral("AssetGenerator: manifest OK (%1)").arg(m_root));
    return true;
}

QString AssetGenerator::resolve(const QString& relativePath) const
{
    if (relativePath.startsWith(QStringLiteral("assets/")))
    {
        const QString assetRel = relativePath.mid(QStringLiteral("assets/").size());
        return AppPaths::resolveAsset(assetRel);
    }

    const QString qrc = QStringLiteral(":/") + relativePath;
    if (QFileInfo::exists(qrc))
        return qrc;

    const QString disk = diskPath(relativePath);
    if (QFileInfo::exists(disk))
        return disk;

    const QString inResources = QDir(AppPaths::resourcesRoot()).filePath(relativePath);
    if (QFileInfo::exists(inResources))
        return inResources;

    if (relativePath.endsWith(QStringLiteral(".svg")))
    {
        const QByteArray tpl = m_svgTemplates.value(relativePath);
        if (!tpl.isEmpty())
        {
            const_cast<AssetGenerator*>(this)->writeIfMissing(relativePath, tpl);
            if (QFileInfo::exists(disk))
                return disk;
        }
    }

    return qrc;
}

QByteArray AssetGenerator::svgHeader() { return {}; }
QByteArray AssetGenerator::svgFooter() { return {}; }

QIcon AssetGenerator::icon(const QString& relativePath, const QSize& size) const
{
    return QIcon(pixmap(relativePath, size));
}

QPixmap AssetGenerator::loadPixmap(const QString& relativePath) const
{
    const QString path = resolve(relativePath);
    QPixmap pm(path);
    if (!pm.isNull())
        return pm;

    if (path.startsWith(QStringLiteral(":/")))
    {
        QPixmap fromQrc(path);
        if (!fromQrc.isNull())
            return fromQrc;
    }
    return {};
}

QPixmap AssetGenerator::pixmap(const QString& relativePath, const QSize& size) const
{
    const QString path = resolve(relativePath);
    QSvgRenderer renderer(path);
    if (!renderer.isValid())
    {
        QPixmap fallback(size);
        fallback.fill(Qt::transparent);
        QPainter painter(&fallback);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QColor(20, 20, 28));
        painter.setPen(QPen(QColor(52, 152, 219), 2));
        painter.drawRoundedRect(QRectF(QPointF(1, 1), size - QSize(2, 2)), 6, 6);
        return fallback;
    }

    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing);
    renderer.render(&painter, QRectF(QPointF(0, 0), size));
    return pm;
}
