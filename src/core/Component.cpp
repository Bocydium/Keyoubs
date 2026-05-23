#include "Component.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

namespace keyoubs {

Component::Component(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
}

QRect Component::geometry() const
{
    return m_geometry;
}

void Component::setGeometry(const QRect &rect)
{
    if (m_geometry == rect) return;
    m_geometry = rect;
    emit geometryChanged(rect);
}

void Component::addSubComponent(Component *comp)
{
    if (!comp || m_subComponents.contains(comp)) return;
    m_subComponents.append(comp);
    comp->setParent(this);
}

void Component::removeSubComponent(Component *comp)
{
    m_subComponents.removeAll(comp);
}

QString Component::componentsDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/keyoubs/components";
}

bool Component::save() const
{
    QString dir = componentsDir() + "/" + m_name;
    QDir().mkpath(dir);

    QString path = dir + "/component.json";
    QJsonDocument doc(toJson());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save component:" << path;
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool Component::load()
{
    QString path = componentsDir() + "/" + m_name + "/component.json";
    if (!QFile::exists(path)) {
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return loadFromJson(doc.object());
}

bool Component::loadFromJson(const QJsonObject &obj)
{
    m_type = obj.value("type").toString();

    QJsonObject geo = obj.value("geometry").toObject();
    m_geometry = QRect(
        geo.value("x").toInt(),
        geo.value("y").toInt(),
        geo.value("width").toInt(),
        geo.value("height").toInt()
    );

    m_layer = static_cast<SurfaceLayer>(obj.value("layer").toInt(2));
    m_anchor = static_cast<SurfaceAnchor>(obj.value("anchor").toInt(1));
    m_monitor = obj.value("monitor").toInt(-1);
    m_autostart = obj.value("autostart").toBool(false);

    QJsonObject style = obj.value("styleOverrides").toObject();
    m_styleOverrides = style.toVariantMap();

    QJsonObject behavior = obj.value("behavior").toObject();
    m_behavior = behavior.toVariantMap();

    // Load sub-components
    QJsonArray subs = obj.value("subComponents").toArray();
    for (const auto &subVal : subs) {
        QJsonObject subObj = subVal.toObject();
        QString subName = subObj.value("name").toString();
        if (!subName.isEmpty()) {
            auto *sub = new Component(subName, this);
            sub->loadFromJson(subObj);
            m_subComponents.append(sub);
        }
    }

    return true;
}

QJsonObject Component::toJson() const
{
    QJsonObject obj;
    obj["name"] = m_name;
    obj["type"] = m_type;

    QJsonObject geo;
    geo["x"] = m_geometry.x();
    geo["y"] = m_geometry.y();
    geo["width"] = m_geometry.width();
    geo["height"] = m_geometry.height();
    obj["geometry"] = geo;

    obj["layer"] = static_cast<int>(m_layer);
    obj["anchor"] = static_cast<int>(m_anchor);
    obj["monitor"] = m_monitor;
    obj["autostart"] = m_autostart;

    obj["styleOverrides"] = QJsonObject::fromVariantMap(m_styleOverrides);
    obj["behavior"] = QJsonObject::fromVariantMap(m_behavior);

    QJsonArray subs;
    for (const auto *sub : m_subComponents) {
        subs.append(sub->toJson());
    }
    obj["subComponents"] = subs;

    return obj;
}

QList<Component*> Component::loadAllComponents(QObject *parent)
{
    QList<Component*> components;
    QDir dir(componentsDir());

    QStringList names = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &name : names) {
        auto *comp = new Component(name, parent);
        if (comp->load()) {
            components.append(comp);
        } else {
            delete comp;
        }
    }

    return components;
}

} // namespace keyoubs
