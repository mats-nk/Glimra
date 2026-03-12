// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Mats Nils Karlsson
// Glimra — MQTT LED Matrix Display Firmware
// https://github.com/mats-nk/Glimra

#pragma once
// mqtt_client.h - PubSubClient wrapper with auto-reconnect and cmd topics.
// Deliberately does NOT include webserver.h to avoid circular deps.
// State is shared via plain pointers set in begin().
//
// ── Command topics ────────────────────────────────────────────────────────────
// Derived automatically from the primary topic by replacing the last path
// segment with "cmd/<parameter>".
//
//   Primary topic     : display/text
//   Cmd topic base    : display/cmd/
//
// Full list (payload is always a plain string):
//
//   display/cmd/text       — same as primary topic, show a message
//   display/cmd/intensity  — 0–15
//   display/cmd/speed      — scroll speed in ms (10–200)
//   display/cmd/pause      — pause time in ms (0–10000)
//   display/cmd/anim_in    — entry effect index (0–26)
//   display/cmd/anim_out   — exit effect index (0–26)
//   display/cmd/rotation   — 0/1/2/3  (0°/90°/180°/270°)
//   display/cmd/invert     — 1 = invert, 0 = normal
//   display/cmd/hw_type    — 0=Generic 1=FC16 2=Parola 3=ICStation
//   display/cmd/modules    — number of daisy-chained panels
//   display/cmd/hostname   — rename device (persisted, takes effect immediately)
// ─────────────────────────────────────────────────────────────────────────────

#include <PubSubClient.h>
#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif
#include "config.h"
#include "storage.h"
#include "display.h"

extern DisplayManager gDisplay;
extern AppConfig      gCfg;

class MqttManager {
public:
    MqttManager() : _client(_wifiClient) {}

    // Pass pointers to shared state.  onCmdApplied is called (from the MQTT
    // callback, i.e. inside loop()) whenever a cmd topic changes gCfg so that
    // main.cpp can trigger reconfigure() on the next loop iteration.
    void begin(const AppConfig& cfg,
               bool*  mqttOkPtr,
               char*  lastMsgPtr,
               void (*onCmdApplied)() = nullptr) {
        _cfg          = cfg;
        _mqttOkPtr    = mqttOkPtr;
        _lastMsgPtr   = lastMsgPtr;
        _onCmdApplied = onCmdApplied;
        _buildCmdBase();
        _setup();
    }

    void reconfigure(const AppConfig& cfg) {
        _cfg = cfg;
        _buildCmdBase();
        _client.disconnect();
        _setup();
    }

    void loop() {
        if (!_client.connected()) {
            unsigned long now = millis();
            if (now - _lastRetry > 5000) {
                _lastRetry = now;
                _connect();
            }
        }
        _client.loop();
        if (_mqttOkPtr) *_mqttOkPtr = _client.connected();
    }

    bool connected() { return _client.connected(); }

private:
    AppConfig       _cfg;
    WiFiClient      _wifiClient;
    PubSubClient    _client;
    bool*           _mqttOkPtr    = nullptr;
    char*           _lastMsgPtr   = nullptr;
    void          (*_onCmdApplied)() = nullptr;
    unsigned long   _lastRetry    = 0;
    char            _cmdBase[140] = "";   // e.g. "display/cmd/"

    // ── Derive cmd base from primary topic ───────────────────────────────────
    // "display/text"  →  "display/cmd/"
    // "matrix"        →  "cmd/"          (no slash in topic)
    void _buildCmdBase() {
        const char* slash = strrchr(_cfg.mqtt_topic, '/');
        if (slash) {
            size_t prefixLen = (size_t)(slash - _cfg.mqtt_topic);
            prefixLen = (prefixLen < sizeof(_cmdBase) - 6) ? prefixLen : sizeof(_cmdBase) - 6;
            strncpy(_cmdBase, _cfg.mqtt_topic, prefixLen);
            _cmdBase[prefixLen] = '\0';
            strncat(_cmdBase, "/cmd/", sizeof(_cmdBase) - strlen(_cmdBase) - 1);
        } else {
            strncpy(_cmdBase, "cmd/", sizeof(_cmdBase) - 1);
        }
        _cmdBase[sizeof(_cmdBase)-1] = '\0';
    }

    void _setup() {
        _client.setServer(_cfg.mqtt_server, _cfg.mqtt_port);
        _client.setCallback([this](char* topic, uint8_t* payload, unsigned int len){
            _onMessage(topic, payload, len);
        });
        _connect();
    }

