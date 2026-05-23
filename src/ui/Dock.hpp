#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QList>
#include <QIcon>

namespace keyoubs {

// ─── Dock Surface ───
//
// A persistent or auto-hiding application dock.
// Shows pinned and running applications as icons.
// Anchored to a screen edge.

class DockItem : public QWidget
{
    Q_OBJECT

public:
    explicit DockItem(const QString &appId, const QIcon &icon, QWidget *parent = nullptr);

    [[nodiscard]] QString appId() const { return m_appId; }
    void setRunning(bool running);
    void setIcon(const QIcon &icon);

signals:
    void clicked(const QString &appId);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString m_appId;
    QIcon m_icon;
    bool m_running = false;
    bool m_hovered = false;
    int m_iconSize = 40;
};

class Dock : public QWidget
{
    Q_OBJECT

public:
    explicit Dock(QWidget *parent = nullptr);

    void addItem(const QString &appId, const QIcon &icon);
    void removeItem(const QString &appId);
    void setItemRunning(const QString &appId, bool running);
    void clearItems();

    void setIconSize(int size);
    void setOrientation(Qt::Orientation orientation);

signals:
    void itemClicked(const QString &appId);

private:
    QBoxLayout *m_layout = nullptr;
    QList<DockItem*> m_items;
    int m_iconSize = 40;
    Qt::Orientation m_orientation = Qt::Horizontal;
};

} // namespace keyoubs
