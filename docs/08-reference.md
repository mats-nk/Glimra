# Reference

← [Back to README](../README.md)

---

## Configuration Reference

All settings are stored in `/config.json` on the LittleFS partition. They can be edited via the Web GUI, the REST API (`POST /api/config`), or MQTT cmd topics.

| Key | Type | Default | Range / Notes |
|---|---|---|---|
| `hostname` | string | `matrix-XXXXXXXX` | Auto-generated from full chip ID on first boot (e.g. `matrix-A1B2C3D4`). Max 31 chars. Used for AP name and mDNS. Blank value triggers regeneration |
| `mqtt_server` | string | *(empty)* | IP or hostname of broker |
| `mqtt_port` | number | `1883` | 1–65535 |
| `mqtt_user` | string | *(empty)* | Leave blank for anonymous |
| `mqtt_pass` | string | *(empty)* | Leave blank for anonymous |
| `mqtt_topic` | string | `display/text` | Primary subscribe topic |
| `mqtt_topic2` | string | *(empty)* | Second topic (optional) |
| `mqtt_topic3` | string | *(empty)* | Third topic (optional) |
| `cs_pin` | number | `12` | GPIO for CS/LOAD — D6 on Wemos D1 Mini |
| `clk_pin` | number | `14` | GPIO for CLK — D5 on Wemos D1 Mini |
| `data_pin` | number | `13` | GPIO for DIN/MOSI — D7 on Wemos D1 Mini |
| `modules` | number | `4` | 1–32 daisy-chained panels |
| `hw_type` | number | `0` | PCB wiring variant: 0=Generic, 1=FC16, 2=Parola, 3=ICStation |
| `rotation` | number | `0` | `0` = normal, `2` = 180° (upside-down). Values 1 and 3 treated as 0° |
| `intensity` | number | `0` | 0 (dim) – 15 (bright) |
| `scroll_speed` | number | `40` | ms per animation step (10–200) |
| `pause_time` | number | `2000` | ms text holds at rest (0–10000) |
| `anim_in` | number | `4` | Entry effect index (see table below) |
| `anim_out` | number | `4` | Exit effect index (see table below) |
| `invert` | boolean | `false` | Invert all pixels |

---

## Animation Effects

| Index | Name | Index | Name |
|---|---|---|---|
| 0 | None (print in place) | 14 | Scan Vertical |
| 1 | Print | 15 | Opening |
| 2 | Scroll Up | 16 | Closing |
| 3 | Scroll Down | 17 | Opening Cursor |
| 4 | Scroll Left | 18 | Closing Cursor |
| 5 | Scroll Right | 19 | Scroll Up Left |
| 6 | Sprite (Pac-Man) | 20 | Scroll Up Right |
| 7 | Slice | 21 | Scroll Down Left |
| 8 | Mesh | 22 | Scroll Down Right |
| 9 | Fade | 23 | Scan Base |
| 10 | Dissolve | 24 | Grow Up |
| 11 | Blinds | 25 | Grow Down |
| 12 | Random | 26 | *(reserved)* |
| 13 | Scan Horizontal | | |

> **Sprite (index 6):** uses a built-in Pac-Man / ghost animation. No additional configuration is required — the sprite data is embedded in the firmware.

---

## Serial Monitor Log Prefixes

Connect at **115200 baud**. Key log prefixes:

| Prefix | Module |
|---|---|
| `[APP]` | Main application lifecycle |
| `[WIFI]` | WiFi connection events |
| `[WEB]` | HTTP server |
| `[MQTT]` | MQTT connection and messages |
| `[DISP]` | MAX7219 / Parola display |
| `[FS]` | LittleFS filesystem |

---

## Troubleshooting

| Symptom | Likely cause | Fix |
|---|---|---|
| Display shows garbled characters | Wrong hardware type | Try FC-16 first — it covers the majority of cheap strips. Cycle through all 4 options until text is legible. Rotation cannot fix this |
| Display is mirrored horizontally | Wrong hardware type | Change hardware type first. Only use rotation 180° if type is already correct and the module is physically mounted upside-down |
| Text scrolls in the wrong direction | Modules wired right-to-left | Use rotation 180°, or physically reverse the module order in the chain |
| Nothing displays at all | Wrong CS / CLK / DATA pins | Verify wiring and update pins in the Web GUI |
| Sprite animation shows a dark display | *(should not occur in current firmware)* | Upgrade to latest firmware — sprite data is now embedded automatically |
| MQTT not connecting | Wrong broker IP or credentials | Check broker is reachable; verify user/pass in the MQTT tab |
| Web GUI not loading | Device not on network | Check serial monitor for IP; try reconnecting |
| Config not saving | LittleFS corrupted | Use Factory Reset to reformat |
| Brightness slider has no effect | Display not initialised | Check serial monitor for `[DISP] begin() failed` |
| AP portal never appears | Device found saved WiFi | Use **Reset WiFi** in the GUI header, or power-cycle quickly 3 times |
| Rotation 180° has no effect | Old firmware using buffer transform | Upgrade to latest firmware — rotation now uses persistent zone effects |

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
