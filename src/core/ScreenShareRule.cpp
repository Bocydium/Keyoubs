#include "ScreenShareRule.hpp"

namespace keyoubs {

ScreenShareRule::ScreenShareRule(QObject *parent)
    : QObject(parent)
{
}

void ScreenShareRule::addRule(const ShareRule &rule)
{
    m_rules.append(rule);
    emit ruleAdded(m_rules.size() - 1);
}

void ScreenShareRule::removeRule(int index)
{
    if (index < 0 || index >= m_rules.size()) return;
    m_rules.removeAt(index);
    emit ruleRemoved(index);
}

void ScreenShareRule::setLayerNoShare(const QString &layerName, bool noShare)
{
    m_layerNoShare[layerName] = noShare;
}

bool ScreenShareRule::isLayerNoShare(const QString &layerName) const
{
    return m_layerNoShare.value(layerName, false);
}

QString ScreenShareRule::toHyprlandConfig() const
{
    QString out;
    out += "# Screen share rules\n";

    for (const auto &rule : m_rules) {
        if (rule.exclude) {
            out += "windowrulev2 = noscreenshare, " + rule.matcher + "\n";
        }
    }

    for (auto it = m_layerNoShare.begin(); it != m_layerNoShare.end(); ++it) {
        if (it.value()) {
            out += "layerrule = noscreenshare, " + it.key() + "\n";
        }
    }

    return out;
}

} // namespace keyoubs
