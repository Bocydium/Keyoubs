#pragma once

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QVariant>
#include <functional>

namespace keyoubs {

// ─── Background IPC Worker Thread ───
//
// All IPC socket communication runs on a dedicated thread.
// The UI thread never blocks on socket I/O.
//
// Jobs are queued and processed sequentially on the worker thread.
// Results are emitted back to the main thread via signals.

struct IpcJob {
    QString type;       // "getWorkspaces", "getWindows", "setGapSize", etc.
    QVariantMap params; // Job parameters
    int id = 0;         // Unique job ID for matching responses
};

struct IpcResult {
    int jobId = 0;
    bool success = false;
    QVariant data;
    QString error;
};

class IpcWorker : public QThread
{
    Q_OBJECT

public:
    explicit IpcWorker(QObject *parent = nullptr);
    ~IpcWorker() override;

    void stop();
    int enqueueJob(const QString &type, const QVariantMap &params);

signals:
    void jobCompleted(const IpcResult &result);
    void jobFailed(int jobId, const QString &error);

protected:
    void run() override;

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    QQueue<IpcJob> m_queue;
    bool m_running = true;
    int m_nextJobId = 1;

    IpcJob dequeueJob();
    IpcResult processJob(const IpcJob &job);
};

} // namespace keyoubs
