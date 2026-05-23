#include "I3SwayAdapter.hpp"
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <cstdlib>

namespace keyoubs {

I3SwayAdapter::I3SwayAdapter(QObject *parent)
    : IpcAdapter(parent)
    , m_socketPath(resolveSocketPath())
    , m_socket(std::make_unique<QLocalSocket>())
    , m_eventSocket(std::make_unique<QLocalSocket>())
{
    detectCompositor();
}

I3SwayAdapter::~I3SwayAdapter() = default;

QString I3SwayAdapter::resolveSocketPath() const
{
    // Try Sway first
    const char *swaySock = std::getenv("SWAYSOCK");
    if (swaySock && QFile::exists(swaySock)) {
        return QString::fromUtf8(swaySock);
    }

    // Try i3
    const char *i3Sock = std::getenv("I3SOCK");
    if (i3Sock && QFile::exists(i3Sock)) {
        return QString::fromUtf8(i3Sock);
    }

    // Try sway --get-socketpath
    QProcess swayProc;
    swayProc.start("sway", QStringList() << "--get-socketpath");
    swayProc.waitForFinished(500);
    if (swayProc.exitCode() == 0) {
        QString path = swayProc.readAllStandardOutput().trimmed();
        if (!path.isEmpty() && QFile::exists(path)) {
            return path;
        }
    }

    // Try i3 --get-socketpath
    QProcess i3Proc;
    i3Proc.start("i3", QStringList() << "--get-socketpath");
    i3Proc.waitForFinished(500);
    if (i3Proc.exitCode() == 0) {
        QString path = i3Proc.readAllStandardOutput().trimmed();
        if (!path.isEmpty() && QFile::exists(path)) {
            return path;
        }
    }

    return QString();
}

bool I3SwayAdapter::detectCompositor()
{
    if (m_socketPath.isEmpty()) {
        return false;
    }

    QByteArray response = sendMessage(MsgType::GetVersion);
    if (response.isEmpty()) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    // Sway includes "sway_version" or "variant" == "sway"
    if (obj.contains("sway_version") || obj.value("variant").toString() == "sway") {
        m_compositorName = "Sway";
        m_isSway = true;
    } else {
        m_compositorName = "i3";
        m_isSway = false;
    }

    qInfo() << "Detected" << m_compositorName << "via IPC";
    return true;
}

QString I3SwayAdapter::compositorName() const
{
    return m_compositorName;
}

bool I3SwayAdapter::isAvailable() const
{
    return !m_socketPath.isEmpty() && QFile::exists(m_socketPath) && !m_compositorName.isEmpty();
}

bool I3SwayAdapter::connectSocket(QLocalSocket *socket)
{
    if (!isAvailable()) {
        qWarning() << "i3/Sway socket not available at:" << m_socketPath;
        return false;
    }
    socket->connectToServer(m_socketPath);
    if (!socket->waitForConnected(1000)) {
        qWarning() << "Failed to connect to i3/Sway socket:" << socket->errorString();
        return false;
    }
    return true;
}

QByteArray I3SwayAdapter::buildMessage(MsgType type, const QByteArray &payload)
{
    QByteArray msg;
    msg.append("i3-ipc");                          // magic (6 bytes)
    uint32_t len = static_cast<uint32_t>(payload.size());
    msg.append(reinterpret_cast<const char*>(&len), 4); // length (4 bytes, native endian)
    uint32_t typeVal = static_cast<uint32_t>(type);
    msg.append(reinterpret_cast<const char*>(&typeVal), 4);        // type (4 bytes, native endian)
    msg.append(payload);                           // payload
    return msg;
}

QByteArray I3SwayAdapter::sendMessage(MsgType type, const QByteArray &payload)
{
    if (!connectSocket(m_socket.get())) {
        return QByteArray();
    }

    QByteArray msg = buildMessage(type, payload);
    m_socket->write(msg);
    m_socket->flush();
    m_socket->waitForBytesWritten(1000);

    // Read header: magic(6) + length(4) + type(4)
    if (!m_socket->waitForReadyRead(2000)) {
        qWarning() << "i3/Sway IPC timeout waiting for header";
        m_socket->disconnectFromServer();
        return QByteArray();
    }

    QByteArray header = m_socket->read(14);
    if (header.size() < 14) {
        qWarning() << "i3/Sway IPC incomplete header";
        m_socket->disconnectFromServer();
        return QByteArray();
    }

    // Parse length (bytes 6-9, little-endian)
    uint32_t replyLen = *reinterpret_cast<const uint32_t*>(header.constData() + 6);

    // Read payload
    QByteArray replyPayload;
    while (replyPayload.size() < static_cast<int>(replyLen)) {
        if (!m_socket->waitForReadyRead(500)) break;
        replyPayload.append(m_socket->read(replyLen - replyPayload.size()));
    }

    m_socket->disconnectFromServer();
    return replyPayload;
}

QJsonDocument I3SwayAdapter::sendJsonMessage(MsgType type, const QJsonObject &payload)
{
    QByteArray data = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    QByteArray response = sendMessage(type, data);
    return QJsonDocument::fromJson(response);
}

QVariant I3SwayAdapter::getWorkspaces()
{
    QByteArray data = sendMessage(MsgType::GetWorkspaces);
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

QVariant I3SwayAdapter::getWindows()
{
    // i3/Sway: GET_TREE returns the full container tree
    QByteArray data = sendMessage(MsgType::GetTree);
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

QVariant I3SwayAdapter::getOutputs()
{
    QByteArray data = sendMessage(MsgType::GetOutputs);
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

bool I3SwayAdapter::setGapSize(int px)
{
    // i3 >= 4.22 supports gaps via commands
    QString cmd = QString("gaps inner all set %1; gaps outer all set %1").arg(px);
    QByteArray response = sendMessage(MsgType::RunCommand, cmd.toUtf8());

    // Parse response: array of { success: bool, error: string }
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray arr = doc.array();
    for (const auto &item : arr) {
        if (!item.toObject().value("success").toBool()) {
            QString err = item.toObject().value("error").toString();
            qWarning() << "i3/Sway gap command failed:" << err;
            return false;
        }
    }
    return true;
}

bool I3SwayAdapter::setBorderWidth(int px)
{
    QString cmd = QString("default_border pixel %1").arg(px);
    QByteArray response = sendMessage(MsgType::RunCommand, cmd.toUtf8());

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray arr = doc.array();
    for (const auto &item : arr) {
        if (!item.toObject().value("success").toBool()) {
            return false;
        }
    }
    return true;
}

bool I3SwayAdapter::setBorderColor(const QString &color)
{
    // i3/Sway: client.focused border bg text indicator child_border
    QString cmd = QString("client.focused %1 %1 %1 %1 %1").arg(color);
    QByteArray response = sendMessage(MsgType::RunCommand, cmd.toUtf8());

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray arr = doc.array();
    for (const auto &item : arr) {
        if (!item.toObject().value("success").toBool()) {
            return false;
        }
    }
    return true;
}

bool I3SwayAdapter::setOpacity(float opacity)
{
    // i3 doesn't support opacity natively; Sway does via window rules or compositor
    if (m_isSway) {
        QString cmd = QString("for_window [class=\".*\"] opacity %1").arg(opacity, 0, 'f', 2);
        sendMessage(MsgType::RunCommand, cmd.toUtf8());
    }
    qInfo() << "Opacity changes on i3/Sway may require compositor (picom/wlroots) support";
    return m_isSway;
}

bool I3SwayAdapter::setWorkspaceName(int id, const QString &name)
{
    QString cmd = QString("rename workspace number %1 to \"%2\"").arg(id).arg(name);
    QByteArray response = sendMessage(MsgType::RunCommand, cmd.toUtf8());

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray arr = doc.array();
    for (const auto &item : arr) {
        if (!item.toObject().value("success").toBool()) {
            return false;
        }
    }
    return true;
}

bool I3SwayAdapter::moveWindowToWorkspace(int windowId, int workspaceId)
{
    // Use con_id for precise window targeting
    QString cmd = QString("[con_id=%1] move to workspace number %2").arg(windowId).arg(workspaceId);
    QByteArray response = sendMessage(MsgType::RunCommand, cmd.toUtf8());

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray arr = doc.array();
    for (const auto &item : arr) {
        if (!item.toObject().value("success").toBool()) {
            return false;
        }
    }
    return true;
}

bool I3SwayAdapter::reloadConfig()
{
    QByteArray response = sendMessage(MsgType::RunCommand, QByteArray("reload"));
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray arr = doc.array();
    for (const auto &item : arr) {
        if (!item.toObject().value("success").toBool()) {
            return false;
        }
    }
    return true;
}

void I3SwayAdapter::startEventStream()
{
    if (!connectSocket(m_eventSocket.get())) {
        return;
    }

    QJsonArray events;
    events.append("workspace");
    events.append("window");
    events.append("output");
    if (m_isSway) {
        events.append("input");
    }

    QJsonObject payload;
    payload["events"] = events;

    QByteArray data = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    QByteArray msg = buildMessage(MsgType::Subscribe, data);
    m_eventSocket->write(msg);
    m_eventSocket->flush();

    // TODO: Connect readyRead to handleEvent for continuous event processing
}

void I3SwayAdapter::stopEventStream()
{
    if (m_eventSocket->state() == QLocalSocket::ConnectedState) {
        m_eventSocket->disconnectFromServer();
    }
}

void I3SwayAdapter::handleEvent(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    QString change = obj.value("change").toString();
    QString eventType = obj.value("event").toString();

    if (eventType == "workspace") {
        emit workspaceChanged(doc.toVariant());
    } else if (eventType == "window") {
        emit windowChanged(doc.toVariant());
    } else if (eventType == "output") {
        emit outputChanged(doc.toVariant());
    }
}

QVariant I3SwayAdapter::getInputs() const
{
    if (!m_isSway) {
        return QVariant();
    }
    // Cast away const for the non-const sendMessage
    QByteArray data = const_cast<I3SwayAdapter*>(this)->sendMessage(MsgType::GetInputs);
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

QVariant I3SwayAdapter::getSeats() const
{
    if (!m_isSway) {
        return QVariant();
    }
    QByteArray data = const_cast<I3SwayAdapter*>(this)->sendMessage(MsgType::GetSeats);
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.toVariant();
}

} // namespace keyoubs
