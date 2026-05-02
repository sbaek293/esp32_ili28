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

    ui.begin();   // init display, draws splash, backlight on
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
