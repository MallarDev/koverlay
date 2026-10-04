# KOverlay — system monitor fork

Fork of [erik96/koverlay](https://github.com/erik96/koverlay), adding a KDE 6 system-monitor HUD.
The original text overlay is still available. The monitor uses **KQuickCharts** and
**org.kde.ksysguard.sensors**, with the existing Wayland overlay layer and input transparency.

## Start the monitor

```bash
./build/koverlay --monitor --show --screen-index 0
```

For a customized setup, use the supplied configuration without overwriting your current one:

```bash
KOVERLAY_CONFIG="$PWD/examples/monitor.ini" ./build/koverlay --show --screen-index 0
```

Default layout: CPU and GPU on the first row, RAM and VRAM on the second, swap and
disk space on the third. Graphs show 60 seconds of history, a current value, and
percentage or binary-capacity axes. Disk means **occupied space**, not I/O activity.
GPU and disk defaults aggregate the devices exposed by KDE; override their sensor IDs
if you want a specific device. This recreates a similar layout; it does not import your
existing Plasma widgets or their settings.

## Configuration

Only one KOverlay instance runs per user D-Bus session. Launching it again shows
the existing overlay if hidden and does nothing if it is already visible, even
without `--show`. The second process exits without creating another window or
changing the first instance's configuration, monitor mode, screen, or chart history.
The first launch still uses `--show` to start visible. D-Bus must be available;
registration or activation failures are reported instead of creating an extra overlay.

After upgrading from a version without this guard, quit all old KOverlay processes
once before starting the new binary. Existing D-Bus Toggle/Show/Hide shortcuts
continue to work.

Use `mode=monitor` in `[overlay]`, or force monitor mode with `--monitor`.
The existing position, margin, text color, background opacity, screen-index and DBus
visibility controls apply to both modes. `panelOpacity=0` removes the background.
Edits to the configuration reload while running. `--monitor` takes precedence over
`mode=text`; omit the flag to switch modes through the file.

`[monitor]` options:

| Key | Default | Meaning |
| --- | --- | --- |
| `columns` | 2 | Grid columns (1–6) |
| `chartWidth` | 260 | Width per chart, logical pixels (200–800) |
| `chartHeight` | 130 | Height per chart, logical pixels (100–400) |
| `fontSize` | 12 | Chart label size (9–24) |
| `historySeconds` | 60 | History duration (10–3600 seconds) |
| `updateInterval` | 1000 | Chart sampling interval (500–60000 ms) |

Each of `cpu`, `gpu`, `ram`, `vram`, `swap` and `disk` also accepts `Enabled`, `Sensor`,
`Label` and `Color` suffixes, for example `swapEnabled=false` or `diskLabel=SSD`.
Use percentage sensors for CPU/GPU slots and byte-valued sensors for memory/disk slots.
See [examples/monitor.ini](examples/monitor.ini) for the complete preset.

The sampling interval controls the graph, not the systemstats daemon's collection rate.
No history is invented before startup. Missing or stale sensors show **Unavailable**;
zero-capacity swap shows **Not configured**. Hidden overlays unsubscribe from sensors
and clear their history; history starts again when shown. Config changes that rebuild
cards also restart their history.

## Build on Fedora / Bazzite

On a regular Fedora installation, install:

```bash
sudo dnf install cmake ninja-build gcc-c++ extra-cmake-modules \
  qt6-qtbase-devel qt6-qtbase-private-devel qt6-qtdeclarative-devel qt6-qtwayland-devel \
  layer-shell-qt-devel wayland-devel libxkbcommon-devel \
  libksysguard kf6-kquickcharts ksystemstats
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build --parallel 2
```

On Bazzite, compile in a Fedora Distrobox/Toolbox matching your host release, then run
`build/koverlay` on the host in your KDE Wayland session. The host needs the Qt 6,
LayerShellQt, libksysguard, KQuickCharts and ksystemstats runtime packages. Most are
already part of KDE; check with `rpm -q layer-shell-qt qt6-qtdeclarative libksysguard
kf6-kquickcharts ksystemstats`. Use Bazzite's host package mechanism for missing
packages; do not run `dnf install` directly on its immutable host.

The CI workflow builds on Fedora 43, runs configuration and QML tests against actual
KDE modules, and produces an RPM artifact. Qt 5 versions of the KDE modules do not work.
Other distributions need the equivalent Qt 6 / Plasma 6 packages.

## Verification and limitations

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Graph tests require a graphical display; CI uses Xvfb and Mesa. They cover bounded
history, real zero values versus unavailable readings, memory scales, QML imports and
layout construction. They do **not** replace a real KWin/Wayland smoke test:

1. Run the monitor over a browser, then click and type through the charts.
2. Check that opening/toggling the overlay never changes keyboard focus.
3. Compare readings with Plasma System Monitor, especially GPU/VRAM and disk totals.
4. Check the requested screen and fullscreen applications; secure/lock screens are
   compositor-controlled and are not targets for this overlay.

A missing QML module produces a visible dependency error and detailed terminal output.
Unsupported GPU drivers or unavailable KDE sensors cannot be fixed by the overlay.

---

## Original project documentation

<p align="center">
  <img src="assets/koverlay-banner-layers.svg" alt="KOverlay — Wayland Overlay" width="100%">
</p>


[![Copr build status](https://copr.fedorainfracloud.org/coprs/erx96/KOverlay/package/koverlay/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/erx96/KOverlay/package/koverlay/)



# KOverlay

Click‑through, always‑on‑top **overlay panel** for Wayland desktops (tested on **Fedora KDE/Plasma Wayland**) built with **Qt 6 + QML + LayerShellQt**.  
Perfect for **sticky notes**, **cheat sheets**, **keybindings**, or any text you want visible above apps - without stealing focus. Supports **live‑reloading** from a config file.

<p align="center">
  <i>Toggle from a global shortcut, stays above everything, passes all mouse/keyboard through.</i>
</p>

---

## Screenshots

<p align="center">
  <a href="assets/code.png">
    <img src="assets/code.png" alt="Overlay on KDE Wayland" width="49%">
  </a>
  <a href="assets/desktop.png">
    <img src="assets/desktop.png" alt="Overlay on Desktop" width="49%">
  </a>
</p>

---

## Features

- 🧼 **Click‑through**: never steals input; all clicks/keys go to the app underneath.
- 📌 **Always on top**: pinned using `layer-shell` on the compositor.
- 🖥️ **Multi‑monitor aware**: choose target monitor with `--screen-index`.
- 🧭 **NEW: Custom position**: In config.ini it is now possible to specify position (`top-left/right`, `bottom-left/right` or `custom` via x,y coordinates.
- ⚡ **Hot‑reloading config**: edit `~/.config/koverlay/config.ini` and changes apply immediately.
- ✍️ **Customizable text**: font family, size, color, bold.
- 🌫️ **Panel background opacity**: fade the backdrop while keeping text fully opaque.
- 🚌 **DBus control**: `Toggle`, `Show`, `Hide` for easy desktop shortcuts.
- 📦 **RPM packaging** via CPack.

> **Wayland only.** Tested on Fedora KDE (KWin/Wayland). Other Wayland compositors may work, but KOverlay currently targets KDE Plasmashell/KWin + LayerShellQt.

---

## Dependencies

On **Fedora** (42+), install the runtime/build deps:

```bash
sudo dnf install -y   qt6-qtbase-devel qt6-qtbase-private-devel qt6-qtdeclarative-devel qt6-qtwayland-devel   layer-shell-qt-devel wayland-devel
```

Runtime packages users typically need (automatically pulled by RPM):
- `layer-shell-qt`
- `qt6-qtbase`
- `qt6-qtdeclarative`
- `qt6-qtwayland`

---

## Build & Run

This repo includes three helper scripts:

```text
./build_bin.sh   # builds a Release binary into ./build and leaves it there
./build_rpm.sh   # builds an RPM using CPack into ./build
./run.sh         # runs the built binary with Wayland platform
```

### Manual build (CMake)
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```

### Run
```bash
QT_QPA_PLATFORM=wayland ./koverlay --show --screen-index 0
# --show          start visible
# --screen-index  choose monitor (0..N-1; 0 is typically primary)
```

---

## Configuration (hot‑reloading)

Default config file path:
- `~/.config/koverlay/config.ini`

Override with environment variable:
- `KOVERLAY_CONFIG=/path/to/config.ini`

KOverlay watches the file and reloads on save.

### Supported keys (section `[overlay]`)

| Key         | Type     | Example                                  | Notes |
|-------------|----------|-------------------------------------------|-------|
| `text`      | string / multiline | see examples below              | Overlay text. Multiline supported (triple quotes or YAML‑style pipe). `\n` also works. |
| `textFile`  | path     | `~/.config/koverlay/overlay.txt`          | If set, file contents override `text`. |
| `fontFamily`| string   | `Fira Code`                               | Leave empty to use system default. |
| `fontSize`  | int      | `22`                                      | Pixels. |
| `textColor` | string   | `#FFCC00` or `tomato`                     | Any QML color. |
| `bold`      | bool     | `true` / `false`                          | Toggle bold text. |
| `panelOpacity` | double | `0.30`                                   | 0.0 (transparent) … 1.0 (opaque). Only the background fades; text remains fully opaque. |

### Examples

**A) Triple‑quoted multiline inside INI (recommended)**
```ini
[overlay]
text="""
⌨ Keybindings:
• Super+Enter — Terminal
• Ctrl+Alt+H — Toggle Overlay
"""
fontFamily=Fira Code
fontSize=22
textColor=#FFCC00
bold=false
panelOpacity=0.30
```

**B) Multiline via external file**
```ini
[overlay]
textFile=~/.config/koverlay/overlay.txt
fontFamily=Inter
fontSize=20
textColor=white
bold=true
panelOpacity=0.35
```

**C) Single line with \n escapes**
```ini
[overlay]
text=Line 1\nLine 2\nLine 3
fontSize=18
```

**D) Full config demo showing all features**
```ini
[overlay]
#text="""
#⌨ Keybindings:
#• Super+Enter — Terminal
#• Ctrl+Alt+H — Toggle Overlay

#Things to do:
#• Buy milk
#"""

# this now hot-reloads as well
textFile=~/.config/koverlay/overlay-list.txt

# one of: top-left | top-right | bottom-left | bottom-right | custom
position=bottom-left


# margins (applied depending on position); defaults 16
marginTop=16
marginRight=16
marginBottom=16
marginLeft=16

# used only when position=custom (offsets from top-left)
x=80
y=60

fontFamily=Fira Code
fontSize=18
bold=false
textColor=#40E0D0
panelOpacity=0.75
```

