#include "PrivacyIndicator.hpp"

namespace keyoubs {

PrivacyIndicator::PrivacyIndicator(QObject *parent)
    : QObject(parent)
{
}

void PrivacyIndicator::addSession(const CaptureSession &session)
{
    m_sessions[session.portalHandle] = session;
    emit captureStarted(session.type, session.appName);
    emit activeCapturesChanged();
}

void PrivacyIndicator::removeSession(const QString &portalHandle)
{
    if (!m_sessions.contains(portalHandle)) return;
    auto session = m_sessions.take(portalHandle);
    emit captureStopped(session.type, session.appName);
    emit activeCapturesChanged();
}

int PrivacyIndicator::activeCaptureCount() const
{
    return m_sessions.size();
}

bool PrivacyIndicator::isCapturing(CaptureType type) const
{
    for (const auto &session : m_sessions) {
        if (session.type == type) return true;
    }
    return false;
}

void PrivacyIndicator::onScreenCaptureStarted(const QString &appName, const QString &handle)
{
    CaptureSession session;
    session.appName = appName;
    session.type = CaptureType::Screen;
    session.portalHandle = handle;
    addSession(session);
}

void PrivacyIndicator::onScreenCaptureStopped(const QString &handle)
{
    removeSession(handle);
}

void PrivacyIndicator::onMicrophoneCaptureStarted(const QString &appName, const QString &handle)
{
    CaptureSession session;
    session.appName = appName;
    session.type = CaptureType::Microphone;
    session.portalHandle = handle;
    addSession(session);
}

void PrivacyIndicator::onMicrophoneCaptureStopped(const QString &handle)
{
    removeSession(handle);
}

void PrivacyIndicator::onCameraCaptureStarted(const QString &appName, const QString &handle)
{
    CaptureSession session;
    session.appName = appName;
    session.type = CaptureType::Camera;
    session.portalHandle = handle;
    addSession(session);
}

void PrivacyIndicator::onCameraCaptureStopped(const QString &handle)
{
    removeSession(handle);
}

} // namespace keyoubs
