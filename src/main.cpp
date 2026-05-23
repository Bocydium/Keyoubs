#include "core/Application.hpp"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Keyoubs");
    app.setApplicationDisplayName("Keyoubs");
    app.setOrganizationName("keyoubs");

    QFont font("Inter", 10);
    font.setStyleHint(QFont::SansSerif);
    app.setFont(font);

    keyoubs::Application keyoubsApp;
    keyoubsApp.start();

    return app.exec();
}
