#include "WindowUrgency.hpp"

namespace keyoubs {

WindowUrgency::WindowUrgency(QObject *parent)
    : QObject(parent)
{
}

void WindowUrgency::setUrgent(const QString &windowId, bool urgent)
{
    bool wasUrgent = m_urgentWindows.value(windowId, false);
    if (wasUrgent == urgent) return;

    m_urgentWindows[windowId] = urgent;

    if (!urgent) {
        m_urgentWindows.remove(windowId);
    }

    emit windowUrgencyChanged(windowId, urgent);
    emit urgentCountChanged(urgentCount());
}

bool WindowUrgency::isUrgent(const QString &windowId) const
{
    return m_urgentWindows.value(windowId, false);
}

int WindowUrgency::urgentCount() const
{
    int count = 0;
    for (auto it = m_urgentWindows.begin(); it != m_urgentWindows.end(); ++it) {
        if (it.value()) ++count;
    }
    return count;
}

QStringList WindowUrgency::urgentWindows() const
{
    QStringList ids;
    for (auto it = m_urgentWindows.begin(); it != m_urgentWindows.end(); ++it) {
        if (it.value()) ids.append(it.key());
    }
    return ids;
}

void WindowUrgency::setAppUrgencyRule(const QString &appId, const UrgencyStyle &rule)
{
    m_appRules[appId] = rule;
}

UrgencyStyle WindowUrgency::ruleForApp(const QString &appId) const
{
    return m_appRules.value(appId, m_style);
}

void WindowUrgency::clearAll()
{
    m_urgentWindows.clear();
    emit urgentCountChanged(0);
}

} // namespace keyoubs
