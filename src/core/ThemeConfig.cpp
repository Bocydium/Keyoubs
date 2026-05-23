#include "ThemeConfig.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

namespace keyoubs {

ThemeConfig::ThemeConfig(QObject *parent)
    : QObject(parent)
{
    reload();
    setupWatcher();
}

QString ThemeConfig::configDir() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    return path + "/keyoubs";
}

void ThemeConfig::setupWatcher()
{
    if (m_watcherSetup) return;

    QString dir = configDir();
    QDir().mkpath(dir);

    QStringList files;
    files << dir + "/theme.json";
    files << dir + "/layout.json";
    files << dir + "/properties.json";

    for (const QString &f : files) {
        if (QFile::exists(f)) {
            m_watcher.addPath(f);
        }
    }

    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, [this]() {
        qInfo() << "Config file changed, reloading...";
        reload();
        emit themeChanged();
    });

    m_watcherSetup = true;
}

static QColor parseColor(const QJsonValue &val, const QColor &fallback)
{
    if (val.isString()) {
        return QColor(val.toString());
    }
    if (val.isArray() && val.toArray().size() >= 3) {
        QJsonArray arr = val.toArray();
        return QColor(arr[0].toInt(), arr[1].toInt(), arr[2].toInt());
    }
    return fallback;
}

void ThemeConfig::reload()
{
    loadThemeJson();
    loadLayoutJson();
    loadPropertiesJson();
}

void ThemeConfig::loadThemeJson()
{
    QString path = configDir() + "/theme.json";
    if (!QFile::exists(path)) {
        // Write default theme file
        QJsonObject def;
        QJsonObject colors;
        colors["bg"] = "#101010";
        colors["surface"] = "#1A1A1A";
        colors["surfaceHover"] = "#222222";
        colors["border"] = "#2A2A2A";
        colors["borderHover"] = "#3A3A3A";
        colors["borderFocus"] = "#3A3A3A";
        colors["text"] = "#E0E0E0";
        colors["textMuted"] = "#888888";
        colors["accent"] = "#62A9E3";
        colors["accentHover"] = "#7AB8E8";
        colors["danger"] = "#FF5F5F";
        colors["dangerHover"] = "#FF7A7A";
        def["colors"] = colors;

        QJsonObject sizes;
        sizes["toolbarHeight"] = 52;
        sizes["toolbarHeightFloating"] = 56;
        sizes["toolButton"] = 40;
        sizes["toolButtonIcon"] = 22;
        sizes["actionButtonPaddingH"] = 18;
        sizes["actionButtonPaddingV"] = 6;
        sizes["borderRadius"] = 8;
        sizes["panelMinWidth"] = 260;
        sizes["panelMaxWidth"] = 400;
        sizes["panelContentSpacing"] = 10;
        sizes["checkboxSize"] = 18;
        sizes["inputMinHeight"] = 28;
        def["sizes"] = sizes;

        QJsonObject font;
        font["family"] = "Inter";
        font["size"] = 10;
        font["titleSize"] = 15;
        font["headerSize"] = 14;
        font["sectionSize"] = 10;
        font["bodySize"] = 13;
        def["font"] = font;

        QJsonDocument doc(def);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
        }
        return; // Defaults already set in member initializers
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    QJsonObject colors = root.value("colors").toObject();
    m_colors.bg = parseColor(colors["bg"], m_colors.bg);
    m_colors.surface = parseColor(colors["surface"], m_colors.surface);
    m_colors.surfaceHover = parseColor(colors["surfaceHover"], m_colors.surfaceHover);
    m_colors.border = parseColor(colors["border"], m_colors.border);
    m_colors.borderHover = parseColor(colors["borderHover"], m_colors.borderHover);
    m_colors.borderFocus = parseColor(colors["borderFocus"], m_colors.borderFocus);
    m_colors.text = parseColor(colors["text"], m_colors.text);
    m_colors.textMuted = parseColor(colors["textMuted"], m_colors.textMuted);
    m_colors.accent = parseColor(colors["accent"], m_colors.accent);
    m_colors.accentHover = parseColor(colors["accentHover"], m_colors.accentHover);
    m_colors.danger = parseColor(colors["danger"], m_colors.danger);
    m_colors.dangerHover = parseColor(colors["dangerHover"], m_colors.dangerHover);

    QJsonObject sizes = root.value("sizes").toObject();
    m_sizes.toolbarHeight = sizes["toolbarHeight"].toInt(m_sizes.toolbarHeight);
    m_sizes.toolbarHeightFloating = sizes["toolbarHeightFloating"].toInt(m_sizes.toolbarHeightFloating);
    m_sizes.toolButton = sizes["toolButton"].toInt(m_sizes.toolButton);
    m_sizes.toolButtonIcon = sizes["toolButtonIcon"].toInt(m_sizes.toolButtonIcon);
    m_sizes.actionButtonPaddingH = sizes["actionButtonPaddingH"].toInt(m_sizes.actionButtonPaddingH);
    m_sizes.actionButtonPaddingV = sizes["actionButtonPaddingV"].toInt(m_sizes.actionButtonPaddingV);
    m_sizes.borderRadius = sizes["borderRadius"].toInt(m_sizes.borderRadius);
    m_sizes.panelMinWidth = sizes["panelMinWidth"].toInt(m_sizes.panelMinWidth);
    m_sizes.panelMaxWidth = sizes["panelMaxWidth"].toInt(m_sizes.panelMaxWidth);
    m_sizes.panelContentSpacing = sizes["panelContentSpacing"].toInt(m_sizes.panelContentSpacing);
    m_sizes.checkboxSize = sizes["checkboxSize"].toInt(m_sizes.checkboxSize);
    m_sizes.inputMinHeight = sizes["inputMinHeight"].toInt(m_sizes.inputMinHeight);

    QJsonObject font = root.value("font").toObject();
    m_font.family = font["family"].toString(m_font.family);
    m_font.size = font["size"].toInt(m_font.size);
    m_font.titleSize = font["titleSize"].toInt(m_font.titleSize);
    m_font.headerSize = font["headerSize"].toInt(m_font.headerSize);
    m_font.sectionSize = font["sectionSize"].toInt(m_font.sectionSize);
    m_font.bodySize = font["bodySize"].toInt(m_font.bodySize);
}

