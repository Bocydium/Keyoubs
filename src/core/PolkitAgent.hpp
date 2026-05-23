#pragma once

#include <QObject>
#include <QString>

namespace keyoubs {

// ─── Polkit Agent ───
//
// Built-in permission dialog for elevated access requests.
// Appears as a centered modal overlay when an app needs authentication.
// Style is fully editable via the component designer.
// Activated by KEYOUBES_WITH_POLKIT cmake flag.

struct PolkitRequest {
    QString actionId;
    QString message;
    QString iconName;
    QString cookie;
    QString user;
};

class PolkitAgent : public QObject
{
    Q_OBJECT

public:
    explicit PolkitAgent(QObject *parent = nullptr);

    void initiateAuthentication(const PolkitRequest &request);
    void cancelAuthentication();

    [[nodiscard]] bool isAuthenticating() const { return m_active; }
    [[nodiscard]] PolkitRequest currentRequest() const { return m_currentRequest; }

signals:
    void authenticationRequested(const PolkitRequest &request);
    void authenticationCompleted(bool success);
    void authenticationCancelled();

public slots:
    void submitPassword(const QString &password);
    void dismiss();

private:
    bool m_active = false;
    PolkitRequest m_currentRequest;
};

} // namespace keyoubs
