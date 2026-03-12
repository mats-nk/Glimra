# Web GUI

← [Back to README](../README.md)

---

The Web GUI is a single-page app with selectable themes and four configuration tabs plus a live control panel. It is served entirely from the ESP's flash — no external files or CDN dependencies.

---

## Themes

Three colour themes are available via the circle buttons in the header:

| Theme | Description |
|---|---|
| ● Dark | Default — dark purple/blue palette |
| ● Light | White background, high readability |
| ● High Contrast | Black background with yellow accent — maximum visibility |

The chosen theme is saved in the browser's `localStorage` and restored automatically on next visit.

---

## Live Control Panel

Always visible at the top of the page.

| Control | Description |
|---|---|
| **Message input + Send** | Type a message and send it to the display immediately. Also triggered by pressing Enter |
| **Last message display** | Shows the most recently displayed message (from MQTT or manual send) |
| **Brightness slider** | Adjusts MAX7219 intensity from 0 (dimmest) to 15 (brightest). Applied instantly and saved to flash |

---

## Tab: MQTT

| Field | Default | Description |
|---|---|---|
| Broker IP / Host | *(empty)* | IP address or hostname of your MQTT broker |
| Port | `1883` | MQTT broker port |
| Username | *(empty)* | Leave blank for anonymous access |
| Password | *(empty)* | Leave blank for anonymous access |
| Topic 1 | `display/text` | Primary subscribe topic |
| Topic 2 | *(empty)* | Optional second topic |
| Topic 3 | *(empty)* | Optional third topic |

When two or more topics are active, incoming messages are automatically prefixed with the last segment of the topic path. For example, a message on `sensors/temperature` will display as `[temperature] 23.5`.

---

## Tab: Display

| Field | Default | Description |
|---|---|---|
| CS / LOAD Pin | `12` | GPIO connected to MAX7219 CS/LOAD — D6 on Wemos D1 Mini |
| CLK Pin | `14` | GPIO connected to MAX7219 CLK — D5 on Wemos D1 Mini |
| DATA Pin | `13` | GPIO connected to MAX7219 DIN / MOSI — D7 on Wemos D1 Mini |
| Modules | `4` | Number of 8×8 panels in the daisy chain |
| Hardware Type | `Generic` | PCB wiring layout — must match your physical module |
| Rotation | `0°` | `0` = normal · `180°` = upside-down |
| Invert display | off | Inverts all pixel states |

### Hardware Type vs Rotation

These two settings solve completely different problems and should be configured in order.

**Hardware Type** tells the library how a specific manufacturer has wired the MAX7219 pins to the LED matrix. If the wrong type is selected, characters appear scrambled or mirrored — rotation cannot fix this. Set this first.

**Rotation** flips the entire display 180° for when modules are physically mounted upside-down. Set this second, only after Hardware Type is correct.

The correct setup order is:
1. Change **Hardware Type** until text is legible — FC-16 is correct for most cheap strips
2. Only then set **Rotation** to 180° if the modules are mounted upside-down

> 90° and 270° rotation are not supported for scrolling text — MD_Parola scrolls horizontally only.

---

## Tab: Animation

| Field | Default | Range | Description |
|---|---|---|---|
| Entry Effect | Scroll Left | 0–26 | Animation used when text first appears |
| Exit Effect | Scroll Left | 0–26 | Animation used when text leaves |
| Scroll Speed | `40` ms | 10–200 ms | Time per animation step — lower = faster |
| Pause Time | `2000` ms | 0–10000 ms | How long text holds at rest before exit animation |

See [docs/08-reference.md](08-reference.md) for the full animation effects table.

> When **Sprite** is selected the display shows a Pac-Man animation between messages.

---

## Tab: System

| Field | Default | Description |
|---|---|---|
| Hostname | `matrix-XXXX` | Unique per device — last 4 hex digits of chip ID. Used for AP name and mDNS. Editable and persisted to flash |

The System tab also shows live read-only info: board type, IP address, WiFi RSSI, uptime in seconds, and free heap memory.

---

## Buttons

| Button | Action |
|---|---|
| **Save & Apply** | Writes config to LittleFS and reinitialises the display and MQTT client — no reboot needed |
| **Factory Reset** | Erases all saved config and WiFi credentials, then reboots into the captive portal |
| **Reset WiFi** (header) | Clears only WiFi credentials and reboots, keeping all other config |

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
