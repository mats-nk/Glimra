# Hardware Requirements & Wiring

← [Back to README](../README.md)

---

## Requirements

- ESP32-S2 **or** ESP8266 (Wemos D1 Mini, NodeMCU, etc.)
- One or more MAX7219 8×8 LED matrix modules (daisy-chained)
- 5 V power supply capable of ~330 mA per module under full load
- Three GPIO-capable pins for SPI (any pins — software SPI is used)

> **Power note:** Power the MAX7219 chain from a dedicated 5 V rail, not from the ESP's 3.3 V regulator. The ESP signal levels (3.3 V) are accepted by the MAX7219 data inputs.

---

## Default Wiring

All pin assignments are configurable in the Web GUI without reflashing.

| Signal | Default GPIO | Wemos D1 Mini label |
|--------|:-----------:|:-------------------:|
| CLK | **14** | D5 |
| CS / LOAD | **12** | D6 |
| DATA / DIN | **13** | D7 |
| VCC | 5 V | 5 V |
| GND | GND | GND |

---

## Hardware Type

Different manufacturers wire the MAX7219's segment and digit pins to the LED matrix in different orders. If characters appear scrambled or mirrored, cycle through the Hardware Type options until text is legible. **FC-16 is correct for the majority of cheap 4-module strips sold online.**

| Value | Name | Common modules |
|---|---|---|
| 0 | Generic | Generic eBay/AliExpress modules |
| 1 | FC-16 | FC-16 labelled modules (most common) |
| 2 | Parola | MajicDesigns Parola boards |
| 3 | ICStation | ICStation branded modules |

> Hardware Type must be set correctly **before** adjusting Rotation — rotation cannot fix scrambled characters.

---

<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (c) 2026 Mats Nils Karlsson — https://github.com/mats-nk/Glimra -->
