#include "protocol.h"
#include <ArduinoJson.h>

Protocol::Protocol(UI &ui) : _ui(ui) {}

// ============================================================
// tick – read serial bytes and parse complete lines
// ============================================================
bool Protocol::tick() {
    bool got = false;
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\n' || c == '\r') {
            if (_len > 0) {
                _buf[_len] = '\0';
                _parseLine(_buf, _len);
                _len = 0;
                got  = true;
            }
        } else {
            if (_len < MAX_SERIAL_BUF - 1) {
                _buf[_len++] = c;
            }
            // Overflow protection: reset buffer
            else {
                _len = 0;
            }
        }
    }
    return got;
}

// ============================================================
// _parseLine – decode one JSON message and push data to UI
// ============================================================
void Protocol::_parseLine(const char *line, uint16_t len) {
    // Ignore empty lines or non-JSON
    if (len < 3 || line[0] != '{') return;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, line, len);
    if (err) return;

    const char *type = doc["t"] | "";

    // ── Time message ─────────────────────────────────────────
    if (type[0] == 'T') {
        TimeData t;
        strlcpy(t.hh,   doc["hh"]   | "00", sizeof(t.hh));
        strlcpy(t.mm,   doc["mm"]   | "00", sizeof(t.mm));
        strlcpy(t.ss,   doc["ss"]   | "00", sizeof(t.ss));
        strlcpy(t.date, doc["date"] | "",   sizeof(t.date));
        strlcpy(t.year, doc["year"] | "",   sizeof(t.year));
        t.valid = true;
        _ui.setTime(t);
        _ui.setConnected(true);
        return;
    }

    // ── Weather message ───────────────────────────────────────
    if (type[0] == 'W') {
        WeatherData w;
        strlcpy(w.city,  doc["city"]  | "---",    sizeof(w.city));
        strlcpy(w.temp,  doc["temp"]  | "--",      sizeof(w.temp));
        strlcpy(w.feels, doc["feels"] | "--",      sizeof(w.feels));
        strlcpy(w.desc,  doc["desc"]  | "---",     sizeof(w.desc));
        strlcpy(w.hum,   doc["hum"]   | "--%",     sizeof(w.hum));
        w.valid = true;
        _ui.setWeather(w);
        return;
    }

    // ── Media message ─────────────────────────────────────────
    if (type[0] == 'M') {
        MediaData m;
        strlcpy(m.title,  doc["title"]  | "No Media", sizeof(m.title));
        strlcpy(m.artist, doc["artist"] | "---",       sizeof(m.artist));
        m.playing = (bool)(int)doc["playing"];
        m.vol     = (uint8_t)constrain((int)doc["vol"], 0, 100);
        m.valid   = true;
        _ui.setMedia(m);
        return;
    }
}

// ============================================================
// sendCommand – translate button index to command string
// ============================================================
void Protocol::sendCommand(int8_t btnIdx) {
    switch (btnIdx) {
        case BTN_PREV:      Serial.println("CMD:prev");       break;
        case BTN_NEXT:      Serial.println("CMD:next");       break;
        case BTN_PLAY_PAUSE:Serial.println("CMD:play_pause"); break;
        case BTN_VOL_UP:    Serial.println("CMD:vol_up");     break;
        case BTN_VOL_DOWN:  Serial.println("CMD:vol_down");   break;
        default: break;
    }
    Serial.flush();
}
