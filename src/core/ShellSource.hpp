#pragma once

#include "DataSource.hpp"
#include <QProcess>

namespace keyoubs {

// ─── Shell Command Data Source ───
//
// Runs a shell command periodically and emits its stdout as the value.
// Useful for custom scripts, system info, or any command-line tool output.

class ShellSource : public PollingDataSource
{
    Q_OBJECT

public:
    explicit ShellSource(const QString &name,
                         const QString &command,
                         int intervalMs = 5000,
                         QObject *parent = nullptr);

    QString type() const override { return QStringLiteral("shell"); }
    void setCommand(const QString &cmd) { m_command = cmd; }

protected:
    void poll() override;

private:
    QString m_command;
};

} // namespace keyoubs
