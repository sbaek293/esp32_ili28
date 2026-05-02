// ============================================================
// ESP32 ILI9341 Dongle Display – TFT_eSPI User Setup
// Board: ESP32-S3 Supermini
//
// How to use:
//   1. Copy this file to your Arduino libraries folder:
//      <Arduino>/libraries/TFT_eSPI/User_Setup.h
//      (overwrite the existing User_Setup.h)
//   2. Select board: Tools > Board > ESP32S3 Dev Module
//
// Note: ESP32-S3 Supermini does NOT expose GPIO 45/46.
//       T_CS = GPIO 15, T_IRQ = GPIO 16 are used instead.
// ============================================================

// Driver
#define ILI9341_DRIVER

// ── SPI Pins ─────────────────────────────────────────────────
//   ILI9341 display SPI
#define TFT_MOSI 11    // SDI / DIN
#define TFT_MISO 13    // SDO / DOUT
#define TFT_SCLK 12    // SCK / CLK
#define TFT_CS   10    // Chip select (display)
#define TFT_DC    9    // Data / Command (RS)
#define TFT_RST  14    // Reset
#define TFT_BL   21    // Backlight (HIGH = on)

//   XPT2046 touch SPI (shares MOSI/MISO/SCK with display)
//   GPIO 45/46 are not exposed on S3 Supermini – use 15/16
#define TOUCH_CS 15    // Touch chip select
// TOUCH_IRQ = GPIO 16  (read in firmware, not required here)

// ── Fonts ────────────────────────────────────────────────────
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// ── SPI Speeds ───────────────────────────────────────────────
#define SPI_FREQUENCY        27000000   // 27 MHz – safe default for ILI9341
#define SPI_READ_FREQUENCY   16000000   // 16 MHz display read
#define SPI_TOUCH_FREQUENCY   2500000   //  2.5 MHz touch (XPT2046 max)
