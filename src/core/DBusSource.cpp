#include "DBusSource.hpp"
#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>

namespace keyoubs {

DBusSource::DBusSource(const QString &name,
                       const QString &service,
                       const QString &path,
                       const QString &interface,
                       QObject *parent)
    : DataSource(name, parent)
    , m_service(service)
    , m_path(path)
    , m_interface(interface)
{
}

void DBusSource::start()
{
    m_active = true;

    m_dbusInterface = std::make_unique<QDBusInterface>(
        m_service, m_path, m_interface,
        QDBusConnection::systemBus(), this);

    if (!m_dbusInterface->isValid()) {
        // Try session bus
        m_dbusInterface = std::make_unique<QDBusInterface>(
            m_service, m_path, m_interface,
            QDBusConnection::sessionBus(), this);
    }

    if (!m_dbusInterface->isValid()) {
        emit error(QString("Failed to connect to D-Bus service: %1").arg(m_service));
        return;
    }

    // Listen for PropertiesChanged signals
    QDBusConnection::sessionBus().connect(
        m_service, m_path, "org.freedesktop.DBus.Properties",
        "PropertiesChanged", this,
        SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

    // Initial read
    QVariantMap allProps;
    QDBusMessage msg = m_dbusInterface->call("GetAll", m_interface);
    if (msg.type() == QDBusMessage::ReplyMessage && msg.arguments().size() > 0) {
        allProps = msg.arguments().first().toMap();
        setValue(allProps);
    }
}

void DBusSource::stop()
{
    m_active = false;
    QDBusConnection::sessionBus().disconnect(
        m_service, m_path, "org.freedesktop.DBus.Properties",
        "PropertiesChanged", this,
        SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
    m_dbusInterface.reset();
}

QVariant DBusSource::readProperty(const QString &propertyName)
{
    if (!m_dbusInterface || !m_dbusInterface->isValid()) {
        return QVariant();
    }
    QDBusReply<QVariant> reply = m_dbusInterface->call("Get", m_interface, propertyName);
    return reply.isValid() ? reply.value() : QVariant();
}

QDBusMessage DBusSource::callMethod(const QString &method, const QList<QVariant> &args)
{
    if (!m_dbusInterface || !m_dbusInterface->isValid()) {
        return QDBusMessage::createError("NotConnected", "D-Bus interface not available");
    }
    return m_dbusInterface->callWithArgumentList(QDBus::Block, method, args);
}

void DBusSource::onPropertiesChanged(const QString &interface,
                                      const QVariantMap &changed,
                                      const QStringList &invalidated)
{
    Q_UNUSED(invalidated)
    if (interface != m_interface) return;

    emit propertiesChanged(changed);

    // Merge changed properties into current value
    QVariantMap current = m_value.toMap();
    for (auto it = changed.begin(); it != changed.end(); ++it) {
        current[it.key()] = it.value();
    }
    setValue(current);
}

// ─── Battery Source ───

BatterySource::BatterySource(QObject *parent)
    : DBusSource("battery",
                 "org.freedesktop.UPower",
                 "/org/freedesktop/UPower/devices/DisplayDevice",
                 "org.freedesktop.UPower.Device",
                 parent)
{
}

int BatterySource::percentage() const
{
    return m_value.toMap().value("Percentage").toInt();
}

bool BatterySource::isCharging() const
{
    QString state = m_value.toMap().value("State").toString();
    return state == "1" || state == "charging"; // 1 = Charging in UPower
}

QString BatterySource::status() const
{
    uint state = m_value.toMap().value("State").toUInt();
    switch (state) {
    case 1: return "charging";
    case 2: return "discharging";
    case 3: return "empty";
    case 4: return "full";
    default: return "unknown";
    }
}

// ─── Network Source ───

NetworkSource::NetworkSource(QObject *parent)
    : DBusSource("network",
                 "org.freedesktop.NetworkManager",
                 "/org/freedesktop/NetworkManager",
                 "org.freedesktop.NetworkManager",
                 parent)
{
}

bool NetworkSource::isConnected() const
{
    uint state = m_value.toMap().value("State").toUInt();
    return state == 70; // NM_STATE_CONNECTED_GLOBAL
}

QString NetworkSource::ssid() const
{
    return m_value.toMap().value("ActiveConnection").toString();
}

QString NetworkSource::deviceType() const
{
    return m_value.toMap().value("PrimaryConnectionType").toString();
}

// ─── MPRIS Source ───

MprisSource::MprisSource(QObject *parent)
    : DBusSource("mpris",
                 "org.mpris.MediaPlayer2.playerctld",
                 "/org/mpris/MediaPlayer2",
                 "org.mpris.MediaPlayer2.Player",
                 parent)
{
}

QString MprisSource::title() const
{
    QVariantMap metadata = m_value.toMap().value("Metadata").toMap();
    return metadata.value("xesam:title").toString();
}

QString MprisSource::artist() const
{
    QVariantMap metadata = m_value.toMap().value("Metadata").toMap();
    QVariant artists = metadata.value("xesam:artist");
    if (artists.typeId() == QMetaType::QStringList) {
        return artists.toStringList().join(", ");
    }
    return artists.toString();
}

QString MprisSource::album() const
{
    QVariantMap metadata = m_value.toMap().value("Metadata").toMap();
    return metadata.value("xesam:album").toString();
}

bool MprisSource::isPlaying() const
{
    QString status = m_value.toMap().value("PlaybackStatus").toString();
    return status == "Playing";
}

} // namespace keyoubs
