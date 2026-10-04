#pragma once

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>

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

    // Avoid introspection: the owner may still be initializing its overlay.
    // The call is dispatched when its event loop starts, after object registration.
    const auto request = QDBusMessage::createMethodCall(service, path, service, "Show");
    const auto reply = session.call(request, QDBus::Block, 10000);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCritical() << "koverlay: cannot show the existing instance:" << reply.errorMessage();
        return Result::Error;
    }
    return Result::Activated;
}
}
