#include "NiriBlurEditor.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDebug>

namespace keyoubs {

NiriBlurEditor::NiriBlurEditor(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setStyleSheet(R"(
        NiriBlurEditor {
            background-color: #1A1A1A;
            border: 1px solid #2A2A2A;
            border-radius: 8px;
        }
        QLabel {
            color: #CCCCCC;
            font-size: 11px;
        }
        QComboBox {
            background-color: #2A2A2A;
            color: #CCCCCC;
            border: 1px solid #3A3A3A;
            border-radius: 4px;
            padding: 4px 8px;
            min-width: 120px;
        }
        QComboBox::drop-down {
            border: none;
        }
        QSlider::groove:horizontal {
            height: 4px;
            background: #2A2A2A;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            width: 14px;
            height: 14px;
            background: #62A9E3;
            border-radius: 7px;
            margin: -5px 0;
        }
        QSlider::sub-page:horizontal {
            background: #62A9E3;
            border-radius: 2px;
        }
    )");
}

void NiriBlurEditor::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Title
    auto *title = new QLabel("Background Blur", this);
    QFont titleFont = title->font();
    titleFont.setWeight(QFont::Bold);
    titleFont.setPointSize(12);
    title->setFont(titleFont);
    title->setStyleSheet("color: #FFFFFF;");
    layout->addWidget(title);

    // Type selector
    auto *typeLayout = new QHBoxLayout();
    auto *typeLabel = new QLabel("Type:", this);
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItem("None", static_cast<int>(NiriBlurType::None));
    m_typeCombo->addItem("Normal (Gaussian)", static_cast<int>(NiriBlurType::Normal));
    m_typeCombo->addItem("Xray (Wallpaper only)", static_cast<int>(NiriBlurType::Xray));
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(m_typeCombo);
    typeLayout->addStretch();
    layout->addLayout(typeLayout);

    // Radius slider
    auto *radiusLayout = new QHBoxLayout();
    auto *radiusTextLabel = new QLabel("Radius:", this);
    m_radiusSlider = new QSlider(Qt::Horizontal, this);
    m_radiusSlider->setRange(0, 64);
    m_radiusSlider->setValue(8);
    m_radiusLabel = new QLabel("8px", this);
    m_radiusLabel->setMinimumWidth(40);
    radiusLayout->addWidget(radiusTextLabel);
    radiusLayout->addWidget(m_radiusSlider);
    radiusLayout->addWidget(m_radiusLabel);
    layout->addLayout(radiusLayout);

    // Brightness slider
    auto *brightnessLayout = new QHBoxLayout();
    auto *brightnessTextLabel = new QLabel("Brightness:", this);
    m_brightnessSlider = new QSlider(Qt::Horizontal, this);
    m_brightnessSlider->setRange(0, 200);
    m_brightnessSlider->setValue(100);
    m_brightnessLabel = new QLabel("100%", this);
    m_brightnessLabel->setMinimumWidth(40);
    brightnessLayout->addWidget(brightnessTextLabel);
    brightnessLayout->addWidget(m_brightnessSlider);
    brightnessLayout->addWidget(m_brightnessLabel);
    layout->addLayout(brightnessLayout);

    // Connections
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NiriBlurEditor::onTypeChanged);
    connect(m_radiusSlider, &QSlider::valueChanged,
            this, &NiriBlurEditor::onRadiusChanged);
    connect(m_brightnessSlider, &QSlider::valueChanged,
            this, &NiriBlurEditor::onBrightnessChanged);

    layout->addStretch();
}

void NiriBlurEditor::setSettings(const NiriBlurSettings &settings)
{
    m_settings = settings;
    m_typeCombo->setCurrentIndex(static_cast<int>(settings.type));
    m_radiusSlider->setValue(settings.radius);
    m_brightnessSlider->setValue(settings.brightness);
    m_radiusLabel->setText(QString("%1px").arg(settings.radius));
    m_brightnessLabel->setText(QString("%1%").arg(settings.brightness));
}

void NiriBlurEditor::onTypeChanged(int index)
{
    m_settings.type = static_cast<NiriBlurType>(index);
    emit settingsChanged(m_settings);
}

void NiriBlurEditor::onRadiusChanged(int value)
{
    m_settings.radius = value;
    m_radiusLabel->setText(QString("%1px").arg(value));
    emit settingsChanged(m_settings);
}

void NiriBlurEditor::onBrightnessChanged(int value)
{
    m_settings.brightness = value;
    m_brightnessLabel->setText(QString("%1%").arg(value));
    emit settingsChanged(m_settings);
}

} // namespace keyoubs
