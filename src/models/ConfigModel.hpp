#pragma once

#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QColor>

namespace keyoubs {

// ─── Reactive Configuration Model ───
//
// Every property is a Qt property with change signals.
// External edits propagate instantly. No polling, no timers.
//
// Used as the single source of truth for the compositor's
// editable configuration state. All UI panels bind to this.

class ConfigModel : public QObject
{
    Q_OBJECT

    // Layout
    Q_PROPERTY(int gapSize READ gapSize WRITE setGapSize NOTIFY gapSizeChanged)
    Q_PROPERTY(bool gapsOnTop READ gapsOnTop WRITE setGapsOnTop NOTIFY gapsOnTopChanged)
    Q_PROPERTY(bool gapsOnBottom READ gapsOnBottom WRITE setGapsOnBottom NOTIFY gapsOnBottomChanged)
    Q_PROPERTY(bool gapsOnLeft READ gapsOnLeft WRITE setGapsOnLeft NOTIFY gapsOnLeftChanged)
    Q_PROPERTY(bool gapsOnRight READ gapsOnRight WRITE setGapsOnRight NOTIFY gapsOnRightChanged)

    // Borders
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(QColor borderColorActive READ borderColorActive WRITE setBorderColorActive NOTIFY borderColorActiveChanged)
    Q_PROPERTY(QColor borderColorInactive READ borderColorInactive WRITE setBorderColorInactive NOTIFY borderColorInactiveChanged)
    Q_PROPERTY(bool borderOnTop READ borderOnTop WRITE setBorderOnTop NOTIFY borderOnTopChanged)
    Q_PROPERTY(bool borderOnBottom READ borderOnBottom WRITE setBorderOnBottom NOTIFY borderOnBottomChanged)
    Q_PROPERTY(bool borderOnLeft READ borderOnLeft WRITE setBorderOnLeft NOTIFY borderOnLeftChanged)
    Q_PROPERTY(bool borderOnRight READ borderOnRight WRITE setBorderOnRight NOTIFY borderOnRightChanged)

    // Appearance
    Q_PROPERTY(float windowOpacity READ windowOpacity WRITE setWindowOpacity NOTIFY windowOpacityChanged)
    Q_PROPERTY(bool blur READ blur WRITE setBlur NOTIFY blurChanged)
    Q_PROPERTY(int blurSize READ blurSize WRITE setBlurSize NOTIFY blurSizeChanged)
    Q_PROPERTY(bool shadows READ shadows WRITE setShadows NOTIFY shadowsChanged)

    // Behavior
    Q_PROPERTY(int focusFollowsMouseDelay READ focusFollowsMouseDelay WRITE setFocusFollowsMouseDelay NOTIFY focusFollowsMouseDelayChanged)
    Q_PROPERTY(bool focusFollowsMouse READ focusFollowsMouse WRITE setFocusFollowsMouse NOTIFY focusFollowsMouseChanged)
    Q_PROPERTY(bool clickToFocus READ clickToFocus WRITE setClickToFocus NOTIFY clickToFocusChanged)

    // Workspaces
    Q_PROPERTY(QVariantList workspaces READ workspaces WRITE setWorkspaces NOTIFY workspacesChanged)
    Q_PROPERTY(int activeWorkspaceId READ activeWorkspaceId WRITE setActiveWorkspaceId NOTIFY activeWorkspaceIdChanged)

    // Windows
    Q_PROPERTY(QVariantList windows READ windows WRITE setWindows NOTIFY windowsChanged)
    Q_PROPERTY(int focusedWindowId READ focusedWindowId WRITE setFocusedWindowId NOTIFY focusedWindowIdChanged)

    // Outputs
    Q_PROPERTY(QVariantList outputs READ outputs WRITE setOutputs NOTIFY outputsChanged)

    // Niri-specific
    Q_PROPERTY(bool centerFocusedColumn READ centerFocusedColumn WRITE setCenterFocusedColumn NOTIFY centerFocusedColumnChanged)

    // Hyprland-specific
    Q_PROPERTY(QString layoutMode READ layoutMode WRITE setLayoutMode NOTIFY layoutModeChanged)
    Q_PROPERTY(bool preserveSplit READ preserveSplit WRITE setPreserveSplit NOTIFY preserveSplitChanged)

public:
    explicit ConfigModel(QObject *parent = nullptr);

    // Layout getters/setters
    int gapSize() const { return m_gapSize; }
    void setGapSize(int v);
    bool gapsOnTop() const { return m_gapsOnTop; }
    void setGapsOnTop(bool v);
    bool gapsOnBottom() const { return m_gapsOnBottom; }
    void setGapsOnBottom(bool v);
    bool gapsOnLeft() const { return m_gapsOnLeft; }
    void setGapsOnLeft(bool v);
    bool gapsOnRight() const { return m_gapsOnRight; }
    void setGapsOnRight(bool v);

