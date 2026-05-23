#pragma once

#include <QObject>
#include <QStack>
#include <QString>
#include <functional>
#include <memory>

namespace keyoubs {

// ─── Command Pattern for Undo/Redo ───
//
// Every mutating operation wraps in a Command:
//   execute() — does the thing
//   undo()    — reverses it
//   redo()    — same as execute (for clarity)
//
// CommandManager maintains two stacks:
//   undoStack — commands that can be undone
//   redoStack — commands that were undone and can be redone
//
// New commands clear the redoStack.

class Command
{
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() { execute(); }
    virtual QString description() const = 0;
};

class CommandManager : public QObject
{
    Q_OBJECT

public:
    explicit CommandManager(QObject *parent = nullptr);

    void execute(std::unique_ptr<Command> cmd);
    void undo();
    void redo();

    [[nodiscard]] bool canUndo() const { return !m_undoStack.isEmpty(); }
    [[nodiscard]] bool canRedo() const { return !m_redoStack.isEmpty(); }
    [[nodiscard]] QString undoDescription() const;
    [[nodiscard]] QString redoDescription() const;
    [[nodiscard]] const QStack<Command*>& undoStack() const { return m_undoStack; }

    void clear();

    // Maximum number of commands to keep in undo stack
    void setMaxHistory(int max) { m_maxHistory = max; }

signals:
    void canUndoChanged(bool can);
    void canRedoChanged(bool can);
    void commandExecuted(const QString &description);
    void commandUndone(const QString &description);
    void commandRedone(const QString &description);
    void historyChanged();

private:
    QStack<Command*> m_undoStack;
    QStack<Command*> m_redoStack;
    int m_maxHistory = 100;

    void trimUndoStack();
};

// Convenience macro for simple lambda-based commands
class LambdaCommand : public Command
{
public:
    LambdaCommand(QString desc,
                  std::function<void()> exec,
                  std::function<void()> undofn)
        : m_desc(std::move(desc))
        , m_exec(std::move(exec))
        , m_undo(std::move(undofn))
    {
    }

    void execute() override { if (m_exec) m_exec(); }
    void undo() override { if (m_undo) m_undo(); }
    QString description() const override { return m_desc; }

private:
    QString m_desc;
    std::function<void()> m_exec;
    std::function<void()> m_undo;
};

} // namespace keyoubs
