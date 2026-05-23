#pragma once

#include "DataSource.hpp"
#include <QProcess>

namespace keyoubs {

// ─── Process Data Source ───
//
// Runs a long-lived process and reads its stdout.
// Can optionally write to stdin.
// Supports restart policies: never, on-error, always, backoff.

enum class ProcessRestartPolicy {
    Never,
    OnError,
    Always,
    Backoff
};

class ProcessModule : public DataSource
{
    Q_OBJECT

public:
    explicit ProcessModule(const QString &name,
                           const QString &command,
                           const QStringList &args = {},
                           QObject *parent = nullptr);

    void start() override;
    void stop() override;
    QString type() const override { return QStringLiteral("process"); }

    void setCommand(const QString &cmd, const QStringList &args);
    void writeToStdin(const QString &data);

    void setRestartPolicy(ProcessRestartPolicy policy) { m_restartPolicy = policy; }
    void setBackoffMs(int ms) { m_backoffMs = ms; }

signals:
    void processStarted();
    void processStopped(int exitCode);
    void stdoutReceived(const QString &data);
    void stderrReceived(const QString &data);

private:
    QString m_command;
    QStringList m_args;
    QProcess *m_process = nullptr;
    ProcessRestartPolicy m_restartPolicy = ProcessRestartPolicy::OnError;
    int m_backoffMs = 1000;
    int m_restartAttempts = 0;

    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onReadyReadStdout();
    void onReadyReadStderr();
    void restartProcess();
};

} // namespace keyoubs
