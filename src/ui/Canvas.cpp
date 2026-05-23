#include "Canvas.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>

namespace keyoubs {

Canvas::Canvas(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    updateDemoElements();
}

void Canvas::updateDemoElements()
{
    m_elements.clear();

    // Demo window elements
    CanvasElement w1;
    w1.rect = QRect(120, 100, 300, 200);
    w1.title = "Alacritty";
    w1.focused = true;
    w1.id = 1;
    m_elements.append(w1);

    CanvasElement w2;
    w2.rect = QRect(120 + 300 + m_gapSize, 100, 300, 200);
    w2.title = "Firefox";
    w2.focused = false;
    w2.id = 2;
    m_elements.append(w2);

    CanvasElement w3;
    w3.rect = QRect(120, 100 + 200 + m_gapSize, 300, 200);
    w3.title = "nvim";
    w3.focused = false;
    w3.id = 3;
    m_elements.append(w3);
}

void Canvas::setActiveTool(Toolbar::Tool tool)
{
    m_activeTool = tool;
    update();
}

void Canvas::onPropertyChanged(const QString &key, const QVariant &value)
{
    if (key == "gapSize") {
        m_gapSize = value.toInt();
        updateDemoElements();
    } else if (key == "borderWidth") {
        m_borderWidth = value.toInt();
    } else if (key == "borderColor") {
        m_borderColor = value.value<QColor>();
    }
    update();
    emit propertyPreviewRequested(key, value);
}

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Solid dark background
    p.fillRect(rect(), QColor(0x0D, 0x0D, 0x12));

    // Subtle dark dim overlay — monochrome, not blue-tinted
    if (m_activeTool == Toolbar::Tool::Select) {
        p.fillRect(rect(), QColor(0, 0, 0, 25));
    }

    drawGrid(p);
    drawGaps(p);
    drawBorders(p);
    drawWorkspaces(p);
    drawWindows(p);
    drawSelectionHighlight(p);

    // Tool cursor indicator — subtle blue accent
    if (!m_dragging) {
        QPen pen(QColor(0x62, 0xA9, 0xE3, 100));
        pen.setWidth(1);
        pen.setStyle(Qt::DashLine);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(m_lastMousePos, 8, 8);
    }
}

void Canvas::drawGrid(QPainter &p)
{
    const int step = 32;
    // Very subtle grid — monochrome gray
    QPen pen(QColor(0x44, 0x44, 0x44, 25));
    pen.setWidth(1);
    p.setPen(pen);

    for (int x = 0; x < width(); x += step) {
        p.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += step) {
        p.drawLine(0, y, width(), y);
    }
}

void Canvas::drawWindows(QPainter &p)
{
    if (!m_windowsData.isEmpty()) {
        // Real IPC data
        for (const auto &winVar : m_windowsData) {
            QVariantMap win = winVar.toMap();
            QRect rect(
                win.value("x", 120).toInt(),
                win.value("y", 100).toInt(),
                win.value("width", 300).toInt(),
                win.value("height", 200).toInt()
            );

            bool isFocused = win.value("focused", false).toBool();
            QColor bg = isFocused ? QColor(0x1E, 0x1E, 0x28, 220) : QColor(0x18, 0x18, 0x22, 180);
            p.fillRect(rect, bg);

            QPen pen(m_borderColor);
            pen.setWidth(m_borderWidth);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.drawRect(rect.adjusted(-m_borderWidth/2, -m_borderWidth/2, m_borderWidth/2, m_borderWidth/2));

            QRect titleBar(rect.left(), rect.top(), rect.width(), 28);
            p.fillRect(titleBar, isFocused ? QColor(0x62, 0xA9, 0xE3, 140) : QColor(0x33, 0x33, 0x3D, 140));

            p.setPen(m_textPrimary);
            QString title = win.value("title", "Window").toString();
            p.drawText(titleBar.adjusted(10, 0, 0, 0), Qt::AlignVCenter, "  " + title);
        }
        return;
    }

    // Demo elements
    for (const auto &elem : m_elements) {
        QColor bg = elem.focused ? QColor(0x1E, 0x1E, 0x28, 220) : QColor(0x18, 0x18, 0x22, 180);
        p.fillRect(elem.rect, bg);

        QPen pen(elem.selected ? QColor(0x62, 0xA9, 0xE3) : m_borderColor);
        pen.setWidth(m_borderWidth);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawRect(elem.rect.adjusted(-m_borderWidth/2, -m_borderWidth/2, m_borderWidth/2, m_borderWidth/2));

        QRect titleBar(elem.rect.left(), elem.rect.top(), elem.rect.width(), 28);
        p.fillRect(titleBar, elem.focused ? QColor(0x62, 0xA9, 0xE3, 140) : QColor(0x33, 0x33, 0x3D, 140));

        p.setPen(m_textPrimary);
        p.drawText(titleBar.adjusted(10, 0, 0, 0), Qt::AlignVCenter, "  " + elem.title);
    }
}

