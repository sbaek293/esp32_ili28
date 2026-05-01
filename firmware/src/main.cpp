#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "ui.h"
#include "protocol.h"

// ============================================================
// Global objects
// ============================================================
TFT_eSPI tft;
UI       ui(tft);
Protocol proto(ui);

// ============================================================
// setup
// ============================================================
void setup() {
    Serial.begin(SERIAL_BAUD);

    ui.begin();   // init display, set calibration, backlight on

    // Show splash until PC sends first message
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(4);
    tft.drawString("ESP32 Dongle", SCREEN_W / 2, SCREEN_H / 2 - 20);
    tft.setTextFont(2);
    tft.drawString("Waiting for PC...", SCREEN_W / 2, SCREEN_H / 2 + 16);
}

// ============================================================
// loop
// ============================================================
void loop() {
    // 1. Process incoming serial data, update UI state
    proto.tick();

    // 2. Redraw changed screen regions, animate scroll
    ui.tick();

    // 3. Check for touch and send commands
    int8_t btn = ui.pollTouch();
    if (btn >= 0) {
        proto.sendCommand(btn);
    }
}
