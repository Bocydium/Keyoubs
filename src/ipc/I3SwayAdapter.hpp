#pragma once

#include "IpcAdapter.hpp"
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDataStream>
#include <memory>

namespace keyoubs {

// i3 / Sway IPC adapter.
// Both compositors share the same wire protocol (magic "i3-ipc" + length + type + JSON payload).
// Auto-detects whether we're talking to i3 or Sway via GET_VERSION.
class I3SwayAdapter : public IpcAdapter
{
    Q_OBJECT

public:
    explicit I3SwayAdapter(QObject *parent = nullptr);
    ~I3SwayAdapter() override;

    QString compositorName() const override;
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

    // Sway-specific extras
    QVariant getInputs() const;
    QVariant getSeats() const;

private:
    enum class MsgType : uint32_t {
        RunCommand      = 0,
        GetWorkspaces   = 1,
        Subscribe       = 2,
        GetOutputs      = 3,
        GetTree         = 4,
        GetMarks        = 5,
        GetBarConfig    = 6,
        GetVersion      = 7,
        GetBindingModes = 8,
        GetConfig       = 9,
        SendTick        = 10,
        Sync            = 11,
        GetBindingState = 12,
        // Sway extensions
        GetInputs       = 100,
        GetSeats        = 101,
    };

    QString m_socketPath;
    QString m_compositorName; // "i3" or "Sway"
    bool m_isSway = false;

    std::unique_ptr<QLocalSocket> m_socket;
    std::unique_ptr<QLocalSocket> m_eventSocket;

    QString resolveSocketPath() const;
    bool detectCompositor();
    bool connectSocket(QLocalSocket *socket);
    QByteArray sendMessage(MsgType type, const QByteArray &payload = QByteArray());
    QJsonDocument sendJsonMessage(MsgType type, const QJsonObject &payload = QJsonObject());
    void handleEvent(const QByteArray &data);

    static QByteArray buildMessage(MsgType type, const QByteArray &payload);
};

} // namespace keyoubs