    void _connect() {
        if (strlen(_cfg.mqtt_server) == 0) return;
        Serial.printf("[MQTT] Connecting to %s:%d ...\n",
                      _cfg.mqtt_server, _cfg.mqtt_port);
        String cid = "matrix-";
#ifdef ESP8266
        cid += String(ESP.getChipId(), HEX);
#else
        cid += String((uint32_t)ESP.getEfuseMac(), HEX);
#endif
        bool ok = (strlen(_cfg.mqtt_user) > 0)
            ? _client.connect(cid.c_str(), _cfg.mqtt_user, _cfg.mqtt_pass)
            : _client.connect(cid.c_str());

        if (ok) {
            Serial.println("[MQTT] Connected");
            _subscribe();
        } else {
            Serial.printf("[MQTT] Failed rc=%d\n", _client.state());
        }
    }

    void _subscribe() {
        // Text topics
        if (strlen(_cfg.mqtt_topic)  > 0) _client.subscribe(_cfg.mqtt_topic);
        if (strlen(_cfg.mqtt_topic2) > 0) _client.subscribe(_cfg.mqtt_topic2);
        if (strlen(_cfg.mqtt_topic3) > 0) _client.subscribe(_cfg.mqtt_topic3);

        // Cmd wildcard — subscribes to display/cmd/# in one call
        char cmdWildcard[148];
        snprintf(cmdWildcard, sizeof(cmdWildcard), "%s#", _cmdBase);
        _client.subscribe(cmdWildcard);

        Serial.printf("[MQTT] Subscribed — text topics + %s\n", cmdWildcard);
    }

    // ── Route incoming messages ───────────────────────────────────────────────
    void _onMessage(char* topic, uint8_t* payload, unsigned int len) {
        char buf[256];
        unsigned int copyLen = (len < sizeof(buf)-1) ? len : sizeof(buf)-1;
        memcpy(buf, payload, copyLen);
        buf[copyLen] = '\0';
        Serial.printf("[MQTT] %s => %s\n", topic, buf);

        // Check if this is a cmd topic
        size_t baseLen = strlen(_cmdBase);
        if (strncmp(topic, _cmdBase, baseLen) == 0) {
            _handleCmd(topic + baseLen, buf);
            return;
        }

        // Regular text message
        _utf8ToLatin1(buf);
        char msg[280];
        if (strlen(_cfg.mqtt_topic2) > 0 || strlen(_cfg.mqtt_topic3) > 0) {
            char* label = strrchr(topic, '/');
            label = label ? label + 1 : topic;
            snprintf(msg, sizeof(msg), "[%s] %s", label, buf);
        } else {
            strncpy(msg, buf, sizeof(msg)-1);
            msg[sizeof(msg)-1] = '\0';
        }
        gDisplay.showMessage(msg);
        if (_lastMsgPtr) strncpy(_lastMsgPtr, msg, 255);
        _publishAutoScrollWarning(msg);
    }

    // ── Handle a single cmd ───────────────────────────────────────────────────
    // 'cmd'     is the topic suffix after the base (e.g. "intensity")
    // 'payload' is the null-terminated string value
    void _handleCmd(const char* cmd, const char* payload) {
        bool needReconfig  = false;   // requires display + mqtt reinit
        bool needSave      = true;    // persist to flash (almost always yes)

        if (strcmp(cmd, "text") == 0) {
            // Same as publishing to the primary topic
            char buf[256];
            strncpy(buf, payload, sizeof(buf)-1);
            buf[sizeof(buf)-1] = '\0';
            _utf8ToLatin1(buf);
            gDisplay.showMessage(buf);
            if (_lastMsgPtr) strncpy(_lastMsgPtr, buf, 255);
            _publishAutoScrollWarning(buf);
            needSave = false;

        } else if (strcmp(cmd, "intensity") == 0) {
            int v = atoi(payload);
            if (v <  0) v =  0;
            if (v > 15) v = 15;
            gCfg.intensity = (uint8_t)v;
            gDisplay.setIntensity(gCfg.intensity);
            // setIntensity is live — no full reconfig needed

        } else if (strcmp(cmd, "speed") == 0) {
            int v = atoi(payload);
            if (v <  10) v =  10;
            if (v > 200) v = 200;
            gCfg.scroll_speed = (uint16_t)v;
            needReconfig = true;

        } else if (strcmp(cmd, "pause") == 0) {
            int v = atoi(payload);
            if (v <     0) v =     0;
            if (v > 10000) v = 10000;
            gCfg.pause_time = (uint16_t)v;
            needReconfig = true;

        } else if (strcmp(cmd, "anim_in") == 0) {
            int v = atoi(payload);
            if (v <  0) v =  0;
            if (v > 26) v = 26;
            gCfg.anim_in = (uint8_t)v;
            needReconfig = true;

        } else if (strcmp(cmd, "anim_out") == 0) {
            int v = atoi(payload);
            if (v <  0) v =  0;
            if (v > 26) v = 26;
            gCfg.anim_out = (uint8_t)v;
            needReconfig = true;

        } else if (strcmp(cmd, "rotation") == 0) {
            // Only 0° and 180° (value 2) are supported for scrolling text.
            // Any non-zero value is rounded to 2.
            int v = atoi(payload);
            gCfg.rotation = (v != 0) ? 2 : 0;
            needReconfig = true;

        } else if (strcmp(cmd, "invert") == 0) {
            gCfg.invert = (atoi(payload) != 0);
            needReconfig = true;

        } else if (strcmp(cmd, "hw_type") == 0) {
            int v = atoi(payload);
            if (v < 0) v = 0;
            if (v > 3) v = 3;
            gCfg.hw_type = (uint8_t)v;
            needReconfig = true;

        } else if (strcmp(cmd, "modules") == 0) {
            int v = atoi(payload);
            if (v <  1) v =  1;
            if (v > 32) v = 32;
            gCfg.modules = (uint8_t)v;
            needReconfig = true;

        } else if (strcmp(cmd, "hostname") == 0) {
            if (strlen(payload) > 0 && strlen(payload) < 32) {
                strncpy(gCfg.hostname, payload, sizeof(gCfg.hostname)-1);
                gCfg.hostname[sizeof(gCfg.hostname)-1] = '\0';
                // hostname change doesn't need display reinit
            }

        } else {
            Serial.printf("[MQTT] Unknown cmd: %s\n", cmd);
            return;
        }

        if (needSave) {
            Storage::saveConfig(gCfg);
            Serial.printf("[MQTT] cmd/%s applied and saved\n", cmd);
        }
        if (needReconfig && _onCmdApplied) {
            _onCmdApplied();
        }
    }

