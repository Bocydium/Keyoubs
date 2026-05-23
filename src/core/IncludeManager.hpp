#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QFileInfo>

namespace keyoubs {

// ─── Include Manager ───
//
// Niri 26.04 added optional include statements in the config.
// This panel lists all include directives with file paths,
// existence status, and a "create" button for missing files.

enum class IncludeStatus {
    Exists,
    Missing,
    Broken  // file exists but is unreadable or invalid
};

struct IncludeEntry {
    QString path;            // file path
    bool isOptional = false; // optional-include vs include
    IncludeStatus status;
};

class IncludeManager : public QObject
{
    Q_OBJECT

public:
    explicit IncludeManager(QObject *parent = nullptr);

    void setConfigDir(const QString &dir) { m_configDir = dir; }
    void scanIncludes(const QString &configContent);

    void addInclude(const QString &path, bool optional);
    void removeInclude(int index);

    [[nodiscard]] QList<IncludeEntry> entries() const { return m_entries; }
    [[nodiscard]] IncludeStatus checkFile(const QString &path) const;

    // Generate the include structure for machine-specific overrides
    void generatePerMachineSplit(const QString &baseFile, const QString &overrideFile);

signals:
    void entryAdded(int index);
    void entryRemoved(int index);
    void entriesChanged();

private:
    QList<IncludeEntry> m_entries;
    QString m_configDir;
};

} // namespace keyoubs
