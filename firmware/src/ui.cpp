#include "ui.h"

// ============================================================
// Constructor / begin
// ============================================================
UI::UI(TFT_eSPI &tft) : _tft(tft) {}

void UI::begin() {
    // Allow Serial to stabilise before touching the SPI bus.
    // This also lets the debug prints below appear before a potential crash.
    delay(500);

    Serial.println("[UI] begin: tft.init()...");
    Serial.flush();
    _tft.init();
    Serial.println("[UI] tft.init() OK");
    Serial.flush();

    _tft.setRotation(SCREEN_ROTATION);
    Serial.println("[UI] setRotation OK");
    Serial.flush();

    _tft.setSwapBytes(true);

    // Apply touch calibration
    Serial.println("[UI] setTouch...");
    Serial.flush();
    _tft.setTouch(_calData);
    Serial.println("[UI] setTouch OK");
    Serial.flush();

    // Backlight on (active-high; adjust if your module is active-low)
#ifdef TFT_BL
    Serial.println("[UI] backlight on...");
    Serial.flush();
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    Serial.println("[UI] backlight OK");
    Serial.flush();
#endif

    Serial.println("[UI] fillScreen...");
    Serial.flush();
    _tft.fillScreen(C_BG);
    Serial.println("[UI] fillScreen OK");
    Serial.flush();

    // Show splash until the PC sends the first message
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextDatum(MC_DATUM);
    _tft.setTextFont(4);
    _tft.drawString("ESP32 Dongle", SCREEN_W / 2, SCREEN_H / 2 - 20);
    _tft.setTextFont(2);
    _tft.drawString("Waiting for PC...", SCREEN_W / 2, SCREEN_H / 2 + 16);

    Serial.println("[UI] begin complete");
    Serial.flush();

    // Don't set _fullRedraw here – the splash must stay until first data arrives
    _fullRedraw = false;
}

// ============================================================
// State setters
// ============================================================
void UI::setTime(const TimeData &t) {
    _time = t;
}

void UI::setWeather(const WeatherData &w) {
    _wx = w;
}

void UI::setMedia(const MediaData &m) {
    // Reset scroll when title changes
    if (strcmp(m.title, _media.title) != 0) {
        _scrollX     = 0;
        _scrollTimer = millis();
        _scrollPaused = true;
    }
    _media = m;
}

void UI::setConnected(bool connected) {
    // First time we get a connection trigger a full redraw to replace the splash
    if (connected && !_connected) {
        _fullRedraw = true;
    }
    _connected = connected;
}

