#pragma once

#include <QWidget>
#include <QComboBox>
#include <QSlider>
#include <QLabel>

namespace keyoubs {

// ─── Niri Blur Editor ───
//
// Niri 26.04 added blur support via ext-background-effect Wayland protocol.
// Both normal (gaussian-style) and xray blur (shows only wallpaper).
// This editor is per layer-shell surface.

enum class NiriBlurType {
    None = 0,
    Normal = 1,  // Gaussian-style blur
    Xray = 2     // Shows wallpaper only, not windows behind
};

struct NiriBlurSettings {
    NiriBlurType type = NiriBlurType::None;
    int radius = 8;        // Blur radius
    int brightness = 100;  // 0-200%, 100 = normal
};

class NiriBlurEditor : public QWidget
{
    Q_OBJECT

public:
    explicit NiriBlurEditor(QWidget *parent = nullptr);

    void setSettings(const NiriBlurSettings &settings);
    [[nodiscard]] NiriBlurSettings settings() const { return m_settings; }

signals:
    void settingsChanged(const NiriBlurSettings &settings);

private:
    NiriBlurSettings m_settings;
    QComboBox *m_typeCombo = nullptr;
    QSlider *m_radiusSlider = nullptr;
    QSlider *m_brightnessSlider = nullptr;
    QLabel *m_radiusLabel = nullptr;
    QLabel *m_brightnessLabel = nullptr;

    void setupUI();
    void onTypeChanged(int index);
    void onRadiusChanged(int value);
    void onBrightnessChanged(int value);
};

} // namespace keyoubs
