cmake_minimum_required(VERSION 3.20)
project(Keyoubs VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Core
    Gui
    Network
    Svg
)

# LayerShellQt is optional — works on wlroots/Smithay compositors,
# gracefully degrades on GNOME (standard window instead of overlay)
find_package(LayerShellQt QUIET)

set(KEYOUBES_SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/ConfigManager.cpp
    src/core/LivePreview.cpp
    src/ipc/IpcAdapter.cpp
    src/ipc/NiriAdapter.cpp
    src/ipc/HyprlandAdapter.cpp
    src/ui/OverlayWindow.cpp
    src/ui/Toolbar.cpp
    src/ui/PropertyPanel.cpp
    src/ui/Canvas.cpp
    src/ui/PhosphorIcons.cpp
    src/models/ConfigModel.cpp
)

set(KEYOUBES_HEADERS
    src/core/Application.hpp
    src/core/ConfigManager.hpp
    src/core/LivePreview.hpp
    src/ipc/IpcAdapter.hpp
    src/ipc/NiriAdapter.hpp
    src/ipc/HyprlandAdapter.hpp
    src/ui/OverlayWindow.hpp
    src/ui/Toolbar.hpp
    src/ui/PropertyPanel.hpp
    src/ui/Canvas.hpp
    src/ui/PhosphorIcons.hpp
    src/models/ConfigModel.hpp
)

add_executable(keyoubs ${KEYOUBES_SOURCES} ${KEYOUBES_HEADERS} resources/resources.qrc)

target_include_directories(keyoubs PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(keyoubs PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Svg
)

if(LayerShellQt_FOUND)
    target_link_libraries(keyoubs PRIVATE LayerShellQt::Interface)
    target_compile_definitions(keyoubs PRIVATE KEYOUBES_HAS_LAYER_SHELL=1)
endif()

target_compile_options(keyoubs PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O2
)

install(TARGETS keyoubs DESTINATION bin)
