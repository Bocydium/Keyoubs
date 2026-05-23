#pragma once

#include "IpcAdapter.hpp"
#include <QLocalSocket>
#include <QJsonDocument>
#include <memory>

namespace keyoubs {

// Hyprland IPC adapter.
// Communicates via Unix domain socket at /tmp/hypr/
// Uses hyprctl command-line tool as fallback.
class HyprlandAdapter : public IpcAdapter
{
    Q_OBJECT

public:
    explicit HyprlandAdapter(QObject *parent = nullptr);
    ~HyprlandAdapter() override;

    QString compositorName() const override { return QStringLiteral("Hyprland"); }
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

    QString getSocketPath() const;
    bool connectSocket();
    QByteArray sendRawRequest(const QByteArray &request);

    // hyprctl fallback for commands
    bool runHyprctl(const QStringList &args);
};

} // namespace keyoubs
