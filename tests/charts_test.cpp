#include <QtQuickTest/quicktest.h>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTemporaryDir>
#include <QQmlPropertyMap>
#include "overlay_config.h"
#include "monitor_settings.h"

class Setup : public QObject {
    Q_OBJECT
public slots:
    void qmlEngineAvailable(QQmlEngine *engine) {
        QSettings settings(directory.filePath("config.ini"), QSettings::IniFormat);
        config.setMonitor(readMonitorSettings(settings));
        config.setMonitorMode(true);
        engine->rootContext()->setContextProperty("cfg", &config);
        auto *window = new QQmlPropertyMap(engine);
        window->insert("visible", false);
        engine->rootContext()->setContextProperty("overlayWindow", window);
    }
private:
    QTemporaryDir directory;
    OverlayConfig config;
};

QUICK_TEST_MAIN_WITH_SETUP(charts, Setup)
#include "charts_test.moc"
