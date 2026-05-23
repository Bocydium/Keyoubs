#include "SubmapEditor.hpp"
#include <QDebug>

namespace keyoubs {

SubmapEditor::SubmapEditor(QObject *parent)
    : QObject(parent)
{
}

void SubmapEditor::addSubmap(const Submap &submap)
{
    // Check for duplicate names
    for (const auto &existing : m_submaps) {
        if (existing.name == submap.name) {
            qWarning() << "Submap already exists:" << submap.name;
            return;
        }
    }
    m_submaps.append(submap);
    emit submapAdded(submap.name);
    emit flowDiagramChanged();
}

void SubmapEditor::removeSubmap(const QString &name)
{
    for (int i = 0; i < m_submaps.size(); ++i) {
        if (m_submaps[i].name == name) {
            m_submaps.removeAt(i);
            emit submapRemoved(name);
            emit flowDiagramChanged();
            return;
        }
    }
}

void SubmapEditor::updateSubmap(const QString &name, const Submap &submap)
{
    for (auto &existing : m_submaps) {
        if (existing.name == name) {
            existing = submap;
            emit submapChanged(name);
            emit flowDiagramChanged();
            return;
        }
    }
}

Submap* SubmapEditor::submap(const QString &name)
{
    for (auto &sm : m_submaps) {
        if (sm.name == name) return &sm;
    }
    return nullptr;
}

QVariantMap SubmapEditor::flowDiagramData() const
{
    QVariantMap data;
    QVariantList nodes;
    QVariantList edges;

    // Add a "default" node for the global state
    QVariantMap defaultNode;
    defaultNode["id"] = "default";
    defaultNode["label"] = "Default";
    defaultNode["type"] = "root";
    nodes.append(defaultNode);

    // Add submap nodes
    for (const auto &sm : m_submaps) {
        QVariantMap node;
        node["id"] = sm.name;
        node["label"] = sm.name;
        node["type"] = "submap";
        nodes.append(node);

        // Edge: default → submap (entry keybind)
        QVariantMap entryEdge;
        entryEdge["from"] = "default";
        entryEdge["to"] = sm.name;
        entryEdge["label"] = sm.entryKeybind;
        entryEdge["type"] = "entry";
        edges.append(entryEdge);

        // Edge: submap → default (exit keybind)
        QVariantMap exitEdge;
        exitEdge["from"] = sm.name;
        exitEdge["to"] = "default";
        exitEdge["label"] = sm.exitKeybind.isEmpty() ? "Escape" : sm.exitKeybind;
        exitEdge["type"] = "exit";
        edges.append(exitEdge);
    }

    data["nodes"] = nodes;
    data["edges"] = edges;
    return data;
}

QString SubmapEditor::toHyprlandConfig() const
{
    QString out;

    for (const auto &sm : m_submaps) {
        out += "# Submap: " + sm.name + "\n";
        out += "submap = " + sm.name + "\n";

        for (const auto &binding : sm.bindings) {
            out += "bind = " + binding.key + ", " + binding.action + "\n";
        }

        if (sm.autoClose) {
            out += "bind = , escape, submap, reset\n";
        } else {
            out += "bind = " + (sm.exitKeybind.isEmpty() ? "escape" : sm.exitKeybind)
                   + ", submap, reset\n";
        }

        out += "submap = reset\n\n";
    }

    // Entry keybinds in default submap
    out += "# Submap entry keybinds\n";
    for (const auto &sm : m_submaps) {
        out += "bind = " + sm.entryKeybind + ", submap, " + sm.name + "\n";
    }

    return out;
}

void SubmapEditor::fromHyprlandConfig(const QString &configSection)
{
    // Simplified parser for submap sections
    QStringList lines = configSection.split('\n');
    Submap currentSubmap;
    bool inSubmap = false;

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith('#')) continue;

        if (trimmed.startsWith("submap = ")) {
            QString name = trimmed.mid(9).trimmed();
            if (name == "reset") {
                if (inSubmap && !currentSubmap.name.isEmpty()) {
                    m_submaps.append(currentSubmap);
                    emit submapAdded(currentSubmap.name);
                }
                inSubmap = false;
                currentSubmap = Submap();
            } else {
                inSubmap = true;
                currentSubmap.name = name;
            }
        } else if (inSubmap && trimmed.startsWith("bind = ")) {
            QString bindLine = trimmed.mid(7);
            QStringList parts = bindLine.split(',');
            if (parts.size() >= 2) {
                SubmapBinding binding;
                binding.key = parts[0].trimmed();
                binding.action = parts[1].trimmed();
                if (parts.size() >= 3) {
                    binding.action += ", " + parts[2].trimmed();
                }

                // Check if this is an exit binding
                if (binding.action.contains("submap, reset")) {
                    currentSubmap.exitKeybind = binding.key;
                } else {
                    currentSubmap.bindings.append(binding);
                }
            }
        }
    }

    emit flowDiagramChanged();
}

} // namespace keyoubs
