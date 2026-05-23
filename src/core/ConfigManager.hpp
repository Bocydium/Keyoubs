#pragma once

#include "../models/ConfigModel.hpp"
#include <QObject>
#include <memory>

namespace keyoubs {

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);

    [[nodiscard]] ConfigModel* currentConfig() { return &m_config; }
    [[nodiscard]] const ConfigModel* currentConfig() const { return &m_config; }

    void loadFromFile(const QString &path);
    void saveToFile(const QString &path);

    void applyLiveChange(const QString &section, const QString &key, const QVariant &value);

signals:
    void configChanged(ConfigModel *config);

private:
    ConfigModel m_config;
    QString m_configPath;
};

} // namespace keyoubs
