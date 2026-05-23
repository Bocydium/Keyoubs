#include "DataSource.hpp"
#include <QDebug>

namespace keyoubs {

// ─── DataSource ───

DataSource::DataSource(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
}

void DataSource::setValue(const QVariant &value)
{
    if (m_value == value) return;
    m_value = value;
    emit valueChanged(value);
}

// ─── PollingDataSource ───

PollingDataSource::PollingDataSource(const QString &name, int intervalMs, QObject *parent)
    : DataSource(name, parent)
{
    m_timer.setInterval(intervalMs);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        if (m_active) poll();
    });
}

void PollingDataSource::start()
{
    m_active = true;
    poll(); // immediate first poll
    m_timer.start();
}

void PollingDataSource::stop()
{
    m_active = false;
    m_timer.stop();
}

void PollingDataSource::setInterval(int ms)
{
    m_timer.setInterval(ms);
}

// ─── DataSourceRegistry ───

DataSourceRegistry::DataSourceRegistry(QObject *parent)
    : QObject(parent)
{
}

void DataSourceRegistry::registerSource(DataSource *source)
{
    if (!source || m_sources.contains(source->sourceName())) return;
    m_sources[source->sourceName()] = source;
    emit sourceRegistered(source->sourceName());
}

void DataSourceRegistry::unregisterSource(const QString &name)
{
    if (!m_sources.contains(name)) return;
    m_sources.remove(name);
    emit sourceUnregistered(name);
}

DataSource *DataSourceRegistry::source(const QString &name) const
{
    return m_sources.value(name, nullptr);
}

QList<DataSource*> DataSourceRegistry::allSources() const
{
    return m_sources.values();
}

void DataSourceRegistry::startAll()
{
    for (auto *src : m_sources) {
        src->start();
    }
}

void DataSourceRegistry::stopAll()
{
    for (auto *src : m_sources) {
        src->stop();
    }
}

} // namespace keyoubs
