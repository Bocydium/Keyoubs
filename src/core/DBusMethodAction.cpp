#include "DBusMethodAction.hpp"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusPendingReply>
#include <QDebug>

namespace keyoubs {

DBusMethodAction::DBusMethodAction(QObject *parent)
    : QObject(parent)
{
}

void DBusMethodAction::callMethod(const DBusMethodCall &call)
{
    QDBusConnection bus = call.useSystemBus
        ? QDBusConnection::systemBus()
        : QDBusConnection::sessionBus();

    QDBusMessage msg = QDBusMessage::createMethodCall(
        call.service, call.path, call.interface, call.method);
    msg.setArguments(call.args);

    QDBusPendingCall pending = bus.asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(pending, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [this, call](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QVariant> reply = *w;
        if (reply.isError()) {
            emit methodFailed(call.method, reply.error().message());
        } else {
            emit methodCompleted(call.method, reply.value());
        }
        w->deleteLater();
    });
}

void DBusMethodAction::sendNotification(const QString &appName, const QString &title,
                                         const QString &body, const QString &icon)
{
    DBusMethodCall call;
    call.service = "org.freedesktop.Notifications";
    call.path = "/org/freedesktop/Notifications";
    call.interface = "org.freedesktop.Notifications";
    call.method = "Notify";
    call.args = QVariantList{
        appName,
        0u,           // replaces_id
        icon,
        title,
        body,
        QStringList(), // actions
        QVariantMap(), // hints
        5000          // timeout ms
    };
    callMethod(call);
}

void DBusMethodAction::setScreenBrightness(int percentage)
{
    DBusMethodCall call;
    call.service = "org.freedesktop.UPower";
    call.path = "/org/freedesktop/UPower/KbdBacklight";
    call.interface = "org.freedesktop.UPower.KbdBacklight";
    call.method = "SetBrightness";
    call.args = QVariantList{percentage};
    callMethod(call);
}

void DBusMethodAction::suspendSystem()
{
    DBusMethodCall call;
    call.service = "org.freedesktop.login1";
    call.path = "/org/freedesktop/login1";
    call.interface = "org.freedesktop.login1.Manager";
    call.method = "Suspend";
    call.args = QVariantList{true}; // interactive
    call.useSystemBus = true;
    callMethod(call);
}

void DBusMethodAction::lockScreen()
{
    DBusMethodCall call;
    call.service = "org.freedesktop.ScreenSaver";
    call.path = "/org/freedesktop/ScreenSaver";
    call.interface = "org.freedesktop.ScreenSaver";
    call.method = "Lock";
    callMethod(call);
}

} // namespace keyoubs
