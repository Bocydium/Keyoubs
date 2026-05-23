#include "PropertyPanel.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QFrame>
#include <QColorDialog>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QScreen>

namespace keyoubs {

PropertyPanel::PropertyPanel(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_headerLabel(new QLabel("Properties", this))
    , m_contentWidget(new QWidget(this))
    , m_contentLayout(new QVBoxLayout(m_contentWidget))
{
    setupUI();
    updateStyleForPosition();
}

void PropertyPanel::setupUI()
{
    setFixedWidth(260);

    m_layout->setContentsMargins(14, 12, 14, 12);
    m_layout->setSpacing(4);

    m_headerLabel->setObjectName("header");
    m_layout->addWidget(m_headerLabel);

    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(6);
    m_layout->addWidget(m_contentWidget);
    m_layout->addStretch();

    showGlobalProperties();
}

void PropertyPanel::updateStyleForPosition()
{
    QString baseStyle = R"(
        PropertyPanel {
            background-color: #121218;
            border: none;
        }
        QLabel {
            color: #C0C0D0;
            font-size: 12px;
        }
        QLabel#header {
            color: #62A9E3;
            font-size: 13px;
            font-weight: 600;
            padding: 8px 0;
        }
        QLabel#section {
            color: #8888A0;
            font-size: 10px;
            font-weight: 600;
            text-transform: uppercase;
            letter-spacing: 1px;
            padding-top: 12px;
            padding-bottom: 4px;
        }
        QSpinBox, QDoubleSpinBox, QLineEdit, QComboBox {
            background-color: #1E1E28;
            color: #E8E8F0;
            border: 1px solid rgba(98, 169, 227, 0.2);
            border-radius: 5px;
            padding: 4px 8px;
            font-size: 12px;
        }
        QSpinBox:focus, QDoubleSpinBox:focus, QLineEdit:focus, QComboBox:focus {
            border-color: rgba(98, 169, 227, 0.5);
        }
        QCheckBox {
            color: #C0C0D0;
            font-size: 12px;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 4px;
            border: 1px solid rgba(98, 169, 227, 0.3);
            background-color: #1E1E28;
        }
        QCheckBox::indicator:checked {
            background-color: #62A9E3;
            border-color: #62A9E3;
        }
    )";

    if (m_position == Position::Left) {
        baseStyle.replace("border: none;",
                          "border: none;\n            border-right: 1px solid rgba(98, 169, 227, 0.15);");
    } else if (m_position == Position::Right) {
        baseStyle.replace("border: none;",
                          "border: none;\n            border-left: 1px solid rgba(98, 169, 227, 0.15);");
    } else {
        // Floating: rounded corners
        baseStyle.replace("background-color: rgba(18, 18, 24, 0.92);",
                          "background-color: rgba(18, 18, 24, 0.92);\n            border-radius: 10px;\n            border: 1px solid rgba(98, 169, 227, 0.2);");
    }

    setStyleSheet(baseStyle);
}

void PropertyPanel::setPosition(Position pos)
{
    if (m_position == pos) return;
    m_position = pos;
    updateStyleForPosition();
    emit positionChanged(pos);
}

void PropertyPanel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->pos();
        m_dragStartGlobal = mapToGlobal(event->pos());
        emit dragStarted();
    }
    QWidget::mousePressEvent(event);
}

void PropertyPanel::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        QPoint globalPos = mapToGlobal(event->pos());
        move(pos() + (globalPos - m_dragStartGlobal));
        m_dragStartGlobal = globalPos;
    }
    QWidget::mouseMoveEvent(event);
}

