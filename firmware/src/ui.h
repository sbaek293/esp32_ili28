#pragma once
#include <TFT_eSPI.h>
#include <Arduino.h>
#include "config.h"

// ============================================================
// Shared application state (updated by Protocol, read by UI)
// ============================================================
struct TimeData {
    char hh[3]   = "00";
    char mm[3]   = "00";
    char ss[3]   = "00";
    char date[20] = "";      // e.g. "Mon Jan 15"
    char year[5]  = "2024";
    bool valid    = false;
};

struct WeatherData {
    char city[32]  = "---";
    char temp[8]   = "--";
    char feels[8]  = "--";
    char desc[32]  = "Connecting...";
    char hum[6]    = "--%";
    bool valid     = false;
};

struct MediaData {
    char title[64]  = "No Media";
    char artist[48] = "---";
    bool playing    = false;
    uint8_t vol     = 0;
    bool valid      = false;
};

// ============================================================
// UI class
// ============================================================
class UI {
public:
    explicit UI(TFT_eSPI &tft);

    // Call once in setup()
    void begin();

    // Call every loop() – handles scroll animation and partial redraws
    void tick();

    // Update individual sections (called by Protocol after parsing)
    void setTime(const TimeData &t);
    void setWeather(const WeatherData &w);
    void setMedia(const MediaData &m);

    // Connection indicator (true = PC connected)
    void setConnected(bool connected);

    // Returns button index (BTN_PREV…BTN_NEXT) if a button was pressed,
    // or -1 if no touch event occurred.
    int8_t pollTouch();

private:
    TFT_eSPI &_tft;

    TimeData    _time;
    WeatherData _wx;
    MediaData   _media;
    bool        _connected = false;

    // Track what's drawn to avoid unnecessary redraws
    char _prevHHMM[6] = "";
    char _prevSS[5]   = "";  // ":SS\0" = 4 bytes; was [3] which caused truncation
    char _prevDate[20] = "";
    char _prevCity[32] = "";
    char _prevTemp[8]  = "";
    char _prevFeels[8] = "";
    char _prevDesc[32] = "";
    char _prevHum[6]   = "";
    char _prevArtist[48] = "";
    bool _prevPlaying  = false;
    uint8_t _prevVol   = 255;
    bool _prevConn     = false;
    bool _fullRedraw   = true;

    // Title scroll state
    char    _prevTitle[64] = "";
    int16_t _scrollX       = 0;
    int16_t _titlePixW     = 0;  // pixel width of title string
    uint32_t _scrollTimer  = 0;
    bool    _scrollPaused  = true;

    // Touch state
    uint32_t _lastTouchMs = 0;
    bool     _calDone     = false;
    uint16_t _calData[5]  = TOUCH_CAL_DATA;

    // Title scroll sprite (member to avoid stack pressure each frame)
    TFT_eSprite _spr;

    // Drawing helpers
    void _drawStatusBar();
    void _drawClockSection();
    void _drawWeatherSection();
    void _drawMediaSection();
    void _drawButtons();
    void _drawButton(uint8_t idx, bool pressed);
    void _drawPlayIcon(int16_t cx, int16_t cy, bool playing);
    void _drawVolBar();
    void _drawTitleScroll();
    void _eraseRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t col = C_BG);
    int16_t _btnX(uint8_t idx);
};
