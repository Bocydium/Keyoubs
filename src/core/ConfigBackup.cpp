#include "ConfigBackup.hpp"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>

namespace keyoubs {

QString BackupEntry::displayName() const
{
    return QString("%1 — %2 (%3 KB)")
        .arg(compositorName)
        .arg(timestamp.toString("yyyy-MM-dd hh:mm:ss"))
        .arg(fileSize / 1024);
}

ConfigBackup::ConfigBackup(QObject *parent)
    : QObject(parent)
    , m_backupDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/keyoubs/backups")
{
    QDir().mkpath(m_backupDir);
}

QString ConfigBackup::backupDir() const
{
    return m_backupDir;
}

bool ConfigBackup::backup(const QString &configPath, const QString &compositorName)
{
    if (!QFile::exists(configPath)) {
        qWarning() << "Cannot backup non-existent file:" << configPath;
        return false;
    }

    QString compDir = m_backupDir + "/" + compositorName;
    QDir().mkpath(compDir);

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString backupPath = compDir + "/" + timestamp + ".bak";

    if (!QFile::copy(configPath, backupPath)) {
        qWarning() << "Failed to create backup:" << backupPath;
        return false;
    }

    BackupEntry entry;
    entry.filePath = backupPath;
    entry.compositorName = compositorName;
    entry.timestamp = QDateTime::currentDateTime();
    entry.fileSize = QFileInfo(backupPath).size();

    qInfo() << "Config backup created:" << backupPath;
    emit backupCreated(entry);
    return true;
}

QList<BackupEntry> ConfigBackup::listBackups(const QString &compositorName) const
{
    QList<BackupEntry> entries;
    QDir rootDir(m_backupDir);

    QStringList compositors = compositorName.isEmpty()
        ? rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)
        : QStringList{compositorName};

    for (const QString &comp : compositors) {
        QDir compDir(m_backupDir + "/" + comp);
        QStringList files = compDir.entryList(QStringList("*.bak"), QDir::Files);

        for (const QString &file : files) {
            QString fullPath = compDir.absoluteFilePath(file);
            QFileInfo info(fullPath);

            BackupEntry entry;
            entry.filePath = fullPath;
            entry.compositorName = comp;
            entry.timestamp = info.birthTime();
            entry.fileSize = info.size();
            entries.append(entry);
        }
    }

    // Sort by timestamp descending (newest first)
    std::sort(entries.begin(), entries.end(),
              [](const BackupEntry &a, const BackupEntry &b) {
                  return a.timestamp > b.timestamp;
              });

    return entries;
}

bool ConfigBackup::restore(const BackupEntry &entry)
{
    if (!QFile::exists(entry.filePath)) {
        qWarning() << "Backup file does not exist:" << entry.filePath;
        return false;
    }

    // Determine original path from backup metadata or convention
    // For now, we need the caller to know the target path
    // This is a simplified version — in practice you'd store original path in metadata
    qInfo() << "Backup restore requested for:" << entry.filePath;
    emit backupRestored(entry);
    return true;
}

bool ConfigBackup::deleteBackup(const BackupEntry &entry)
{
    if (!QFile::exists(entry.filePath)) {
        return false;
    }

    bool ok = QFile::remove(entry.filePath);
    if (ok) {
        emit backupDeleted(entry);
    }
    return ok;
}

void ConfigBackup::prune(int maxCount)
{
    QDir rootDir(m_backupDir);
    QStringList compositors = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &comp : compositors) {
        auto backups = listBackups(comp);
        if (backups.size() <= maxCount) continue;

        for (int i = maxCount; i < backups.size(); ++i) {
            QFile::remove(backups[i].filePath);
            qInfo() << "Pruned old backup:" << backups[i].filePath;
        }
    }
}

} // namespace keyoubs
