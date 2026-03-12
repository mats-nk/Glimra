// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Mats Nils Karlsson
// Glimra — MQTT LED Matrix Display Firmware
// https://github.com/mats-nk/Glimra

#pragma once
// display.h - MAX7219 display manager via MD_Parola
//
// Two-phase animation:
//   Phase 0 (ENTER) — displayText with anim_in only, exit = PA_NO_EFFECT.
//                     Parola scrolls text fully into the display and holds.
//   Phase 1 (EXIT)  — displayText with PA_PRINT entry, anim_out only.
//                     Parola starts from the settled position and scrolls out.
//
// This avoids the MD_Parola bug where asymmetric in/out effects on text wider
// than the display cause the exit to start mid-scroll, truncating the message.

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "config.h"

// ── Built-in sprite: Pac-Man chased by a ghost ────────────────────────────────
#define W_PMAN1  8
#define F_PMAN1  6
static const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] = {
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
};
#define W_PMAN2  18
#define F_PMAN2   6
static const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] = {
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00,
  0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00,
  0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00,
  0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00,
  0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00,
  0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00,
  0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
};
// ─────────────────────────────────────────────────────────────────────────────

class DisplayManager {
public:
    DisplayManager() : _parola(nullptr) {}
    ~DisplayManager() {
        if (_parola) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
            MD_Parola* p = _parola; _parola = nullptr; delete p;
#pragma GCC diagnostic pop
        }
    }

    void begin(const AppConfig& cfg)          { _cfg = cfg; _init(); }
    void reconfigure(const AppConfig& cfg) {
        if (_parola) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
            MD_Parola* p = _parola; _parola = nullptr; delete p;
#pragma GCC diagnostic pop
        }
        _cfg = cfg; _init();
    }

    // Queue a message — applied at the next animation cycle boundary.
    void showMessage(const char* msg) {
        strncpy(_pendingBuf, msg, sizeof(_pendingBuf)-1);
        _pendingBuf[sizeof(_pendingBuf)-1] = 0;
        _hasPending = true;
    }

    // Show 'msg' immediately, then resume 'resumeMsg' when done.
    void showMessageOnce(const char* msg, const char* resumeMsg) {
        strncpy(_msgBuf,    msg,       sizeof(_msgBuf)-1);
        strncpy(_resumeBuf, resumeMsg, sizeof(_resumeBuf)-1);
        _msgBuf[sizeof(_msgBuf)-1]       = 0;
        _resumeBuf[sizeof(_resumeBuf)-1] = 0;
        _hasPending = false;
        _newMsg     = true;
    }

    void setIntensity(uint8_t val) {
        _cfg.intensity = val;
        if (_parola) _parola->setIntensity(val);
    }

    // Returns true (and clears the flag) if the last message was wider than
    // the display and scroll was substituted for the configured animation.
    bool wasAutoScrolled() {
        bool v = _autoScrolled;
        _autoScrolled = false;
        return v;
    }

    // Returns true if the given text is wider than the physical display.
    bool isLongMessage(const char* text) {
        return _textPixelWidth(text) > (_cfg.modules * 8);
    }

    void update() {
        if (!_parola) return;
        if (!_parola->displayAnimate()) return;   // still animating

        // ── Cycle boundary reached ────────────────────────────────────────────
        if (_phase == PHASE_ENTER && !_isLongMsg) {
            // Short message: entry done, now run the exit effect from settled position.
            // PA_PRINT snaps text in place instantly, then anim_out scrolls it away.
            // Clear _newMsg here — if we leave it set until PHASE_EXIT completes,
            // the bottom of update() would replay _msgBuf instead of going to _resumeBuf.
            _phase  = PHASE_EXIT;
            _newMsg = false;
            _parola->displayText(_msgBuf, PA_CENTER,
                _cfg.scroll_speed, _cfg.pause_time,
                PA_PRINT, _effOut);
            _setSprite();
            return;
        }
        // Long message: PA_SCROLL_LEFT entry scrolls the full text through in one
        // pass — there is no separate exit phase. Fall through to load next message.

        // PHASE_EXIT complete (or no separate phases needed) — load next msg.
        _phase = PHASE_ENTER;

        if (_newMsg) {
            _startEnter(_msgBuf);
            _newMsg = false;
        } else if (_resumeBuf[0] != '\0') {
            strncpy(_msgBuf, _resumeBuf, sizeof(_msgBuf)-1);
            _msgBuf[sizeof(_msgBuf)-1] = 0;
            _resumeBuf[0] = '\0';
            _startEnter(_msgBuf);
        } else if (_hasPending) {
            strncpy(_msgBuf, _pendingBuf, sizeof(_msgBuf)-1);
            _msgBuf[sizeof(_msgBuf)-1] = 0;
            _hasPending = false;
            _startEnter(_msgBuf);
        } else {
            // No new message — loop the current one
            if (_msgBuf[0] != '\0')
                _startEnter(_msgBuf);
            else
                _parola->displayClear();
        }
    }

    bool isReady() const { return _parola != nullptr; }

