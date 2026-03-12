# Project Structure

← [Back to README](../README.md)

---

## File Layout

```
esp_mqtt_matrix/
├── platformio.ini              # Build targets: esp32s2 / esp8266
├── compress_html.py            # Pre-build script: gzips web/index.html → include/html_page.h
├── README.md                   # This documentation index
├── docs/                       # Per-topic documentation
├── web/
│   └── index.html              # Web UI source — edit this freely
├── include/
│   ├── defaults.h              # Compile-time constants (edit here to change factory defaults)
│   ├── config.h                # AppConfig struct — includes defaults.h, no JSON dependency
│   ├── config_io.h             # JSON serialisation — includes config.h + ArduinoJson
│   ├── storage.h               # LittleFS load / save — includes config_io.h
│   ├── display.h               # MD_Parola wrapper (Software SPI) — includes config.h only
│   ├── html_page.h             # AUTO-GENERATED: Web UI as PROGMEM gzip byte array
│   ├── webserver.h             # HTTP server + REST API — includes config_io.h
│   └── mqtt_client.h           # PubSubClient wrapper with auto-reconnect and cmd topics
└── src/
    └── main.cpp                # setup(), loop(), globals, callbacks
```

> `web/index.html` is the human-editable Web UI source. `include/html_page.h` is generated automatically at every build by `compress_html.py` — **do not edit it directly**.

---

## Include Dependency Graph

```
defaults.h          (no dependencies)
    └── config.h    (struct only — no ArduinoJson)
            ├── display.h       — only needs the struct
            ├── mqtt_client.h   — only needs the struct
            └── config_io.h     (adds ArduinoJson)
                    ├── storage.h
                    └── webserver.h
```

`display.h` and `mqtt_client.h` do **not** pull in ArduinoJson, keeping their compilation units lean. Only files that actually read or write JSON include `config_io.h`.

---

## Dependencies

All libraries are fetched automatically by PlatformIO on first build — no manual installation needed.

| Library | Version | Purpose |
|---|---|---|
| `knolleary/PubSubClient` | `^2.8` | MQTT client |
| `bblanchon/ArduinoJson` | `^7.0` | JSON config serialisation |
| `tzapu/WiFiManager` | `^2.0.17` | Captive-portal WiFi provisioning |
| `majicdesigns/MD_MAX72XX` | `^3.5` | Low-level MAX7219 driver |
| `majicdesigns/MD_Parola` | `^3.7` | High-level text/animation engine |

---

## Build Flags Reference

| Flag | Target | Purpose |
|---|---|---|
| `-DESP32` / `-DESP32S2` | ESP32-S2 | Selects ESP32 WiFi/FS headers |
| `-DESP8266` | ESP8266 | Selects ESP8266 WiFi/FS headers |
| `-DMQTT_MAX_PACKET_SIZE=512` | Both | Increases PubSubClient buffer for longer payloads |

---

## HTML Compression Pipeline

The Web UI is gzip-compressed at build time and stored as a PROGMEM byte array. This means:

- **No filesystem upload required** — the UI is baked into the firmware binary
- **~72% smaller** than the raw HTML in flash
- `web/index.html` is the source of truth — edit it freely and rebuild

The `compress_html.py` pre-build script runs automatically before every compile. It skips regeneration if the HTML has not changed since the last build.

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
