#pragma once

#include "DataSource.hpp"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>

namespace keyoubs {

// ─── D-Bus Data Source ───
//
// Connects to a D-Bus service and listens for property changes.
// Supports both polling and signal-based updates.

class DBusSource : public DataSource
{
    Q_OBJECT

public:
    explicit DBusSource(const QString &name,
                        const QString &service,
                        const QString &path,
                        const QString &interface,
                        QObject *parent = nullptr);

    void start() override;
    void stop() override;
    QString type() const override { return QStringLiteral("dbus"); }

    // Read a property from the D-Bus interface
    QVariant readProperty(const QString &propertyName);

    // Call a method on the D-Bus interface
    QDBusMessage callMethod(const QString &method, const QList<QVariant> &args = {});

signals:
    void propertiesChanged(const QVariantMap &changed);

private:
    QString m_service;
    QString m_path;
    QString m_interface;
    std::unique_ptr<QDBusInterface> m_dbusInterface;

    void onPropertiesChanged(const QString &interface,
                             const QVariantMap &changed,
                             const QStringList &invalidated);
};

// ─── Pre-built D-Bus Sources ───

class BatterySource : public DBusSource
{
    Q_OBJECT
public:
    explicit BatterySource(QObject *parent = nullptr);
    [[nodiscard]] int percentage() const;
    [[nodiscard]] bool isCharging() const;
    [[nodiscard]] QString status() const;
};

class NetworkSource : public DBusSource
{
    Q_OBJECT
public:
    explicit NetworkSource(QObject *parent = nullptr);
    [[nodiscard]] bool isConnected() const;
    [[nodiscard]] QString ssid() const;
    [[nodiscard]] QString deviceType() const;
};

class MprisSource : public DBusSource
{
    Q_OBJECT
public:
    explicit MprisSource(QObject *parent = nullptr);
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString artist() const;
    [[nodiscard]] QString album() const;
    [[nodiscard]] bool isPlaying() const;
};

} // namespace keyoubs
