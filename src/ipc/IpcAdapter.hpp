#pragma once

#include <QObject>
#include <QVariant>
#include <QString>

namespace keyoubs {

// Abstract base for compositor IPC adapters.
// Each supported compositor (Niri, Hyprland, etc.) implements this interface.
class IpcAdapter : public QObject
{
    Q_OBJECT

public:
    explicit IpcAdapter(QObject *parent = nullptr);
    virtual ~IpcAdapter();

    virtual QString compositorName() const = 0;
    virtual bool isAvailable() const = 0;

    // Query current state
    virtual QVariant getWorkspaces() = 0;
    virtual QVariant getWindows() = 0;
    virtual QVariant getOutputs() = 0;

    // Apply live changes
    virtual bool setGapSize(int px) = 0;
    virtual bool setBorderWidth(int px) = 0;
    virtual bool setBorderColor(const QString &color) = 0;
    virtual bool setOpacity(float opacity) = 0;
    virtual bool setWorkspaceName(int id, const QString &name) = 0;
    virtual bool moveWindowToWorkspace(int windowId, int workspaceId) = 0;
    virtual bool reloadConfig() = 0;

    // Event stream
    virtual void startEventStream() = 0;
    virtual void stopEventStream() = 0;

signals:
    void workspaceChanged(const QVariant &data);
    void windowChanged(const QVariant &data);
    void outputChanged(const QVariant &data);
    void eventStreamError(const QString &error);
};

} // namespace keyoubs
