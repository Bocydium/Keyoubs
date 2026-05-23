#include "DebouncedWriter.hpp"
#include <QDebug>

namespace keyoubs {

DebouncedWriter::DebouncedWriter(QObject *parent)
    : QObject(parent)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &DebouncedWriter::onTimeout);
}

void DebouncedWriter::scheduleWrite(const QString &filePath, const QVariantMap &configData)
{
    m_pendingPath = filePath;
    m_pendingData = configData;
    m_timer.start(m_delayMs);
}

void DebouncedWriter::writeNow()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        emit writeCancelled();
    }
    onTimeout();
}

void DebouncedWriter::setWriter(std::function<bool(const QString&, const QVariantMap&)> writer)
{
    m_writer = std::move(writer);
}

void DebouncedWriter::onTimeout()
{
    if (m_pendingPath.isEmpty() || !m_writer) return;

    emit writeStarted(m_pendingPath);
    bool success = m_writer(m_pendingPath, m_pendingData);
    emit writeFinished(m_pendingPath, success);

    m_pendingPath.clear();
    m_pendingData.clear();
}

} // namespace keyoubs
