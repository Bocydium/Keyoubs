#include "CompositorCompatibility.hpp"

namespace keyoubs {

CompositorCompatibility::CompositorCompatibility(QObject *parent)
    : QObject(parent)
{
}

void CompositorCompatibility::setCompositor(const CompositorInfo &info)
{
    m_compositor = info;
    evaluateAll();
}

void CompositorCompatibility::registerFeature(const QString &id, const QString &name,
                                               const QString &requiredVersion,
                                               const QString &tooltip)
{
    FeatureSupport fs;
    fs.featureId = id;
    fs.featureName = name;
    fs.requiredVersion = requiredVersion;
    fs.tooltip = tooltip;
    fs.level = SupportLevel::Unknown;
    m_features[id] = fs;
}

void CompositorCompatibility::evaluateAll()
{
    m_results.clear();
    for (auto it = m_features.begin(); it != m_features.end(); ++it) {
        FeatureSupport fs = it.value();
        fs.level = checkVersion(fs.requiredVersion);
        if (fs.tooltip.isEmpty()) {
            fs.tooltip = QString("Requires %1 >= %2")
                            .arg(m_compositor.name)
                            .arg(fs.requiredVersion);
        }
        m_results.append(fs);
    }
    emit resultsChanged();
}

SupportLevel CompositorCompatibility::supportFor(const QString &featureId) const
{
    for (const auto &fs : m_results) {
        if (fs.featureId == featureId) return fs.level;
    }
    return SupportLevel::Unknown;
}

bool CompositorCompatibility::shouldDisable(const QString &featureId) const
{
    auto level = supportFor(featureId);
    return level == SupportLevel::Unsupported || level == SupportLevel::Unknown;
}

QString CompositorCompatibility::tooltipFor(const QString &featureId) const
{
    for (const auto &fs : m_results) {
        if (fs.featureId == featureId) return fs.tooltip;
    }
    return QString();
}

SupportLevel CompositorCompatibility::checkVersion(const QString &required) const
{
    if (!m_compositor.isConnected) return SupportLevel::Unknown;

    QVersionNumber req = QVersionNumber::fromString(required);
    if (m_compositor.version >= req) {
        return SupportLevel::Supported;
    }
    return SupportLevel::Unsupported;
}

} // namespace keyoubs
