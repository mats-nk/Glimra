// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Mats Nils Karlsson
// Glimra — MQTT LED Matrix Display Firmware
// https://github.com/mats-nk/Glimra

#pragma once
// =============================================================
//  defaults.h  —  Compile-time default values
//
//  Edit this file to change factory defaults without touching
//  any struct definitions or serialisation logic.
// =============================================================

// ── Firmware version ─────────────────────────────────────────
// Format: YYYY-MM-DD_<sequential number>
// Increment the sequential number for multiple releases on the same date.
#define FIRMWARE_VERSION "2026-03-12_12"

// ── Network ──────────────────────────────────────────────────
// Base prefix for the auto-generated hostname.
// At runtime the full 8 hex digits of the chip ID are appended,
// e.g. "glimra-A1B2C3D4".  The full hostname is still user-configurable
// via the Web GUI and is persisted to flash once changed.
#define DEFAULT_HOSTNAME_PREFIX "glimra"

// ── MQTT ─────────────────────────────────────────────────────
#define DEFAULT_MQTT_PORT       1883
#define DEFAULT_MQTT_TOPIC      "display/text"

// ── MAX7219 hardware ─────────────────────────────────────────
#define DEFAULT_CS_PIN          12      // SPI CS / LOAD  — D6 on Wemos D1 Mini
#define DEFAULT_CLK_PIN         14      // SPI CLK        — D5 on Wemos D1 Mini
#define DEFAULT_DATA_PIN        13      // SPI MOSI / DIN — D7 on Wemos D1 Mini
#define DEFAULT_MODULES         4       // daisy-chained 8x8 panels
#define DEFAULT_HW_TYPE         0       // 0=Generic 1=FC16 2=Parola 3=ICStation
#define DEFAULT_ROTATION        0       // 0=0deg 1=90deg 2=180deg 3=270deg

// ── Display behaviour ─────────────────────────────────────────
#define DEFAULT_INTENSITY       0       // 0 (dim) – 15 (bright)
#define DEFAULT_SCROLL_SPEED    40      // ms per animation step
#define DEFAULT_PAUSE_TIME      2000    // ms text rests before exit animation
#define DEFAULT_ANIM_IN         4       // PA_SCROLL_LEFT
#define DEFAULT_ANIM_OUT        4       // PA_SCROLL_LEFT

// ── Filesystem ───────────────────────────────────────────────
#define CONFIG_FILE             "/config.json"
