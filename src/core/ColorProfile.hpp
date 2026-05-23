#pragma once

#include <QObject>
#include <QString>
#include <QColor>

namespace keyoubs {

// ─── Color Profile Selector ───
//
// Per-monitor color profile management.
// Supports sRGB, DCI-P3, Adobe RGB, dp3.
// ICC profile file picker for Hyprland 0.55+ and future Niri.
// HDR toggle with peak luminance and SDR boost sliders.
// FP16 rendering toggle.

enum class ColorSpace {
    sRGB,
    DCIP3,
    AdobeRGB,
    DisplayP3
};

struct ColorProfileSettings {
    ColorSpace space = ColorSpace::sRGB;
    QString iccFilePath;     // empty = use named space
    bool hdrEnabled = false;
    int peakLuminance = 400; // nits
    int sdrBoost = 100;      // percentage
    bool fp16Rendering = false;
};

class ColorProfile : public QObject
{
    Q_OBJECT

public:
    explicit ColorProfile(QObject *parent = nullptr);

    void setSettings(const ColorProfileSettings &settings);
    [[nodiscard]] ColorProfileSettings settings() const { return m_settings; }

    // Convenience helpers
    [[nodiscard]] QString colorSpaceName() const;
    [[nodiscard]] static QStringList availableColorSpaces();
    [[nodiscard]] static bool isValidIccFile(const QString &path);

    // Export to compositor config format
    [[nodiscard]] QString toHyprlandConfig(const QString &monitorName) const;

signals:
    void settingsChanged(const ColorProfileSettings &settings);

private:
    ColorProfileSettings m_settings;
};

} // namespace keyoubs
