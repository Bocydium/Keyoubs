#include "IpcWorker.hpp"
#include <QDebug>

namespace keyoubs {

IpcWorker::IpcWorker(QObject *parent)
    : QThread(parent)
{
}

IpcWorker::~IpcWorker()
{
    stop();
    wait(2000);
}

void IpcWorker::stop()
{
    QMutexLocker lock(&m_mutex);
    m_running = false;
    m_condition.wakeOne();
}

int IpcWorker::enqueueJob(const QString &type, const QVariantMap &params)
{
    QMutexLocker lock(&m_mutex);
    IpcJob job;
    job.type = type;
    job.params = params;
    job.id = m_nextJobId++;
    m_queue.enqueue(job);
    m_condition.wakeOne();
    return job.id;
}

IpcJob IpcWorker::dequeueJob()
{
    QMutexLocker lock(&m_mutex);
    while (m_queue.isEmpty() && m_running) {
        m_condition.wait(&m_mutex, 100);
    }
    if (!m_queue.isEmpty()) {
        return m_queue.dequeue();
    }
    return IpcJob{};
}

IpcResult IpcWorker::processJob(const IpcJob &job)
{
    IpcResult result;
    result.jobId = job.id;

    // Placeholder: actual IPC calls would go here
    // In practice, the worker would call into the IpcAdapter
    // For now, simulate success
    result.success = true;
    result.data = QVariantMap{{"type", job.type}};

    return result;
}

void IpcWorker::run()
{
    while (true) {
        IpcJob job = dequeueJob();
        if (!m_running && job.id == 0) break;
        if (job.id == 0) continue;

        IpcResult result = processJob(job);
        if (result.success) {
            emit jobCompleted(result);
        } else {
            emit jobFailed(result.jobId, result.error);
        }
    }
}

} // namespace keyoubs
