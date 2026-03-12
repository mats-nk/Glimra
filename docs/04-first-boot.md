# First Boot & WiFi Setup

← [Back to README](../README.md)

---

## Initial Setup Walkthrough

1. Power on the device — it will show **"Connecting WiFi..."** on the display.
2. If no saved WiFi credentials are found, it creates a WiFi access point named:
   ```
   matrix-XXXX
   ```
   where `XXXX` are the last 4 hex digits of the chip ID — unique per device.
3. Connect to that AP from your phone or computer. A captive portal opens automatically (or browse to `192.168.4.1`).
4. Select your WiFi network and enter the password. Click **Save**.
5. The device connects and scrolls its assigned IP address on the display.
6. **Browse to that IP** in any browser to open the Web GUI.

> The portal times out after **180 seconds** if no credentials are saved. The device will reboot and try again.

---

## Unique Hostname

The device generates a unique hostname on first boot using the last 4 hex digits of the chip ID — for example `matrix-a1b2`. This name is used for:

- The WiFi captive portal AP name
- mDNS hostname (accessible as `matrix-a1b2.local` on supported networks)

The hostname can be renamed at any time in the **System** tab of the Web GUI or via the `display/cmd/hostname` MQTT topic. The new name is persisted to flash immediately.

---

## Resetting WiFi Credentials

| Method | Effect |
|---|---|
| **Reset WiFi** button in Web GUI header | Clears WiFi credentials only — all other config is kept |
| **Factory Reset** button in Web GUI | Erases all config and WiFi credentials |
| `display/cmd/hostname` MQTT topic | — |

After either reset the device reboots into the captive portal with the same chip-ID hostname.

---

## Serial Monitor

Connect at **115200 baud** to watch the boot sequence. You will see the assigned IP address, MQTT connection status, and any errors.

```
[APP] Glimra booting
[APP] Config loaded
[APP] Hostname: matrix-a1b2
[WIFI] 192.168.1.42
[WEB] HTTP server on port 80
[MQTT] Connecting to 192.168.1.100:1883 ...
[MQTT] Connected
[MQTT] Subscribed — text topics + display/cmd/#
[APP] Ready
```

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
