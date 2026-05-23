#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

namespace keyoubs {

// ─── Config File Backup System ───
//
// Before writing any change to a compositor config, Keyoubs copies
// the current file to ~/.config/keyoubs/backups/<compositor>/<timestamp>.bak
//
// Backup count capped at 20 by default. Browsable and one-click restorable.

struct BackupEntry {
    QString filePath;
    QString compositorName;
    QDateTime timestamp;
    qint64 fileSize;

    QString displayName() const;
};

class ConfigBackup : public QObject
{
    Q_OBJECT

public:
    explicit ConfigBackup(QObject *parent = nullptr);

    // Create a backup of the given config file
    bool backup(const QString &configPath, const QString &compositorName);

    // List all backups for a compositor (or all if compositorName is empty)
    QList<BackupEntry> listBackups(const QString &compositorName = QString()) const;

    // Restore a backup to its original path
    bool restore(const BackupEntry &entry);

    // Delete a specific backup
    bool deleteBackup(const BackupEntry &entry);

    // Clean old backups, keeping only maxCount most recent per compositor
    void prune(int maxCount = 20);

    QString backupDir() const;

signals:
    void backupCreated(const BackupEntry &entry);
    void backupRestored(const BackupEntry &entry);
    void backupDeleted(const BackupEntry &entry);

private:
    QString m_backupDir;
};

} // namespace keyoubs
