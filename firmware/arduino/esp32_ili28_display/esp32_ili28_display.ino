/*
 * ESP32 ILI9341 Dongle Display
 * Arduino IDE main sketch – rename of main.cpp as .ino
 *
 * Board support:
 *   ESP32-C3 → Tools > Board > ESP32C3 Dev Module
 *   ESP32-S3 → Tools > Board > ESP32S3 Dev Module
 *
 * Required libraries (install via Library Manager):
 *   - TFT_eSPI  by Bodmer  (>= 2.5.34)
 *   - ArduinoJson           (>= 7.1.0)
 *
 * Before compiling: copy the matching User_Setup_C3.h or User_Setup_S3.h
 * from the firmware/arduino/ folder into your TFT_eSPI library folder,
 * overwriting User_Setup.h.
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "ui.h"
#include "protocol.h"

TFT_eSPI tft;
UI       ui(tft);
Protocol proto(ui);

void setup() {
    Serial.begin(SERIAL_BAUD);
    ui.begin();   // init display, draws splash, waits for PC
}

void loop() {
    proto.tick();
    ui.tick();

    int8_t btn = ui.pollTouch();
    if (btn >= 0) {
        proto.sendCommand(btn);
    }
}
