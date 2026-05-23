#include "IpcLog.hpp"
#include <QDebug>

namespace keyoubs {

IpcLog::IpcLog(QObject *parent)
    : QObject(parent)
{
}

void IpcLog::logOutgoing(const QString &compositor, const QString &type,
                          const QString &payload, bool success)
{
    IpcLogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.isOutgoing = true;
    entry.compositor = compositor;
    entry.messageType = type;
    entry.payload = payload.length() > 200 ? payload.left(200) + "..." : payload;
    entry.success = success;

    m_entries.append(entry);
    trimEntries();
    emit entryAdded(entry);
}

void IpcLog::logIncoming(const QString &compositor, const QString &type,
                          const QString &payload)
{
    IpcLogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.isOutgoing = false;
    entry.compositor = compositor;
    entry.messageType = type;
    entry.payload = payload.length() > 200 ? payload.left(200) + "..." : payload;
    entry.success = true;

    m_entries.append(entry);
    trimEntries();
    emit entryAdded(entry);
}

void IpcLog::clear()
{
    m_entries.clear();
    emit logCleared();
}

void IpcLog::trimEntries()
{
    while (m_entries.size() > m_maxEntries) {
        m_entries.removeFirst();
    }
}

} // namespace keyoubs
