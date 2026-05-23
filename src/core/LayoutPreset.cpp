#include "LayoutPreset.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

namespace keyoubs {

LayoutPresetLibrary::LayoutPresetLibrary(QObject *parent)
    : QObject(parent)
    , m_presetsDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/keyoubs/presets")
{
    QDir().mkpath(m_presetsDir);
}

QString LayoutPresetLibrary::presetsDir() const
{
    return m_presetsDir;
}

bool LayoutPresetLibrary::savePreset(const QString &name,
                                      const QString &description,
                                      const QString &tag,
                                      const QVariantMap &configData)
{
    QJsonObject obj;
    obj["name"] = name;
    obj["description"] = description;
    obj["tag"] = tag;
    obj["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    obj["config"] = QJsonObject::fromVariantMap(configData);

    QString path = m_presetsDir + "/" + name + ".json";
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save preset:" << path;
        return false;
    }

    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    emit presetSaved(name);
    return true;
}

bool LayoutPresetLibrary::loadPreset(const QString &name, QVariantMap *outConfigData)
{
    QString path = m_presetsDir + "/" + name + ".json";
    if (!QFile::exists(path)) return false;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    if (outConfigData) {
        *outConfigData = obj.value("config").toObject().toVariantMap();
    }

    emit presetLoaded(name);
    return true;
}

bool LayoutPresetLibrary::deletePreset(const QString &name)
{
    QString path = m_presetsDir + "/" + name + ".json";
    bool ok = QFile::remove(path);
    if (ok) emit presetDeleted(name);
    return ok;
}

QList<LayoutPreset> LayoutPresetLibrary::listPresets(const QString &tagFilter) const
{
    QList<LayoutPreset> presets;
    QDir dir(m_presetsDir);
    QStringList files = dir.entryList(QStringList("*.json"), QDir::Files);

    for (const QString &file : files) {
        QString path = dir.absoluteFilePath(file);
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) continue;

        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonObject obj = doc.object();

        LayoutPreset preset;
        preset.name = obj.value("name").toString();
        preset.description = obj.value("description").toString();
        preset.tag = obj.value("tag").toString();
        preset.created = QDateTime::fromString(obj.value("created").toString(), Qt::ISODate);
        preset.configData = obj.value("config").toObject().toVariantMap();

        if (!tagFilter.isEmpty() && preset.tag != tagFilter) continue;
        presets.append(preset);
    }

    // Sort by creation date descending
    std::sort(presets.begin(), presets.end(),
              [](const LayoutPreset &a, const LayoutPreset &b) {
                  return a.created > b.created;
              });

    return presets;
}

bool LayoutPresetLibrary::hasPreset(const QString &name) const
{
    return QFile::exists(m_presetsDir + "/" + name + ".json");
}

} // namespace keyoubs