    // Border getters/setters
    int borderWidth() const { return m_borderWidth; }
    void setBorderWidth(int v);
    QColor borderColorActive() const { return m_borderColorActive; }
    void setBorderColorActive(const QColor &v);
    QColor borderColorInactive() const { return m_borderColorInactive; }
    void setBorderColorInactive(const QColor &v);
    bool borderOnTop() const { return m_borderOnTop; }
    void setBorderOnTop(bool v);
    bool borderOnBottom() const { return m_borderOnBottom; }
    void setBorderOnBottom(bool v);
    bool borderOnLeft() const { return m_borderOnLeft; }
    void setBorderOnLeft(bool v);
    bool borderOnRight() const { return m_borderOnRight; }
    void setBorderOnRight(bool v);

    // Appearance getters/setters
    float windowOpacity() const { return m_windowOpacity; }
    void setWindowOpacity(float v);
    bool blur() const { return m_blur; }
    void setBlur(bool v);
    int blurSize() const { return m_blurSize; }
    void setBlurSize(int v);
    bool shadows() const { return m_shadows; }
    void setShadows(bool v);

    // Behavior getters/setters
    int focusFollowsMouseDelay() const { return m_focusFollowsMouseDelay; }
    void setFocusFollowsMouseDelay(int v);
    bool focusFollowsMouse() const { return m_focusFollowsMouse; }
    void setFocusFollowsMouse(bool v);
    bool clickToFocus() const { return m_clickToFocus; }
    void setClickToFocus(bool v);

    // Workspace getters/setters
    QVariantList workspaces() const { return m_workspaces; }
    void setWorkspaces(const QVariantList &v);
    int activeWorkspaceId() const { return m_activeWorkspaceId; }
    void setActiveWorkspaceId(int v);

    // Window getters/setters
    QVariantList windows() const { return m_windows; }
    void setWindows(const QVariantList &v);
    int focusedWindowId() const { return m_focusedWindowId; }
    void setFocusedWindowId(int v);

    // Output getters/setters
    QVariantList outputs() const { return m_outputs; }
    void setOutputs(const QVariantList &v);

    // Niri-specific
    bool centerFocusedColumn() const { return m_centerFocusedColumn; }
    void setCenterFocusedColumn(bool v);

    // Hyprland-specific
    QString layoutMode() const { return m_layoutMode; }
    void setLayoutMode(const QString &v);
    bool preserveSplit() const { return m_preserveSplit; }
    void setPreserveSplit(bool v);

    // Batch update — emits a single modelChanged after all changes
    void beginBatchUpdate();
    void endBatchUpdate();

    // Serialize/deserialize
    QVariantMap toVariantMap() const;
    void fromVariantMap(const QVariantMap &map);

signals:
    void gapSizeChanged(int value);
    void gapsOnTopChanged(bool value);
    void gapsOnBottomChanged(bool value);
    void gapsOnLeftChanged(bool value);
    void gapsOnRightChanged(bool value);
    void borderWidthChanged(int value);
    void borderColorActiveChanged(const QColor &value);
    void borderColorInactiveChanged(const QColor &value);
    void borderOnTopChanged(bool value);
    void borderOnBottomChanged(bool value);
    void borderOnLeftChanged(bool value);
    void borderOnRightChanged(bool value);
    void windowOpacityChanged(float value);
    void blurChanged(bool value);
    void blurSizeChanged(int value);
    void shadowsChanged(bool value);
    void focusFollowsMouseDelayChanged(int value);
    void focusFollowsMouseChanged(bool value);
    void clickToFocusChanged(bool value);
    void workspacesChanged(const QVariantList &value);
    void activeWorkspaceIdChanged(int value);
    void windowsChanged(const QVariantList &value);
    void focusedWindowIdChanged(int value);
    void outputsChanged(const QVariantList &value);
    void centerFocusedColumnChanged(bool value);
    void layoutModeChanged(const QString &value);
    void preserveSplitChanged(bool value);

    // Emitted after batch update completes
    void modelChanged();

private:
    // Layout
    int m_gapSize = 8;
    bool m_gapsOnTop = true;
    bool m_gapsOnBottom = true;
    bool m_gapsOnLeft = true;
    bool m_gapsOnRight = true;

    // Borders
    int m_borderWidth = 2;
    QColor m_borderColorActive{0x62, 0xA9, 0xE3};
    QColor m_borderColorInactive{0x33, 0x33, 0x33};
    bool m_borderOnTop = true;
    bool m_borderOnBottom = true;
    bool m_borderOnLeft = true;
    bool m_borderOnRight = true;

    // Appearance
    float m_windowOpacity = 1.0f;
    bool m_blur = false;
    int m_blurSize = 8;
    bool m_shadows = false;

    // Behavior
    int m_focusFollowsMouseDelay = 0;
    bool m_focusFollowsMouse = false;
    bool m_clickToFocus = true;

    // Workspaces
    QVariantList m_workspaces;
    int m_activeWorkspaceId = 1;

    // Windows
    QVariantList m_windows;
    int m_focusedWindowId = -1;

    // Outputs
    QVariantList m_outputs;

    // Niri-specific
    bool m_centerFocusedColumn = false;

    // Hyprland-specific
    QString m_layoutMode = QStringLiteral("dwindle");
    bool m_preserveSplit = false;

    // Batch update state
    bool m_batchUpdating = false;
    bool m_batchChanged = false;
};

} // namespace keyoubs

Q_DECLARE_METATYPE(keyoubs::ConfigModel*)