private:
    enum Phase { PHASE_ENTER, PHASE_EXIT };

    AppConfig  _cfg;
    MD_Parola* _parola       = nullptr;
    char _msgBuf[256]        = "";
    char _pendingBuf[256]    = "";
    char _resumeBuf[256]     = "";
    bool _newMsg             = false;
    bool _hasPending         = false;
    bool _autoScrolled       = false;
    bool _isLongMsg          = false;   // true when auto-scroll substitution is active
    textEffect_t _effOut     = PA_SCROLL_LEFT;  // effective exit effect for current message
    Phase _phase             = PHASE_ENTER;

    static MD_MAX72XX::moduleType_t hwType(uint8_t t) {
        switch(t){
            case 1: return MD_MAX72XX::FC16_HW;
            case 2: return MD_MAX72XX::PAROLA_HW;
            case 3: return MD_MAX72XX::ICSTATION_HW;
            default:return MD_MAX72XX::GENERIC_HW;
        }
    }

    // Measures the total pixel width of a string using the current font.
    // Sums individual character widths from MD_MAX72XX::getChar() plus
    // one column of inter-character spacing between each glyph.
    // Returns 0 if the display is not initialised.
    uint16_t _textPixelWidth(const char* text) {
        if (!_parola) return 0;
        MD_MAX72XX* mx = _parola->getGraphicObject();
        if (!mx || !text || text[0] == '\0') return 0;

        uint8_t cbuf[8];   // max glyph width for default 8-pixel-high font
        uint16_t width = 0;
        bool first = true;
        for (const char* p = text; *p != '\0'; p++) {
            uint8_t w = mx->getChar((uint8_t)*p, sizeof(cbuf), cbuf);
            if (w == 0) continue;
            if (!first) width += 1;   // inter-character spacing column
            width += w;
            first = false;
        }
        return width;
    }

    // Start the entry phase.
    // - Short messages: two-phase approach (entry effect → pause → exit effect).
    // - Long messages: displayScroll() for a single continuous pass that shows
    //   the full text. No entry/exit split — the text scrolls in from the right,
    //   travels fully across the display, and scrolls off the left in one cycle.
    void _startEnter(const char* text) {
        _phase = PHASE_ENTER;

        bool isLong = (_textPixelWidth(text) > (uint16_t)(_cfg.modules * 8));

        if (isLong) {
            // displayScroll() is the only Parola call that shows text wider than
            // the display — it scrolls the full content through in one pass.
            // All other effects clip anything outside the display boundaries.
            bool inIsScroll  = (_cfg.anim_in  == 4 || _cfg.anim_in  == 5);
            bool outIsScroll = (_cfg.anim_out == 4 || _cfg.anim_out == 5);
            _autoScrolled = (!inIsScroll || !outIsScroll);
            _isLongMsg    = true;
            _parola->displayScroll(text, PA_LEFT, PA_SCROLL_LEFT, _cfg.scroll_speed);
        } else {
            _autoScrolled = false;
            _isLongMsg    = false;
            _effOut = (textEffect_t)_cfg.anim_out;
            _parola->displayText(text, PA_CENTER,
                _cfg.scroll_speed, _cfg.pause_time,
                (textEffect_t)_cfg.anim_in, PA_NO_EFFECT);
            _setSprite();
        }
    }

    void _setSprite() {
        if (_cfg.anim_in == 6 || _cfg.anim_out == 6) {
            _parola->setSpriteData(
                pacman1, W_PMAN1, F_PMAN1,
                pacman2, W_PMAN2, F_PMAN2);
        }
    }

    void _init() {
        _parola = new MD_Parola(hwType(_cfg.hw_type),
            _cfg.data_pin, _cfg.clk_pin, _cfg.cs_pin, _cfg.modules);
        if (!_parola->begin()) { Serial.println("[DISP] begin() failed"); return; }
        _parola->setIntensity(_cfg.intensity);
        _parola->setTextAlignment(PA_CENTER);
        _parola->setInvert(_cfg.invert);

        bool doFlip = (_cfg.rotation == 2);
        _parola->setZoneEffect(0, doFlip, PA_FLIP_UD);
        _parola->setZoneEffect(0, doFlip, PA_FLIP_LR);

        _msgBuf[0]    = '\0';
        _resumeBuf[0] = '\0';
        _hasPending   = false;
        _newMsg       = false;
        _isLongMsg    = false;
        _phase        = PHASE_ENTER;
        _parola->displayClear();
    }
};

