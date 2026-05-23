#include "GlowDecoration.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>

namespace keyoubs {

GlowDecoration::GlowDecoration(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void GlowDecoration::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Title
    auto *title = new QLabel("Glow Decoration", this);
    QFont titleFont = title->font();
    titleFont.setWeight(QFont::Bold);
    titleFont.setPointSize(12);
    title->setFont(titleFont);
    title->setStyleSheet("color: #FFFFFF;");
    layout->addWidget(title);

    // Color picker (simplified - text input)
    auto *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel("Color:", this));
    auto *colorInput = new QLineEdit(this);
    colorInput->setText(m_settings.color.name());
    colorInput->setStyleSheet("background: #2A2A2A; color: #CCCCCC; border: 1px solid #3A3A3A; border-radius: 4px; padding: 4px;");
    colorLayout->addWidget(colorInput);
    layout->addLayout(colorLayout);
    connect(colorInput, &QLineEdit::textChanged, this, &GlowDecoration::onColorChanged);

    // Radius slider
    auto *radiusLayout = new QHBoxLayout();
    radiusLayout->addWidget(new QLabel("Radius:", this));
    auto *radiusSlider = new QSlider(Qt::Horizontal, this);
    radiusSlider->setRange(0, 64);
    radiusSlider->setValue(m_settings.radius);
    radiusLayout->addWidget(radiusSlider);
    auto *radiusLabel = new QLabel(QString::number(m_settings.radius), this);
    radiusLayout->addWidget(radiusLabel);
    layout->addLayout(radiusLayout);
    connect(radiusSlider, &QSlider::valueChanged, this, [this, radiusLabel](int val) {
        radiusLabel->setText(QString::number(val));
        onRadiusChanged(val);
    });

    // Opacity slider
    auto *opacityLayout = new QHBoxLayout();
    opacityLayout->addWidget(new QLabel("Opacity:", this));
    auto *opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(0, 100);
    opacitySlider->setValue(static_cast<int>(m_settings.opacity * 100));
    opacityLayout->addWidget(opacitySlider);
    auto *opacityLabel = new QLabel(QString("%1%").arg(m_settings.opacity * 100), this);
    opacityLayout->addWidget(opacityLabel);
    layout->addLayout(opacityLayout);
    connect(opacitySlider, &QSlider::valueChanged, this, [this, opacityLabel](int val) {
        opacityLabel->setText(QString("%1%").arg(val));
        onOpacityChanged(val / 100.0);
    });

    // Passes slider
    auto *passesLayout = new QHBoxLayout();
    passesLayout->addWidget(new QLabel("Passes:", this));
    auto *passesSlider = new QSlider(Qt::Horizontal, this);
    passesSlider->setRange(1, 8);
    passesSlider->setValue(m_settings.passes);
    passesLayout->addWidget(passesSlider);
    auto *passesLabel = new QLabel(QString::number(m_settings.passes), this);
    passesLayout->addWidget(passesLabel);
    layout->addLayout(passesLayout);
    connect(passesSlider, &QSlider::valueChanged, this, [this, passesLabel](int val) {
        passesLabel->setText(QString::number(val));
        onPassesChanged(val);
    });

    // Window matcher
    auto *matcherLayout = new QHBoxLayout();
    matcherLayout->addWidget(new QLabel("Match:", this));
    auto *matcherInput = new QLineEdit(this);
    matcherInput->setPlaceholderText("app-id or class");
    matcherInput->setText(m_settings.windowMatcher);
    matcherInput->setStyleSheet("background: #2A2A2A; color: #CCCCCC; border: 1px solid #3A3A3A; border-radius: 4px; padding: 4px;");
    matcherLayout->addWidget(matcherInput);
    layout->addLayout(matcherLayout);
    connect(matcherInput, &QLineEdit::textChanged, this, &GlowDecoration::onMatcherChanged);

    // Config export
    auto *exportBtn = new QPushButton("Copy config snippet", this);
    exportBtn->setStyleSheet("background: #62A9E3; color: #101010; border: none; border-radius: 4px; padding: 6px 12px;");
    layout->addWidget(exportBtn);
    connect(exportBtn, &QPushButton::clicked, this, [this]() {
        QString snippet = QString("decoration:glow {\n"
                                  "    color = %1\n"
                                  "    radius = %2px\n"
                                  "    passes = %3\n"
                                  "}\n"
                                  "windowrulev2 = glow, %4\n")
                              .arg(m_settings.color.name())
                              .arg(m_settings.radius)
                              .arg(m_settings.passes)
                              .arg(m_settings.windowMatcher);
        // In real implementation, copy to clipboard
    });

    layout->addStretch();

    setStyleSheet(R"(
        GlowDecoration {
            background-color: #1A1A1A;
            border: 1px solid #2A2A2A;
            border-radius: 8px;
        }
        QLabel {
            color: #CCCCCC;
            font-size: 11px;
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

void GlowDecoration::setSettings(const GlowSettings &settings)
{
    m_settings = settings;
}

void GlowDecoration::onColorChanged()
{
    auto *input = qobject_cast<QLineEdit*>(sender());
    if (input) m_settings.color = QColor(input->text());
    emit settingsChanged(m_settings);
}

void GlowDecoration::onRadiusChanged(int value)
{
    m_settings.radius = value;
    emit settingsChanged(m_settings);
}

void GlowDecoration::onOpacityChanged(double value)
{
    m_settings.opacity = value;
    emit settingsChanged(m_settings);
}

void GlowDecoration::onPassesChanged(int value)
{
    m_settings.passes = value;
    emit settingsChanged(m_settings);
}

void GlowDecoration::onMatcherChanged(const QString &text)
{
    m_settings.windowMatcher = text;
    emit settingsChanged(m_settings);
}

} // namespace keyoubs
