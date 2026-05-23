#include "IncludeManager.hpp"
#include <QFile>
#include <QDir>
#include <QRegularExpression>

namespace keyoubs {

IncludeManager::IncludeManager(QObject *parent)
    : QObject(parent)
{
}

void IncludeManager::scanIncludes(const QString &configContent)
{
    m_entries.clear();

    QRegularExpression optionalRe(R"RE(optional-include\s+"([^"]+)")RE");
    QRegularExpression requiredRe(R"RE(include\s+"([^"]+)")RE");

    QRegularExpressionMatchIterator it = optionalRe.globalMatch(configContent);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        IncludeEntry entry;
        entry.path = match.captured(1);
        entry.isOptional = true;
        entry.status = checkFile(entry.path);
        m_entries.append(entry);
    }

    it = requiredRe.globalMatch(configContent);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        IncludeEntry entry;
        entry.path = match.captured(1);
        entry.isOptional = false;
        entry.status = checkFile(entry.path);
        m_entries.append(entry);
    }

    emit entriesChanged();
}

void IncludeManager::addInclude(const QString &path, bool optional)
{
    IncludeEntry entry;
    entry.path = path;
    entry.isOptional = optional;
    entry.status = checkFile(path);
    m_entries.append(entry);
    emit entryAdded(m_entries.size() - 1);
    emit entriesChanged();
}

void IncludeManager::removeInclude(int index)
{
    if (index < 0 || index >= m_entries.size()) return;
    m_entries.removeAt(index);
    emit entryRemoved(index);
    emit entriesChanged();
}

IncludeStatus IncludeManager::checkFile(const QString &path) const
{
    QFileInfo info(path);
    if (!info.exists()) return IncludeStatus::Missing;
    if (!info.isReadable()) return IncludeStatus::Broken;
    return IncludeStatus::Exists;
}

void IncludeManager::generatePerMachineSplit(const QString &baseFile,
                                              const QString &overrideFile)
{
    // Generate a base config with an include for machine-specific overrides
    QString baseContent = "# Base config (shared across machines)\n";
    baseContent += "# Include machine-specific overrides\n";
    baseContent += QString("optional-include \"%1\"\n").arg(overrideFile);

    QString overrideContent = "# Machine-specific overrides\n";
    overrideContent += "# This file is not tracked in dotfiles\n";

    QFile base(baseFile);
    if (base.open(QIODevice::WriteOnly)) {
        base.write(baseContent.toUtf8());
    }

    QFile overrideF(overrideFile);
    if (overrideF.open(QIODevice::WriteOnly)) {
        overrideF.write(overrideContent.toUtf8());
    }
}

} // namespace keyoubs
