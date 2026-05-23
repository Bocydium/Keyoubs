#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>

namespace keyoubs {

// ─── D-Bus Method Action ───
//
// Components can call arbitrary D-Bus methods as actions.
// Not just listening for signals — actively calling methods
// on system/session bus services.

struct DBusMethodCall {
    QString service;       // e.g. "org.freedesktop.Notifications"
    QString path;          // e.g. "/org/freedesktop/Notifications"
    QString interface;     // e.g. "org.freedesktop.Notifications"
    QString method;        // e.g. "Notify"
    QVariantList args;     // Method arguments
    bool useSystemBus = false;
};

class DBusMethodAction : public QObject
{
    Q_OBJECT

public:
    explicit DBusMethodAction(QObject *parent = nullptr);

    void callMethod(const DBusMethodCall &call);

    // Convenience methods for common actions
    void sendNotification(const QString &appName, const QString &title,
                          const QString &body, const QString &icon = QString());
    void setScreenBrightness(int percentage);
    void suspendSystem();
    void lockScreen();

signals:
    void methodCompleted(const QString &method, const QVariant &result);
    void methodFailed(const QString &method, const QString &error);

private:
    void onCallFinished(const QString &method, const QVariant &result, bool success,
                        const QString &error);
};

} // namespace keyoubs
