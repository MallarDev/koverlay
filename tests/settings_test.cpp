#include <QtTest>
#include <QTemporaryDir>
#include "monitor_settings.h"

class SettingsTest : public QObject {
    Q_OBJECT
private slots:
    void defaultsAndValidation() {
        QTemporaryDir directory;
        QSettings settings(directory.filePath("config.ini"), QSettings::IniFormat);
        const auto defaults = readMonitorSettings(settings);
        QCOMPARE(defaults.value("cards").toList().size(), 6);
        QCOMPARE(defaults.value("updateInterval").toInt(), 1000);
        settings.setValue("monitor/columns", -5);
        settings.setValue("monitor/updateInterval", "invalid");
        settings.setValue("monitor/historySeconds", 999999);
        settings.setValue("monitor/gpuEnabled", false);
        settings.setValue("monitor/cpuSensor", "cpu/cpu0/usage");
        settings.setValue("monitor/cpuColor", "not-a-color");
        const auto result = readMonitorSettings(settings);
        QCOMPARE(result.value("columns").toInt(), 1);
        QCOMPARE(result.value("updateInterval").toInt(), 1000);
        QCOMPARE(result.value("historySeconds").toInt(), 3600);
        const auto cards = result.value("cards").toList();
        QCOMPARE(cards.size(), 5);
        QCOMPARE(cards[0].toMap().value("sensorId").toString(), "cpu/cpu0/usage");
        QCOMPARE(cards[0].toMap().value("accent").toString(), "#3daee9");
    }
};

QTEST_GUILESS_MAIN(SettingsTest)
#include "settings_test.moc"
