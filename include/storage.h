// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Mats Nils Karlsson
// Glimra — MQTT LED Matrix Display Firmware
// https://github.com/mats-nk/Glimra

#pragma once
// storage.h - LittleFS config load/save (ESP32-S2 + ESP8266)
#include "config_io.h"  // AppConfig + JSON helpers + defaults
#include <LittleFS.h>

#ifdef ESP32
  #define FSYS LittleFS
  #define FS_BEGIN() LittleFS.begin(true)
#else
  #define FSYS LittleFS
  #define FS_BEGIN() LittleFS.begin()
#endif

namespace Storage {

inline bool begin() { return FS_BEGIN(); }

inline bool loadConfig(AppConfig& cfg) {
    if (!FSYS.exists(CONFIG_FILE)) return false;
    File f = FSYS.open(CONFIG_FILE, "r");
    if (!f) return false;
    JsonDocument doc;
    auto err = deserializeJson(doc, f);
    f.close();
    if (err) return false;
    jsonToConfig(doc, cfg);
    return true;
}

inline bool saveConfig(const AppConfig& cfg) {
    File f = FSYS.open(CONFIG_FILE, "w");
    if (!f) return false;
    JsonDocument doc;
    configToJson(cfg, doc);
    serializeJson(doc, f);
    f.close();
    return true;
}

inline void resetConfig() {
    if (FSYS.exists(CONFIG_FILE)) FSYS.remove(CONFIG_FILE);
}

} // namespace Storage
