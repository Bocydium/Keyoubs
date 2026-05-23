#pragma once

#include <QObject>
#include <QTimer>
#include <QHash>
#include <QString>
#include <QVariant>

namespace keyoubs {

// ─── IPC Message Coalescing ───
//
// Rapid compositor event bursts are batched into a single canvas update.
// Events of the same type arriving within the coalescing window
// are merged, keeping only the latest value.

class EventCoalescer : public QObject
{
    Q_OBJECT

public:
    explicit EventCoalescer(QObject *parent = nullptr);

    // Submit an event for coalescing
    void submitEvent(const QString &eventType, const QVariant &data);

    // Force flush all pending events immediately
    void flush();

    void setCoalesceWindow(int ms) { m_coalesceWindowMs = ms; }
    [[nodiscard]] int coalesceWindow() const { return m_coalesceWindowMs; }

signals:
    // Emitted when the coalescing window expires
    void eventsReady(const QHash<QString, QVariant> &events);

private:
    QTimer m_timer;
    QHash<QString, QVariant> m_pendingEvents;
    int m_coalesceWindowMs = 50; // 50ms default

    void onTimeout();
};

} // namespace keyoubs
