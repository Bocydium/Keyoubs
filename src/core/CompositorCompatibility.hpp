#pragma once

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include <QVersionNumber>

namespace keyoubs {

// ─── Compositor Compatibility Matrix ───
//
// Shows a matrix of Keyoubs features vs. the connected compositor
// and version. Supports: Niri, Hyprland, Sway, i3.
//
// Green: fully supported
// Yellow: partial or requires newer version
// Red: not supported
// Grey: disabled (version-gated UI)

enum class SupportLevel {
    Supported,    // Green
    Partial,      // Yellow
    Unsupported,  // Red
    Unknown       // Not yet checked
};

struct FeatureSupport {
    QString featureId;
    QString featureName;
    SupportLevel level;
    QString requiredVersion; // e.g. "0.55"
    QString tooltip;         // why: "requires Hyprland >= 0.55"
};

struct CompositorInfo {
    QString name;        // "hyprland", "niri", "sway", "i3"
    QVersionNumber version;
    bool isConnected = false;
};

class CompositorCompatibility : public QObject
{
    Q_OBJECT

public:
    explicit CompositorCompatibility(QObject *parent = nullptr);

    void setCompositor(const CompositorInfo &info);
    void registerFeature(const QString &id, const QString &name,
                         const QString &requiredVersion,
                         const QString &tooltip = QString());

    // Check support for each registered feature
    void evaluateAll();
    [[nodiscard]] QList<FeatureSupport> results() const { return m_results; }
    [[nodiscard]] SupportLevel supportFor(const QString &featureId) const;

    // For UI: should this feature be shown as disabled?
    [[nodiscard]] bool shouldDisable(const QString &featureId) const;
    [[nodiscard]] QString tooltipFor(const QString &featureId) const;

signals:
    void resultsChanged();

private:
    CompositorInfo m_compositor;
    QHash<QString, FeatureSupport> m_features;
    QList<FeatureSupport> m_results;

    SupportLevel checkVersion(const QString &required) const;
};

} // namespace keyoubs
