#include "ColorProfile.hpp"
#include <QFile>
#include <QDebug>

namespace keyoubs {

ColorProfile::ColorProfile(QObject *parent)
    : QObject(parent)
{
}

void ColorProfile::setSettings(const ColorProfileSettings &settings)
{
    m_settings = settings;
    emit settingsChanged(settings);
}

QString ColorProfile::colorSpaceName() const
{
    switch (m_settings.space) {
    case ColorSpace::sRGB: return "sRGB";
    case ColorSpace::DCIP3: return "DCI-P3";
    case ColorSpace::AdobeRGB: return "Adobe RGB";
    case ColorSpace::DisplayP3: return "Display P3";
    }
    return "sRGB";
}

QStringList ColorProfile::availableColorSpaces()
{
    return {"sRGB", "DCI-P3", "Adobe RGB", "Display P3"};
}

bool ColorProfile::isValidIccFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    // ICC profile magic: first 4 bytes should be profile size
    // Simplified check: file must be at least 128 bytes
    return file.size() >= 128;
}

QString ColorProfile::toHyprlandConfig(const QString &monitorName) const
{
    QString out;
    out += "# Color profile for " + monitorName + "\n";

    if (!m_settings.iccFilePath.isEmpty()) {
        out += QString("monitor = %1, icc_profile, %2\n")
                   .arg(monitorName)
                   .arg(m_settings.iccFilePath);
    } else {
        out += QString("monitor = %1, color_space, %2\n")
                   .arg(monitorName)
                   .arg(colorSpaceName().toLower());
    }

    if (m_settings.hdrEnabled) {
        out += QString("monitor = %1, hdr, true\n").arg(monitorName);
        out += QString("monitor = %1, peak_luminance, %2\n")
                   .arg(monitorName)
                   .arg(m_settings.peakLuminance);
        out += QString("monitor = %1, sdr_boost, %2\n")
                   .arg(monitorName)
                   .arg(m_settings.sdrBoost);
    }

    if (m_settings.fp16Rendering) {
        out += "# FP16 color pipeline\n";
        out += "misc:fp16 = true\n";
    }

    return out;
}

} // namespace keyoubs
