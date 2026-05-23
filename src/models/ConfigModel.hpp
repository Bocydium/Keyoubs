#pragma once

#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QColor>

namespace keyoubs {

// Represents the editable configuration state of a TWM.
// Shared across all compositor backends via a normalized model.
struct ConfigModel
{
    // Layout
    int gapSize = 8;
    bool gapsOnTop = true;
    bool gapsOnBottom = true;
    bool gapsOnLeft = true;
    bool gapsOnRight = true;

    // Borders
    int borderWidth = 2;
    QColor borderColorActive{0x62, 0xA9, 0xE3};
    QColor borderColorInactive{0x33, 0x33, 0x33};
    bool borderOnTop = true;
    bool borderOnBottom = true;
    bool borderOnLeft = true;
    bool borderOnRight = true;

    // Appearance
    float windowOpacity = 1.0f;
    bool blur = false;
    int blurSize = 8;
    bool shadows = false;

    // Behavior
    int focusFollowsMouseDelay = 0;
    bool focusFollowsMouse = false;
    bool clickToFocus = true;

    // Workspaces
    QVariantList workspaces; // normalized workspace list
    int activeWorkspaceId = 1;

    // Windows
    QVariantList windows; // normalized window list
    int focusedWindowId = -1;

    // Outputs/Monitors
    QVariantList outputs;

    // Niri-specific
    bool centerFocusedColumn = false;

    // Hyprland-specific
    QString layoutMode = QStringLiteral("dwindle");
    bool preserveSplit = false;

    bool operator==(const ConfigModel &other) const = default;
    bool operator!=(const ConfigModel &other) const = default;
};

} // namespace keyoubs

Q_DECLARE_METATYPE(keyoubs::ConfigModel)
