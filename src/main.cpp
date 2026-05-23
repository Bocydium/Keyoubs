#include "core/Application.hpp"
#include <QApplication>
#include <QFont>
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Keyoubs");
    app.setApplicationDisplayName("Keyoubs");
    app.setOrganizationName("keyoubs");

    QCommandLineParser parser;
    parser.setApplicationDescription("A visual toolkit for building desktop environments on Wayland.");
    parser.addHelpOption();
    parser.addVersionOption();

    // ─── Runtime mode ───
    // Headless process that keeps all deployed component surfaces alive,
    // listens to data sources, and responds to IPC. Does not open the editor.
    QCommandLineOption runtimeOption(
        QStringList() << "r" << "runtime",
        "Run in headless runtime mode (keeps deployed surfaces alive without editor)"
    );
    parser.addOption(runtimeOption);

    // ─── Debug / profile flags ───
    QCommandLineOption profileOption(
        QStringList() << "profile-startup",
        "Print millisecond timestamps for each initialization phase (debug builds)"
    );
    parser.addOption(profileOption);

    QCommandLineOption verboseOption(
        QStringList() << "v" << "verbose",
        "Enable verbose logging"
    );
    parser.addOption(verboseOption);

    parser.process(app);

    if (parser.isSet(runtimeOption)) {
        qInfo() << "Keyoubs runtime mode starting...";
        // TODO: Launch Runtime instead of Application
        // Runtime would load all autostart components and keep them alive
        // For now, fall through to normal mode with a warning
        qWarning() << "Runtime mode not yet fully implemented. Starting editor instead.";
    }

    QFont font("Inter", 10);
    font.setStyleHint(QFont::SansSerif);
    app.setFont(font);

    keyoubs::Application keyoubsApp;
    keyoubsApp.start();

    return app.exec();
}
