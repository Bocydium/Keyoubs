#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

namespace keyoubs {

// Floating toolbar with drag-to-reposition support.
// Can snap to Top, Bottom, or float freely.
class Toolbar : public QWidget
{
    Q_OBJECT

public:
    enum class Tool {
        Select,
        Window,
        Workspace,
        GapBoundary,
        BorderBoundary,
        ColorPicker,
        Measure
    };

    enum class Position {
        Top,
        Bottom,
        Floating
    };

    explicit Toolbar(QWidget *parent = nullptr);

    void setCompositorName(const QString &name);
    void setPosition(Position pos);
    [[nodiscard]] Position position() const { return m_position; }

signals:
    void toolSelected(Tool tool);
    void applyClicked();
    void discardClicked();
    void closeClicked();
    void positionChanged(Position pos);
    void dragStarted();
    void dragEnded();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void createToolButton(const QString &tooltip, const QString &iconName, Tool tool);
    void updateStyleForPosition();

    QHBoxLayout *m_layout;
    QLabel *m_titleLabel;
    QLabel *m_compositorLabel;
    QPushButton *m_applyBtn;
    QPushButton *m_discardBtn;
    QPushButton *m_closeBtn;

    Tool m_activeTool = Tool::Select;
    Position m_position = Position::Top;

    // Drag state
    bool m_dragging = false;
    QPoint m_dragStartPos;
    QPoint m_dragStartGlobal;
};

} // namespace keyoubs
