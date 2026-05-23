#include "EventCoalescer.hpp"

namespace keyoubs {

EventCoalescer::EventCoalescer(QObject *parent)
    : QObject(parent)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &EventCoalescer::onTimeout);
}

void EventCoalescer::submitEvent(const QString &eventType, const QVariant &data)
{
    m_pendingEvents[eventType] = data;

    // Restart the timer — only emit after events stop arriving
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    m_timer.start(m_coalesceWindowMs);
}

void EventCoalescer::flush()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    onTimeout();
}

void EventCoalescer::onTimeout()
{
    if (m_pendingEvents.isEmpty()) return;

    QHash<QString, QVariant> events = m_pendingEvents;
    m_pendingEvents.clear();
    emit eventsReady(events);
}

} // namespace keyoubs
