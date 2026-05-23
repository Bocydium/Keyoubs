#include "DeviceManager.hpp"
#include <QDebug>

namespace keyoubs {

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
{
}

void DeviceManager::setDevices(const QList<InputDevice> &devices)
{
    m_devices = devices;
    for (const auto &dev : devices) {
        emit deviceAdded(dev.name);
    }
}

void DeviceManager::addDevice(const InputDevice &device)
{
    for (const auto &existing : m_devices) {
        if (existing.name == device.name) {
            qWarning() << "Device already exists:" << device.name;
            return;
        }
    }
    m_devices.append(device);
    emit deviceAdded(device.name);
}

void DeviceManager::removeDevice(const QString &name)
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].name == name) {
            m_devices.removeAt(i);
            emit deviceRemoved(name);
            return;
        }
    }
}

InputDevice* DeviceManager::device(const QString &name)
{
    for (auto &dev : m_devices) {
        if (dev.name == name) return &dev;
    }
    return nullptr;
}

void DeviceManager::updateDeviceTag(const QString &name, const QStringList &tags)
{
    for (auto &dev : m_devices) {
        if (dev.name == name) {
            dev.tags = tags;
            emit deviceChanged(name);
            return;
        }
    }
}

void DeviceManager::updateDeviceSettings(const QString &name, const QVariantMap &settings)
{
    for (auto &dev : m_devices) {
        if (dev.name == name) {
            if (settings.contains("scrollFactor"))
                dev.scrollFactor = settings["scrollFactor"].toDouble();
            if (settings.contains("sensitivity"))
                dev.sensitivity = settings["sensitivity"].toDouble();
            if (settings.contains("naturalScroll"))
                dev.naturalScroll = settings["naturalScroll"].toBool();
            if (settings.contains("accelProfile"))
                dev.accelProfile = settings["accelProfile"].toString();
            if (settings.contains("tapToClick"))
                dev.tapToClick = settings["tapToClick"].toBool();
            if (settings.contains("rotation"))
                dev.rotation = settings["rotation"].toInt();

            emit deviceChanged(name);
            return;
        }
    }
}

QString DeviceManager::toHyprlandConfig() const
{
    QString out;
    out += "# Input device configuration\n";

    for (const auto &dev : m_devices) {
        out += "# Device: " + dev.name + " (" + dev.type + ")\n";

        if (!dev.tags.isEmpty()) {
            out += "device:" + dev.name + " {\n";
            out += "    tag = " + dev.tags.join(", ") + "\n";
            out += "}\n";
        }

        out += "input {\n";
        out += QString("    device = %1\n").arg(dev.name);
        out += QString("    scroll_factor = %1\n").arg(dev.scrollFactor);
        out += QString("    sensitivity = %1\n").arg(dev.sensitivity);
        out += QString("    natural_scroll = %1\n").arg(dev.naturalScroll ? "true" : "false");
        if (!dev.accelProfile.isEmpty()) {
            out += QString("    accel_profile = %1\n").arg(dev.accelProfile);
        }
        out += QString("    tap-to-click = %1\n").arg(dev.tapToClick ? "true" : "false");
        if (dev.rotation != 0) {
            out += QString("    rotation = %1\n").arg(dev.rotation);
        }
        out += "}\n\n";
    }

    return out;
}

} // namespace keyoubs
