#include "ConfigManager.hpp"
#include <QFile>
#include <QDebug>

namespace keyoubs {

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
}

void ConfigManager::loadFromFile(const QString &path)
{
    // TODO: Parse KDL (Niri) or custom config (Hyprland)
    qInfo() << "Loading config from:" << path;
    m_configPath = path;
}

void ConfigManager::saveToFile(const QString &path)
{
    // TODO: Serialize back to config format
    qInfo() << "Saving config to:" << path;
}

void ConfigManager::applyLiveChange(const QString &section, const QString &key, const QVariant &value)
{
    // TODO: Update internal model and emit change
    Q_UNUSED(section)
    Q_UNUSED(key)
    Q_UNUSED(value)
    emit configChanged(m_config);
}

} // namespace keyoubs
