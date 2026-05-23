#include "OverlayWindow.hpp"
#include "Toolbar.hpp"
#include "PropertyPanel.hpp"
#include "Canvas.hpp"
#include "../core/LivePreview.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QDebug>

#ifdef KEYOUBES_HAS_LAYER_SHELL
#include <LayerShellQt/Window>
#endif

namespace keyoubs {

OverlayWindow::OverlayWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(new QWidget(this))
    , m_mainLayout(new QHBoxLayout(m_centralWidget))
    , m_toolbar(new Toolbar(this))
    , m_canvas(new Canvas(this))
    , m_propertyPanel(new PropertyPanel(this))
    , m_settings("keyoubs", "Keyoubs")
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0x0D, 0x0D, 0x12));
    setPalette(pal);

    setCentralWidget(m_centralWidget);
    m_centralWidget->setLayout(m_mainLayout);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setupUI();
    setupLayerShell();
    updateGeometryForScreens();
    restoreLayout();
    positionPanels();

    // Connect screen changes
    connect(qApp, &QApplication::screenAdded, this, &OverlayWindow::updateGeometryForScreens);
    connect(qApp, &QApplication::screenRemoved, this, &OverlayWindow::updateGeometryForScreens);
    for (QScreen *screen : QApplication::screens()) {
        connect(screen, &QScreen::geometryChanged, this, &OverlayWindow::updateGeometryForScreens);
    }
}

OverlayWindow::~OverlayWindow()
{
    saveLayout();
}

void OverlayWindow::setupUI()
{
    // Canvas fills center
    m_mainLayout->addWidget(m_canvas, 1);

    // Property panel on right (initially)
    m_mainLayout->addWidget(m_propertyPanel);

    // Connect signals
    connect(m_toolbar, &Toolbar::closeClicked, this, &OverlayWindow::closeRequested);
    connect(m_toolbar, &Toolbar::applyClicked, this, &OverlayWindow::applyRequested);
    connect(m_toolbar, &Toolbar::discardClicked, this, &OverlayWindow::discardRequested);
    connect(m_toolbar, &Toolbar::toolSelected, m_canvas, &Canvas::setActiveTool);
    connect(m_propertyPanel, &PropertyPanel::propertyChanged, m_canvas, &Canvas::onPropertyChanged);
    connect(m_canvas, &Canvas::selectionChanged, m_propertyPanel, &PropertyPanel::onSelectionChanged);

    // Panel position changes trigger re-layout
    connect(m_toolbar, &Toolbar::positionChanged, this, &OverlayWindow::positionPanels);
    connect(m_propertyPanel, &PropertyPanel::positionChanged, this, &OverlayWindow::positionPanels);
}

void OverlayWindow::setupLayerShell()
{
#ifdef KEYOUBES_HAS_LAYER_SHELL
    if (windowHandle()) {
        auto *layerWindow = LayerShellQt::Window::get(windowHandle());
        if (layerWindow) {
            layerWindow->setLayer(LayerShellQt::Window::LayerTop);
            layerWindow->setAnchors({LayerShellQt::Window::AnchorTop,
                                     LayerShellQt::Window::AnchorBottom,
                                     LayerShellQt::Window::AnchorLeft,
                                     LayerShellQt::Window::AnchorRight});
            layerWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
        }
    }
#else
    // GNOME fallback: standard borderless window on top
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
#endif
}

void OverlayWindow::updateGeometryForScreens()
{
    // Calculate bounding rect of all screens
    QRect totalGeo;
    for (QScreen *screen : QApplication::screens()) {
        totalGeo = totalGeo.united(screen->geometry());
    }
    if (!totalGeo.isValid()) {
        totalGeo = QApplication::primaryScreen()->geometry();
    }

    setGeometry(totalGeo);
    positionPanels();
}

void OverlayWindow::positionPanels()
{
    QRect geo = geometry();

    // Toolbar positioning
    if (m_toolbar->position() == Toolbar::Position::Top) {
        m_toolbar->setParent(m_centralWidget);
        m_toolbar->move((geo.width() - m_toolbar->width()) / 2, 0);
    } else if (m_toolbar->position() == Toolbar::Position::Bottom) {
        m_toolbar->setParent(m_centralWidget);
        m_toolbar->move((geo.width() - m_toolbar->width()) / 2, geo.height() - m_toolbar->height());
    } else {
        // Floating: keep as top-level or child, centered at top by default
        m_toolbar->setParent(m_centralWidget);
        m_toolbar->move((geo.width() - m_toolbar->width()) / 2, 12);
    }
    m_toolbar->raise();
    m_toolbar->setVisible(m_toolbarVisible);

    // Property panel positioning in layout
    if (m_propertyPanel->position() == PropertyPanel::Position::Left) {
        m_mainLayout->removeWidget(m_propertyPanel);
        m_mainLayout->insertWidget(0, m_propertyPanel);
        m_propertyPanel->setParent(m_centralWidget);
    } else if (m_propertyPanel->position() == PropertyPanel::Position::Right) {
        m_mainLayout->removeWidget(m_propertyPanel);
        m_mainLayout->addWidget(m_propertyPanel);
        m_propertyPanel->setParent(m_centralWidget);
    } else {
        // Floating: remove from layout and position freely
        m_mainLayout->removeWidget(m_propertyPanel);
        m_propertyPanel->setParent(m_centralWidget);
        m_propertyPanel->move(geo.width() - m_propertyPanel->width() - 20, 60);
    }
    m_propertyPanel->raise();
    m_propertyPanel->setVisible(m_panelVisible);
}

void OverlayWindow::saveLayout()
{
    m_settings.setValue("toolbar/position", static_cast<int>(m_toolbar->position()));
    m_settings.setValue("toolbar/visible", m_toolbarVisible);
    m_settings.setValue("panel/position", static_cast<int>(m_propertyPanel->position()));
    m_settings.setValue("panel/visible", m_panelVisible);
}

void OverlayWindow::restoreLayout()
{
    int toolbarPos = m_settings.value("toolbar/position", static_cast<int>(Toolbar::Position::Top)).toInt();
    m_toolbar->setPosition(static_cast<Toolbar::Position>(toolbarPos));
    m_toolbarVisible = m_settings.value("toolbar/visible", true).toBool();

    int panelPos = m_settings.value("panel/position", static_cast<int>(PropertyPanel::Position::Right)).toInt();
    m_propertyPanel->setPosition(static_cast<PropertyPanel::Position>(panelPos));
    m_panelVisible = m_settings.value("panel/visible", true).toBool();
}

void OverlayWindow::toggleToolbarVisibility()
{
    m_toolbarVisible = !m_toolbarVisible;
    m_toolbar->setVisible(m_toolbarVisible);
}

void OverlayWindow::togglePanelVisibility()
{
    m_panelVisible = !m_panelVisible;
    m_propertyPanel->setVisible(m_panelVisible);
}

void OverlayWindow::setCompositorName(const QString &name)
{
    m_toolbar->setCompositorName(name);
}

void OverlayWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    positionPanels();
}

void OverlayWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        emit closeRequested();
        break;
    case Qt::Key_F1:
        toggleToolbarVisibility();
        break;
    case Qt::Key_F2:
        togglePanelVisibility();
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void OverlayWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    positionPanels();
}

void OverlayWindow::closeEvent(QCloseEvent *event)
{
    saveLayout();
    QMainWindow::closeEvent(event);
}

} // namespace keyoubs
