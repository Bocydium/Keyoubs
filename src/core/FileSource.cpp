#include "FileSource.hpp"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace keyoubs {

FileSource::FileSource(const QString &name,
                       const QString &filePath,
                       ParseMode mode,
                       QObject *parent)
    : DataSource(name, parent)
    , m_filePath(filePath)
    , m_parseMode(mode)
{
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &FileSource::readFile);
}

void FileSource::start()
{
    m_active = true;
    if (!m_filePath.isEmpty() && QFile::exists(m_filePath)) {
        m_watcher.addPath(m_filePath);
        readFile();
    }
}

void FileSource::stop()
{
    m_active = false;
    if (!m_filePath.isEmpty()) {
        m_watcher.removePath(m_filePath);
    }
}

void FileSource::setFilePath(const QString &path)
{
    if (m_filePath == path) return;

    if (m_active && !m_filePath.isEmpty()) {
        m_watcher.removePath(m_filePath);
    }

    m_filePath = path;

    if (m_active && !m_filePath.isEmpty() && QFile::exists(m_filePath)) {
        m_watcher.addPath(m_filePath);
        readFile();
    }
}

void FileSource::readFile()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Cannot read file: %1").arg(m_filePath));
        return;
    }

    QByteArray data = file.readAll();

    switch (m_parseMode) {
    case ParseMode::PlainText:
        setValue(QString::fromUtf8(data).trimmed());
        break;

    case ParseMode::Json: {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            emit error(QString("Invalid JSON in file: %1").arg(m_filePath));
        } else {
            setValue(doc.toVariant());
        }
        break;
    }

    case ParseMode::Lines:
        setValue(QString::fromUtf8(data).split('\n', Qt::SkipEmptyParts));
        break;
    }
}

} // namespace keyoubs
