#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QList>
#include "../core/Module.hpp"

namespace keyoubs {

// ─── Status Bar Surface ───
//
// A Wayland layer-shell surface anchored to a screen edge.
// Contains modules: clock, workspace pips, battery, network, etc.
// Modules are draggable for reordering.

class StatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);

    void addModule(Module *module);
    void removeModule(Module *module);
    void clearModules();

    [[nodiscard]] QList<Module*> modules() const;

    void setBarHeight(int height);
    void setMargins(int left, int right);

signals:
    void moduleClicked(Module *module);

private:
    QHBoxLayout *m_layout = nullptr;
    QList<Module*> m_modules;
    int m_barHeight = 32;
};

} // namespace keyoubs
