#include "NiriAdapter.hpp"
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <cstdlib>

namespace keyoubs {

NiriAdapter::NiriAdapter(QObject *parent)
    : IpcAdapter(parent)
    , m_socketPath(QString::fromUtf8(std::getenv("NIRI_SOCKET") ? std::getenv("NIRI_SOCKET") : ""))
    , m_socket(std::make_unique<QLocalSocket>())
    , m_eventSocket(std::make_unique<QLocalSocket>())
{
}

NiriAdapter::~NiriAdapter() = default;

bool NiriAdapter::isAvailable() const
{
    return !m_socketPath.isEmpty() && QFile::exists(m_socketPath);
}

bool NiriAdapter::connectSocket(QLocalSocket *socket)
{
    if (!isAvailable()) {
        qWarning() << "Niri socket not available at:" << m_socketPath;
        return false;
    }
    socket->connectToServer(m_socketPath);
    if (!socket->waitForConnected(1000)) {
        qWarning() << "Failed to connect to Niri socket:" << socket->errorString();
        return false;
    }
    return true;
}

QJsonDocument NiriAdapter::sendRequest(const QJsonObject &request)
{
    if (!connectSocket(m_socket.get())) {
        return QJsonDocument();
    }

    QByteArray data = QJsonDocument(request).toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(data);
    m_socket->flush();
    m_socket->waitForBytesWritten(1000);

    if (!m_socket->waitForReadyRead(2000)) {
        qWarning() << "Niri IPC timeout";
        return QJsonDocument();
    }

    QByteArray response = m_socket->readLine().trimmed();
    m_socket->disconnectFromServer();

    return QJsonDocument::fromJson(response);
}

QVariant NiriAdapter::getWorkspaces()
{
    QJsonObject req;
    req["Request"] = QStringLiteral("Workspaces");
    QJsonDocument doc = sendRequest(req);
    return doc.toVariant();
}

QVariant NiriAdapter::getWindows()
{
    QJsonObject req;
    req["Request"] = QStringLiteral("Windows");
    QJsonDocument doc = sendRequest(req);
    return doc.toVariant();
}

QVariant NiriAdapter::getOutputs()
{
    QJsonObject req;
    req["Request"] = QStringLiteral("Outputs");
    QJsonDocument doc = sendRequest(req);
    return doc.toVariant();
}

bool NiriAdapter::setGapSize(int px)
{
    // Niri: gaps are config-only, need to edit config.kdl and reload
    // Or use output configuration if available via IPC
    Q_UNUSED(px)
    qInfo() << "Niri gap changes require config reload";
    return false;
}

bool NiriAdapter::setBorderWidth(int px)
{
    Q_UNUSED(px)
    qInfo() << "Niri border changes require config reload";
    return false;
}

bool NiriAdapter::setBorderColor(const QString &color)
{
    Q_UNUSED(color)
    qInfo() << "Niri border color changes require config reload";
    return false;
}

bool NiriAdapter::setOpacity(float opacity)
{
    Q_UNUSED(opacity)
    qInfo() << "Niri opacity changes require window rules config reload";
    return false;
}

bool NiriAdapter::setWorkspaceName(int id, const QString &name)
{
    QJsonObject req;
    req["Action"] = QJsonObject{
        {"SetWorkspaceName", QJsonObject{
            {"name", name},
            {"workspace", QJsonObject{{"Index", id}}}
        }}
    };
    QJsonDocument doc = sendRequest(req);
    return !doc.isNull();
}

bool NiriAdapter::moveWindowToWorkspace(int windowId, int workspaceId)
{
    QJsonObject req;
    req["Action"] = QJsonObject{
        {"MoveWindowToWorkspace", QJsonObject{
            {"window_id", windowId},
            {"reference", QJsonObject{{"Index", workspaceId}}},
            {"focus", false}
        }}
    };
    QJsonDocument doc = sendRequest(req);
    return !doc.isNull();
}

bool NiriAdapter::reloadConfig()
{
    QJsonObject req;
    req["Action"] = QStringLiteral("LoadConfigFile");
    QJsonDocument doc = sendRequest(req);
    return !doc.isNull();
}

void NiriAdapter::startEventStream()
{
    if (!connectSocket(m_eventSocket.get())) {
        return;
    }

    QJsonObject req;
    req["Request"] = QStringLiteral("EventStream");
    QByteArray data = QJsonDocument(req).toJson(QJsonDocument::Compact) + "\n";
    m_eventSocket->write(data);
    m_eventSocket->flush();

    // TODO: Connect readyRead signal to handleEvent
}

void NiriAdapter::stopEventStream()
{
    if (m_eventSocket->state() == QLocalSocket::ConnectedState) {
        m_eventSocket->disconnectFromServer();
    }
}

void NiriAdapter::handleEvent(const QJsonObject &event)
{
    // TODO: Parse and emit workspaceChanged, windowChanged, outputChanged
    Q_UNUSED(event)
}

} // namespace keyoubs
