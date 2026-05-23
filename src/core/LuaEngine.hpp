#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <functional>

// Forward declare lua_State to avoid including lua.h in headers
struct lua_State;

namespace keyoubs {

// ─── Lua Scripting Engine ───
//
// Loads ~/.config/keyoubs/scripts/init.lua and exposes hooks:
//   onToolSelected(toolName)
//   onPropertyChanged(key, value)
//   onWindowSelected(windowId)
//   onCompositorDetected(name)
//
// Also exposes a keyoubs table with helper functions:
//   keyoubs.notify(title, message)
//   keyoubs.run(command)
//   keyoubs.log(level, message)

class LuaEngine : public QObject
{
    Q_OBJECT

public:
    explicit LuaEngine(QObject *parent = nullptr);
    ~LuaEngine() override;

    bool loadScript(const QString &path);
    bool reload();

    // Hook invokers
    void onToolSelected(const QString &tool);
    void onPropertyChanged(const QString &key, const QVariant &value);
    void onWindowSelected(int windowId);
    void onCompositorDetected(const QString &name);

    bool isLoaded() const;

signals:
    void scriptError(const QString &error);
    void scriptLoaded();

private:
    lua_State *m_L = nullptr;
    QString m_scriptPath;
    bool m_loaded = false;

    bool initLua();
    void registerApi();
    void closeLua();
    bool callHook(const char *name, int nargs = 0);
    static void pushVariant(lua_State *L, const QVariant &value);
};

} // namespace keyoubs
