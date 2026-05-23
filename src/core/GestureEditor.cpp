#include "GestureEditor.hpp"
#include <QDebug>

namespace keyoubs {

GestureEditor::GestureEditor(QObject *parent)
    : QObject(parent)
{
}

void GestureEditor::addGesture(const GestureBinding &gesture)
{
    m_gestures.append(gesture);
    int index = m_gestures.size() - 1;
    emit gestureAdded(index);

    auto conflicts = findConflicts();
    if (!conflicts.isEmpty()) {
        emit conflictsDetected(conflicts);
    }
}

void GestureEditor::removeGesture(int index)
{
    if (index < 0 || index >= m_gestures.size()) return;
    m_gestures.removeAt(index);
    emit gestureRemoved(index);
}

void GestureEditor::updateGesture(int index, const GestureBinding &gesture)
{
    if (index < 0 || index >= m_gestures.size()) return;
    m_gestures[index] = gesture;
    emit gestureChanged(index);

    auto conflicts = findConflicts();
    if (!conflicts.isEmpty()) {
        emit conflictsDetected(conflicts);
    }
}

QList<QPair<int, int>> GestureEditor::findConflicts() const
{
    QList<QPair<int, int>> conflicts;

    for (int i = 0; i < m_gestures.size(); ++i) {
        for (int j = i + 1; j < m_gestures.size(); ++j) {
            const auto &a = m_gestures[i];
            const auto &b = m_gestures[j];

            if (a.type == b.type &&
                a.fingerCount == b.fingerCount &&
                a.direction == b.direction) {
                conflicts.append({i, j});
            }
        }
    }

    return conflicts;
}

QString GestureEditor::toHyprlandConfig() const
{
    QString out;
    out += "# Gesture configuration\n";

    for (const auto &g : m_gestures) {
        QString typeStr;
        switch (g.type) {
        case GestureType::Swipe: typeStr = "swipe"; break;
        case GestureType::Pinch: typeStr = "pinch"; break;
        case GestureType::Rotate: typeStr = "rotate"; break;
        }

        QString dirStr;
        switch (g.direction) {
        case GestureDirection::Up: dirStr = "up"; break;
        case GestureDirection::Down: dirStr = "down"; break;
        case GestureDirection::Left: dirStr = "left"; break;
        case GestureDirection::Right: dirStr = "right"; break;
        case GestureDirection::In: dirStr = "in"; break;
        case GestureDirection::Out: dirStr = "out"; break;
        case GestureDirection::Clockwise: dirStr = "clockwise"; break;
        case GestureDirection::CounterClockwise: dirStr = "counterclockwise"; break;
        }

        out += QString("gesture:%1:%2:%3 = %4\n")
                   .arg(typeStr)
                   .arg(g.fingerCount)
                   .arg(dirStr)
                   .arg(g.action);
    }

    return out;
}

QString GestureEditor::toNiriConfig() const
{
    QString out;
    out += "# Niri gesture configuration\n";

    for (const auto &g : m_gestures) {
        if (g.type != GestureType::Swipe) continue;

        QString dirStr;
        switch (g.direction) {
        case GestureDirection::Up: dirStr = "up"; break;
        case GestureDirection::Down: dirStr = "down"; break;
        case GestureDirection::Left: dirStr = "left"; break;
        case GestureDirection::Right: dirStr = "right"; break;
        default: continue;
        }

        out += QString("bind-gesture swipe:%1:%2 %3\n")
                   .arg(g.fingerCount)
                   .arg(dirStr)
                   .arg(g.action);
    }

    return out;
}

} // namespace keyoubs
