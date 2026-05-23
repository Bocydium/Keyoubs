#include "Module.hpp"
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>

namespace keyoubs {

// ─── Module Base ───

Module::Module(const QString &name, QWidget *parent)
    : QWidget(parent)
    , m_name(name)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void Module::setDataSource(DataSource *source)
{
    if (m_source) {
        disconnect(m_source, &DataSource::valueChanged, this, nullptr);
    }
    m_source = source;
    if (m_source) {
        connect(m_source, &DataSource::valueChanged, this, &Module::updateDisplay);
    }
}

void Module::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
}

// ─── Clock Module ───

ClockModule::ClockModule(QWidget *parent)
    : Module("clock", parent)
{
    m_label = new QLabel("00:00", this);
    m_label->setAlignment(Qt::AlignCenter);
    QFont f = m_label->font();
    f.setPointSize(10);
    f.setWeight(QFont::Medium);
    m_label->setFont(f);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 0, 8, 0);
    layout->addWidget(m_label);

    setFixedSize(moduleSizeHint());
}

void ClockModule::updateDisplay(const QVariant &value)
{
    Q_UNUSED(value)
    QString time = QDateTime::currentDateTime().toString("HH:mm");
    m_label->setText(time);
}

// ─── Battery Module ───

BatteryModule::BatteryModule(QWidget *parent)
    : Module("battery", parent)
{
    m_label = new QLabel("--%", this);
    m_label->setAlignment(Qt::AlignCenter);
    QFont f = m_label->font();
    f.setPointSize(9);
    m_label->setFont(f);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 0, 6, 0);
    layout->addWidget(m_label);

    setFixedSize(moduleSizeHint());
}

void BatteryModule::updateDisplay(const QVariant &value)
{
    QVariantMap map = value.toMap();
    m_percentage = map.value("Percentage").toInt();
    uint state = map.value("State").toUInt();
    m_charging = (state == 1);

    QString icon = m_charging ? "⚡" : "";
    m_label->setText(QString("%1%2%").arg(icon).arg(m_percentage));
}

// ─── Workspace Pips Module ───

WorkspacePipsModule::WorkspacePipsModule(QWidget *parent)
    : Module("workspaces", parent)
{
    setFixedSize(moduleSizeHint());
}

void WorkspacePipsModule::updateDisplay(const QVariant &value)
{
    QVariantList workspaces = value.toList();
    m_count = workspaces.size();
    if (m_count == 0) m_count = 5;

    for (const auto &ws : workspaces) {
        QVariantMap map = ws.toMap();
        if (map.value("focused", false).toBool()) {
            m_activeId = map.value("id", 1).toInt();
            break;
        }
    }

    update();
}

void WorkspacePipsModule::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int pipSize = 8;
    int gap = 6;
    int totalWidth = m_count * pipSize + (m_count - 1) * gap;
    int startX = (width() - totalWidth) / 2;
    int startY = (height() - pipSize) / 2;

    for (int i = 0; i < m_count; ++i) {
        QRect pip(startX + i * (pipSize + gap), startY, pipSize, pipSize);
        bool isActive = (i + 1 == m_activeId);

        p.setPen(Qt::NoPen);
        p.setBrush(isActive ? QColor(0x62, 0xA9, 0xE3) : QColor(0x3A, 0x3A, 0x3A));
        p.drawEllipse(pip);
    }
}

} // namespace keyoubs
