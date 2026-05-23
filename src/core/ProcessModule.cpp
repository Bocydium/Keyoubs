#include "ProcessModule.hpp"
#include <QTimer>
#include <QDebug>

namespace keyoubs {

ProcessModule::ProcessModule(const QString &name,
                              const QString &command,
                              const QStringList &args,
                              QObject *parent)
    : DataSource(name, parent)
    , m_command(command)
    , m_args(args)
{
}

void ProcessModule::start()
{
    m_active = true;
    m_restartAttempts = 0;
    restartProcess();
}

void ProcessModule::stop()
{
    m_active = false;
    if (m_process) {
        m_process->terminate();
        if (!m_process->waitForFinished(2000)) {
            m_process->kill();
        }
        delete m_process;
        m_process = nullptr;
    }
}

void ProcessModule::setCommand(const QString &cmd, const QStringList &args)
{
    m_command = cmd;
    m_args = args;
}

void ProcessModule::writeToStdin(const QString &data)
{
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->write(data.toUtf8());
        m_process->write("\n");
    }
}

void ProcessModule::restartProcess()
{
    if (!m_active) return;

    if (m_process) {
        m_process->deleteLater();
    }

    m_process = new QProcess(this);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ProcessModule::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &ProcessModule::onReadyReadStdout);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &ProcessModule::onReadyReadStderr);

    m_process->start(m_command, m_args);
    emit processStarted();
}

void ProcessModule::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    emit processStopped(exitCode);

    if (!m_active) return;

    bool shouldRestart = false;
    switch (m_restartPolicy) {
    case ProcessRestartPolicy::Never:
        shouldRestart = false;
        break;
    case ProcessRestartPolicy::OnError:
        shouldRestart = (status == QProcess::CrashExit || exitCode != 0);
        break;
    case ProcessRestartPolicy::Always:
        shouldRestart = true;
        break;
    case ProcessRestartPolicy::Backoff:
        shouldRestart = true;
        ++m_restartAttempts;
        break;
    }

    if (shouldRestart) {
        int delay = m_backoffMs;
        if (m_restartPolicy == ProcessRestartPolicy::Backoff) {
            delay = qMin(m_backoffMs * m_restartAttempts, 30000);
        }
        QTimer::singleShot(delay, this, &ProcessModule::restartProcess);
    }
}

void ProcessModule::onReadyReadStdout()
{
    if (!m_process) return;
    QString data = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    if (!data.isEmpty()) {
        setValue(data);
        emit stdoutReceived(data);
    }
}

void ProcessModule::onReadyReadStderr()
{
    if (!m_process) return;
    QString data = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
    if (!data.isEmpty()) {
        emit stderrReceived(data);
    }
}

} // namespace keyoubs
