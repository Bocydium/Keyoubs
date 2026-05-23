#pragma once

#include "DataSource.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

namespace keyoubs {

// ─── HTTP Data Source ───
//
// Fetches JSON data from a URL on a schedule.
// Supports GET/POST, bearer token, basic auth, custom headers.
// Optional: enabled via KEYOUBES_WITH_NETWORK cmake flag.

struct HttpAuth {
    enum Type { None, Bearer, Basic };
    Type type = None;
    QString token;      // for Bearer
    QString username;   // for Basic
    QString password;   // for Basic
};

class HttpDataSource : public PollingDataSource
{
    Q_OBJECT

public:
    explicit HttpDataSource(const QString &name,
                            const QString &url,
                            int intervalMs = 60000,
                            QObject *parent = nullptr);

    QString type() const override { return QStringLiteral("http"); }

    void setUrl(const QString &url) { m_url = url; }
    void setMethod(const QString &method) { m_method = method; } // GET, POST
    void setBody(const QString &body) { m_body = body; }
    void setAuth(const HttpAuth &auth) { m_auth = auth; }
    void addHeader(const QString &key, const QString &value);

protected:
    void poll() override;

private:
    QString m_url;
    QString m_method = "GET";
    QString m_body;
    HttpAuth m_auth;
    QHash<QString, QString> m_headers;
    QNetworkAccessManager *m_network = nullptr;

    void setupRequest(QNetworkRequest &request);
};

} // namespace keyoubs
