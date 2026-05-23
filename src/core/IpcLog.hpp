#pragma once

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QString>

namespace keyoubs {

// ─── IPC Command Log ───
//
// A toggleable panel showing every IPC message sent and received,
// with timestamps. Used for debugging compositor communication.

struct IpcLogEntry {
    QDateTime timestamp;
    bool isOutgoing; // true = sent, false = received
    QString compositor;
    QString messageType;
    QString payload; // truncated for display
    bool success;
};

class IpcLog : public QObject
{
    Q_OBJECT

public:
    explicit IpcLog(QObject *parent = nullptr);

    void logOutgoing(const QString &compositor, const QString &type,
                     const QString &payload, bool success = true);
    void logIncoming(const QString &compositor, const QString &type,
                     const QString &payload);

    [[nodiscard]] const QList<IpcLogEntry>& entries() const { return m_entries; }
    void clear();
    void setMaxEntries(int max) { m_maxEntries = max; }

signals:
    void entryAdded(const IpcLogEntry &entry);
    void logCleared();

private:
    QList<IpcLogEntry> m_entries;
    int m_maxEntries = 500;

    void trimEntries();
};

} // namespace keyoubs
