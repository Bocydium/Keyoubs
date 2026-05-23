#pragma once

#include <QObject>
#include <memory>

namespace keyoubs {

class OverlayWindow;
class ConfigManager;
class ThemeConfig;
class LuaEngine;
class IpcAdapter;

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);
    ~Application();

    void start();
    void shutdown();

private:
    std::unique_ptr<OverlayWindow> m_overlay;
    std::unique_ptr<ConfigManager> m_configManager;
    std::unique_ptr<ThemeConfig> m_themeConfig;
    std::unique_ptr<LuaEngine> m_luaEngine;
    std::unique_ptr<IpcAdapter> m_ipcAdapter;

    void detectCompositor();
    void setupOverlay();
};

} // namespace keyoubs
