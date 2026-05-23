#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include <QList>

namespace keyoubs {

// ─── Layout Preset Library ───
//
// Named snapshots of the full compositor layout config.
// Save, switch, tag, and export presets as config snippets.
// Stored in ~/.config/keyoubs/presets/<name>.json

struct LayoutPreset {
    QString name;
    QString description;
    QString tag; // "work", "gaming", "minimal", etc.
    QDateTime created;
    QVariantMap configData;

    QString fileName() const { return name + ".json"; }
};

class LayoutPresetLibrary : public QObject
{
    Q_OBJECT

public:
    explicit LayoutPresetLibrary(QObject *parent = nullptr);

    bool savePreset(const QString &name,
                    const QString &description,
                    const QString &tag,
                    const QVariantMap &configData);

    bool loadPreset(const QString &name, QVariantMap *outConfigData);
    bool deletePreset(const QString &name);

    [[nodiscard]] QList<LayoutPreset> listPresets(const QString &tagFilter = QString()) const;
    [[nodiscard]] bool hasPreset(const QString &name) const;

    QString presetsDir() const;

signals:
    void presetSaved(const QString &name);
    void presetLoaded(const QString &name);
    void presetDeleted(const QString &name);

private:
    QString m_presetsDir;
};

} // namespace keyoubs
