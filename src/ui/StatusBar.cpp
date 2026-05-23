#include "StatusBar.hpp"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>

namespace keyoubs {

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QHBoxLayout(this))
{
    setFixedHeight(m_barHeight);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_layout->setContentsMargins(12, 0, 12, 0);
    m_layout->setSpacing(4);
    m_layout->setAlignment(Qt::AlignVCenter);

    // Add a stretchable spacer on the right
    m_layout->addStretch();

    setStyleSheet(R"(
        StatusBar {
            background-color: #101010;
            border-bottom: 1px solid #2A2A2A;
        }
    )");
}

void StatusBar::addModule(Module *module)
{
    if (!module || m_modules.contains(module)) return;

    m_modules.append(module);
    // Insert before the stretch
    m_layout->insertWidget(m_layout->count() - 1, module);

    connect(module, &Module::clicked, this, [this, module]() {
        emit moduleClicked(module);
    });
}

void StatusBar::removeModule(Module *module)
{
    if (!m_modules.contains(module)) return;
    m_modules.removeAll(module);
    m_layout->removeWidget(module);
    module->setParent(nullptr);
}

void StatusBar::clearModules()
{
    for (auto *mod : m_modules) {
        m_layout->removeWidget(mod);
        mod->setParent(nullptr);
    }
    m_modules.clear();
}

QList<Module*> StatusBar::modules() const
{
    return m_modules;
}

void StatusBar::setBarHeight(int height)
{
    m_barHeight = height;
    setFixedHeight(height);
}

void StatusBar::setMargins(int left, int right)
{
    m_layout->setContentsMargins(left, 0, right, 0);
}

} // namespace keyoubs
