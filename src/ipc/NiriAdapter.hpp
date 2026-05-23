#pragma once

#include "IpcAdapter.hpp"
#include <QLocalSocket>
#include <QJsonDocument>
#include <QFile>
#include <memory>

namespace keyoubs {

// Niri IPC adapter.
// Communicates via Unix domain socket at $NIRI_SOCKET.
// Protocol: JSON messages, one per line.
class NiriAdapter : public IpcAdapter
{
    Q_OBJECT

public:
    explicit NiriAdapter(QObject *parent = nullptr);
    ~NiriAdapter() override;

    QString compositorName() const override { return QStringLiteral("Niri"); }
    bool isAvailable() const override;

    QVariant getWorkspaces() override;
    QVariant getWindows() override;
    QVariant getOutputs() override;

    bool setGapSize(int px) override;
    bool setBorderWidth(int px) override;
    bool setBorderColor(const QString &color) override;
    bool setOpacity(float opacity) override;
    bool setWorkspaceName(int id, const QString &name) override;
    bool moveWindowToWorkspace(int windowId, int workspaceId) override;
    bool reloadConfig() override;

    void startEventStream() override;
    void stopEventStream() override;

private:
    QString m_socketPath;
    std::unique_ptr<QLocalSocket> m_socket;
    std::unique_ptr<QLocalSocket> m_eventSocket;

    bool connectSocket(QLocalSocket *socket);
    QJsonDocument sendRequest(const QJsonObject &request);
    void handleEvent(const QJsonObject &event);
};

} // namespace keyoubs
