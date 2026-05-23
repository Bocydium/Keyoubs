#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QJsonObject>
#include <QRect>

namespace keyoubs {

// ─── Component System ───
//
// Every built item in Keyoubs is a Component — a named, saveable, deployable unit
// stored in ~/.config/keyoubs/components/<name>/component.json
//
// Each component has:
//   - visual definition (position, size, style)
//   - behavior definition (data sources, triggers, hooks)
//   - deploy configuration (monitor, layer, anchor, autostart)
//
// Components are composable. A status bar embeds clock, workspace, battery sub-components.

enum class SurfaceLayer {
    Background = 0,
    Bottom = 1,
    Top = 2,
    Overlay = 3
};

enum class SurfaceAnchor {
    None = 0,
    Top = 1,
    Bottom = 2,
    Left = 4,
    Right = 8,
    TopLeft = Top | Left,
    TopRight = Top | Right,
    BottomLeft = Bottom | Left,
    BottomRight = Bottom | Right
};

class Component : public QObject
{
    Q_OBJECT

public:
    explicit Component(const QString &name, QObject *parent = nullptr);

    [[nodiscard]] QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    [[nodiscard]] QString type() const { return m_type; }
    void setType(const QString &type) { m_type = type; }

    // Visual properties
    [[nodiscard]] QRect geometry() const;
    void setGeometry(const QRect &rect);

    [[nodiscard]] SurfaceLayer layer() const { return m_layer; }
    void setLayer(SurfaceLayer layer) { m_layer = layer; }

    [[nodiscard]] SurfaceAnchor anchor() const { return m_anchor; }
    void setAnchor(SurfaceAnchor anchor) { m_anchor = anchor; }

    [[nodiscard]] int monitor() const { return m_monitor; }
    void setMonitor(int monitor) { m_monitor = monitor; }

    [[nodiscard]] bool autostart() const { return m_autostart; }
    void setAutostart(bool v) { m_autostart = v; }

    // Style overrides (merged with global theme)
    [[nodiscard]] QVariantMap styleOverrides() const { return m_styleOverrides; }
    void setStyleOverrides(const QVariantMap &map) { m_styleOverrides = map; }

    // Behavior / data sources
    [[nodiscard]] QVariantMap behavior() const { return m_behavior; }
    void setBehavior(const QVariantMap &map) { m_behavior = map; }

    // Sub-components (for composite components like status bars)
    [[nodiscard]] QList<Component*> subComponents() const { return m_subComponents; }
    void addSubComponent(Component *comp);
    void removeSubComponent(Component *comp);

    // Persistence
    bool save() const;
    bool load();
    bool loadFromJson(const QJsonObject &obj);
    QJsonObject toJson() const;

    static QString componentsDir();
    static QList<Component*> loadAllComponents(QObject *parent = nullptr);

signals:
    void geometryChanged(const QRect &rect);
    void styleOverridesChanged(const QVariantMap &map);
    void behaviorChanged(const QVariantMap &map);

private:
    QString m_name;
    QString m_type; // "statusbar", "dock", "widget", "launcher", etc.
    QRect m_geometry;
    SurfaceLayer m_layer = SurfaceLayer::Top;
    SurfaceAnchor m_anchor = SurfaceAnchor::Top;
    int m_monitor = -1; // -1 = all monitors
    bool m_autostart = false;

    QVariantMap m_styleOverrides;
    QVariantMap m_behavior;
    QList<Component*> m_subComponents;
};

} // namespace keyoubs
