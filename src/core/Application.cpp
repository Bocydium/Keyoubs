#include "Application.hpp"
#include "ui/OverlayWindow.hpp"
#include "ConfigManager.hpp"
#include "ThemeConfig.hpp"
#include "LuaEngine.hpp"
#include "../ipc/NiriAdapter.hpp"
#include "../ipc/HyprlandAdapter.hpp"
#include "../ipc/I3SwayAdapter.hpp"

#include <QProcess>
#include <QDebug>
#include <QApplication>
#include <cstdlib>

namespace keyoubs {

Application::Application(QObject *parent)
    : QObject(parent)
    , m_overlay(nullptr)
    , m_configManager(std::make_unique<ConfigManager>())
    , m_themeConfig(std::make_unique<ThemeConfig>())
    , m_ipcAdapter(nullptr)
{
}

Application::~Application() = default;

void Application::start()
{
    detectCompositor();
    setupOverlay();

    // Connect theme hot-reload
    connect(m_themeConfig.get(), &ThemeConfig::themeChanged, this, [this]() {
        if (m_overlay) {
            m_overlay->update();
        }
    });
}

void Application::shutdown()
{
    m_overlay.reset();
    m_ipcAdapter.reset();
}

void Application::detectCompositor()
{
    const char *waylandDisplay = std::getenv("WAYLAND_DISPLAY");
    const char *display = std::getenv("DISPLAY");

    if (!waylandDisplay && !display) {
        qWarning() << "No display detected. Keyoubs requires X11 or Wayland.";
        return;
    }

    // Check for Niri
    QProcess niriCheck;
    niriCheck.start("sh", QStringList() << "-c" << "niri msg --version");
    niriCheck.waitForFinished(500);
    if (niriCheck.exitCode() == 0) {
        qInfo() << "Detected Niri compositor";
        m_ipcAdapter = std::make_unique<NiriAdapter>();
        return;
    }

    // Check for Hyprland
    const char *hyprlandSig = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    if (hyprlandSig) {
        qInfo() << "Detected Hyprland compositor";
        m_ipcAdapter = std::make_unique<HyprlandAdapter>();
        return;
    }

    // Check for Sway
    const char *swaySock = std::getenv("SWAYSOCK");
    if (swaySock) {
        qInfo() << "Detected Sway compositor (via SWAYSOCK)";
        m_ipcAdapter = std::make_unique<I3SwayAdapter>();
        return;
    }

    // Check for i3
    const char *i3Sock = std::getenv("I3SOCK");
    if (i3Sock) {
        qInfo() << "Detected i3 compositor (via I3SOCK)";
        m_ipcAdapter = std::make_unique<I3SwayAdapter>();
        return;
    }

    // Try sway --get-socketpath as fallback
    QProcess swayCheck;
    swayCheck.start("sway", QStringList() << "--get-socketpath");
    swayCheck.waitForFinished(500);
    if (swayCheck.exitCode() == 0 && !swayCheck.readAllStandardOutput().trimmed().isEmpty()) {
        qInfo() << "Detected Sway compositor (via sway --get-socketpath)";
        m_ipcAdapter = std::make_unique<I3SwayAdapter>();
        return;
    }

    // Try i3 --get-socketpath as fallback
    QProcess i3Check;
    i3Check.start("i3", QStringList() << "--get-socketpath");
    i3Check.waitForFinished(500);
    if (i3Check.exitCode() == 0 && !i3Check.readAllStandardOutput().trimmed().isEmpty()) {
        qInfo() << "Detected i3 compositor (via i3 --get-socketpath)";
        m_ipcAdapter = std::make_unique<I3SwayAdapter>();
        return;
    }

    qWarning() << "No supported compositor detected (Niri, Hyprland, Sway, or i3)";
}

void Application::setupOverlay()
{
    m_overlay = std::make_unique<OverlayWindow>();

    // ESC quits the entire application
    connect(m_overlay.get(), &OverlayWindow::closeRequested, qApp, &QApplication::quit);

    if (m_ipcAdapter) {
        m_overlay->setCompositorName(m_ipcAdapter->compositorName());
    }

    m_overlay->show();
}

} // namespace keyoubs
