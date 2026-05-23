#include "ConfigDiff.hpp"

namespace keyoubs {

ConfigDiff::ConfigDiff(QObject *parent)
    : QObject(parent)
{
}

QList<DiffLine> ConfigDiff::computeDiff(const QString &oldText,
                                         const QString &newText) const
{
    QStringList oldLines = oldText.split('\n', Qt::KeepEmptyParts);
    QStringList newLines = newText.split('\n', Qt::KeepEmptyParts);
    return computeLineDiff(oldLines, newLines);
}

QList<DiffLine> ConfigDiff::computeLineDiff(const QStringList &oldLines,
                                             const QStringList &newLines) const
{
    QList<DiffLine> result;
    int oldIdx = 0, newIdx = 0;

    while (oldIdx < oldLines.size() || newIdx < newLines.size()) {
        if (oldIdx < oldLines.size() && newIdx < newLines.size()) {
            if (oldLines[oldIdx] == newLines[newIdx]) {
                // Context line
                result.append({DiffLineType::Context, oldLines[oldIdx], oldIdx + 1});
                ++oldIdx;
                ++newIdx;
            } else {
                // Check if this old line appears later in new (removed)
                // Check if this new line appeared earlier in old (added)
                // Simple approach: mark old as removed, new as added
                result.append({DiffLineType::Removed, oldLines[oldIdx], oldIdx + 1});
                result.append({DiffLineType::Added, newLines[newIdx], newIdx + 1});
                ++oldIdx;
                ++newIdx;
            }
        } else if (oldIdx < oldLines.size()) {
            // Remaining old lines removed
            result.append({DiffLineType::Removed, oldLines[oldIdx], oldIdx + 1});
            ++oldIdx;
        } else {
            // Remaining new lines added
            result.append({DiffLineType::Added, newLines[newIdx], newIdx + 1});
            ++newIdx;
        }
    }

    return result;
}

ConfigDiff::DiffStats ConfigDiff::stats(const QList<DiffLine> &diff) const
{
    DiffStats s;
    for (const auto &line : diff) {
        switch (line.type) {
        case DiffLineType::Added: ++s.linesAdded; break;
        case DiffLineType::Removed: ++s.linesRemoved; break;
        default: break;
        }
    }
    s.linesChanged = s.linesAdded + s.linesRemoved;
    return s;
}

} // namespace keyoubs
