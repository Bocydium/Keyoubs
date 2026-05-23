#include "NiriCanvas.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

namespace keyoubs {

NiriCanvas::NiriCanvas(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(400, 300);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void NiriCanvas::setColumns(const QList<NiriColumn> &columns)
{
    m_columns = columns;
    update();
}

void NiriCanvas::addColumn(const NiriColumn &column)
{
    m_columns.append(column);
    update();
}

void NiriCanvas::removeColumn(int id)
{
    for (int i = 0; i < m_columns.size(); ++i) {
        if (m_columns[i].id == id) {
            m_columns.removeAt(i);
            update();
            return;
        }
    }
}

void NiriCanvas::setViewportOffset(int offset)
{
    m_viewportOffset = qMax(0, offset);
    update();
    emit viewportChanged(m_viewportOffset);
}

void NiriCanvas::setColumnWidth(int columnId, int width)
{
    for (auto &col : m_columns) {
        if (col.id == columnId) {
            col.preferredWidth = width;
            col.isFixedWidth = (width > 0);
            update();
            emit columnWidthChanged(columnId, width);
            return;
        }
    }
}

int NiriCanvas::columnXPosition(int columnIndex) const
{
    int x = m_gapSize;
    for (int i = 0; i < columnIndex && i < m_columns.size(); ++i) {
        int colWidth = m_columns[i].isFixedWidth
            ? m_columns[i].preferredWidth
            : 300; // default column width
        x += colWidth + m_columnSpacing;
    }
    return x - m_viewportOffset;
}

int NiriCanvas::totalStripWidth() const
{
    int width = m_gapSize;
    for (const auto &col : m_columns) {
        int colWidth = col.isFixedWidth ? col.preferredWidth : 300;
        width += colWidth + m_columnSpacing;
    }
    return width;
}

void NiriCanvas::drawViewportIndicator(QPainter &p)
{
    // Draw a semi-transparent overlay showing which part of the strip
    // is currently visible on the physical screen
    int vpX = m_gapSize;
    int vpWidth = qMin(m_viewportWidth, width() - m_gapSize * 2);

    p.setPen(QPen(QColor(0x62, 0xA9, 0xE3), 2, Qt::DashLine));
    p.setBrush(Qt::NoBrush);
    p.drawRect(vpX, m_gapSize, vpWidth, height() - m_gapSize * 2);

    // Label
    p.setPen(QColor(0x62, 0xA9, 0xE3));
    p.setFont(QFont("Inter", 8));
    p.drawText(vpX + 4, m_gapSize + 14, "Viewport");
}

void NiriCanvas::drawColumnHandles(QPainter &p, int columnIndex, const QRect &colRect)
{
    if (!m_showHandles) return;

    // Consume handle (right edge) — pull window from next column
    if (columnIndex < m_columns.size() - 1) {
        QRect consumeHandle(colRect.right() - 8, colRect.center().y() - 12, 16, 24);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x62, 0xA9, 0xE3));
        p.drawRoundedRect(consumeHandle, 4, 4);

        // Arrow icon
        p.setPen(QColor(0x10, 0x10, 0x10));
        p.drawText(consumeHandle, Qt::AlignCenter, "←");
    }

    // Expel handle (bottom edge) — push bottom window to new column
    if (!m_columns[columnIndex].windowRects.isEmpty()) {
        QRect expelHandle(colRect.center().x() - 12, colRect.bottom() - 8, 24, 16);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0xE3, 0xA9, 0x62));
        p.drawRoundedRect(expelHandle, 4, 4);

        p.setPen(QColor(0x10, 0x10, 0x10));
        p.drawText(expelHandle, Qt::AlignCenter, "↓");
    }

    // Width resize handle (right edge, full height)
    QRect resizeHandle(colRect.right() - 4, colRect.top(), 8, colRect.height());
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0x3A, 0x3A, 0x3A));
    p.drawRect(resizeHandle);
}

void NiriCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background
    p.fillRect(rect(), QColor(0x10, 0x10, 0x10));

    // Draw columns
    for (int i = 0; i < m_columns.size(); ++i) {
        int x = columnXPosition(i);
        int colWidth = m_columns[i].isFixedWidth
            ? m_columns[i].preferredWidth
            : 300;

        QRect colRect(x, m_gapSize, colWidth, height() - m_gapSize * 2);

        // Column background
        p.setPen(QPen(QColor(0x2A, 0x2A, 0x2A), 1));
        p.setBrush(QColor(0x1A, 0x1A, 0x1A));
        p.drawRoundedRect(colRect, 8, 8);

        // Windows in column
        int winY = colRect.top() + m_gapSize;
        for (const auto &winRect : m_columns[i].windowRects) {
            QRect win(x + m_gapSize, winY,
                      colWidth - m_gapSize * 2,
                      winRect.height());
            p.setPen(QPen(QColor(0x3A, 0x3A, 0x3A), 1));
            p.setBrush(QColor(0x22, 0x22, 0x22));
            p.drawRoundedRect(win, 6, 6);
            winY += winRect.height() + m_gapSize;
        }

        // Column label
        p.setPen(QColor(0x88, 0x88, 0x88));
        p.setFont(QFont("Inter", 9));
        p.drawText(colRect.adjusted(8, 4, -8, 0), Qt::AlignTop | Qt::AlignLeft,
                   QString("Column %1").arg(m_columns[i].id));

        // Handles
        drawColumnHandles(p, i, colRect);
    }

    // Viewport indicator
    drawViewportIndicator(p);

    // Scroll hint if content overflows
    int totalWidth = totalStripWidth();
    if (totalWidth > width()) {
        p.setPen(QColor(0x62, 0xA9, 0xE3));
        p.setFont(QFont("Inter", 8));
        QString hint = QString("← Scrollable: %1px →").arg(totalWidth);
        p.drawText(rect().adjusted(0, 0, 0, -4), Qt::AlignBottom | Qt::AlignHCenter, hint);
    }
}

void NiriCanvas::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x() + m_viewportOffset;

    // Find which column was clicked
    for (int i = 0; i < m_columns.size(); ++i) {
        int colX = columnXPosition(i) + m_viewportOffset;
        int colWidth = m_columns[i].isFixedWidth
            ? m_columns[i].preferredWidth
            : 300;

        if (x >= colX && x <= colX + colWidth) {
            // Check for handle clicks
            QRect colRect(colX - m_viewportOffset, m_gapSize, colWidth, height() - m_gapSize * 2);

            // Consume handle
            if (i < m_columns.size() - 1) {
                QRect consumeHandle(colRect.right() - 8, colRect.center().y() - 12, 16, 24);
                if (consumeHandle.contains(event->pos())) {
                    emit columnConsumeRequested(m_columns[i + 1].id);
                    return;
                }
            }

            // Expel handle
            if (!m_columns[i].windowRects.isEmpty()) {
                QRect expelHandle(colRect.center().x() - 12, colRect.bottom() - 8, 24, 16);
                if (expelHandle.contains(event->pos())) {
                    emit columnExpelRequested(m_columns[i].id);
                    return;
                }
            }

            // Width resize handle
            QRect resizeHandle(colRect.right() - 4, colRect.top(), 8, colRect.height());
            if (resizeHandle.contains(event->pos())) {
                m_draggingColumn = i;
                m_dragStartX = event->pos().x();
                return;
            }

            emit columnClicked(m_columns[i].id);
            return;
        }
    }
}

void NiriCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingColumn >= 0 && m_draggingColumn < m_columns.size()) {
        int delta = event->pos().x() - m_dragStartX;
        int newWidth = m_columns[m_draggingColumn].preferredWidth + delta;
        newWidth = qMax(100, newWidth);
        setColumnWidth(m_columns[m_draggingColumn].id, newWidth);
        m_dragStartX = event->pos().x();
    }
}

void NiriCanvas::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

} // namespace keyoubs