void ThemeConfig::loadLayoutJson()
{
    QString path = configDir() + "/layout.json";
    if (!QFile::exists(path)) {
        QJsonObject def;
        def["toolbarPosition"] = "top"; // top, bottom, floating
        def["panelPosition"] = "right"; // left, right, floating
        def["toolbarVisible"] = true;
        def["panelVisible"] = true;
        def["floatingToolbarWidth"] = 800;
        def["floatingToolbarMargin"] = 16;
        def["floatingPanelMargin"] = 24;

        QJsonArray tools;
        tools.append("select");
        tools.append("window");
        tools.append("workspace");
        tools.append("gaps");
        tools.append("borders");
        def["visibleTools"] = tools;
        def["defaultTool"] = "select";

        QJsonDocument doc(def);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
        }
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    m_layout = doc.object().toVariantMap();
}

void ThemeConfig::loadPropertiesJson()
{
    QString path = configDir() + "/properties.json";
    if (!QFile::exists(path)) {
        QJsonObject def;
        QJsonArray sections;
        sections.append("gaps");
        sections.append("borders");
        sections.append("appearance");
        sections.append("behavior");
        def["visibleSections"] = sections;

        QJsonObject defaults;
        defaults["gapSize"] = 8;
        defaults["borderWidth"] = 2;
        defaults["opacity"] = 1.0;
        defaults["blur"] = false;
        defaults["focusFollowsMouse"] = false;
        def["defaults"] = defaults;

        QJsonDocument doc(def);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
        }
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    m_properties = doc.object().toVariantMap();
}

