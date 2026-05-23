#include "Dock.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

namespace keyoubs {

// ─── DockItem ───

DockItem::DockItem(const QString &appId, const QIcon &icon, QWidget *parent)
    : QWidget(parent)
    , m_appId(appId)
    , m_icon(icon)
{
    setFixedSize(m_iconSize + 16, m_iconSize + 16);
    setCursor(Qt::PointingHandCursor);
}

void DockItem::setRunning(bool running)
{
    if (m_running == running) return;
    m_running = running;
    update();
}

void DockItem::setIcon(const QIcon &icon)
{
    m_icon = icon;
    update();
}

void DockItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect iconRect(8, 8, m_iconSize, m_iconSize);

    // Hover background
    if (m_hovered) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x22, 0x22, 0x22));
        p.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 10, 10);
    }

    // Draw icon
    if (!m_icon.isNull()) {
        m_icon.paint(&p, iconRect);
    } else {
        p.setPen(QColor(0x88, 0x88, 0x88));
        p.drawRoundedRect(iconRect, 8, 8);
    }

    // Running indicator dot
    if (m_running) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x62, 0xA9, 0xE3));
        int dotSize = 4;
        int dotX = (width() - dotSize) / 2;
        int dotY = height() - 6;
        p.drawEllipse(dotX, dotY, dotSize, dotSize);
    }
}

void DockItem::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked(m_appId);
}

void DockItem::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    m_hovered = true;
    update();
}

void DockItem::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_hovered = false;
    update();
}

// ─── Dock ───

Dock::Dock(QWidget *parent)
    : QWidget(parent)
{
    setOrientation(m_orientation);

    setStyleSheet(R"(
        Dock {
            background-color: #101010;
            border: 1px solid #2A2A2A;
            border-radius: 16px;
        }
    )");
}

void Dock::addItem(const QString &appId, const QIcon &icon)
{
    for (auto *item : m_items) {
        if (item->appId() == appId) return;
    }

    auto *item = new DockItem(appId, icon, this);
    m_items.append(item);
    m_layout->addWidget(item);

    connect(item, &DockItem::clicked, this, &Dock::itemClicked);
}

void Dock::removeItem(const QString &appId)
{
    for (auto *item : m_items) {
        if (item->appId() == appId) {
            m_items.removeAll(item);
            m_layout->removeWidget(item);
            delete item;
            return;
        }
    }
}

void Dock::setItemRunning(const QString &appId, bool running)
{
    for (auto *item : m_items) {
        if (item->appId() == appId) {
            item->setRunning(running);
            return;
        }
    }
}

void Dock::clearItems()
{
    for (auto *item : m_items) {
        m_layout->removeWidget(item);
        delete item;
    }
    m_items.clear();
}

void Dock::setIconSize(int size)
{
    m_iconSize = size;
    for (auto *item : m_items) {
        item->setFixedSize(size + 16, size + 16);
    }
}

void Dock::setOrientation(Qt::Orientation orientation)
{
    m_orientation = orientation;

    delete m_layout;
    if (orientation == Qt::Horizontal) {
        m_layout = new QHBoxLayout(this);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    } else {
        m_layout = new QVBoxLayout(this);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(4);
    m_layout->setAlignment(Qt::AlignCenter);
}

} // namespace keyoubs
