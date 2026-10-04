#pragma once

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

namespace SingleInstance {
inline constexpr auto service = "org.erx.KOverlay";
inline constexpr auto path = "/Overlay";

enum class Result { Primary, Activated, Error };

// Claim the session-wide name before creating a window, or show its current owner.
inline Result acquireOrShow(const QDBusConnection &session) {
    if (!session.isConnected() || !session.interface()) {
        qCritical() << "koverlay: session D-Bus is unavailable:" << session.lastError().message();
        return Result::Error;
    }

    const auto registration = session.interface()->registerService(
        service, QDBusConnectionInterface::DontQueueService,
        QDBusConnectionInterface::DontAllowReplacement);
    if (!registration.isValid()) {
        qCritical() << "koverlay: cannot register D-Bus service:" << registration.error().message();
        return Result::Error;
    }
    if (registration.value() == QDBusConnectionInterface::ServiceRegistered)
        return Result::Primary;

    // Pin activation to this owner, even if it exits during startup.
    const auto owner = session.interface()->serviceOwner(service);
    if (!owner.isValid()) {
        qCritical() << "koverlay: cannot find the existing instance:" << owner.error().message();
        return Result::Error;
    }
    const auto request = QDBusMessage::createMethodCall(owner.value(), path, service, "Show");
    QElapsedTimer timer;
    timer.start();
    while (true) {
        const auto reply = session.call(request, QDBus::Block, 5000);
        if (reply.type() != QDBusMessage::ErrorMessage)
            return Result::Activated;

        // Qt's D-Bus thread can reject calls before the window is exported.
        const auto error = QDBusError(reply).type();
        if ((error == QDBusError::UnknownObject || error == QDBusError::UnknownMethod)
            && timer.elapsed() < 5000) {
            QThread::msleep(50);
            continue;
        }
        qCritical() << "koverlay: cannot show the existing instance:" << reply.errorMessage();
        return Result::Error;
    }
}
}
