# Publishing Messages via MQTT & CLI

← [Back to README](../README.md)

---

## Install mosquitto Clients

```bash
# Debian / Ubuntu / Raspberry Pi
sudo apt install mosquitto-clients

# macOS (Homebrew)
brew install mosquitto

# Windows — download the installer from https://mosquitto.org/download/
# mosquitto_pub.exe is included in the bin folder
```

---

## Basic Usage

Replace `192.168.1.100` with your broker's IP and `display/text` with your configured topic.

```bash
# Simple message
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "Hello world"

# With broker authentication
mosquitto_pub -h 192.168.1.100 -u myuser -P mypassword -t "display/text" -m "Hello world"

# Non-default port
mosquitto_pub -h 192.168.1.100 -p 1884 -t "display/text" -m "Hello world"
```

---

## Extended Characters (Swedish / German / French)

The firmware converts UTF-8 to Latin-1 automatically — accented characters work out of the box.

```bash
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "Välkommen till Stockholm"
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "Öppettider: 08:00–17:00"
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "Außentemperatur: 21°C"
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "Réservation confirmée"
```

---

## Multiple Topics

When Topic 2 or Topic 3 are configured the display automatically prefixes the message with the last path segment of the topic.

```bash
# Displays: "[temp] 23.5 C"
mosquitto_pub -h 192.168.1.100 -t "sensors/temp" -m "23.5 C"

# Displays: "[status] Door open"
mosquitto_pub -h 192.168.1.100 -t "home/alarm/status" -m "Door open"
```

---

## Retained Messages

A retained message is stored by the broker and replayed to the display immediately on reconnect or reboot — useful for status boards.

```bash
# Set a retained message
mosquitto_pub -h 192.168.1.100 -t "display/text" -r -m "Production line running"

# Clear a retained message (publish empty payload)
mosquitto_pub -h 192.168.1.100 -t "display/text" -r -n
```

---

## Scripting and Automation

```bash
# Publish the current date and time
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "$(date '+%a %d %b  %H:%M')"

# Publish from a variable
MESSAGE="Shift starts in 5 min"
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "$MESSAGE"

# Loop — update the display every minute with the current time
while true; do
  mosquitto_pub -h 192.168.1.100 -t "display/text" -m "$(date '+%H:%M')"
  sleep 60
done
```

---

## Windows (PowerShell)

```powershell
# Basic message
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "Hello from PowerShell"

# Current time
$time = Get-Date -Format "HH:mm"
mosquitto_pub -h 192.168.1.100 -t "display/text" -m "Time: $time"
```

---

## Python (paho-mqtt)

```python
import paho.mqtt.publish as publish

# pip install paho-mqtt

publish.single(
    topic    = "display/text",
    payload  = "Hello from Python",
    hostname = "192.168.1.100",
)

# With authentication
publish.single(
    topic    = "display/text",
    payload  = "Temperatur: 22°C",
    hostname = "192.168.1.100",
    auth     = {"username": "myuser", "password": "mypassword"},
)
```

---

## Control Topics (cmd)

Every setting that can be changed in the Web GUI can also be changed by publishing to a `cmd/` topic. The base prefix is derived automatically from your primary topic — if your topic is `display/text` the cmd base is `display/cmd/`.

| Topic | Payload | Example |
|---|---|---|
| `display/cmd/text` | Any string | `Hello world` |
| `display/cmd/intensity` | `0` – `15` | `8` |
| `display/cmd/speed` | `10` – `200` ms | `60` |
| `display/cmd/pause` | `0` – `10000` ms | `3000` |
| `display/cmd/anim_in` | `0` – `26` (see [animation table](08-reference.md#animation-effects)) | `4` |
| `display/cmd/anim_out` | `0` – `26` | `4` |
| `display/cmd/rotation` | `0` = normal · `2` = 180° (upside-down) | `2` |
| `display/cmd/invert` | `1` = invert · `0` = normal | `1` |
| `display/cmd/hw_type` | `0`=Generic `1`=FC16 `2`=Parola `3`=ICStation | `1` |
| `display/cmd/modules` | `1` – `32` | `8` |
| `display/cmd/hostname` | Max 31 chars | `lobby-display` |

All cmd changes except `text` are saved to flash immediately. Changes that affect the display hardware (speed, pause, animation, rotation, invert, hw_type, modules) automatically reinitialise the display without a reboot.

```bash
# Dim the display
mosquitto_pub -h 192.168.1.100 -t "display/cmd/intensity" -m "2"

# Speed up scrolling
mosquitto_pub -h 192.168.1.100 -t "display/cmd/speed" -m "20"

# Switch to Scroll Right entry / Scroll Left exit
mosquitto_pub -h 192.168.1.100 -t "display/cmd/anim_in"  -m "5"
mosquitto_pub -h 192.168.1.100 -t "display/cmd/anim_out" -m "4"

# Flip display upside-down
mosquitto_pub -h 192.168.1.100 -t "display/cmd/rotation" -m "2"

# Rename the device
mosquitto_pub -h 192.168.1.100 -t "display/cmd/hostname" -m "lobby-display"
```

> **Tip:** if you use a custom primary topic (e.g. `office/ticker`) the cmd base follows automatically — use `office/cmd/intensity`, `office/cmd/speed`, etc.

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
