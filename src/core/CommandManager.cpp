#include "CommandManager.hpp"
#include <QDebug>

namespace keyoubs {

CommandManager::CommandManager(QObject *parent)
    : QObject(parent)
{
}

void CommandManager::execute(std::unique_ptr<Command> cmd)
{
    if (!cmd) return;

    // Execute the command
    cmd->execute();

    // Clear redo stack — new command invalidates redo history
    while (!m_redoStack.isEmpty()) {
        delete m_redoStack.pop();
    }

    // Push to undo stack
    m_undoStack.push(cmd.release());
    trimUndoStack();

    emit canUndoChanged(true);
    emit canRedoChanged(false);
    emit commandExecuted(m_undoStack.top()->description());
    emit historyChanged();
}

void CommandManager::undo()
{
    if (m_undoStack.isEmpty()) return;

    Command *cmd = m_undoStack.pop();
    QString desc = cmd->description();
    cmd->undo();
    m_redoStack.push(cmd);

    emit canUndoChanged(!m_undoStack.isEmpty());
    emit canRedoChanged(true);
    emit commandUndone(desc);
    emit historyChanged();
}

void CommandManager::redo()
{
    if (m_redoStack.isEmpty()) return;

    Command *cmd = m_redoStack.pop();
    QString desc = cmd->description();
    cmd->redo();
    m_undoStack.push(cmd);
    trimUndoStack();

    emit canUndoChanged(true);
    emit canRedoChanged(!m_redoStack.isEmpty());
    emit commandRedone(desc);
    emit historyChanged();
}

QString CommandManager::undoDescription() const
{
    return m_undoStack.isEmpty() ? QString() : m_undoStack.top()->description();
}

QString CommandManager::redoDescription() const
{
    return m_redoStack.isEmpty() ? QString() : m_redoStack.top()->description();
}

void CommandManager::clear()
{
    while (!m_undoStack.isEmpty()) delete m_undoStack.pop();
    while (!m_redoStack.isEmpty()) delete m_redoStack.pop();
    emit canUndoChanged(false);
    emit canRedoChanged(false);
    emit historyChanged();
}

void CommandManager::trimUndoStack()
{
    while (m_undoStack.size() > m_maxHistory) {
        delete m_undoStack.takeFirst();
    }
}

} // namespace keyoubs
