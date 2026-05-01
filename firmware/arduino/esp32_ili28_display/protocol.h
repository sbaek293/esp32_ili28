#pragma once
#include <Arduino.h>
#include "ui.h"   // for TimeData / WeatherData / MediaData

// ============================================================
// Protocol class
//
// PC → ESP32 (newline-terminated JSON, sent over USB Serial):
//   {"t":"T","hh":"14","mm":"30","ss":"25","date":"Mon Jan 15","year":"2024"}
//   {"t":"W","city":"Seoul","temp":"23","feels":"21","desc":"Partly Cloudy","hum":"60%"}
//   {"t":"M","title":"Bohemian Rhapsody","artist":"Queen","playing":1,"vol":75}
//
// ESP32 → PC (newline-terminated command string):
//   CMD:prev
//   CMD:play_pause
//   CMD:next
//   CMD:vol_down
//   CMD:vol_up
// ============================================================
class Protocol {
public:
    explicit Protocol(UI &ui);

    // Call every loop()
    // Reads available Serial bytes, parses complete lines, updates UI state.
    // Returns true when a new message was processed.
    bool tick();

    // Send a button-press command to the PC host
    void sendCommand(int8_t btnIdx);

private:
    UI      &_ui;
    char     _buf[MAX_SERIAL_BUF];
    uint16_t _len = 0;

    void _parseLine(const char *line, uint16_t len);
};
