#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>

namespace keyoubs {

// ─── Device Manager ───
//
// Hyprland 0.55 added device tags — label input devices and reference
// them in rules. This panel lists all connected input devices,
// shows their settings, and lets users assign tags.

struct InputDevice {
    QString name;
    QString type; // "mouse", "keyboard", "touchpad", "tablet", etc.
    QStringList tags;

    // Settings
    double scrollFactor = 1.0;
    double sensitivity = 0.0;
    bool naturalScroll = false;
    QString accelProfile; // "adaptive", "flat"
    bool tapToClick = false;
    int rotation = 0; // degrees
};

class DeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit DeviceManager(QObject *parent = nullptr);

    void setDevices(const QList<InputDevice> &devices);
    void addDevice(const InputDevice &device);
    void removeDevice(const QString &name);

    [[nodiscard]] QList<InputDevice> devices() const { return m_devices; }
    [[nodiscard]] InputDevice* device(const QString &name);

    void updateDeviceTag(const QString &name, const QStringList &tags);
    void updateDeviceSettings(const QString &name, const QVariantMap &settings);

    // Export to compositor config format
    [[nodiscard]] QString toHyprlandConfig() const;

signals:
    void deviceAdded(const QString &name);
    void deviceRemoved(const QString &name);
    void deviceChanged(const QString &name);

private:
    QList<InputDevice> m_devices;
};

} // namespace keyoubs
