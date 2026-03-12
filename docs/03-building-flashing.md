# Building & Flashing

← [Back to README](../README.md)

---

## Prerequisites

[Visual Studio Code](https://code.visualstudio.com/) with the [PlatformIO extension](https://platformio.org/install/ide?install=vscode), **or** the [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation/index.html).

---

## Choosing Your Target

Open `platformio.ini` and set `default_envs`. This controls what the VS Code **✓ Build** button and bare `pio run` command act on:

```ini
; Build & flash ESP32-S2 only (default)
default_envs = esp32s2

; Build & flash ESP8266 only
default_envs = esp8266

; Build & verify both MCUs at once (useful for CI or cross-checking)
default_envs = esp32s2, esp8266
```

---

## Switching Board Variant

Commented-out alternatives are listed inside each env block in `platformio.ini`:

```ini
; ESP32-S2 variants
board = esp32-s2-saola-1   ; default
; board = lolin_s2_mini
; board = esp32-s2-kaluga-1

; ESP8266 variants
board = d1_mini            ; default (Wemos D1 Mini)
; board = nodemcuv2
; board = d1_mini_pro
; board = esp12e
```

---

## Setting the Upload Port Manually

If PlatformIO cannot auto-detect the port, uncomment `upload_port` inside the relevant env:

```ini
; upload_port = /dev/ttyUSB0   ; Linux / macOS
; upload_port = COM3            ; Windows
```

---

## Build & Upload Commands

```bash
# Flash using whichever env(s) are set in default_envs
pio run -t upload

# Flash a specific target regardless of default_envs
pio run -e esp32s2 -t upload
pio run -e esp8266 -t upload

# Build only (no upload) — useful to check both targets compile cleanly
pio run -e esp32s2
pio run -e esp8266

# Open serial monitor (115200 baud)
pio device monitor
```

> No LittleFS filesystem image upload is required. The Web UI HTML is compiled directly into the firmware binary as a PROGMEM gzip constant by the `compress_html.py` pre-build script.

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
