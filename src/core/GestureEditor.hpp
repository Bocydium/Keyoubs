#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>

namespace keyoubs {

// ─── Gesture Editor ───
//
// Visual editor for touchpad gestures: swipe, pinch, rotate.
// Shows gesture direction diagrams. Conflict detection.
// Supports Hyprland and Niri gesture systems.

enum class GestureType {
    Swipe,
    Pinch,
    Rotate
};

enum class GestureDirection {
    Up,
    Down,
    Left,
    Right,
    In,    // pinch in
    Out,   // pinch out
    Clockwise,
    CounterClockwise
};

struct GestureBinding {
    GestureType type = GestureType::Swipe;
    int fingerCount = 3;
    GestureDirection direction = GestureDirection::Up;
    QString action;
    QString description;
};

class GestureEditor : public QObject
{
    Q_OBJECT

public:
    explicit GestureEditor(QObject *parent = nullptr);

    void addGesture(const GestureBinding &gesture);
    void removeGesture(int index);
    void updateGesture(int index, const GestureBinding &gesture);

    [[nodiscard]] QList<GestureBinding> gestures() const { return m_gestures; }

    // Conflict detection: same finger count + direction
    [[nodiscard]] QList<QPair<int, int>> findConflicts() const;

    // Export to compositor config
    [[nodiscard]] QString toHyprlandConfig() const;
    [[nodiscard]] QString toNiriConfig() const;

signals:
    void gestureAdded(int index);
    void gestureRemoved(int index);
    void gestureChanged(int index);
    void conflictsDetected(const QList<QPair<int, int>> &conflicts);

private:
    QList<GestureBinding> m_gestures;
};

} // namespace keyoubs
