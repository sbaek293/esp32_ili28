// ============================================================
// ESP32 ILI9341 Dongle Display – TFT_eSPI User Setup
// Board: ESP32-C3 DevKitM-1
//
// How to use:
//   1. Copy this file to your Arduino libraries folder:
//      <Arduino>/libraries/TFT_eSPI/User_Setup.h
//      (overwrite the existing User_Setup.h)
//   2. Select board: Tools > Board > ESP32C3 Dev Module
// ============================================================

// Driver
#define ILI9341_DRIVER

// ── SPI Pins ─────────────────────────────────────────────────
//   ILI9341 display SPI
#define TFT_MOSI  7    // SDI / DIN
#define TFT_MISO  2    // SDO / DOUT
#define TFT_SCLK  6    // SCK / CLK
#define TFT_CS   10    // Chip select (display)
#define TFT_DC    4    // Data / Command (RS)
#define TFT_RST   8    // Reset (connect to RST pin; -1 = tied to 3.3V)
#define TFT_BL    3    // Backlight (HIGH = on)

//   XPT2046 touch SPI (shares MOSI/MISO/SCK with display)
#define TOUCH_CS  5    // Touch chip select
// TOUCH_IRQ = GPIO 9  (read in firmware, not required here)

// ── Fonts ────────────────────────────────────────────────────
#define LOAD_GLCD   // 8px GLCD font
#define LOAD_FONT2  // 16px
#define LOAD_FONT4  // 26px
#define LOAD_FONT6  // 48px number font
#define LOAD_FONT7  // 7-segment 48px number font  ← used for HH:MM
#define LOAD_FONT8  // 75px number font
#define LOAD_GFXFF  // Adafruit GFX free fonts
#define SMOOTH_FONT // Anti-aliased font support

// ── SPI Speeds ───────────────────────────────────────────────
#define SPI_FREQUENCY        27000000   // 27 MHz – safe default for ILI9341 (40 MHz can be unstable)
#define SPI_READ_FREQUENCY   16000000   // 16 MHz display read
#define SPI_TOUCH_FREQUENCY   2500000   //  2.5 MHz touch (XPT2046 max)
