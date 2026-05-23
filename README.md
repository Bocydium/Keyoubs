# Keyoubs

Build your own desktop environment visually. Like Figma, but for your Linux desktop.

Design status bars, docks, widgets, and edit your compositor layout — all with a live canvas. Every change happens in real time. No config files required.

![Branding](branding.png)

---

## What it looks like

Keyoubs opens as a sleek dark overlay on top of your workspace. The editor floats above everything with:

- **A draggable toolbar** at the top (or wherever you put it) with icon buttons for each tool
- **A live canvas** showing your actual desktop windows as movable tiles
- **A property panel** on the right with sliders and toggles for gaps, borders, opacity
- **Glassy dark surfaces** with subtle borders and a muted blue accent (#62A9E3)
- **Crisp SVG icons** at native DPI — no blurry pixels on HiDPI screens

The whole UI sits on a 4px spacing grid. Everything aligns. Nothing feels random.

---

## What you can do right now

### Tiling layout editor
- Drag windows around the canvas to rearrange them
- Resize gaps by dragging handles directly on the layout
- Change border colors and width with live preview
- Every edit writes back to your compositor config

### Compositor support
| Compositor | What works |
|------------|-----------|
| **Niri** | Full IPC: workspaces, windows, gaps, borders, config reload |
| **Hyprland** | Full IPC: layout mode, window rules, monitor config |
| **Sway** | Full IPC: tree navigation, window moves, workspace commands |
| **i3** | Full IPC: same as Sway via shared protocol |

### Undo / Redo
- Press `Ctrl+Z` to undo any layout change
- Press `Ctrl+Shift+Z` to redo
- Full command history visible in a panel

### Config backups
- Every save automatically backs up your compositor config
- Up to 20 backups kept per compositor
- One-click restore from the backup browser

### Layout presets
- Save snapshots of your layout as named presets
- Switch between "Work," "Gaming," or "Minimal" layouts instantly
- Presets stored in `~/.config/keyoubs/presets/`

---

## Status bar builder (Preview)

Drop modules onto a bar and see them live:

| Module | Shows |
|--------|-------|
| **Clock** | Live time, updates every second |
| **Battery** | Percentage + charging bolt from UPower |
| **Workspaces** | Colored dots for each workspace, active one highlighted |
| **Network** | WiFi SSID or ethernet status from NetworkManager |
| **Media** | Now playing title/artist from MPRIS |

The bar anchors to any screen edge. Modules drag to reorder.

### Dock (Preview)

A rounded floating dock for your apps:
- Pin applications from `.desktop` files
- Running apps get a blue indicator dot below their icon
- Hover dims the background for the hovered icon
- Switches between horizontal and vertical layouts

---

## Customize everything

### Hot-reload theme

Create `~/.config/keyoubs/theme.json` and it applies instantly:

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

No rebuild. No restart. Just save the file.

### Lua scripting

Add `~/.config/keyoubs/scripts/init.lua` to run code on events:

```lua
function onToolSelected(tool)
    keyoubs.log("info", "Switched to: " .. tool)
end

function onPropertyChanged(key, value)
    keyoubs.notify("Keyoubs", key .. " = " .. tostring(value))
end
```

Hooks available: `onToolSelected`, `onPropertyChanged`, `onWindowSelected`, `onCompositorDetected`.

---

## Keyboard shortcuts

| Key | Action |
|-----|--------|
| `F1` | Toggle toolbar |
| `F2` | Toggle property panel |
| `Ctrl+Z` | Undo |
| `Ctrl+Shift+Z` | Redo |
| `Ctrl+P` | Command palette (coming) |
| `Escape` | Close Keyoubs |

---

## Install

### Arch Linux

```bash
sudo pacman -S --needed cmake gcc qt6-base qt6-tools qt6-svg
# Optional: layer-shell support on wlroots compositors
sudo pacman -S --needed layer-shell-qt
# Optional: Lua scripting
sudo pacman -S --needed lua
```

### Build

```bash
git clone https://github.com/Bocydium/Keyoubs.git
cd Keyoubs
mkdir build && cd build
cmake ..
make -j$(nproc)
./keyoubs
```

---

## Dependencies

| Package | Required | Why |
|---------|----------|-----|
| `cmake` | Yes | Build system (>= 3.20) |
| `qt6-base` | Yes | GUI, networking, file watching |
| `qt6-svg` | Yes | Icon rendering |
| `lua` | Optional | Scripting engine (5.4+) |
| `layer-shell-qt` | Optional | Overlay mode on wlroots/Smithay |

---

## File structure

```
~/.config/keyoubs/
├── theme.json          # Colors, sizes, fonts
├── layout.json         # Toolbar position, visible tools
├── properties.json     # Default property values
├── scripts/
│   └── init.lua        # Event hooks and automation
├── presets/
│   ├── work.json       # Saved layout presets
│   └── gaming.json
├── backups/
│   ├── niri/           # Automatic config backups
│   └── hyprland/
└── components/         # Saved surface components
    └── statusbar/
        └── component.json
```

---

## Feature roadmap

**Now (built):**
- Tiling layout editor with live compositor sync
- Undo/redo, config backups, preset library
- i3, Sway, Hyprland, Niri IPC adapters
- Status bar + dock surfaces
- Data sources: D-Bus, shell, file watcher
- Modules: clock, battery, workspaces, network, media
- Lua scripting
- Hot-reload themes

**Next:**
- Desktop widgets (persistent layer-shell surfaces)
- Animation editor with bezier curve canvas
- App launcher (searchable overlay)
- Notification center panel
- Screenshot/capture UI
- Workspace overview / exposé
- Per-workspace layout editor
- Window class inspector
- Real-time config validation
- Config diff view UI panel
- Detached canvas window
- Pinch-to-zoom, canvas panning with momentum

---

## Tech stack

- **C++20** — fast, no garbage collection pauses
- **Qt6** — GPU-accelerated rendering, cross-platform
- **CMake** — simple, no magic build system
- **Lua 5.5** — optional scripting
- **LayerShellQt** — optional Wayland layer-shell

---

## License

TBD
