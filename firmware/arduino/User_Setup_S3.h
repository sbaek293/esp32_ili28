// ============================================================
// ESP32 ILI9341 Dongle Display – TFT_eSPI User Setup
// Board: ESP32-S3 DevKitC-1
//
// How to use:
//   1. Copy this file to your Arduino libraries folder:
//      <Arduino>/libraries/TFT_eSPI/User_Setup.h
//      (overwrite the existing User_Setup.h)
//   2. Select board: Tools > Board > ESP32S3 Dev Module
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
#define TOUCH_CS 46    // Touch chip select
// TOUCH_IRQ = GPIO 45  (read in firmware, not required here)

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
#define SPI_FREQUENCY        40000000
#define SPI_READ_FREQUENCY   20000000
#define SPI_TOUCH_FREQUENCY   2500000
