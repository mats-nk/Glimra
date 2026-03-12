// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Mats Nils Karlsson
// Glimra — MQTT LED Matrix Display Firmware
// https://github.com/mats-nk/Glimra

#pragma once
// =============================================================
//  config.h  —  AppConfig data structure
//
//  Only defines the struct. No JSON, no serialisation logic.
//  Include config_io.h when you need to load/save JSON.
// =============================================================

#include "defaults.h"

struct AppConfig {
    // ── Network ───────────────────────────────────────────────
    // Intentionally blank — main.cpp fills this with
    // DEFAULT_HOSTNAME_PREFIX + chip ID on first boot.
    // Once the user saves a custom name it is persisted to flash.
    char     hostname[32]     = "";

    // ── MQTT ──────────────────────────────────────────────────
    char     mqtt_server[64]  = "";
    uint16_t mqtt_port        = DEFAULT_MQTT_PORT;
    char     mqtt_user[32]    = "";
    char     mqtt_pass[32]    = "";
    char     mqtt_topic[128]  = DEFAULT_MQTT_TOPIC;
    char     mqtt_topic2[128] = "";
    char     mqtt_topic3[128] = "";

    // ── MAX7219 hardware ──────────────────────────────────────
    uint8_t  cs_pin           = DEFAULT_CS_PIN;
    uint8_t  clk_pin          = DEFAULT_CLK_PIN;
    uint8_t  data_pin         = DEFAULT_DATA_PIN;
    uint8_t  modules          = DEFAULT_MODULES;
    uint8_t  hw_type          = DEFAULT_HW_TYPE;
    uint8_t  rotation         = DEFAULT_ROTATION;

    // ── Display behaviour ─────────────────────────────────────
    uint8_t  intensity        = DEFAULT_INTENSITY;
    uint16_t scroll_speed     = DEFAULT_SCROLL_SPEED;
    uint16_t pause_time       = DEFAULT_PAUSE_TIME;
    uint8_t  anim_in          = DEFAULT_ANIM_IN;
    uint8_t  anim_out         = DEFAULT_ANIM_OUT;
    bool     invert           = false;
};