---

## DBus interface & Shortcuts

KOverlay registers a DBus service on the **session bus** while running:

- **Service:** `org.erx.KOverlay`
- **Object path:** `/Overlay`
- **Interface:** `org.erx.KOverlay`
- **Methods:** `Toggle()`, `Show()`, `Hide()`

### CLI examples (qdbus)
```bash
qdbus org.erx.KOverlay /Overlay org.erx.KOverlay.Toggle
qdbus org.erx.KOverlay /Overlay org.erx.KOverlay.Show
qdbus org.erx.KOverlay /Overlay org.erx.KOverlay.Hide
```

### KDE Global Shortcuts
1. Open **System Settings → Shortcuts → Custom Shortcuts**.
2. Add a **Command/URL** action.
3. Command: `qdbus org.erx.KOverlay /Overlay org.erx.KOverlay.Toggle`
4. Assign your preferred keybinding (e.g., `Meta+H`).

> The DBus registration is **ephemeral** (per session); it re‑appears when the app starts.

---

## Autostart (optional)

### Option 1 — KDE Autostart
Create `~/.config/autostart/koverlay.desktop`:
```ini
[Desktop Entry]
Type=Application
Name=KOverlay
Exec=koverlay --show
X-KDE-StartupNotify=false
OnlyShowIn=KDE;
```

