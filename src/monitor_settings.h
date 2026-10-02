#pragma once

#include <QColor>
#include <QSettings>
#include <QVariantMap>
#include <algorithm>

/** Read and validate the optional system-monitor configuration. */
inline QVariantMap readMonitorSettings(QSettings &settings)
{
    settings.beginGroup(QStringLiteral("monitor"));
    const auto bounded = [&settings](const char *key, int fallback, int low, int high) {
        bool ok = false;
        const int value = settings.value(key, fallback).toInt(&ok);
        return ok ? std::clamp(value, low, high) : fallback;
    };
    QVariantMap result{
        {"columns", bounded("columns", 2, 1, 6)},
        {"chartWidth", bounded("chartWidth", 260, 200, 800)},
        {"chartHeight", bounded("chartHeight", 130, 100, 400)},
        {"historySeconds", bounded("historySeconds", 60, 10, 3600)},
        {"updateInterval", bounded("updateInterval", 1000, 500, 60000)},
        {"fontSize", bounded("fontSize", 12, 9, 24)}
    };
    struct Card { const char *key; const char *label; const char *sensor; const char *color; bool percent; };
    const Card defaults[] = {
        {"cpu", "CPU", "cpu/all/usage", "#3daee9", true},
        {"gpu", "GPU", "gpu/all/usage", "#a78bfa", true},
        {"ram", "RAM", "memory/physical/used", "#1cdc9a", false},
        {"vram", "VRAM", "gpu/all/usedVram", "#f6b44b", false},
        {"swap", "Swap", "memory/swap/used", "#e879b9", false},
        {"disk", "Disk", "disk/all/used", "#f6746b", false}
    };
    QVariantList cards;
    for (const auto &card : defaults) {
        const QString key = QString::fromLatin1(card.key);
        if (!settings.value(key + "Enabled", true).toBool()) continue;
        const QString configuredColor = settings.value(key + "Color", card.color).toString();
        cards.append(QVariantMap{
            {"label", settings.value(key + "Label", card.label).toString()},
            {"sensorId", settings.value(key + "Sensor", card.sensor).toString().trimmed()},
            {"accent", QColor::isValidColorName(configuredColor) ? configuredColor : QString::fromLatin1(card.color)},
            {"percent", card.percent}
        });
    }
    result.insert("cards", cards);
    settings.endGroup();
    return result;
}
