#pragma once

#include <QObject>

namespace keyoubs {

// Manages live preview state - tracking what changes are pending
// vs what has been applied to the compositor
class LivePreview : public QObject
{
    Q_OBJECT

public:
    explicit LivePreview(QObject *parent = nullptr);

    void previewChange(const QString &property, const QVariant &value);
    void commitChanges();
    void discardChanges();

    [[nodiscard]] bool hasPendingChanges() const;

signals:
    void previewReady(const QString &property, const QVariant &value);
    void changesCommitted();
    void changesDiscarded();

private:
    bool m_hasPendingChanges = false;
};

} // namespace keyoubs