    // ── Auto-scroll warning publisher ────────────────────────────────────────
    // Called after every text message. If the message is wider than the display
    // and the configured effects are not scroll-based, publishes a warning to
    // the status topic (e.g. "display/status") so the sender can be notified.
    // The status topic base mirrors the cmd base: "display/cmd/" → "display/status"
    void _publishAutoScrollWarning(const char* msg) {
        if (!_client.connected()) return;
        if (!gDisplay.isLongMessage(msg)) return;

        bool inIsScroll  = (_cfg.anim_in  == 4 || _cfg.anim_in  == 5);
        bool outIsScroll = (_cfg.anim_out == 4 || _cfg.anim_out == 5);
        if (inIsScroll && outIsScroll) return;  // no substitution needed

        // Build status topic: same prefix as cmd base but ending in "status"
        char statusTopic[148];
        _buildStatusTopic(statusTopic, sizeof(statusTopic));
        const char* notice = "auto_scroll: message wider than display, scroll animation used";
        _client.publish(statusTopic, notice);
        Serial.printf("[MQTT] auto-scroll warning published to %s\n", statusTopic);
    }

    // Derives the status topic from the primary topic.
    // "display/text" → "display/status"
    // "matrix"       → "status"
    void _buildStatusTopic(char* buf, size_t len) {
        const char* slash = strrchr(_cfg.mqtt_topic, '/');
        if (slash) {
            size_t prefixLen = (size_t)(slash - _cfg.mqtt_topic);
            prefixLen = prefixLen < len - 8 ? prefixLen : len - 8;
            strncpy(buf, _cfg.mqtt_topic, prefixLen);
            buf[prefixLen] = '\0';
            strncat(buf, "/status", len - strlen(buf) - 1);
        } else {
            strncpy(buf, "status", len - 1);
        }
        buf[len-1] = '\0';
    }

    // ── UTF-8 → Latin-1 in-place ─────────────────────────────────────────────
    static void _utf8ToLatin1(char* s) {
        uint8_t* r = (uint8_t*)s;
        uint8_t* w = (uint8_t*)s;
        while (*r) {
            if (*r < 0x80)                                           { *w++=*r++; }
            else if (*r==0xC2 && *(r+1)>=0x80 && *(r+1)<=0xBF)     { *w++=*(r+1); r+=2; }
            else if (*r==0xC3 && *(r+1)>=0x80 && *(r+1)<=0xBF)     { *w++=(uint8_t)(*(r+1)+0x40); r+=2; }
            else if (*r>=0xC4 && *r<=0xDF)                          { *w++='?'; r+=2; }
            else if (*r>=0xE0 && *r<=0xEF)                          { *w++='?'; r+=3; }
            else if (*r>=0xF0)                                       { *w++='?'; r+=4; }
            else                                                     { r++; }
        }
        *w='\0';
    }
};

