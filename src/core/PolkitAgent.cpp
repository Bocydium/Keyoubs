#include "PolkitAgent.hpp"

namespace keyoubs {

PolkitAgent::PolkitAgent(QObject *parent)
    : QObject(parent)
{
}

void PolkitAgent::initiateAuthentication(const PolkitRequest &request)
{
    m_active = true;
    m_currentRequest = request;
    emit authenticationRequested(request);
}

void PolkitAgent::cancelAuthentication()
{
    if (!m_active) return;
    m_active = false;
    emit authenticationCancelled();
}

void PolkitAgent::submitPassword(const QString &password)
{
    Q_UNUSED(password)
    if (!m_active) return;

    // In a real implementation, this would call the polkit D-Bus API
    // to submit the password and complete authentication.
    // For now, we simulate success.
    m_active = false;
    emit authenticationCompleted(true);
}

void PolkitAgent::dismiss()
{
    cancelAuthentication();
}

} // namespace keyoubs
