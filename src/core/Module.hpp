#pragma once

#include <QObject>
#include <QString>
#include <QWidget>
#include <QLabel>
#include <QVariant>
#include "DataSource.hpp"

namespace keyoubs {

// ─── Module Base Class ───
//
// A Module is a visual element that can be dropped into any component.
// It has a data source that feeds it live data, and it renders that data.
//
// Examples: ClockModule, BatteryModule, WorkspacePipsModule, etc.

class Module : public QWidget
{
    Q_OBJECT

public:
    explicit Module(const QString &name, QWidget *parent = nullptr);

    [[nodiscard]] QString moduleName() const { return m_name; }
    [[nodiscard]] virtual QString moduleType() const = 0;

    // Data source binding
    void setDataSource(DataSource *source);
    [[nodiscard]] DataSource *dataSource() const { return m_source; }

    // Called when the module should update its visual state
    virtual void updateDisplay(const QVariant &value) = 0;

    // Minimum / preferred size hints for layout
    [[nodiscard]] virtual QSize moduleSizeHint() const { return QSize(80, 32); }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    QString m_name;
    DataSource *m_source = nullptr;
};

// ─── Pre-built Modules ───

class ClockModule : public Module
{
    Q_OBJECT
public:
    explicit ClockModule(QWidget *parent = nullptr);
    QString moduleType() const override { return QStringLiteral("clock"); }
    void updateDisplay(const QVariant &value) override;
    QSize moduleSizeHint() const override { return QSize(80, 32); }

private:
    QLabel *m_label = nullptr;
};

class BatteryModule : public Module
{
    Q_OBJECT
public:
    explicit BatteryModule(QWidget *parent = nullptr);
    QString moduleType() const override { return QStringLiteral("battery"); }
    void updateDisplay(const QVariant &value) override;
    QSize moduleSizeHint() const override { return QSize(60, 32); }

private:
    QLabel *m_label = nullptr;
    int m_percentage = 0;
    bool m_charging = false;
};

class WorkspacePipsModule : public Module
{
    Q_OBJECT
public:
    explicit WorkspacePipsModule(QWidget *parent = nullptr);
    QString moduleType() const override { return QStringLiteral("workspaces"); }
    void updateDisplay(const QVariant &value) override;
    QSize moduleSizeHint() const override { return QSize(120, 32); }

    void setWorkspaceCount(int count) { m_count = count; update(); }
    void setActiveWorkspace(int id) { m_activeId = id; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_count = 5;
    int m_activeId = 1;
};

} // namespace keyoubs
