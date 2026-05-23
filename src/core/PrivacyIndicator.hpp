#pragma once

#include <QObject>
#include <QHash>
#include <QString>

namespace keyoubs {

// ─── Privacy Indicator ───
//
// Status bar module showing whether any app is currently capturing
// the screen, microphone, or camera. Sourced from pipewire and
// portal D-Bus events.

enum class CaptureType {
    Screen,
    Microphone,
    Camera
};

struct CaptureSession {
    QString appName;
    CaptureType type;
    QString portalHandle;
};

class PrivacyIndicator : public QObject
{
    Q_OBJECT

public:
    explicit PrivacyIndicator(QObject *parent = nullptr);

    void addSession(const CaptureSession &session);
    void removeSession(const QString &portalHandle);

    [[nodiscard]] int activeCaptureCount() const;
    [[nodiscard]] bool isCapturing(CaptureType type) const;
    [[nodiscard]] QList<CaptureSession> sessions() const { return m_sessions.values(); }

    // D-Bus portal signal handlers
    void onScreenCaptureStarted(const QString &appName, const QString &handle);
    void onScreenCaptureStopped(const QString &handle);
    void onMicrophoneCaptureStarted(const QString &appName, const QString &handle);
    void onMicrophoneCaptureStopped(const QString &handle);
    void onCameraCaptureStarted(const QString &appName, const QString &handle);
    void onCameraCaptureStopped(const QString &handle);

signals:
    void captureStarted(CaptureType type, const QString &appName);
    void captureStopped(CaptureType type, const QString &appName);
    void activeCapturesChanged();

private:
    QHash<QString, CaptureSession> m_sessions;
};

} // namespace keyoubs
