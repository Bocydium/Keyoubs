#include "Toolbar.hpp"
#include "PhosphorIcons.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QStyle>
#include <QFont>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QToolButton>
#include <QAction>

namespace keyoubs {

Toolbar::Toolbar(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QHBoxLayout(this))
    , m_titleLabel(new QLabel("Keyoubs", this))
    , m_compositorLabel(new QLabel("—", this))
    , m_applyBtn(new QPushButton("Apply", this))
    , m_discardBtn(new QPushButton("Discard", this))
    , m_closeBtn(new QPushButton("×", this))
{
    setupUI();
    updateStyleForPosition();
}

void Toolbar::setupUI()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumHeight(48);
    m_layout->setContentsMargins(16, 6, 16, 6);
    m_layout->setSpacing(10);

    // Title
    m_titleLabel->setStyleSheet("color: #62A9E3; font-size: 14px; font-weight: 700;");
    m_layout->addWidget(m_titleLabel);

    // Separator
    auto *sep = new QLabel("|", this);
    sep->setStyleSheet("color: rgba(98, 169, 227, 0.35); font-size: 14px;");
    m_layout->addWidget(sep);

    // Compositor indicator
    m_compositorLabel->setStyleSheet("color: #8888A0; font-size: 11px; font-weight: 500;");
    m_layout->addWidget(m_compositorLabel);

    m_layout->addStretch();

    // Tool buttons with Phosphor icons
    createToolButton("Select", PhosphorIcons::Cursor,        Tool::Select);
    createToolButton("Window", PhosphorIcons::AppWindow,      Tool::Window);
    createToolButton("Workspace", PhosphorIcons::SquaresFour, Tool::Workspace);
    createToolButton("Gaps", PhosphorIcons::ArrowsOutLineHorizontal, Tool::GapBoundary);
    createToolButton("Borders", PhosphorIcons::FrameCorners, Tool::BorderBoundary);

    m_layout->addStretch();

    // Apply / Discard / Close — with icons
    m_applyBtn->setObjectName("applyBtn");
    m_applyBtn->setIcon(PhosphorIcons::icon(PhosphorIcons::Check, QColor(0x0D, 0x0D, 0x12), 18));
    m_applyBtn->setIconSize(QSize(18, 18));
    m_applyBtn->setToolTip("Apply changes");

    m_discardBtn->setObjectName("discardBtn");
    m_discardBtn->setIcon(PhosphorIcons::icon(PhosphorIcons::Trash, QColor(0xFF, 0x5F, 0x5F), 18));
    m_discardBtn->setIconSize(QSize(18, 18));
    m_discardBtn->setToolTip("Discard changes");

    m_closeBtn->setObjectName("closeBtn");
    m_closeBtn->setIcon(PhosphorIcons::icon(PhosphorIcons::Cross, QColor(0x88, 0x88, 0xA0), 18));
    m_closeBtn->setIconSize(QSize(18, 18));
    m_closeBtn->setToolTip("Close");

    m_layout->addWidget(m_discardBtn);
    m_layout->addWidget(m_applyBtn);
    m_layout->addWidget(m_closeBtn);

    connect(m_applyBtn, &QPushButton::clicked, this, &Toolbar::applyClicked);
    connect(m_discardBtn, &QPushButton::clicked, this, &Toolbar::discardClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &Toolbar::closeClicked);
}

void Toolbar::updateStyleForPosition()
{
    QString baseStyle = R"(
        Toolbar {
            background-color: #121218;
            border: 1px solid rgba(98, 169, 227, 0.3);
        }
        QLabel {
            color: #E8E8F0;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton {
            background-color: rgba(98, 169, 227, 0.12);
            color: #62A9E3;
            border: 1px solid rgba(98, 169, 227, 0.25);
            border-radius: 6px;
            padding: 4px 14px;
            font-size: 12px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: rgba(98, 169, 227, 0.22);
            border-color: rgba(98, 169, 227, 0.45);
        }
        QPushButton:pressed {
            background-color: rgba(98, 169, 227, 0.30);
        }
        QPushButton#applyBtn {
            background-color: #62A9E3;
            color: #0D0D12;
            border: none;
        }
        QPushButton#applyBtn:hover {
            background-color: #7AB8E8;
        }
        QPushButton#closeBtn {
            background-color: #121218;
            border: none;
            color: #8888A0;
            font-size: 18px;
            padding: 0px 6px;
        }
        QPushButton#closeBtn:hover {
            color: #FF5F5F;
            background-color: rgba(255, 95, 95, 0.15);
            border-radius: 6px;
        }
    )";

    if (m_position == Position::Top || m_position == Position::Bottom) {
        baseStyle.replace("border: 1px solid rgba(98, 169, 227, 0.3);",
                          "border: none;\n            border-bottom: 1px solid rgba(98, 169, 227, 0.2);");
        setFixedHeight(42);
    } else {
        // Floating: rounded corners
        baseStyle.replace("background-color: rgba(18, 18, 24, 0.92);",
                          "background-color: rgba(18, 18, 24, 0.92);\n            border-radius: 10px;");
    }

    setStyleSheet(baseStyle);
}

void Toolbar::createToolButton(const QString &tooltip, const QString &iconName, Tool tool)
{
    auto *btn = new QToolButton(this);
    btn->setToolTip(tooltip);
    btn->setIcon(PhosphorIcons::icon(iconName, QColor(0x62, 0xA9, 0xE3), 20));
    btn->setIconSize(QSize(20, 20));
    btn->setCheckable(true);
    btn->setChecked(tool == m_activeTool);
    btn->setFixedSize(32, 32);
    m_layout->addWidget(btn);

    connect(btn, &QToolButton::clicked, this, [this, tool, btn]() {
        m_activeTool = tool;
        emit toolSelected(tool);
    });
}

void Toolbar::setCompositorName(const QString &name)
{
    m_compositorLabel->setText(name);
}

void Toolbar::setPosition(Position pos)
{
    if (m_position == pos) return;
    m_position = pos;
    updateStyleForPosition();
    emit positionChanged(pos);
}

void Toolbar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->pos();
        m_dragStartGlobal = mapToGlobal(event->pos());
        emit dragStarted();
    }
    QWidget::mousePressEvent(event);
}

void Toolbar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        QPoint globalPos = mapToGlobal(event->pos());
        move(pos() + (globalPos - m_dragStartGlobal));
        m_dragStartGlobal = globalPos;
    }
    QWidget::mouseMoveEvent(event);
}

void Toolbar::mouseReleaseEvent(QMouseEvent *event)
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

        if (geo.top() <= screenGeo.top() + snapThreshold) {
            setPosition(Position::Top);
        } else if (geo.bottom() >= screenGeo.bottom() - snapThreshold) {
            setPosition(Position::Bottom);
        } else {
            setPosition(Position::Floating);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void Toolbar::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Draw subtle drag handle indicator on the left
    QRect handle(6, height() / 2 - 6, 4, 12);
    p.fillRect(handle, QColor(0x62, 0xA9, 0xE3, 80));

    QWidget::paintEvent(event);
}

} // namespace keyoubs
