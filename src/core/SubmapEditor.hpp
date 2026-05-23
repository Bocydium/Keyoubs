#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>

namespace keyoubs {

// ─── Submap Editor ───
//
// Submaps are Hyprland's modal keybind layers — temporary keybind modes
// activated by a keybind and exited by Escape or automatically.
//
// The SubmapEditor provides:
// - Visual list of submaps with names, entry keybind, exit behavior
// - Keybind editor per submap (same as global bindings)
// - Flow diagram showing submap transitions

struct SubmapBinding {
    QString key;
    QString action;
    QString description;
};

struct Submap {
    QString name;
    QString entryKeybind;
    bool autoClose = false; // new in Hyprland 0.52
    QList<SubmapBinding> bindings;
    QString exitKeybind; // usually "Escape"
};

class SubmapEditor : public QObject
{
    Q_OBJECT

public:
    explicit SubmapEditor(QObject *parent = nullptr);

    void addSubmap(const Submap &submap);
    void removeSubmap(const QString &name);
    void updateSubmap(const QString &name, const Submap &submap);

    [[nodiscard]] QList<Submap> submaps() const { return m_submaps; }
    [[nodiscard]] Submap* submap(const QString &name);

    // Flow diagram data: which keybinds enter/exit which submaps
    [[nodiscard]] QVariantMap flowDiagramData() const;

    // Export to Hyprland config format
    [[nodiscard]] QString toHyprlandConfig() const;

    // Import from existing config
    void fromHyprlandConfig(const QString &configSection);

signals:
    void submapAdded(const QString &name);
    void submapRemoved(const QString &name);
    void submapChanged(const QString &name);
    void flowDiagramChanged();

private:
    QList<Submap> m_submaps;
};

} // namespace keyoubs
