#include "HttpDataSource.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace keyoubs {

HttpDataSource::HttpDataSource(const QString &name,
                                const QString &url,
                                int intervalMs,
                                QObject *parent)
    : PollingDataSource(name, intervalMs, parent)
    , m_url(url)
    , m_network(new QNetworkAccessManager(this))
{
}

void HttpDataSource::addHeader(const QString &key, const QString &value)
{
    m_headers[key] = value;
}

void HttpDataSource::setupRequest(QNetworkRequest &request)
{
    // Auth
    switch (m_auth.type) {
    case HttpAuth::Bearer:
        request.setRawHeader("Authorization", "Bearer " + m_auth.token.toUtf8());
        break;
    case HttpAuth::Basic: {
        QString credentials = m_auth.username + ":" + m_auth.password;
        request.setRawHeader("Authorization", "Basic " + credentials.toUtf8().toBase64());
        break;
    }
    default:
        break;
    }

    // Custom headers
    for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
}

void HttpDataSource::poll()
{
    QNetworkRequest request{QUrl(m_url)};
    setupRequest(request);

    QNetworkReply *reply = nullptr;
    if (m_method == "POST") {
        reply = m_network->post(request, m_body.toUtf8());
    } else {
        reply = m_network->get(request);
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit error(QString("HTTP error: %1").arg(reply->errorString()));
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull()) {
            setValue(doc.toVariant());
        } else {
            setValue(QString::fromUtf8(data));
        }

        reply->deleteLater();
    });
}

} // namespace keyoubs