void Canvas::drawWorkspaces(QPainter &p)
{
    // Workspace indicator strip on left — dark monochrome
    QRect strip(12, 80, 40, 200);
    p.fillRect(strip, QColor(0x14, 0x14, 0x1A, 230));

    // Active workspace — subtle blue accent
    QRect active(12, 80 + 8, 40, 32);
    p.fillRect(active, QColor(0x62, 0xA9, 0xE3, 180));

    p.setPen(m_textPrimary);
    p.drawText(active, Qt::AlignCenter, "1");

    // Inactive workspaces
    for (int i = 1; i < 4; ++i) {
        QRect ws(12, 80 + 8 + i * 40, 40, 32);
        p.fillRect(ws, QColor(0x28, 0x28, 0x32, 160));
        p.setPen(m_textSecondary);
        p.drawText(ws, Qt::AlignCenter, QString::number(i + 1));
    }
}

void Canvas::drawGaps(QPainter &p)
{
    if (m_activeTool != Toolbar::Tool::GapBoundary) return;

    // Highlight gaps with the brand color
    QPen pen(QColor(0x62, 0xA9, 0xE3, 200));
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    // Gap between windows
    QRect gap1(420, 100, m_gapSize, 200);
    QRect gap2(120, 300 + m_gapSize, 300, m_gapSize);

    p.drawRect(gap1.adjusted(-2, -2, 2, 2));
    p.drawRect(gap2.adjusted(-2, -2, 2, 2));
}

void Canvas::drawBorders(QPainter &p)
{
    if (m_activeTool != Toolbar::Tool::BorderBoundary) return;

    // Highlight borders — subtle red for visibility
    QPen pen(QColor(0xFF, 0x5F, 0x5F, 200));
    pen.setWidth(3);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    for (const auto &elem : m_elements) {
        p.drawRect(elem.rect.adjusted(-4, -4, 4, 4));
    }
}

void Canvas::drawSelectionHighlight(QPainter &p)
{
    if (!m_selectedElement) return;

    QPen pen(QColor(0x62, 0xA9, 0xE3, 220));
    pen.setWidth(2);
    pen.setStyle(Qt::DotLine);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(m_selectedElement->rect.adjusted(-6, -6, 6, 6));
}

CanvasElement *Canvas::hitTest(const QPoint &pos)
{
    for (auto &elem : m_elements) {
        if (elem.rect.contains(pos)) {
            return &elem;
        }
    }
    return nullptr;
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    m_dragging = true;
    m_dragStart = event->pos();
    m_lastMousePos = event->pos();

    if (m_activeTool == Toolbar::Tool::Select) {
        CanvasElement *hit = hitTest(event->pos());
        if (hit) {
            if (m_selectedElement) m_selectedElement->selected = false;
            m_selectedElement = hit;
            m_selectedElement->selected = true;
            m_draggingElement = true;
            m_elementDragOffset = event->pos() - hit->rect.topLeft();
            emit selectionChanged(PropertyPanel::SelectionType::Window, QVariant::fromValue(hit->id));
        } else {
            if (m_selectedElement) m_selectedElement->selected = false;
            m_selectedElement = nullptr;
            emit selectionChanged(PropertyPanel::SelectionType::Global, QVariant());
        }
    } else if (m_activeTool == Toolbar::Tool::GapBoundary) {
        emit selectionChanged(PropertyPanel::SelectionType::Gap, QVariant());
    } else if (m_activeTool == Toolbar::Tool::BorderBoundary) {
        emit selectionChanged(PropertyPanel::SelectionType::Border, QVariant());
    }

    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    m_lastMousePos = event->pos();

    if (m_dragging && m_draggingElement && m_selectedElement) {
        QPoint newPos = event->pos() - m_elementDragOffset;
        m_selectedElement->rect.moveTo(newPos);
        emit propertyPreviewRequested("windowPosition", QVariant::fromValue(newPos));
    } else if (m_dragging && m_activeTool == Toolbar::Tool::GapBoundary) {
        int dx = event->pos().x() - m_dragStart.x();
        int newGap = qMax(0, m_gapSize + dx / 10);
        if (newGap != m_gapSize) {
            m_gapSize = newGap;
            updateDemoElements();
            emit propertyPreviewRequested("gapSize", m_gapSize);
        }
    }

    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_dragging = false;
    m_draggingElement = false;
    update();
}

void Canvas::setWindows(const QVariantList &windows)
{
    m_windowsData = windows;
    update();
}

void Canvas::setWorkspaces(const QVariantList &workspaces)
{
    m_workspaces = workspaces;
    update();
}

void Canvas::setOutputs(const QVariantList &outputs)
{
    m_outputs = outputs;
    update();
}

} // namespace keyoubs
