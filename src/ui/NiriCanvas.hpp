#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QList>
#include <QRect>

namespace keyoubs {

// ─── Niri Infinite Horizontal Canvas ───
//
// Niri's layout model has windows in columns on an infinite horizontal strip.
// This canvas represents that strip as a horizontally-scrollable view.
// Columns are visible as vertical groups of windows.

struct NiriColumn {
    int id = 0;
    QList<QRect> windowRects;
    int preferredWidth = -1; // -1 = auto
    bool isFixedWidth = false;
};

class NiriCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit NiriCanvas(QWidget *parent = nullptr);

    void setColumns(const QList<NiriColumn> &columns);
    void addColumn(const NiriColumn &column);
    void removeColumn(int id);

    void setViewportWidth(int width) { m_viewportWidth = width; update(); }
    void setViewportOffset(int offset);

    // Column consume/expel handles
    void setShowHandles(bool show) { m_showHandles = show; update(); }

    // Column width editing
    void setColumnWidth(int columnId, int width);

signals:
    void columnClicked(int columnId);
    void columnConsumeRequested(int fromColumnId); // pull from right
    void columnExpelRequested(int columnId);       // push bottom window out
    void columnWidthChanged(int columnId, int width);
    void viewportChanged(int offset);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QList<NiriColumn> m_columns;
    int m_viewportWidth = 1920;
    int m_viewportOffset = 0;
    int m_columnSpacing = 8;
    int m_gapSize = 8;
    bool m_showHandles = true;
    int m_draggingColumn = -1;
    int m_dragStartX = 0;

    int columnXPosition(int columnIndex) const;
    int totalStripWidth() const;
    void drawViewportIndicator(QPainter &p);
    void drawColumnHandles(QPainter &p, int columnIndex, const QRect &colRect);
};

} // namespace keyoubs
