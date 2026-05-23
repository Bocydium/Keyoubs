#pragma once

#include <QObject>
#include <QString>
#include <QList>

namespace keyoubs {

// ─── Config Diff View ───
//
// Before any change is written to disk, Keyoubs shows a split diff panel
// — green lines added, red lines removed — with an Apply or Discard choice.

enum class DiffLineType {
    Context,
    Added,
    Removed
};

struct DiffLine {
    DiffLineType type;
    QString text;
    int lineNumber;
};

class ConfigDiff : public QObject
{
    Q_OBJECT

public:
    explicit ConfigDiff(QObject *parent = nullptr);

    // Compute diff between two config strings
    [[nodiscard]] QList<DiffLine> computeDiff(const QString &oldText,
                                               const QString &newText) const;

    // Simple line-by-line diff
    [[nodiscard]] QList<DiffLine> computeLineDiff(const QStringList &oldLines,
                                                   const QStringList &newLines) const;

    // Summary stats
    struct DiffStats {
        int linesAdded = 0;
        int linesRemoved = 0;
        int linesChanged = 0;
    };
    [[nodiscard]] DiffStats stats(const QList<DiffLine> &diff) const;

signals:
    void diffComputed(const QList<DiffLine> &diff);
};

} // namespace keyoubs
