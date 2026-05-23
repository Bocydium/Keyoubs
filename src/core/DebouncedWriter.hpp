#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <QVariantMap>
#include <functional>

namespace keyoubs {

// ─── Config Write Debounce ───
//
// Config file written only after the user stops interacting,
// not on every intermediate slider value.
// Uses a QTimer to delay writes by a configurable interval.

class DebouncedWriter : public QObject
{
    Q_OBJECT

public:
    explicit DebouncedWriter(QObject *parent = nullptr);

    // Schedule a write. If called again before the delay,
    // the previous write is cancelled and the timer restarts.
    void scheduleWrite(const QString &filePath, const QVariantMap &configData);

    // Force immediate write, cancelling any pending debounced write
    void writeNow();

    void setDelay(int ms) { m_delayMs = ms; }
    [[nodiscard]] int delay() const { return m_delayMs; }

    // Set the actual write function
    void setWriter(std::function<bool(const QString&, const QVariantMap&)> writer);

signals:
    void writeStarted(const QString &filePath);
    void writeFinished(const QString &filePath, bool success);
    void writeCancelled();

private:
    QTimer m_timer;
    QString m_pendingPath;
    QVariantMap m_pendingData;
    int m_delayMs = 500;
    std::function<bool(const QString&, const QVariantMap&)> m_writer;

    void onTimeout();
};

} // namespace keyoubs