// ============================================================
// Main tick – call every loop()
// ============================================================
void UI::tick() {
    if (_fullRedraw) {
        _tft.fillScreen(C_BG);
        _drawStatusBar();
        _drawClockSection();
        _drawWeatherSection();
        _drawMediaSection();
        _drawButtons();
        _fullRedraw = false;
        // Reset previous-value cache to force all redraws
        _prevHHMM[0]   = '\0';
        _prevSS[0]     = '\0';
        _prevDate[0]   = '\0';
        _prevCity[0]   = '\0';
        _prevTemp[0]   = '\0';
        _prevFeels[0]  = '\0';
        _prevDesc[0]   = '\0';
        _prevHum[0]    = '\0';
        _prevArtist[0] = '\0';
        _prevTitle[0]  = '\0';
        _prevPlaying   = !_media.playing;  // force redraw
        _prevVol       = 255;
        _prevConn      = !_connected;
    }

    // Status bar – connection dot
    if (_connected != _prevConn) {
        _drawStatusBar();
        _prevConn = _connected;
    }

    // Clock – HH:MM
    char hhmm[6];
    snprintf(hhmm, sizeof(hhmm), "%s:%s", _time.hh, _time.mm);
    if (strcmp(hhmm, _prevHHMM) != 0) {
        _eraseRect(0, CLK_HHMM_Y, SCREEN_W, 48);
        _tft.setTextColor(C_TIME_HM, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(7);          // Seven-segment 48px
        _tft.drawString(hhmm, SCREEN_W / 2, CLK_HHMM_Y);
        strlcpy(_prevHHMM, hhmm, sizeof(_prevHHMM));
    }

    // Clock – :SS
    char ss_str[4];
    snprintf(ss_str, sizeof(ss_str), ":%s", _time.ss);
    if (strcmp(ss_str, _prevSS) != 0) {
        _eraseRect(0, CLK_SS_Y, SCREEN_W, 22);
        _tft.setTextColor(C_TIME_S, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(4);          // 26px
        _tft.drawString(ss_str, SCREEN_W / 2, CLK_SS_Y);
        strlcpy(_prevSS, ss_str, sizeof(_prevSS));
    }

    // Clock – date
    if (strcmp(_time.date, _prevDate) != 0) {
        _eraseRect(0, CLK_DATE_Y, SCREEN_W, 16);
        _tft.setTextColor(C_DATE, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(2);          // 16px
        char datebuf[32];
        snprintf(datebuf, sizeof(datebuf), "%s  %s", _time.date, _time.year);
        _tft.drawString(datebuf, SCREEN_W / 2, CLK_DATE_Y);
        strlcpy(_prevDate, _time.date, sizeof(_prevDate));
    }

    // Weather – city
    if (strcmp(_wx.city, _prevCity) != 0) {
        _eraseRect(0, WX_CITY_Y, SCREEN_W, 18);
        _tft.setTextColor(C_WEATHER_LABEL, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(2);
        _tft.drawString(_wx.city, SCREEN_W / 2, WX_CITY_Y);
        strlcpy(_prevCity, _wx.city, sizeof(_prevCity));
    }

    // Weather – temperature
    if (strcmp(_wx.temp, _prevTemp) != 0 || strcmp(_wx.feels, _prevFeels) != 0) {
        _eraseRect(0, WX_TEMP_Y, SCREEN_W, 32);
        _tft.setTextColor(C_WEATHER_TEMP, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(4);          // 26px
        char tbuf[24];
        snprintf(tbuf, sizeof(tbuf), "%s\xB0""C  (%s\xB0""C)", _wx.temp, _wx.feels);
        _tft.drawString(tbuf, SCREEN_W / 2, WX_TEMP_Y);
        strlcpy(_prevTemp,  _wx.temp,  sizeof(_prevTemp));
        strlcpy(_prevFeels, _wx.feels, sizeof(_prevFeels));
    }

    // Weather – description
    if (strcmp(_wx.desc, _prevDesc) != 0) {
        _eraseRect(0, WX_DESC_Y, SCREEN_W, 14);
        _tft.setTextColor(C_WEATHER_TEXT, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(1);          // 8px GLCD
        _tft.drawString(_wx.desc, SCREEN_W / 2, WX_DESC_Y);
        strlcpy(_prevDesc, _wx.desc, sizeof(_prevDesc));
    }

    // Weather – humidity
    if (strcmp(_wx.hum, _prevHum) != 0) {
        _eraseRect(0, WX_HUM_Y, SCREEN_W, 14);
        _tft.setTextColor(C_WEATHER_LABEL, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(1);
        char hbuf[24];
        snprintf(hbuf, sizeof(hbuf), "Humidity: %s", _wx.hum);
        _tft.drawString(hbuf, SCREEN_W / 2, WX_HUM_Y);
        strlcpy(_prevHum, _wx.hum, sizeof(_prevHum));
    }

    // Media – artist
    if (strcmp(_media.artist, _prevArtist) != 0) {
        _eraseRect(0, MD_ARTIST_Y, SCREEN_W, 16);
        _tft.setTextColor(C_MEDIA_ARTIST, C_BG);
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextFont(2);
        _tft.drawString(_media.artist, SCREEN_W / 2, MD_ARTIST_Y);
        strlcpy(_prevArtist, _media.artist, sizeof(_prevArtist));
    }

    // Media – play/pause indicator (coloured dot left of artist row)
    if (_media.playing != _prevPlaying) {
        uint16_t col = _media.playing ? C_PLAYING : C_PAUSED;
        _tft.fillCircle(8, MD_ARTIST_Y + 7, 4, col);
        _prevPlaying = _media.playing;
        // Redraw play button to reflect new state
        _drawButton(BTN_PLAY_PAUSE, false);
    }

    // Media – volume bar
    if (_media.vol != _prevVol) {
        _drawVolBar();
        _prevVol = _media.vol;
    }

    // Media – scrolling title
    _drawTitleScroll();
}

// ============================================================
// Touch polling
// ============================================================
int8_t UI::pollTouch() {
    uint16_t tx, ty;
    if (!_tft.getTouch(&tx, &ty)) {
        return -1;
    }
    uint32_t now = millis();
    if (now - _lastTouchMs < TOUCH_DEBOUNCE_MS) {
        return -1;
    }
    _lastTouchMs = now;

    // Only respond to touches in the button row
    if (ty < BTN_Y || ty > BTN_Y + BTN_H) {
        return -1;
    }

    for (int8_t i = 0; i < BTN_COUNT; i++) {
        int16_t bx = _btnX(i);
        if (tx >= bx && tx <= bx + BTN_W) {
            _drawButton(i, true);
            delay(80);
            _drawButton(i, false);
            return i;
        }
    }
    return -1;
}

// ============================================================
// Private helpers
// ============================================================
int16_t UI::_btnX(uint8_t idx) {
    return BTN_MARGIN + idx * (BTN_W + BTN_MARGIN);
}

void UI::_eraseRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t col) {
    _tft.fillRect(x, y, w, h, col);
}

// Draw horizontal divider line
static void drawDivider(TFT_eSPI &tft, int16_t y) {
    tft.drawFastHLine(0, y, SCREEN_W, C_DIVIDER);
}

void UI::_drawStatusBar() {
    _tft.fillRect(0, SB_Y, SCREEN_W, SB_H, C_BG);

    // Connection dot
    uint16_t dotCol = _connected ? C_CONN_OK : C_CONN_NO;
    _tft.fillCircle(8, SB_Y + SB_H / 2, 5, dotCol);

    // Label
    _tft.setTextColor(_connected ? C_CONN_OK : C_CONN_NO, C_BG);
    _tft.setTextDatum(ML_DATUM);
    _tft.setTextFont(1);
    _tft.drawString(_connected ? "PC Connected" : "Waiting for PC...", 18, SB_Y + SB_H / 2);

    drawDivider(_tft, SB_Y + SB_H);
}

void UI::_drawClockSection() {
    // Draw section frame (divider at bottom)
    drawDivider(_tft, CLK_Y + CLK_H);
}

void UI::_drawWeatherSection() {
    drawDivider(_tft, WX_Y + WX_H);
}

void UI::_drawMediaSection() {
    // Static "♪" label
    _tft.setTextColor(C_MEDIA_ARTIST, C_BG);
    _tft.setTextDatum(ML_DATUM);
    _tft.setTextFont(1);
    _tft.drawString("~", 4, MD_TITLE_Y + 4);
}

void UI::_drawButtons() {
    for (uint8_t i = 0; i < BTN_COUNT; i++) {
        _drawButton(i, false);
    }
}

void UI::_drawButton(uint8_t idx, bool pressed) {
    int16_t bx = _btnX(idx);
    int16_t by = BTN_Y;
    uint16_t bg  = pressed ? C_BTN_PRESS : C_BTN_BG;
    int16_t cx = bx + BTN_W / 2;
    int16_t cy = by + BTN_H / 2;

    _tft.fillRoundRect(bx, by, BTN_W, BTN_H, 4, bg);

    _tft.setTextColor(C_BTN_TEXT, bg);
    _tft.setTextDatum(MC_DATUM);

    switch (idx) {
        case BTN_PREV:
            // |◄◄  – two left triangles + bar
            _tft.fillRect(bx + 5, by + 8, 3, BTN_H - 16, C_BTN_TEXT);
            _tft.fillTriangle(bx + 8, cy, bx + 18, by + 9, bx + 18, cy + (cy - by - 9), C_BTN_TEXT);
            _tft.fillTriangle(bx + 18, cy, bx + 28, by + 9, bx + 28, cy + (cy - by - 9), C_BTN_TEXT);
            break;
        case BTN_VOL_DOWN:
            _tft.setTextFont(4);
            _tft.drawString("-", cx, cy - 4);
            break;
        case BTN_PLAY_PAUSE:
            _drawPlayIcon(cx, cy, _media.playing);
            break;
        case BTN_VOL_UP:
            _tft.setTextFont(4);
            _tft.drawString("+", cx, cy - 4);
            break;
        case BTN_NEXT:
            // ►►|
            _tft.fillTriangle(bx + 4,  cy, bx + 14, by + 9, bx + 14, cy + (cy - by - 9), C_BTN_TEXT);
            _tft.fillTriangle(bx + 14, cy, bx + 24, by + 9, bx + 24, cy + (cy - by - 9), C_BTN_TEXT);
            _tft.fillRect(bx + BTN_W - 8, by + 8, 3, BTN_H - 16, C_BTN_TEXT);
            break;
    }
}

void UI::_drawPlayIcon(int16_t cx, int16_t cy, bool playing) {
    uint16_t col = C_BTN_TEXT;
    if (!playing) {
        // Play: right-pointing triangle
        _tft.fillTriangle(cx - 10, cy - 13, cx - 10, cy + 13, cx + 12, cy, col);
    } else {
        // Pause: two vertical bars
        _tft.fillRect(cx - 10, cy - 12, 7, 24, col);
        _tft.fillRect(cx + 3,  cy - 12, 7, 24, col);
    }
}

void UI::_drawVolBar() {
    int16_t fillW = (int32_t)MD_VOL_BAR_W * _media.vol / 100;
    // Background
    _tft.fillRoundRect(MD_VOL_BAR_X, MD_VOL_BAR_Y, MD_VOL_BAR_W, MD_VOL_BAR_H, 3, C_VOL_BG);
    // Fill
    if (fillW > 0) {
        _tft.fillRoundRect(MD_VOL_BAR_X, MD_VOL_BAR_Y, fillW, MD_VOL_BAR_H, 3, C_VOL_BAR);
    }
    // Volume label
    _tft.setTextColor(C_DATE, C_BG);
    _tft.setTextDatum(MR_DATUM);
    _tft.setTextFont(1);
    char vbuf[8];
    snprintf(vbuf, sizeof(vbuf), "%d%%", _media.vol);
    _tft.drawString(vbuf, SCREEN_W - 2, MD_VOL_BAR_Y + MD_VOL_BAR_H / 2);
}

void UI::_drawTitleScroll() {
    const int16_t AREA_X = 12;
    const int16_t AREA_W = SCREEN_W - 14;
    const int16_t AREA_Y = MD_TITLE_Y;
    const int16_t AREA_H = 16;

    bool titleChanged = strcmp(_media.title, _prevTitle) != 0;
    if (titleChanged) {
        strlcpy(_prevTitle, _media.title, sizeof(_prevTitle));
        _scrollX      = 0;
        _scrollTimer  = millis();
        _scrollPaused = true;

        _tft.setTextFont(2);
        _titlePixW = _tft.textWidth(_media.title);
    }

    // No scrolling needed if text fits
    if (_titlePixW <= AREA_W) {
        if (titleChanged) {
            _eraseRect(AREA_X, AREA_Y, AREA_W, AREA_H);
            _tft.setTextColor(C_MEDIA_TITLE, C_BG);
            _tft.setTextDatum(ML_DATUM);
            _tft.setTextFont(2);
            _tft.drawString(_media.title, AREA_X, AREA_Y);
        }
        return;
    }

    // Scrolling
    uint32_t now = millis();
    if (_scrollPaused) {
        if (now - _scrollTimer >= SCROLL_PAUSE_MS) {
            _scrollPaused = false;
            _scrollTimer  = now;
        }
        if (titleChanged) {
            // Draw initial position immediately
            _eraseRect(AREA_X, AREA_Y, AREA_W, AREA_H);
            _tft.setTextColor(C_MEDIA_TITLE, C_BG);
            _tft.setTextDatum(ML_DATUM);
            _tft.setTextFont(2);
            _tft.drawString(_media.title, AREA_X, AREA_Y);
        }
        return;
    }

    if (now - _scrollTimer < SCROLL_SPEED_MS) return;
    _scrollTimer = now;

    _scrollX++;
    if (_scrollX > _titlePixW + AREA_W / 2) {
        _scrollX      = 0;
        _scrollPaused = true;
        _scrollTimer  = now;
    }

    // Use a sprite or simple clip trick: set a viewport, draw offset text
    // TFT_eSPI doesn't have native clipping, so we draw into a sprite
    TFT_eSprite spr(&_tft);
    spr.createSprite(AREA_W, AREA_H);
    if (!spr.created()) {
        // Heap exhausted – skip this frame rather than crash
        return;
    }
    spr.fillSprite(C_BG);
    spr.setTextColor(C_MEDIA_TITLE, C_BG);
    spr.setTextDatum(ML_DATUM);
    spr.setTextFont(2);
    spr.drawString(_media.title, -_scrollX, 0);
    spr.pushSprite(AREA_X, AREA_Y);
    spr.deleteSprite();
}
