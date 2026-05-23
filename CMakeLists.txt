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
    DBus
)

# LayerShellQt is optional — works on wlroots/Smithay compositors,
# gracefully degrades on GNOME (standard window instead of overlay)
find_package(LayerShellQt QUIET)

set(KEYOUBES_SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/ConfigManager.cpp
    src/core/LivePreview.cpp
    src/core/ThemeConfig.cpp
    src/core/LuaEngine.cpp
    src/core/CommandManager.cpp
    src/core/ConfigBackup.cpp
    src/core/Component.cpp
    src/core/DataSource.cpp
    src/core/DBusSource.cpp
    src/core/ShellSource.cpp
    src/core/FileSource.cpp
    src/core/Module.cpp
    src/core/LayoutPreset.cpp
    src/core/IpcLog.cpp
    src/core/ConfigDiff.cpp
    src/core/IpcWorker.cpp
    src/core/DebouncedWriter.cpp
    src/core/EventCoalescer.cpp
    src/core/HyprlandLuaConfig.cpp
    src/core/SubmapEditor.cpp
    src/core/DeviceManager.cpp
    src/core/GestureEditor.cpp
    src/core/WindowUrgency.cpp
    src/core/ScreenShareRule.cpp
    src/core/PrivacyIndicator.cpp
    src/core/PolkitAgent.cpp
    src/core/ProcessModule.cpp
    src/core/HttpDataSource.cpp
    src/core/DBusMethodAction.cpp
    src/ipc/IpcAdapter.cpp
    src/ipc/NiriAdapter.cpp
    src/ipc/HyprlandAdapter.cpp
    src/ipc/I3SwayAdapter.cpp
    src/ui/OverlayWindow.cpp
    src/ui/Toolbar.cpp
    src/ui/PropertyPanel.cpp
    src/ui/Canvas.cpp
    src/ui/PhosphorIcons.cpp
    src/ui/StatusBar.cpp
    src/ui/Dock.cpp
    src/ui/NiriCanvas.cpp
    src/ui/NiriBlurEditor.cpp
    src/models/ConfigModel.cpp
)

set(KEYOUBES_HEADERS
    src/core/Application.hpp
    src/core/ConfigManager.hpp
    src/core/LivePreview.hpp
    src/core/ThemeConfig.hpp
    src/core/LuaEngine.hpp
    src/core/CommandManager.hpp
    src/core/ConfigBackup.hpp
    src/core/Component.hpp
    src/core/DataSource.hpp
    src/core/DBusSource.hpp
    src/core/ShellSource.hpp
    src/core/FileSource.hpp
    src/core/Module.hpp
    src/core/LayoutPreset.hpp
    src/core/IpcLog.hpp
    src/core/ConfigDiff.hpp
    src/core/IpcWorker.hpp
    src/core/DebouncedWriter.hpp
    src/core/EventCoalescer.hpp
    src/core/HyprlandLuaConfig.hpp
    src/core/SubmapEditor.hpp
    src/core/DeviceManager.hpp
    src/core/GestureEditor.hpp
    src/core/WindowUrgency.hpp
    src/core/ScreenShareRule.hpp
    src/core/PrivacyIndicator.hpp
    src/core/PolkitAgent.hpp
    src/core/ProcessModule.hpp
    src/core/HttpDataSource.hpp
    src/core/DBusMethodAction.hpp
    src/ipc/IpcAdapter.hpp
    src/ipc/NiriAdapter.hpp
    src/ipc/HyprlandAdapter.hpp
    src/ipc/I3SwayAdapter.hpp
    src/ui/OverlayWindow.hpp
    src/ui/Toolbar.hpp
    src/ui/PropertyPanel.hpp
    src/ui/Canvas.hpp
    src/ui/PhosphorIcons.hpp
    src/ui/StatusBar.hpp
    src/ui/Dock.hpp
    src/ui/NiriCanvas.hpp
    src/ui/NiriBlurEditor.hpp
    src/models/ConfigModel.hpp
)

add_executable(keyoubs ${KEYOUBES_SOURCES} ${KEYOUBES_HEADERS} resources/resources.qrc)

target_include_directories(keyoubs PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Lua support (optional but recommended for scripting)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA lua-5.5)
if(NOT LUA_FOUND)
    pkg_check_modules(LUA lua5.5)
endif()
if(NOT LUA_FOUND)
    pkg_check_modules(LUA lua5.4)
endif()
if(NOT LUA_FOUND)
    pkg_check_modules(LUA lua-5.4)
endif()

target_link_libraries(keyoubs PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Svg
    Qt6::DBus
)

if(LUA_FOUND)
    target_include_directories(keyoubs PRIVATE ${LUA_INCLUDE_DIRS})
    target_link_libraries(keyoubs PRIVATE ${LUA_LIBRARIES})
    target_compile_definitions(keyoubs PRIVATE KEYOUBES_HAS_LUA=1)
    target_compile_options(keyoubs PRIVATE ${LUA_CFLAGS_OTHER})
else()
    message(WARNING "Lua not found. Scripting support disabled.")
endif()

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
