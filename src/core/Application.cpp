#include "Application.hpp"
#include "ui/OverlayWindow.hpp"
#include "ConfigManager.hpp"
#include "../ipc/NiriAdapter.hpp"
#include "../ipc/HyprlandAdapter.hpp"

#include <QProcess>
#include <QDebug>
#include <QApplication>
#include <cstdlib>

namespace keyoubs {

Application::Application(QObject *parent)
    : QObject(parent)
    , m_overlay(nullptr)
    , m_configManager(std::make_unique<ConfigManager>())
    , m_ipcAdapter(nullptr)
{
}

Application::~Application() = default;

void Application::start()
{
    detectCompositor();
    setupOverlay();
}

void Application::shutdown()
{
    m_overlay.reset();
    m_ipcAdapter.reset();
}

void Application::detectCompositor()
{
    const char *waylandDisplay = std::getenv("WAYLAND_DISPLAY");
    if (!waylandDisplay) {
        qWarning() << "Not running on Wayland. Keyoubs requires Wayland.";
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

    qWarning() << "No supported compositor detected (Niri or Hyprland)";
}

void Application::setupOverlay()
{
    m_overlay = std::make_unique<OverlayWindow>();

    // ESC quits the entire application
    connect(m_overlay.get(), &OverlayWindow::closeRequested, qApp, &QApplication::quit);

    m_overlay->show();
}

} // namespace keyoubs
