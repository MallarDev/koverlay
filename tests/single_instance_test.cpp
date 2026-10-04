#include <QCoreApplication>
#include <QProcess>
#include <QTest>
#include <QThread>

#include "single_instance.h"

class OverlayStub : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.erx.KOverlay")
public:
    int calls = 0;
public slots:
    Q_NOREPLY void Show() { ++calls; }
};

class SingleInstanceTest : public QObject {
    Q_OBJECT
private slots:
    void cleanup() {
        auto session = QDBusConnection::sessionBus();
        session.unregisterObject(SingleInstance::path);
        session.unregisterService(SingleInstance::service);
    }

    void acquiresAndReleasesName() {
        auto session = QDBusConnection::sessionBus();
        QCOMPARE(SingleInstance::acquireOrShow(session), SingleInstance::Result::Primary);
        QVERIFY(session.unregisterService(SingleInstance::service));
        QCOMPARE(SingleInstance::acquireOrShow(session), SingleInstance::Result::Primary);
    }

    void repeatedLaunchesActivateOwner() {
        auto session = QDBusConnection::sessionBus();
        QCOMPARE(SingleInstance::acquireOrShow(session), SingleInstance::Result::Primary);
        OverlayStub overlay;
        QVERIFY(session.registerObject(SingleInstance::path, &overlay, QDBusConnection::ExportAllSlots));
        for (int i = 1; i <= 3; ++i) {
            QProcess child;
            child.start(QCoreApplication::applicationFilePath(), {"--probe"});
            QVERIFY(child.waitForStarted());
            QTRY_COMPARE_WITH_TIMEOUT(child.state(), QProcess::NotRunning, 15000);
            QCOMPARE(child.exitStatus(), QProcess::NormalExit);
            QCOMPARE(child.exitCode(), 0);
            QCOMPARE(overlay.calls, i);
        }
    }

    void activationWaitsForStartup() {
        auto session = QDBusConnection::sessionBus();
        QCOMPARE(SingleInstance::acquireOrShow(session), SingleInstance::Result::Primary);
        QProcess child;
        child.start(QCoreApplication::applicationFilePath(), {"--probe"});
        QVERIFY(child.waitForStarted());
        // Simulate synchronous window construction before the owner's event loop.
        QThread::msleep(200);
        OverlayStub overlay;
        QVERIFY(session.registerObject(SingleInstance::path, &overlay, QDBusConnection::ExportAllSlots));
        QTRY_COMPARE_WITH_TIMEOUT(child.state(), QProcess::NotRunning, 15000);
        QCOMPARE(child.exitCode(), 0);
        QCOMPARE(overlay.calls, 1);
    }

    void unavailableBusFailsClosed() {
        const auto session = QDBusConnection::connectToBus(
            "unix:path=/nonexistent-koverlay-test-bus", "unavailable");
        QCOMPARE(SingleInstance::acquireOrShow(session), SingleInstance::Result::Error);
        QDBusConnection::disconnectFromBus("unavailable");
    }

    void unresponsiveOwnerDoesNotCreateAnotherInstance() {
        auto session = QDBusConnection::sessionBus();
        QCOMPARE(SingleInstance::acquireOrShow(session), SingleInstance::Result::Primary);
        QProcess child;
        child.start(QCoreApplication::applicationFilePath(), {"--probe"});
        QVERIFY(child.waitForStarted());
        QTRY_COMPARE_WITH_TIMEOUT(child.state(), QProcess::NotRunning, 15000);
        QCOMPARE(child.exitCode(), 1);
    }
};

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    if (app.arguments().contains("--probe")) {
        const auto result = SingleInstance::acquireOrShow(QDBusConnection::sessionBus());
        return result == SingleInstance::Result::Activated ? 0 :
               result == SingleInstance::Result::Error ? 1 : 2;
    }
    SingleInstanceTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "single_instance_test.moc"
