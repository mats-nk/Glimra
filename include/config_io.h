// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Mats Nils Karlsson
// Glimra — MQTT LED Matrix Display Firmware
// https://github.com/mats-nk/Glimra

#pragma once
// =============================================================
//  config_io.h  —  JSON serialisation for AppConfig
//
//  Kept separate from config.h so that modules that only need
//  the struct (display.h, mqtt_client.h) do not pull in
//  ArduinoJson as a dependency.
// =============================================================

#include <ArduinoJson.h>
#include "config.h"     // AppConfig + defaults

// ── Struct → JSON ─────────────────────────────────────────────
inline void configToJson(const AppConfig& c, JsonDocument& d) {
    // Network
    d["hostname"]     = c.hostname;

    // MQTT
    d["mqtt_server"]  = c.mqtt_server;
    d["mqtt_port"]    = c.mqtt_port;
    d["mqtt_user"]    = c.mqtt_user;
    d["mqtt_pass"]    = c.mqtt_pass;
    d["mqtt_topic"]   = c.mqtt_topic;
    d["mqtt_topic2"]  = c.mqtt_topic2;
    d["mqtt_topic3"]  = c.mqtt_topic3;

    // MAX7219 hardware
    d["cs_pin"]       = c.cs_pin;
    d["clk_pin"]      = c.clk_pin;
    d["data_pin"]     = c.data_pin;
    d["modules"]      = c.modules;
    d["hw_type"]      = c.hw_type;
    d["rotation"]     = c.rotation;

    // Display behaviour
    d["intensity"]    = c.intensity;
    d["scroll_speed"] = c.scroll_speed;
    d["pause_time"]   = c.pause_time;
    d["anim_in"]      = c.anim_in;
    d["anim_out"]     = c.anim_out;
    d["invert"]       = c.invert;
}

// ── JSON → Struct ─────────────────────────────────────────────
inline void jsonToConfig(const JsonDocument& d, AppConfig& c) {
    // Network — empty string triggers runtime hostname generation in main.cpp
    strlcpy(c.hostname,    d["hostname"]    | "",                  sizeof(c.hostname));

    // MQTT
    strlcpy(c.mqtt_server, d["mqtt_server"] | "",                 sizeof(c.mqtt_server));
    c.mqtt_port = d["mqtt_port"] | DEFAULT_MQTT_PORT;
    strlcpy(c.mqtt_user,   d["mqtt_user"]   | "",                 sizeof(c.mqtt_user));
    strlcpy(c.mqtt_pass,   d["mqtt_pass"]   | "",                 sizeof(c.mqtt_pass));
    strlcpy(c.mqtt_topic,  d["mqtt_topic"]  | DEFAULT_MQTT_TOPIC, sizeof(c.mqtt_topic));
    strlcpy(c.mqtt_topic2, d["mqtt_topic2"] | "",                 sizeof(c.mqtt_topic2));
    strlcpy(c.mqtt_topic3, d["mqtt_topic3"] | "",                 sizeof(c.mqtt_topic3));

    // MAX7219 hardware
    c.cs_pin   = d["cs_pin"]   | DEFAULT_CS_PIN;
    c.clk_pin  = d["clk_pin"]  | DEFAULT_CLK_PIN;
    c.data_pin = d["data_pin"] | DEFAULT_DATA_PIN;
    c.modules  = d["modules"]  | DEFAULT_MODULES;
    c.hw_type  = d["hw_type"]  | DEFAULT_HW_TYPE;
    c.rotation = d["rotation"] | DEFAULT_ROTATION;

    // Display behaviour
    c.intensity    = d["intensity"]    | DEFAULT_INTENSITY;
    c.scroll_speed = d["scroll_speed"] | DEFAULT_SCROLL_SPEED;
    c.pause_time   = d["pause_time"]   | DEFAULT_PAUSE_TIME;
    c.anim_in      = d["anim_in"]      | DEFAULT_ANIM_IN;
    c.anim_out     = d["anim_out"]     | DEFAULT_ANIM_OUT;
    c.invert       = d["invert"]       | false;
}
