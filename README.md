# Keyoubs

A visual GUI editor for tiling window managers — like Figma, but for your desktop layout. Plan, tweak, and preview your Niri, Hyprland, Sway, or i3 configuration in real time with a live overlay right on top of your workspace.

![Branding](branding.png)

## Features

- **Visual Layout Editor** — Drag windows, adjust gaps, and tweak borders on a live canvas
- **Live Desktop Overlay** — See your changes in real time while you edit
- **Compositor Support** — Niri, Hyprland, Sway, and i3 adapters with IPC integration
- **Hot-Reloadable Themes** — Customize colors, sizes, and fonts via JSON configs (no rebuild needed)
- **Lua Scripting** — Write `~/.config/keyoubs/scripts/init.lua` to add hooks and automation
- **Wayland + X11** — Built for Wayland with graceful fallback on X11/i3
- **Clean, Fast UI** — Figma-style monochrome dark theme with hardware-accelerated Qt6 rendering

## Dependencies

| Package | Required | Notes |
|---------|----------|-------|
| `cmake` | Yes | >= 3.20 |
| `qt6-base` | Yes | Widgets, Core, Gui, Network, Svg |
| `qt6-tools` | Yes | For build tools |
| `gcc` / `clang` | Yes | C++20 support |
| `lua` | Optional | 5.4+ for scripting support |
| `layer-shell-qt` | Optional | Overlay mode on wlroots/Smithay compositors |

### Arch Linux

```bash
sudo pacman -S --needed cmake gcc qt6-base qt6-tools
# Optional: for overlay mode on supported compositors
sudo pacman -S --needed layer-shell-qt
# Optional: for Lua scripting support
sudo pacman -S --needed lua
```

### Other Distros

Install the equivalent packages for your distribution (`qt6-base-dev`, `cmake`, etc.).

## Building

```bash
git clone <repo-url>
cd KEYOUBES
mkdir build && cd build
cmake ..
make -j$(nproc)
```

The binary will be at `./keyoubs`.

## Usage

```bash
./keyoubs
```

- **F1** — Toggle toolbar visibility
- **F2** — Toggle property panel visibility
- **Escape** — Close

Drag the toolbar or property panel to snap them to edges, or leave them floating.

## Supported Compositors

| Compositor | Status | Overlay Mode |
|------------|--------|--------------|
| **Niri** | IPC adapter ready | Layer shell (if available) |
| **Hyprland** | IPC adapter ready | Layer shell (if available) |
| **Sway** | IPC adapter ready | Layer shell (if available) |
| **i3** | IPC adapter ready | Standard top-level window (X11) |
| **GNOME / others** | Fallback window mode | Standard top-level window |

## Project Structure

```
KEYOUBES/
├── src/
│   ├── core/          # Application, ConfigManager, LivePreview, ThemeConfig, LuaEngine
│   ├── ipc/           # IpcAdapter, NiriAdapter, HyprlandAdapter, I3SwayAdapter
│   ├── models/        # ConfigModel
│   ├── ui/            # OverlayWindow, Toolbar, PropertyPanel, Canvas, PhosphorIcons
│   └── main.cpp
├── resources/         # Icons and assets
├── CMakeLists.txt
└── build/             # Build output (gitignored)
```

## Tech Stack

- **C++20** — Core language
- **Qt6** — GUI framework (Widgets, Network, Svg)
- **CMake** — Build system
- **LayerShellQt** — Optional Wayland layer-shell integration
- **Lua** — Optional scripting engine

## Customization

Keyoubs supports Quickshell-inspired customization through JSON config files:

### Theme (`~/.config/keyoubs/theme.json`)
```json
{
  "colors": {
    "bg": "#101010",
    "surface": "#1A1A1A",
    "accent": "#62A9E3"
  },
  "sizes": {
    "toolbarHeight": 52,
    "toolButton": 40,
    "borderRadius": 8
  },
  "font": {
    "family": "Inter",
    "size": 10
  }
}
```
Changes are applied instantly without restarting.

### Layout (`~/.config/keyoubs/layout.json`)
Controls toolbar position, visible tools, panel placement, etc.

### Lua Scripts (`~/.config/keyoubs/scripts/init.lua`)
```lua
function onToolSelected(tool)
    keyoubs.log("info", "Tool: " .. tool)
end

function onPropertyChanged(key, value)
    keyoubs.notify("Keyoubs", key .. " = " .. tostring(value))
end
```

## License

TBD
