#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include <QColor>

namespace keyoubs {

// ─── Window Urgency ───
//
// Niri 25.05 added window urgency — apps signal they need attention.
// This class tracks urgency state per window and provides styling rules.

struct UrgencyStyle {
    QColor borderColor = QColor(0xE3, 0x62, 0x62);
    bool pulseAnimation = true;
    bool autoFocus = false;
    bool statusBarBadge = true;
};

class WindowUrgency : public QObject
{
    Q_OBJECT

public:
    explicit WindowUrgency(QObject *parent = nullptr);

    void setUrgent(const QString &windowId, bool urgent);
    [[nodiscard]] bool isUrgent(const QString &windowId) const;
    [[nodiscard]] int urgentCount() const;
    [[nodiscard]] QStringList urgentWindows() const;

    void setGlobalStyle(const UrgencyStyle &style) { m_style = style; }
    [[nodiscard]] UrgencyStyle globalStyle() const { return m_style; }

    // Urgency per-app rules
    void setAppUrgencyRule(const QString &appId, const UrgencyStyle &rule);
    [[nodiscard]] UrgencyStyle ruleForApp(const QString &appId) const;

    void clearAll();

signals:
    void windowUrgencyChanged(const QString &windowId, bool urgent);
    void urgentCountChanged(int count);

private:
    QHash<QString, bool> m_urgentWindows;
    UrgencyStyle m_style;
    QHash<QString, UrgencyStyle> m_appRules;
};

} // namespace keyoubs