### Option 2 — systemd user service
Create `~/.config/systemd/user/koverlay.service`:
```ini
[Unit]
Description=KOverlay overlay
After=graphical-session.target

[Service]
ExecStart=%h/.local/bin/koverlay --show
Restart=on-failure
Environment=QT_QPA_PLATFORM=wayland

[Install]
WantedBy=default.target
```
Then:
```bash
systemctl --user daemon-reload
systemctl --user enable --now koverlay.service
```

---

## Packaging (RPM via CPack)

From the build directory:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=RPM ..
cmake --build . -j
cpack -G RPM

# Result: koverlay-<version>-1.x86_64.rpm
# Inspect:
rpm -qlp koverlay-*.rpm
rpm -qp --requires koverlay-*.rpm | sort
```

CPack will auto‑discover the `.so` requirements; we additionally declare friendly deps:
- `layer-shell-qt, qt6-qtbase, qt6-qtdeclarative, qt6-qtwayland`

---

## Troubleshooting

- **Overlay not visible on the intended monitor?**  
  Use `--screen-index N`. Index starts at 0; 0 is typically the primary.

- **Overlay ever steals focus or blocks input?**  
  It shouldn’t. We set Qt’s `WindowTransparentForInput` and also clear the Wayland input region. If it happens, make sure you are on Wayland (not XWayland) and KOverlay is actually running with `QT_QPA_PLATFORM=wayland`.

- **Doesn’t stay on top?**  
  It uses `LayerShellQt` overlay layer. On non‑KWin compositors, switching to `LayerTop` in code may help.

- **Config doesn’t apply?**  
  Check that your INI section is `[overlay]`. For multiline, ensure triple quotes are balanced. Watch the console logs for `applied config` entries.

---

## License

MIT

---

## Acknowledgements

- **Qt 6 / QML / QtWayland**
- **LayerShellQt** for Wayland layer‑shell integration
- Thanks to KDE / KWin teams for a great Wayland experience.

---

## FAQ

**Is it statically linked?**  
No. It uses distro Qt/LayerShellQt shared libraries. Prefer the RPM for clean dependency handling.

**Will it work on non‑KDE Wayland?**  
It may, but development targets **KDE Plasma (KWin/Wayland)**.

**Where is the config?**  
`~/.config/koverlay/config.ini` (override with `KOVERLAY_CONFIG`).

---

Happy overlays! 🎉
