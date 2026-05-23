#pragma once

#include <QWidget>
#include <QColor>

namespace keyoubs {

// ─── Glow Decoration Editor ───
//
// Hyprland 0.55 added a `glow` window decoration type.
// This widget edits glow color (with opacity), radius,
// and which windows it applies to via window rules.

struct GlowSettings {
    QColor color = QColor(0x62, 0xA9, 0xE3);
    int radius = 12;
    double opacity = 0.6; // 0.0 - 1.0
    int passes = 2;
    QString windowMatcher; // app-id or window class
};

class GlowDecoration : public QWidget
{
    Q_OBJECT

public:
    explicit GlowDecoration(QWidget *parent = nullptr);

    void setSettings(const GlowSettings &settings);
    [[nodiscard]] GlowSettings settings() const { return m_settings; }

signals:
    void settingsChanged(const GlowSettings &settings);

private:
    GlowSettings m_settings;

    void setupUI();
    void onColorChanged();
    void onRadiusChanged(int value);
    void onOpacityChanged(double value);
    void onPassesChanged(int value);
    void onMatcherChanged(const QString &text);
};

} // namespace keyoubs