QString ThemeConfig::toolbarStyleSheet(bool floating) const
{
    QString tpl = R"(
        Toolbar {
            background-color: %1;
            border: 1px solid %2;
        }
        QLabel {
            color: %3;
            font-size: %4px;
            font-weight: 600;
        }
        QPushButton {
            background-color: %5;
            color: %3;
            border: 1px solid %2;
            border-radius: %6px;
            padding: %7px %8px;
            font-size: %4px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: %9;
            border-color: %10;
        }
        QPushButton:pressed {
            background-color: #2E2E2E;
        }
        QPushButton#applyBtn {
            background-color: %11;
            color: %1;
            border: none;
            font-weight: 600;
        }
        QPushButton#applyBtn:hover {
            background-color: %12;
        }
        QPushButton#discardBtn {
            background-color: rgba(255, 95, 95, 0.08);
            color: %13;
            border: 1px solid rgba(255, 95, 95, 0.25);
        }
        QPushButton#discardBtn:hover {
            background-color: rgba(255, 95, 95, 0.15);
            border-color: rgba(255, 95, 95, 0.40);
        }
        QPushButton#closeBtn {
            background-color: transparent;
            border: none;
            color: %14;
            padding: 0px;
        }
        QPushButton#closeBtn:hover {
            color: %13;
            background-color: rgba(255, 95, 95, 0.12);
            border-radius: %6px;
        }
        QToolButton {
            background-color: %5;
            border: 1px solid %2;
            border-radius: %6px;
        }
        QToolButton:hover {
            background-color: %9;
            border-color: %10;
        }
        QToolButton:checked {
            background-color: rgba(98, 169, 227, 0.15);
            border-color: %11;
        }
    )";

    QString style = tpl
        .arg(m_colors.bg.name())
        .arg(m_colors.border.name())
        .arg(m_colors.text.name())
        .arg(m_font.bodySize)
        .arg(m_colors.surface.name())
        .arg(m_sizes.borderRadius)
        .arg(m_sizes.actionButtonPaddingV)
        .arg(m_sizes.actionButtonPaddingH)
        .arg(m_colors.surfaceHover.name())
        .arg(m_colors.borderHover.name())
        .arg(m_colors.accent.name())
        .arg(m_colors.accentHover.name())
        .arg(m_colors.danger.name())
        .arg(m_colors.textMuted.name());

    if (!floating) {
        style.replace(QString("border: 1px solid %1;").arg(m_colors.border.name()),
                      QString("border: none;\n            border-bottom: 1px solid %1;").arg(m_colors.border.name()));
    } else {
        style.replace(QString("background-color: %1;").arg(m_colors.bg.name()),
                      QString("background-color: %1;\n            border-radius: 12px;").arg(m_colors.bg.name()));
    }

    return style;
}

QString ThemeConfig::propertyPanelStyleSheet(bool left, bool right, bool floating) const
{
    QString tpl = R"(
        PropertyPanel {
            background-color: %1;
            border: none;
        }
        QLabel {
            color: %2;
            font-size: %3px;
        }
        QLabel#header {
            color: %4;
            font-size: %5px;
            font-weight: 700;
            padding: 4px 0;
        }
        QLabel#section {
            color: %6;
            font-size: %7px;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 1.5px;
            padding-top: 16px;
            padding-bottom: 6px;
        }
        QSpinBox, QDoubleSpinBox, QLineEdit, QComboBox {
            background-color: %8;
            color: %2;
            border: 1px solid %9;
            border-radius: %10px;
            padding: 6px 10px;
            font-size: %3px;
            min-height: %11px;
        }
        QSpinBox:focus, QDoubleSpinBox:focus, QLineEdit:focus, QComboBox:focus {
            border-color: %12;
        }
        QSpinBox::up-button, QSpinBox::down-button,
        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            width: 20px;
            border-radius: 4px;
        }
        QCheckBox {
            color: %2;
            font-size: %3px;
            spacing: 10px;
        }
        QCheckBox::indicator {
            width: %13px;
            height: %13px;
            border-radius: 5px;
            border: 1px solid %9;
            background-color: %8;
        }
        QCheckBox::indicator:checked {
            background-color: %4;
            border-color: %4;
        }
        QCheckBox::indicator:hover {
            border-color: %12;
        }
        QFrame[frameShape="4"] {
            color: %9;
            max-height: 1px;
        }
    )";

    QString style = tpl
        .arg(m_colors.bg.name())
        .arg(m_colors.text.name())
        .arg(m_font.bodySize)
        .arg(m_colors.accent.name())
        .arg(m_font.headerSize)
        .arg(m_colors.textMuted.name())
        .arg(m_font.sectionSize)
        .arg(m_colors.surface.name())
        .arg(m_colors.border.name())
        .arg(m_sizes.borderRadius)
        .arg(m_sizes.inputMinHeight)
        .arg(m_colors.borderFocus.name())
        .arg(m_sizes.checkboxSize);

    if (left) {
        style.replace("border: none;",
                      QString("border: none;\n            border-right: 1px solid %1;").arg(m_colors.border.name()));
    } else if (right) {
        style.replace("border: none;",
                      QString("border: none;\n            border-left: 1px solid %1;").arg(m_colors.border.name()));
    } else if (floating) {
        style.replace(QString("background-color: %1;").arg(m_colors.bg.name()),
                      QString("background-color: %1;\n            border-radius: 12px;\n            border: 1px solid %2;").arg(m_colors.bg.name()).arg(m_colors.border.name()));
    }

    return style;
}

QString ThemeConfig::baseStyleSheet() const
{
    return QString(R"(
        QWidget {
            font-family: %1;
            font-size: %2px;
        }
    )").arg(m_font.family).arg(m_font.size);
}

} // namespace keyoubs
