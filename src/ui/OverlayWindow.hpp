#pragma once

#include "Toolbar.hpp"
#include "PropertyPanel.hpp"
#include "../models/ConfigModel.hpp"
#include <QMainWindow>
#include <QSettings>
#include <memory>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QScreen;
QT_END_NAMESPACE

namespace LayerShellQt {
class Window;
}

namespace keyoubs {

class Canvas;
class LivePreview;

// The main Keyoubs overlay window.
// Spans all monitors, supports draggable panels, ESC to quit,
// and persists layout via QSettings.
class OverlayWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OverlayWindow(QWidget *parent = nullptr);
    ~OverlayWindow() override;

    void setCompositorName(const QString &name);

signals:
    void applyRequested();
    void discardRequested();
    void closeRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUI();
    void setupLayerShell();
    void positionPanels();
    void saveLayout();
    void restoreLayout();
    void updateGeometryForScreens();
    void toggleToolbarVisibility();
    void togglePanelVisibility();

    QWidget *m_centralWidget;
    QHBoxLayout *m_mainLayout;

    Toolbar *m_toolbar;
    Canvas *m_canvas;
    PropertyPanel *m_propertyPanel;

    ConfigModel m_currentModel;
    bool m_pendingChanges = false;

    QSettings m_settings;
    bool m_toolbarVisible = true;
    bool m_panelVisible = true;
};

} // namespace keyoubs
