#pragma once

#include "../models/ConfigModel.hpp"
#include <QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;
class QColorDialog;
QT_END_NAMESPACE

namespace keyoubs {

// Right-side (or left-side / floating) property panel.
// Shows editable properties for the current selection.
class PropertyPanel : public QWidget
{
    Q_OBJECT

public:
    enum class SelectionType {
        None,
        Global,
        Window,
        Workspace,
        Gap,
        Border,
        Output
    };

    enum class Position {
        Left,
        Right,
        Floating
    };

    explicit PropertyPanel(QWidget *parent = nullptr);

    void setPosition(Position pos);
    [[nodiscard]] Position position() const { return m_position; }

signals:
    void propertyChanged(const QString &key, const QVariant &value);
    void positionChanged(Position pos);
    void dragStarted();
    void dragEnded();

public slots:
    void onSelectionChanged(SelectionType type, const QVariant &data);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void clearPanel();
    void showGlobalProperties();
    void showWindowProperties(const QVariant &data);
    void showGapProperties();
    void showBorderProperties();
    void updateStyleForPosition();

    QVBoxLayout *m_layout;
    QLabel *m_headerLabel;
    QWidget *m_contentWidget;
    QVBoxLayout *m_contentLayout;

    SelectionType m_currentType = SelectionType::None;
    Position m_position = Position::Right;

    // Drag state
    bool m_dragging = false;
    QPoint m_dragStartPos;
    QPoint m_dragStartGlobal;
};

} // namespace keyoubs
