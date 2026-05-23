#pragma once

#include <QObject>
#include <QColor>
#include <QFont>
#include <QVariantMap>
#include <QFileSystemWatcher>
#include <QString>

namespace keyoubs {

// ─── Quickshell-Inspired Theme & Config System ───
//
// Hot-reloadable JSON config files under ~/.config/keyoubs/:
//   theme.json     — colors, sizes, fonts, border-radius
//   layout.json    — toolbar position, panel position, visible tools
//   properties.json — which sections appear, default values
//
// Lua scripting hooks (optional):
//   ~/.config/keyoubs/scripts/init.lua
//   Hooks: onToolSelected, onPropertyChanged, onWindowSelected

struct ThemeColors {
    QColor bg{0x10, 0x10, 0x10};
    QColor surface{0x1A, 0x1A, 0x1A};
    QColor surfaceHover{0x22, 0x22, 0x22};
    QColor border{0x2A, 0x2A, 0x2A};
    QColor borderHover{0x3A, 0x3A, 0x3A};
    QColor borderFocus{0x3A, 0x3A, 0x3A};
    QColor text{0xE0, 0xE0, 0xE0};
    QColor textMuted{0x88, 0x88, 0x88};
    QColor accent{0x62, 0xA9, 0xE3};
    QColor accentHover{0x7A, 0xB8, 0xE8};
    QColor danger{0xFF, 0x5F, 0x5F};
    QColor dangerHover{0xFF, 0x7A, 0x7A};
};

struct ThemeSizes {
    int toolbarHeight = 52;
    int toolbarHeightFloating = 56;
    int toolButton = 40;
    int toolButtonIcon = 22;
    int actionButtonPaddingH = 18;
    int actionButtonPaddingV = 6;
    int borderRadius = 8;
    int panelMinWidth = 260;
    int panelMaxWidth = 400;
    int panelContentSpacing = 10;
    int checkboxSize = 18;
    int inputMinHeight = 28;
    int dragHandleWidth = 4;
    int dragHandleHeight = 14;
};

struct ThemeFont {
    QString family = "Inter";
    int size = 10;
    int titleSize = 15;
    int headerSize = 14;
    int sectionSize = 10;
    int bodySize = 13;
};

class ThemeConfig : public QObject
{
    Q_OBJECT

public:
    explicit ThemeConfig(QObject *parent = nullptr);

    [[nodiscard]] const ThemeColors &colors() const { return m_colors; }
    [[nodiscard]] const ThemeSizes &sizes() const { return m_sizes; }
    [[nodiscard]] const ThemeFont &font() const { return m_font; }
    [[nodiscard]] QVariantMap layoutConfig() const { return m_layout; }
    [[nodiscard]] QVariantMap propertiesConfig() const { return m_properties; }

    // Generate Qt StyleSheet for Toolbar
    QString toolbarStyleSheet(bool floating = false) const;
    // Generate Qt StyleSheet for PropertyPanel
    QString propertyPanelStyleSheet(bool left = false, bool right = false, bool floating = false) const;
    // Generate Qt StyleSheet for general widgets
    QString baseStyleSheet() const;

    QString configDir() const;

public slots:
    void reload();

signals:
    void themeChanged();

private:
    void loadThemeJson();
    void loadLayoutJson();
    void loadPropertiesJson();
    void setupWatcher();

    ThemeColors m_colors;
    ThemeSizes m_sizes;
    ThemeFont m_font;
    QVariantMap m_layout;
    QVariantMap m_properties;

    QFileSystemWatcher m_watcher;
    bool m_watcherSetup = false;
};

} // namespace keyoubs
