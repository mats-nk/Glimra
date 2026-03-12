// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Mats Nils Karlsson
// Glimra — MQTT LED Matrix Display Firmware
// https://github.com/mats-nk/Glimra

// main.cpp - MQTT Matrix Display (ESP32-S2 / ESP8266)
// Features: WiFiManager, Web GUI, MAX7219/Parola, MQTT, LittleFS

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <LittleFS.h>
#else
  #include <WiFi.h>
  #include <LittleFS.h>
#endif

#include <WiFiManager.h>
#include <Arduino.h>

#include "config.h"
#include "storage.h"
#include "display.h"
#include "mqtt_client.h"

// Forward-declare WebUI before including webserver.h
// so display.h is already resolved
#include "webserver.h"

// ---- Globals -----------------------------------------------
AppConfig      gCfg;
DisplayManager gDisplay;
WebUI          gWeb;
MqttManager    gMqtt;
WiFiManager    gWifiMgr;

static bool    gNeedReconfig  = false;
static bool    gNeedIntensity = false;
static uint8_t gNewIntensity  = 4;

// ---- Helpers -----------------------------------------------

// Builds "matrix-XXXX" where XXXX is the last 4 hex digits of the chip ID.
// Guaranteed unique per device; still short enough for mDNS and AP names.
static void generateDefaultHostname(char* buf, size_t len) {
#ifdef ESP8266
    uint32_t id = ESP.getChipId();
#else
    // ESP32: use lower 32 bits of the 48-bit EFuse MAC
    uint32_t id = (uint32_t)ESP.getEfuseMac();
#endif
    snprintf(buf, len, "%s-%08X", DEFAULT_HOSTNAME_PREFIX, id & 0xFFFFFFFF);
}

// ---- Callbacks from WebUI ----------------------------------
void onConfigSaved()            { gNeedReconfig = true; }
void onIntensityChange(uint8_t v){ gNewIntensity=v; gNeedIntensity=true; }
void doWifiReset() {
    gWifiMgr.resetSettings();
    Storage::resetConfig();
    delay(200); ESP.restart();
}

// ---- setup() -----------------------------------------------
void setup() {
    Serial.begin(115200); delay(200);
    Serial.println("\n[APP] MQTT Matrix Display booting");

    if (!Storage::begin()) {
        Serial.println("[FS] mount failed, formatting");
        LittleFS.format();
        Storage::begin();
    }
    if (Storage::loadConfig(gCfg)) {
        Serial.println("[APP] Config loaded");
    } else {
        Serial.println("[APP] Using defaults");
    }

    // Generate unique hostname if none is saved yet.
    // This runs on first boot or after a factory reset.
    if (strlen(gCfg.hostname) == 0) {
        generateDefaultHostname(gCfg.hostname, sizeof(gCfg.hostname));
        Serial.printf("[APP] Hostname: %s\n", gCfg.hostname);
    }

    gDisplay.begin(gCfg);
    gDisplay.showMessage("Connecting WiFi...");

    gWifiMgr.setHostname(gCfg.hostname);
    gWifiMgr.setConfigPortalTimeout(180);

    uint8_t mac[6]; WiFi.macAddress(mac);
    char apName[40];
    snprintf(apName, sizeof(apName), "%s-%02X%02X", gCfg.hostname, mac[4], mac[5]);

    if (!gWifiMgr.autoConnect(apName)) {
        gDisplay.showMessage("WiFi timeout");
        delay(2000); ESP.restart();
    }
    Serial.println("[WIFI] " + WiFi.localIP().toString());
    WiFi.localIP().toString().toCharArray(gWeb.ipStr, sizeof(gWeb.ipStr));
    gDisplay.showMessage(("IP: "+WiFi.localIP().toString()).c_str());

    gWeb.begin();

    if (strlen(gCfg.mqtt_server) > 0)
        gMqtt.begin(gCfg, &gWeb.mqttOk, gWeb.lastMsg, onConfigSaved);
    else {
        Serial.println("[MQTT] No broker - configure via Web GUI");
        gDisplay.showMessage("No MQTT. Configure at web GUI");
    }
    Serial.println("[APP] Ready");
}

// ---- loop() ------------------------------------------------
void loop() {
    gWeb.handle();
    gMqtt.loop();
    gDisplay.update();

    if (gNeedReconfig) {
        gNeedReconfig = false;
        gDisplay.reconfigure(gCfg);
        gMqtt.reconfigure(gCfg);
        WiFi.localIP().toString().toCharArray(gWeb.ipStr, sizeof(gWeb.ipStr));
        // Show "Config applied!" once, then resume the last live-control message.
        // If no message has been received yet, lastMsg is empty — fall back to idle text.
        const char* resume = (strlen(gWeb.lastMsg) > 0) ? gWeb.lastMsg : "MQTT Matrix";
        gDisplay.showMessageOnce("Config applied!", resume);
    }
    if (gNeedIntensity) {
        gNeedIntensity = false;
        gDisplay.setIntensity(gNewIntensity);
    }

    static uint32_t lastRssi = 0;
    if (millis()-lastRssi > 10000) {
        lastRssi = millis();
        gWeb.rssiVal = (int8_t)WiFi.RSSI();
    }
}
