#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <QHash>

namespace keyoubs {

// ─── Data Source Base Class ───
//
// Abstract base for all data sources that feed into Keyoubs components.
// Data sources are polled or event-driven and emit valueChanged when data updates.
//
// Types: DBusSource, ShellSource, FileSource, HttpSource

class DataSource : public QObject
{
    Q_OBJECT

public:
    explicit DataSource(const QString &name, QObject *parent = nullptr);
    virtual ~DataSource() = default;

    [[nodiscard]] QString sourceName() const { return m_name; }
    [[nodiscard]] bool isActive() const { return m_active; }

    virtual void start() = 0;
    virtual void stop() = 0;

    // Latest value from this source
    [[nodiscard]] QVariant currentValue() const { return m_value; }

    // Source type identifier
    [[nodiscard]] virtual QString type() const = 0;

signals:
    void valueChanged(const QVariant &value);
    void error(const QString &message);

protected:
    void setValue(const QVariant &value);

    QString m_name;
    QVariant m_value;
    bool m_active = false;
};

// ─── Polling Data Source ───
// Base for sources that need periodic refresh

class PollingDataSource : public DataSource
{
    Q_OBJECT

public:
    explicit PollingDataSource(const QString &name, int intervalMs = 1000, QObject *parent = nullptr);

    void start() override;
    void stop() override;
    void setInterval(int ms);

protected:
    virtual void poll() = 0;

private:
    QTimer m_timer;
};

// ─── Data Source Registry ───
// Global registry for looking up and managing data sources

class DataSourceRegistry : public QObject
{
    Q_OBJECT

public:
    explicit DataSourceRegistry(QObject *parent = nullptr);

    void registerSource(DataSource *source);
    void unregisterSource(const QString &name);
    [[nodiscard]] DataSource *source(const QString &name) const;
    [[nodiscard]] QList<DataSource*> allSources() const;

    void startAll();
    void stopAll();

signals:
    void sourceRegistered(const QString &name);
    void sourceUnregistered(const QString &name);

private:
    QHash<QString, DataSource*> m_sources;
};

} // namespace keyoubs
