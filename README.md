# Keyoubs

A visual GUI editor for tiling window managers — like Figma, but for your desktop layout. Plan, tweak, and preview your Niri, Hyprland, or TWM configuration in real time with a live overlay right on top of your workspace.

![Branding](branding.png)

## Features

- **Visual Layout Editor** — Drag windows, adjust gaps, and tweak borders on a live canvas
- **Live Desktop Overlay** — See your changes in real time while you edit
- **Compositor Support** — Niri and Hyprland adapters with IPC integration
- **Wayland Native** — Built for Wayland with graceful fallback on X11/GNOME
- **Clean, Fast UI** — Solid dark theme with a blue accent, hardware-accelerated Qt6 rendering

## Dependencies

| Package | Required | Notes |
|---------|----------|-------|
| `cmake` | Yes | >= 3.20 |
| `qt6-base` | Yes | Widgets, Core, Gui, Network |
| `qt6-tools` | Yes | For build tools |
| `gcc` / `clang` | Yes | C++20 support |
| `layer-shell-qt` | Optional | Overlay mode on wlroots/Smithay compositors |

### Arch Linux

```bash
sudo pacman -S --needed cmake gcc qt6-base qt6-tools
# Optional: for overlay mode on supported compositors
sudo pacman -S --needed layer-shell-qt
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
| **GNOME / others** | Fallback window mode | Standard top-level window |

## Project Structure

```
KEYOUBES/
├── src/
│   ├── core/          # Application, ConfigManager, LivePreview
│   ├── ipc/           # IpcAdapter, NiriAdapter, HyprlandAdapter
│   ├── models/        # ConfigModel
│   ├── ui/            # OverlayWindow, Toolbar, PropertyPanel, Canvas
│   └── main.cpp
├── resources/         # Icons and assets
├── CMakeLists.txt
└── build/             # Build output (gitignored)
```

## Tech Stack

- **C++20** — Core language
- **Qt6** — GUI framework (Widgets, Network)
- **CMake** — Build system
- **LayerShellQt** — Optional Wayland layer-shell integration

## License

GNU GPL V3.0
