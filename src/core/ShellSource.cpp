#include "ShellSource.hpp"
#include <QDebug>

namespace keyoubs {

ShellSource::ShellSource(const QString &name,
                         const QString &command,
                         int intervalMs,
                         QObject *parent)
    : PollingDataSource(name, intervalMs, parent)
    , m_command(command)
{
}

void ShellSource::poll()
{
    QProcess proc;
    proc.start("sh", QStringList() << "-c" << m_command);
    if (!proc.waitForFinished(5000)) {
        emit error(QString("Shell command timed out: %1").arg(m_command));
        return;
    }

    if (proc.exitCode() != 0) {
        QString err = proc.readAllStandardError().trimmed();
        emit error(QString("Shell command failed (%1): %2").arg(proc.exitCode()).arg(err));
        return;
    }

    QString output = proc.readAllStandardOutput().trimmed();
    setValue(output);
}

} // namespace keyoubs
