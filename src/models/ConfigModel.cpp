#include "ConfigModel.hpp"

namespace keyoubs {

ConfigModel::ConfigModel(QObject *parent)
    : QObject(parent)
{
}

// ─── Layout ───

void ConfigModel::setGapSize(int v)
{
    if (m_gapSize == v) return;
    m_gapSize = v;
    emit gapSizeChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setGapsOnTop(bool v)
{
    if (m_gapsOnTop == v) return;
    m_gapsOnTop = v;
    emit gapsOnTopChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setGapsOnBottom(bool v)
{
    if (m_gapsOnBottom == v) return;
    m_gapsOnBottom = v;
    emit gapsOnBottomChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setGapsOnLeft(bool v)
{
    if (m_gapsOnLeft == v) return;
    m_gapsOnLeft = v;
    emit gapsOnLeftChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setGapsOnRight(bool v)
{
    if (m_gapsOnRight == v) return;
    m_gapsOnRight = v;
    emit gapsOnRightChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Borders ───

void ConfigModel::setBorderWidth(int v)
{
    if (m_borderWidth == v) return;
    m_borderWidth = v;
    emit borderWidthChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBorderColorActive(const QColor &v)
{
    if (m_borderColorActive == v) return;
    m_borderColorActive = v;
    emit borderColorActiveChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBorderColorInactive(const QColor &v)
{
    if (m_borderColorInactive == v) return;
    m_borderColorInactive = v;
    emit borderColorInactiveChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBorderOnTop(bool v)
{
    if (m_borderOnTop == v) return;
    m_borderOnTop = v;
    emit borderOnTopChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBorderOnBottom(bool v)
{
    if (m_borderOnBottom == v) return;
    m_borderOnBottom = v;
    emit borderOnBottomChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBorderOnLeft(bool v)
{
    if (m_borderOnLeft == v) return;
    m_borderOnLeft = v;
    emit borderOnLeftChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBorderOnRight(bool v)
{
    if (m_borderOnRight == v) return;
    m_borderOnRight = v;
    emit borderOnRightChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Appearance ───

void ConfigModel::setWindowOpacity(float v)
{
    if (qFuzzyCompare(m_windowOpacity, v)) return;
    m_windowOpacity = v;
    emit windowOpacityChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBlur(bool v)
{
    if (m_blur == v) return;
    m_blur = v;
    emit blurChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setBlurSize(int v)
{
    if (m_blurSize == v) return;
    m_blurSize = v;
    emit blurSizeChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setShadows(bool v)
{
    if (m_shadows == v) return;
    m_shadows = v;
    emit shadowsChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Behavior ───

void ConfigModel::setFocusFollowsMouseDelay(int v)
{
    if (m_focusFollowsMouseDelay == v) return;
    m_focusFollowsMouseDelay = v;
    emit focusFollowsMouseDelayChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setFocusFollowsMouse(bool v)
{
    if (m_focusFollowsMouse == v) return;
    m_focusFollowsMouse = v;
    emit focusFollowsMouseChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setClickToFocus(bool v)
{
    if (m_clickToFocus == v) return;
    m_clickToFocus = v;
    emit clickToFocusChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Workspaces ───

void ConfigModel::setWorkspaces(const QVariantList &v)
{
    if (m_workspaces == v) return;
    m_workspaces = v;
    emit workspacesChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setActiveWorkspaceId(int v)
{
    if (m_activeWorkspaceId == v) return;
    m_activeWorkspaceId = v;
    emit activeWorkspaceIdChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Windows ───

void ConfigModel::setWindows(const QVariantList &v)
{
    if (m_windows == v) return;
    m_windows = v;
    emit windowsChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setFocusedWindowId(int v)
{
    if (m_focusedWindowId == v) return;
    m_focusedWindowId = v;
    emit focusedWindowIdChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Outputs ───

void ConfigModel::setOutputs(const QVariantList &v)
{
    if (m_outputs == v) return;
    m_outputs = v;
    emit outputsChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Niri-specific ───

void ConfigModel::setCenterFocusedColumn(bool v)
{
    if (m_centerFocusedColumn == v) return;
    m_centerFocusedColumn = v;
    emit centerFocusedColumnChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Hyprland-specific ───

void ConfigModel::setLayoutMode(const QString &v)
{
    if (m_layoutMode == v) return;
    m_layoutMode = v;
    emit layoutModeChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

void ConfigModel::setPreserveSplit(bool v)
{
    if (m_preserveSplit == v) return;
    m_preserveSplit = v;
    emit preserveSplitChanged(v);
    if (!m_batchUpdating) emit modelChanged();
    else m_batchChanged = true;
}

// ─── Batch Update ───

void ConfigModel::beginBatchUpdate()
{
    m_batchUpdating = true;
    m_batchChanged = false;
}

void ConfigModel::endBatchUpdate()
{
    m_batchUpdating = false;
    if (m_batchChanged) {
        m_batchChanged = false;
        emit modelChanged();
    }
}

// ─── Serialize / Deserialize ───

QVariantMap ConfigModel::toVariantMap() const
{
    QVariantMap map;
    map["gapSize"] = m_gapSize;
    map["gapsOnTop"] = m_gapsOnTop;
    map["gapsOnBottom"] = m_gapsOnBottom;
    map["gapsOnLeft"] = m_gapsOnLeft;
    map["gapsOnRight"] = m_gapsOnRight;
    map["borderWidth"] = m_borderWidth;
    map["borderColorActive"] = m_borderColorActive.name();
    map["borderColorInactive"] = m_borderColorInactive.name();
    map["borderOnTop"] = m_borderOnTop;
    map["borderOnBottom"] = m_borderOnBottom;
    map["borderOnLeft"] = m_borderOnLeft;
    map["borderOnRight"] = m_borderOnRight;
    map["windowOpacity"] = m_windowOpacity;
    map["blur"] = m_blur;
    map["blurSize"] = m_blurSize;
    map["shadows"] = m_shadows;
    map["focusFollowsMouseDelay"] = m_focusFollowsMouseDelay;
    map["focusFollowsMouse"] = m_focusFollowsMouse;
    map["clickToFocus"] = m_clickToFocus;
    map["workspaces"] = m_workspaces;
    map["activeWorkspaceId"] = m_activeWorkspaceId;
    map["windows"] = m_windows;
    map["focusedWindowId"] = m_focusedWindowId;
    map["outputs"] = m_outputs;
    map["centerFocusedColumn"] = m_centerFocusedColumn;
    map["layoutMode"] = m_layoutMode;
    map["preserveSplit"] = m_preserveSplit;
    return map;
}

void ConfigModel::fromVariantMap(const QVariantMap &map)
{
    beginBatchUpdate();

    if (map.contains("gapSize")) setGapSize(map["gapSize"].toInt());
    if (map.contains("gapsOnTop")) setGapsOnTop(map["gapsOnTop"].toBool());
    if (map.contains("gapsOnBottom")) setGapsOnBottom(map["gapsOnBottom"].toBool());
    if (map.contains("gapsOnLeft")) setGapsOnLeft(map["gapsOnLeft"].toBool());
    if (map.contains("gapsOnRight")) setGapsOnRight(map["gapsOnRight"].toBool());
    if (map.contains("borderWidth")) setBorderWidth(map["borderWidth"].toInt());
    if (map.contains("borderColorActive")) setBorderColorActive(QColor(map["borderColorActive"].toString()));
    if (map.contains("borderColorInactive")) setBorderColorInactive(QColor(map["borderColorInactive"].toString()));
    if (map.contains("borderOnTop")) setBorderOnTop(map["borderOnTop"].toBool());
    if (map.contains("borderOnBottom")) setBorderOnBottom(map["borderOnBottom"].toBool());
    if (map.contains("borderOnLeft")) setBorderOnLeft(map["borderOnLeft"].toBool());
    if (map.contains("borderOnRight")) setBorderOnRight(map["borderOnRight"].toBool());
    if (map.contains("windowOpacity")) setWindowOpacity(map["windowOpacity"].toFloat());
    if (map.contains("blur")) setBlur(map["blur"].toBool());
    if (map.contains("blurSize")) setBlurSize(map["blurSize"].toInt());
    if (map.contains("shadows")) setShadows(map["shadows"].toBool());
    if (map.contains("focusFollowsMouseDelay")) setFocusFollowsMouseDelay(map["focusFollowsMouseDelay"].toInt());
    if (map.contains("focusFollowsMouse")) setFocusFollowsMouse(map["focusFollowsMouse"].toBool());
    if (map.contains("clickToFocus")) setClickToFocus(map["clickToFocus"].toBool());
    if (map.contains("workspaces")) setWorkspaces(map["workspaces"].toList());
    if (map.contains("activeWorkspaceId")) setActiveWorkspaceId(map["activeWorkspaceId"].toInt());
    if (map.contains("windows")) setWindows(map["windows"].toList());
    if (map.contains("focusedWindowId")) setFocusedWindowId(map["focusedWindowId"].toInt());
    if (map.contains("outputs")) setOutputs(map["outputs"].toList());
    if (map.contains("centerFocusedColumn")) setCenterFocusedColumn(map["centerFocusedColumn"].toBool());
    if (map.contains("layoutMode")) setLayoutMode(map["layoutMode"].toString());
    if (map.contains("preserveSplit")) setPreserveSplit(map["preserveSplit"].toBool());

    endBatchUpdate();
}

} // namespace keyoubs
