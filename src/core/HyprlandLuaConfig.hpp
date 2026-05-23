#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QJsonObject>

namespace keyoubs {

// ─── Hyprland Lua Config Support ───
//
// Hyprland 0.55+ uses Lua instead of Hyprlang.
// This class reads and writes the Lua config format,
// and provides a migration assistant from .conf to .lua

class HyprlandLuaConfig : public QObject
{
    Q_OBJECT

public:
    explicit HyprlandLuaConfig(QObject *parent = nullptr);

    // Parse a Lua config file into a QVariantMap
    bool loadFromFile(const QString &path);
    bool loadFromString(const QString &luaContent);

    // Write config back to Lua format
    bool saveToFile(const QString &path) const;
    QString toLuaString() const;

    // Config data access
    [[nodiscard]] QVariantMap configData() const { return m_configData; }
    void setConfigData(const QVariantMap &data) { m_configData = data; }

    // Migration: convert Hyprlang .conf to Lua
    static QString migrateFromHyprlang(const QString &confContent);
    static bool isLuaConfig(const QString &filePath);
    static bool isHyprlangConfig(const QString &filePath);

    // Detect which format is active for a given config directory
    static QString detectActiveFormat(const QString &configDir);

signals:
    void configLoaded(const QVariantMap &data);
    void configSaved(const QString &path);
    void parseError(const QString &message, int lineNumber);

private:
    QVariantMap m_configData;

    // Parse helpers
    QVariantMap parseLuaTable(const QString &content, int pos);
    QString parseLuaKey(const QString &content, int pos);
    QVariant parseLuaValue(const QString &content, int pos);

    // Write helpers
    void writeLuaTable(QString &out, const QVariantMap &table, int indent) const;
    void writeLuaValue(QString &out, const QVariant &value, int indent) const;
};

} // namespace keyoubs
