#include "LivePreview.hpp"

namespace keyoubs {

LivePreview::LivePreview(QObject *parent)
    : QObject(parent)
{
}

void LivePreview::previewChange(const QString &property, const QVariant &value)
{
    m_hasPendingChanges = true;
    emit previewReady(property, value);
}

void LivePreview::commitChanges()
{
    m_hasPendingChanges = false;
    emit changesCommitted();
}

void LivePreview::discardChanges()
{
    m_hasPendingChanges = false;
    emit changesDiscarded();
}

bool LivePreview::hasPendingChanges() const
{
    return m_hasPendingChanges;
}

} // namespace keyoubs
