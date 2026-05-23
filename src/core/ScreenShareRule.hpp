#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QHash>

namespace keyoubs {

// ─── Screen Share Rule Editor ───
//
// Controls which windows are excluded from screensharing.
// Also sets noscreenshare on layer-shell components.

struct ShareRule {
    QString matcher; // app-id or window class
    bool exclude = true; // true = black rectangle in captures
};

class ScreenShareRule : public QObject
{
    Q_OBJECT

public:
    explicit ScreenShareRule(QObject *parent = nullptr);

    void addRule(const ShareRule &rule);
    void removeRule(int index);
    [[nodiscard]] QList<ShareRule> rules() const { return m_rules; }

    // Layer-shell component noscreenshare toggle
    void setLayerNoShare(const QString &layerName, bool noShare);
    [[nodiscard]] bool isLayerNoShare(const QString &layerName) const;

    [[nodiscard]] QString toHyprlandConfig() const;

signals:
    void ruleAdded(int index);
    void ruleRemoved(int index);

private:
    QList<ShareRule> m_rules;
    QHash<QString, bool> m_layerNoShare;
};

} // namespace keyoubs
