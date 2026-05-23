#pragma once

#include "DataSource.hpp"
#include <QFileSystemWatcher>

namespace keyoubs {

// ─── File Watcher Data Source ───
//
// Watches a file for changes and emits its contents.
// Supports plain text, JSON, and line-based parsing.

class FileSource : public DataSource
{
    Q_OBJECT

public:
    enum class ParseMode {
        PlainText,
        Json,
        Lines
    };

    explicit FileSource(const QString &name,
                        const QString &filePath,
                        ParseMode mode = ParseMode::PlainText,
                        QObject *parent = nullptr);

    void start() override;
    void stop() override;
    QString type() const override { return QStringLiteral("file"); }

    void setFilePath(const QString &path);
    void setParseMode(ParseMode mode) { m_parseMode = mode; }

private:
    QString m_filePath;
    ParseMode m_parseMode;
    QFileSystemWatcher m_watcher;

    void readFile();
};

} // namespace keyoubs
