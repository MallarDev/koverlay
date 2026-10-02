#include <QtQuickTest/quicktest.h>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTemporaryDir>
#include <QWindow>
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
        engine->rootContext()->setContextProperty("overlayWindow", &window);
    }
private:
    QTemporaryDir directory;
    OverlayConfig config;
    QWindow window;
};

QUICK_TEST_MAIN_WITH_SETUP(charts, Setup)
#include "charts_test.moc"