void PropertyPanel::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging) {
        m_dragging = false;
        emit dragEnded();

        // Snap detection
        QRect geo = geometry();
        QScreen *screen = QApplication::primaryScreen();
        if (!screen) return;
        QRect screenGeo = screen->geometry();
        int snapThreshold = 60;

        if (geo.left() <= screenGeo.left() + snapThreshold) {
            setPosition(Position::Left);
        } else if (geo.right() >= screenGeo.right() - snapThreshold) {
            setPosition(Position::Right);
        } else {
            setPosition(Position::Floating);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void PropertyPanel::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Draw subtle drag handle indicator at top center
    QRect handle(width() / 2 - 12, 4, 24, 3);
    p.fillRect(handle, QColor(0x62, 0xA9, 0xE3, 80));

    QWidget::paintEvent(event);
}

void PropertyPanel::clearPanel()
{
    while (QLayoutItem *item = m_contentLayout->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
}

void PropertyPanel::showGlobalProperties()
{
    clearPanel();
    m_headerLabel->setText("Global Layout");

    // Section: Gaps
    auto *secGaps = new QLabel("Gaps", this);
    secGaps->setObjectName("section");
    m_contentLayout->addWidget(secGaps);

    auto *gapSpin = new QSpinBox(this);
    gapSpin->setRange(0, 64);
    gapSpin->setValue(8);
    gapSpin->setSuffix(" px");
    m_contentLayout->addWidget(gapSpin);
    connect(gapSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v) { emit propertyChanged("gapSize", v); });

    auto *gapTop = new QCheckBox("Top", this);
    auto *gapBot = new QCheckBox("Bottom", this);
    auto *gapL = new QCheckBox("Left", this);
    auto *gapR = new QCheckBox("Right", this);
    gapTop->setChecked(true); gapBot->setChecked(true);
    gapL->setChecked(true); gapR->setChecked(true);
    m_contentLayout->addWidget(gapTop);
    m_contentLayout->addWidget(gapBot);
    m_contentLayout->addWidget(gapL);
    m_contentLayout->addWidget(gapR);

    // Section: Borders
    auto *secBord = new QLabel("Borders", this);
    secBord->setObjectName("section");
    m_contentLayout->addWidget(secBord);

    auto *borderSpin = new QSpinBox(this);
    borderSpin->setRange(0, 10);
    borderSpin->setValue(2);
    borderSpin->setSuffix(" px");
    m_contentLayout->addWidget(borderSpin);

    auto *borderTop = new QCheckBox("Top", this);
    auto *borderBot = new QCheckBox("Bottom", this);
    auto *borderL = new QCheckBox("Left", this);
    auto *borderR = new QCheckBox("Right", this);
    borderTop->setChecked(true); borderBot->setChecked(true);
    borderL->setChecked(true); borderR->setChecked(true);
    m_contentLayout->addWidget(borderTop);
    m_contentLayout->addWidget(borderBot);
    m_contentLayout->addWidget(borderL);
    m_contentLayout->addWidget(borderR);

    // Section: Appearance
    auto *secApp = new QLabel("Appearance", this);
    secApp->setObjectName("section");
    m_contentLayout->addWidget(secApp);

    auto *opacitySpin = new QDoubleSpinBox(this);
    opacitySpin->setRange(0.1, 1.0);
    opacitySpin->setSingleStep(0.05);
    opacitySpin->setValue(1.0);
    opacitySpin->setSuffix(" opacity");
    m_contentLayout->addWidget(opacitySpin);

    auto *blurCheck = new QCheckBox("Background blur", this);
    m_contentLayout->addWidget(blurCheck);

    // Section: Behavior
    auto *secBeh = new QLabel("Behavior", this);
    secBeh->setObjectName("section");
    m_contentLayout->addWidget(secBeh);

    auto *ffmCheck = new QCheckBox("Focus follows mouse", this);
    m_contentLayout->addWidget(ffmCheck);

    m_contentLayout->addStretch();
}

void PropertyPanel::showWindowProperties(const QVariant &data)
{
    clearPanel();
    Q_UNUSED(data)
    m_headerLabel->setText("Window");
    // TODO: populate window-specific properties
    m_contentLayout->addStretch();
}

void PropertyPanel::showGapProperties()
{
    clearPanel();
    m_headerLabel->setText("Gap Boundary");
    showGlobalProperties();
}

void PropertyPanel::showBorderProperties()
{
    clearPanel();
    m_headerLabel->setText("Border Boundary");
    showGlobalProperties();
}

void PropertyPanel::onSelectionChanged(SelectionType type, const QVariant &data)
{
    if (m_currentType == type) return;
    m_currentType = type;

    switch (type) {
    case SelectionType::Global:
        showGlobalProperties();
        break;
    case SelectionType::Window:
        showWindowProperties(data);
        break;
    case SelectionType::Gap:
        showGapProperties();
        break;
    case SelectionType::Border:
        showBorderProperties();
        break;
    default:
        clearPanel();
        m_headerLabel->setText("Properties");
        m_contentLayout->addStretch();
        break;
    }
}

} // namespace keyoubs
