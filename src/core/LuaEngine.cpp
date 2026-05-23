#include "LuaEngine.hpp"
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace keyoubs {

// ─── Static C API Functions ───

static int l_notify(lua_State *L)
{
    const char *title = luaL_checkstring(L, 1);
    const char *msg = luaL_checkstring(L, 2);
    QProcess::execute("notify-send", QStringList() << title << msg);
    return 0;
}

static int l_run(lua_State *L)
{
    const char *cmd = luaL_checkstring(L, 1);
    int ret = QProcess::execute("sh", QStringList() << "-c" << cmd);
    lua_pushinteger(L, ret);
    return 1;
}

static int l_log(lua_State *L)
{
    const char *level = luaL_checkstring(L, 1);
    const char *msg = luaL_checkstring(L, 2);
    if (qstrcmp(level, "error") == 0) {
        qCritical() << "[Lua]" << msg;
    } else if (qstrcmp(level, "warn") == 0) {
        qWarning() << "[Lua]" << msg;
    } else {
        qInfo() << "[Lua]" << msg;
    }
    return 0;
}

// ─── LuaEngine Implementation ───

LuaEngine::LuaEngine(QObject *parent)
    : QObject(parent)
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/keyoubs/scripts";
    m_scriptPath = configDir + "/init.lua";

    if (QFile::exists(m_scriptPath)) {
        loadScript(m_scriptPath);
    }
}

LuaEngine::~LuaEngine()
{
    closeLua();
}

bool LuaEngine::initLua()
{
    closeLua();
    m_L = luaL_newstate();
    if (!m_L) {
        emit scriptError("Failed to create Lua state");
        return false;
    }
    luaL_openlibs(m_L);
    registerApi();
    return true;
}

void LuaEngine::registerApi()
{
    lua_newtable(m_L); // keyoubs table

    lua_pushcfunction(m_L, l_notify);
    lua_setfield(m_L, -2, "notify");

    lua_pushcfunction(m_L, l_run);
    lua_setfield(m_L, -2, "run");

    lua_pushcfunction(m_L, l_log);
    lua_setfield(m_L, -2, "log");

    lua_setglobal(m_L, "keyoubs");
}

void LuaEngine::closeLua()
{
    if (m_L) {
        lua_close(m_L);
        m_L = nullptr;
    }
    m_loaded = false;
}

bool LuaEngine::loadScript(const QString &path)
{
    if (!initLua()) {
        return false;
    }

    m_scriptPath = path;
    QByteArray utf8Path = path.toUtf8();

    if (luaL_dofile(m_L, utf8Path.constData()) != LUA_OK) {
        const char *err = lua_tostring(m_L, -1);
        QString errorMsg = QString::fromUtf8(err ? err : "Unknown Lua error");
        qWarning() << "Lua script error:" << errorMsg;
        emit scriptError(errorMsg);
        lua_pop(m_L, 1);
        m_loaded = false;
        return false;
    }

    m_loaded = true;
    emit scriptLoaded();
    qInfo() << "Lua script loaded:" << path;
    return true;
}

bool LuaEngine::reload()
{
    if (m_scriptPath.isEmpty() || !QFile::exists(m_scriptPath)) {
        return false;
    }
    return loadScript(m_scriptPath);
}

bool LuaEngine::isLoaded() const
{
    return m_loaded && m_L != nullptr;
}

void LuaEngine::pushVariant(lua_State *L, const QVariant &value)
{
    switch (value.typeId()) {
    case QMetaType::Bool:
        lua_pushboolean(L, value.toBool());
        break;
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        lua_pushinteger(L, value.toLongLong());
        break;
    case QMetaType::Double:
    case QMetaType::Float:
        lua_pushnumber(L, value.toDouble());
        break;
    case QMetaType::QString:
        lua_pushstring(L, value.toString().toUtf8().constData());
        break;
    default:
        lua_pushstring(L, value.toString().toUtf8().constData());
        break;
    }
}

bool LuaEngine::callHook(const char *name, int nargs)
{
    if (!m_loaded || !m_L) return false;

    lua_getglobal(m_L, name);
    if (!lua_isfunction(m_L, -1)) {
        lua_pop(m_L, 1);
        return false; // Hook not defined, that's fine
    }

    // Move function below args
    if (nargs > 0) {
        lua_insert(m_L, -nargs - 1);
    }

    if (lua_pcall(m_L, nargs, 0, 0) != LUA_OK) {
        const char *err = lua_tostring(m_L, -1);
        qWarning() << "Lua hook" << name << "error:" << (err ? err : "unknown");
        lua_pop(m_L, 1);
        return false;
    }
    return true;
}

void LuaEngine::onToolSelected(const QString &tool)
{
    if (!m_loaded) return;
    lua_pushstring(m_L, tool.toUtf8().constData());
    callHook("onToolSelected", 1);
}

void LuaEngine::onPropertyChanged(const QString &key, const QVariant &value)
{
    if (!m_loaded) return;
    lua_pushstring(m_L, key.toUtf8().constData());
    pushVariant(m_L, value);
    callHook("onPropertyChanged", 2);
}

void LuaEngine::onWindowSelected(int windowId)
{
    if (!m_loaded) return;
    lua_pushinteger(m_L, windowId);
    callHook("onWindowSelected", 1);
}

void LuaEngine::onCompositorDetected(const QString &name)
{
    if (!m_loaded) return;
    lua_pushstring(m_L, name.toUtf8().constData());
    callHook("onCompositorDetected", 1);
}

} // namespace keyoubs
