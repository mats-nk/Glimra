# REST API

← [Back to README](../README.md)

---

All endpoints return `application/json`. CORS headers are included on every response.

## Endpoints

| Method | Endpoint | Body | Response | Description |
|---|---|---|---|---|
| `GET` | `/` | — | `text/html` | Serves the full Web GUI |
| `GET` | `/api/config` | — | JSON object of all settings | Read current config |
| `POST` | `/api/config` | JSON with any config keys | `{"ok":true}` | Save config and reinitialise |
| `GET` | `/api/status` | — | JSON status object | Live status (MQTT, IP, heap…) |
| `POST` | `/api/message` | `{"msg":"Hello"}` | `{"ok":true}` | Display a message immediately |
| `POST` | `/api/intensity` | `{"value":8}` | `{"ok":true}` | Set brightness 0–15, saved to flash |
| `POST` | `/api/resetwifi` | — | `{"ok":true}` then reboot | Clear WiFi credentials and reboot |
| `POST` | `/api/factory` | — | `{"ok":true}` then reboot | Erase all config and reboot |

---

## Status Response Fields

`GET /api/status` returns:

```json
{
  "mqtt_connected": true,
  "last_msg": "Hello World",
  "ip": "192.168.1.42",
  "rssi": -58,
  "uptime": 3742,
  "heap": 186432,
  "board": "ESP32-S2"
}
```

| Field | Type | Description |
|---|---|---|
| `mqtt_connected` | bool | Whether the MQTT broker connection is currently active |
| `last_msg` | string | Most recently displayed message, UTF-8 encoded |
| `ip` | string | Device IP address on the local network |
| `rssi` | number | WiFi signal strength in dBm (updated every 10 s) |
| `uptime` | number | Seconds since last boot |
| `heap` | number | Free heap memory in bytes |
| `version` | string | Firmware version string, e.g. `2026-03-12-1` |
| `board` | string | `ESP32-S2` or `ESP8266` |

---

## Examples

### Send a message

```bash
curl -X POST http://192.168.1.42/api/message \
     -H "Content-Type: application/json" \
     -d '{"msg":"Hello from curl!"}'
```

### Set brightness

```bash
curl -X POST http://192.168.1.42/api/intensity \
     -H "Content-Type: application/json" \
     -d '{"value":12}'
```

### Read current config

```bash
curl http://192.168.1.42/api/config
```

### Update a single config field

Any subset of config keys can be posted — unrecognised keys are ignored, omitted keys keep their current values.

```bash
curl -X POST http://192.168.1.42/api/config \
     -H "Content-Type: application/json" \
     -d '{"scroll_speed":25,"pause_time":1000}'
```

### Check live status

```bash
curl http://192.168.1.42/api/status
```

### Factory reset

```bash
curl -X POST http://192.168.1.42/api/factory
```

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
