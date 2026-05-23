#pragma once

#include "Toolbar.hpp"
#include "PropertyPanel.hpp"
#include <QWidget>
#include <QVariantList>
#include <QVector>

QT_BEGIN_NAMESPACE
class QPainter;
class QMouseEvent;
QT_END_NAMESPACE

namespace keyoubs {

// The main interactive canvas.
// Draws the desktop layout overlay with windows, workspaces, gaps, borders.
// Handles selection, dragging of elements, and live preview visualization.
struct CanvasElement {
    QRect rect;
    QString title;
    bool focused = false;
    bool selected = false;
    int id = -1;
};

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);

    void setActiveTool(Toolbar::Tool tool);

    void setWindows(const QVariantList &windows);
    void setWorkspaces(const QVariantList &workspaces);
    void setOutputs(const QVariantList &outputs);

signals:
    void selectionChanged(PropertyPanel::SelectionType type, const QVariant &data);
    void propertyPreviewRequested(const QString &key, const QVariant &value);
    void propertyCommitRequested(const QString &key, const QVariant &value);

public slots:
    void onPropertyChanged(const QString &key, const QVariant &value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void drawGrid(QPainter &p);
    void drawWindows(QPainter &p);
    void drawWorkspaces(QPainter &p);
    void drawGaps(QPainter &p);
    void drawBorders(QPainter &p);
    void drawSelectionHighlight(QPainter &p);

    CanvasElement *hitTest(const QPoint &pos);
    void updateDemoElements();

    Toolbar::Tool m_activeTool = Toolbar::Tool::Select;
    bool m_dragging = false;
    bool m_draggingElement = false;
    QPoint m_dragStart;
    QPoint m_lastMousePos;
    QPoint m_elementDragOffset;

    // Visual properties
    int m_gapSize = 8;
    int m_borderWidth = 2;

    // IPC data
    QVariantList m_windowsData;
    QVariantList m_workspaces;
    QVariantList m_outputs;

    // Canvas elements (demo + real)
    QVector<CanvasElement> m_elements;
    CanvasElement *m_selectedElement = nullptr;

    QColor m_borderColor = QColor(0x62, 0xA9, 0xE3);
    QColor m_bgDark = QColor(0x0D, 0x0D, 0x12);
    QColor m_surfaceDark = QColor(0x18, 0x18, 0x22);
    QColor m_surfaceLight = QColor(0x22, 0x22, 0x2E);
    QColor m_textPrimary = QColor(0xE8, 0xE8, 0xF0);
    QColor m_textSecondary = QColor(0x88, 0x88, 0xA0);
};

} // namespace keyoubs
