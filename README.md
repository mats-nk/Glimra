# Glimra

**Glimra** is firmware for **ESP32-S2** and **ESP8266** that drives a MAX7219 LED matrix display.

The main features is:
- Display text from MQTT topics
- Full configuration via MQTT cmd topics or a built-in Web GUI — no hardcoded credentials or filesystem upload required.

🔗 **https://github.com/mats-nk/Glimra**

---

## Documentation

| File | Contents |
|---|---|
| [docs/01-hardware.md](docs/01-hardware.md) | Hardware requirements and wiring |
| [docs/02-project-structure.md](docs/02-project-structure.md) | File layout, include dependency graph, build flags |
| [docs/03-building-flashing.md](docs/03-building-flashing.md) | Prerequisites, targets, upload commands |
| [docs/04-first-boot.md](docs/04-first-boot.md) | WiFi setup and captive portal walkthrough |
| [docs/05-web-gui.md](docs/05-web-gui.md) | Web GUI tabs and all configurable fields |
| [docs/06-mqtt-cli.md](docs/06-mqtt-cli.md) | Publishing messages and control topics via CLI |
| [docs/07-rest-api.md](docs/07-rest-api.md) | HTTP endpoints and curl examples |
| [docs/08-reference.md](docs/08-reference.md) | Config reference, animation table, troubleshooting |

---

## Features

| Category | Detail |
|---|---|
| **Dual target** | Compiles for ESP32-S2 and ESP8266 from the same codebase |
| **WiFiManager** | Captive-portal AP on first boot — no hardcoded WiFi credentials |
| **Web GUI** | Single-page app served directly from MCU flash (no SPIFFS upload) |
| **MQTT** | Up to 3 simultaneous subscribe topics with auto-reconnect every 5 s |
| **MQTT cmd topics** | Every setting changeable via `display/cmd/<param>` topics |
| **MAX7219 control** | Hardware type, SPI pins, module count, rotation — all configurable at runtime |
| **MD_Parola animations** | 27 entry/exit text effects, individually selectable per direction |
| **Live brightness** | Slider 0–15 applied instantly without reboot, persisted to flash |
| **Persistence** | Full config stored as JSON in LittleFS, survives power cycles |
| **Factory reset** | One-click from Web GUI or via MQTT |
| **Multi-topic labels** | When 2+ topics are active, messages are prefixed with the last topic path segment |
| **Extended characters** | UTF-8 → Latin-1 conversion — Swedish, German, French characters work out of the box |

---

## Licence

GPL-3.0-or-later © 2026 Mats Nils Karlsson — see [LICENSE](LICENSE)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

