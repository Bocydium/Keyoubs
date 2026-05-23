#include "HyprlandAdapter.hpp"
#include <QProcess>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include <QDebug>
#include <cstdlib>

namespace keyoubs {

HyprlandAdapter::HyprlandAdapter(QObject *parent)
    : IpcAdapter(parent)
    , m_socketPath(getSocketPath())
    , m_socket(std::make_unique<QLocalSocket>())
{
}

HyprlandAdapter::~HyprlandAdapter() = default;

QString HyprlandAdapter::getSocketPath() const
{
    const char *sig = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    if (!sig) return QString();
    return QStringLiteral("/tmp/hypr/%1/.socket.sock").arg(QString::fromUtf8(sig));
}

bool HyprlandAdapter::isAvailable() const
{
    return !m_socketPath.isEmpty() && QFile::exists(m_socketPath);
}

bool HyprlandAdapter::connectSocket()
{
    if (!isAvailable()) {
        qWarning() << "Hyprland socket not available at:" << m_socketPath;
        return false;
    }
    m_socket->connectToServer(m_socketPath);
    if (!m_socket->waitForConnected(1000)) {
        qWarning() << "Failed to connect to Hyprland socket:" << m_socket->errorString();
        return false;
    }
    return true;
}

QByteArray HyprlandAdapter::sendRawRequest(const QByteArray &request)
{
    if (!connectSocket()) {
        return QByteArray();
    }

    m_socket->write(request + "\n");
    m_socket->flush();
    m_socket->waitForBytesWritten(1000);

    QByteArray response;
    while (m_socket->waitForReadyRead(500)) {
        response.append(m_socket->readAll());
        if (response.endsWith("\n")) break;
    }
    m_socket->disconnectFromServer();

    return response.trimmed();
}

bool HyprlandAdapter::runHyprctl(const QStringList &args)
{
    QProcess proc;
    proc.start("hyprctl", args);
    proc.waitForFinished(2000);
    if (proc.exitCode() != 0) {
        qWarning() << "hyprctl failed:" << proc.readAllStandardError();
        return false;
    }
    return true;
}

QVariant HyprlandAdapter::getWorkspaces()
{
    QByteArray data = sendRawRequest("j/workspaces");
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

QVariant HyprlandAdapter::getWindows()
{
    QByteArray data = sendRawRequest("j/clients");
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

QVariant HyprlandAdapter::getOutputs()
{
    QByteArray data = sendRawRequest("j/monitors");
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

bool HyprlandAdapter::setGapSize(int px)
{
    return runHyprctl(QStringList() << "keyword" << "general:gaps_in" << QString::number(px)
                                    << "&&" << "hyprctl" << "keyword" << "general:gaps_out" << QString::number(px));
}

bool HyprlandAdapter::setBorderWidth(int px)
{
    return runHyprctl(QStringList() << "keyword" << "general:border_size" << QString::number(px));
}

bool HyprlandAdapter::setBorderColor(const QString &color)
{
    return runHyprctl(QStringList() << "keyword" << "general:col.active_border" << color);
}

bool HyprlandAdapter::setOpacity(float opacity)
{
    return runHyprctl(QStringList() << "keyword" << "decoration:active_opacity" << QString::number(opacity, 'f', 2));
}

bool HyprlandAdapter::setWorkspaceName(int id, const QString &name)
{
    return runHyprctl(QStringList() << "renameworkspace" << QString::number(id) << name);
}

bool HyprlandAdapter::moveWindowToWorkspace(int windowId, int workspaceId)
{
    return runHyprctl(QStringList() << "dispatch" << "movetoworkspace"
                                    << QString::number(workspaceId) << "," << "address:0x" + QString::number(windowId, 16));
}

bool HyprlandAdapter::reloadConfig()
{
    return runHyprctl(QStringList() << "reload");
}

void HyprlandAdapter::startEventStream()
{
    // Hyprland events: connect to /tmp/hypr/<sig>/.socket2.sock
    // Stream format: >>eventname>>data
    // TODO: Implement event stream
}

void HyprlandAdapter::stopEventStream()
{
    // TODO
}

} // namespace keyoubs
