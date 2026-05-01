#pragma once
#include <cstdint>

// ============================================================
// Serial protocol
// ============================================================
#define SERIAL_BAUD         115200
#define MAX_SERIAL_BUF      512     // max bytes in one JSON line

// ============================================================
// Display geometry (portrait, USB connector at bottom)
// ============================================================
#define SCREEN_W            240
#define SCREEN_H            320
#define SCREEN_ROTATION     2       // 0=normal, 1=90CW, 2=180, 3=270CW

// ============================================================
// Touch calibration
// Run the TFT_eSPI Touch_calibrate example and paste here.
// Format: { xmin, xmax, ymin, ymax, swap_xy }
// Defaults below work for many common 2.8" ILI9341+XPT2046 modules.
// ============================================================
#define TOUCH_CAL_DATA      { 275, 3620, 264, 3706, 1 }

// Minimum pixels of movement to consider a touch "settled"
#define TOUCH_DEADZONE      8
// Minimum ms between two consecutive button activations
#define TOUCH_DEBOUNCE_MS   300

// ============================================================
// Color palette (RGB565)
// ============================================================
#define C_BG                0x0000u   // Black
#define C_TIME_HM           0x07FFu   // Cyan   – HH:MM
#define C_TIME_S            0x04B6u   // Dim cyan – :SS
#define C_DATE              0xC618u   // Light grey
#define C_WEATHER_TEMP      0xFD20u   // Orange
#define C_WEATHER_TEXT      0xFFFFu   // White
#define C_WEATHER_LABEL     0x8410u   // Dark grey (label text)
#define C_MEDIA_TITLE       0xFFFFu   // White
#define C_MEDIA_ARTIST      0xBDF7u   // Light grey
#define C_DIVIDER           0x4208u   // Medium grey
#define C_BTN_BG            0x2124u   // Dark navy
#define C_BTN_PRESS         0x6B4Du   // Highlighted button
#define C_BTN_TEXT          0xFFFFu   // White
#define C_PLAYING           0x07E0u   // Green  – playing indicator
#define C_PAUSED            0xFD20u   // Orange – paused indicator
#define C_CONN_OK           0x07E0u   // Green  – PC connected dot
#define C_CONN_NO           0xF800u   // Red    – disconnected dot
#define C_VOL_BAR           0x07FFu   // Cyan   – volume fill
#define C_VOL_BG            0x2945u   // Dark   – volume background

// ============================================================
// Layout constants (portrait 240×320)
// ============================================================

// ── Status bar (top) ────────────────────────────────────────
#define SB_Y                0
#define SB_H                22

// ── Clock section ────────────────────────────────────────────
#define CLK_Y               (SB_Y + SB_H + 2)   // 24
#define CLK_H               90
#define CLK_HHMM_Y          (CLK_Y + 4)
#define CLK_SS_Y            (CLK_Y + 52)
#define CLK_DATE_Y          (CLK_Y + 70)

// ── Weather section ──────────────────────────────────────────
#define WX_Y                (CLK_Y + CLK_H + 2)  // 116
#define WX_H                90
#define WX_CITY_Y           (WX_Y + 4)
#define WX_TEMP_Y           (WX_Y + 26)
#define WX_DESC_Y           (WX_Y + 60)
#define WX_HUM_Y            (WX_Y + 74)

// ── Media section ────────────────────────────────────────────
#define MD_Y                (WX_Y + WX_H + 2)    // 208
#define MD_H                (SCREEN_H - MD_Y)     // 112

#define MD_TITLE_Y          (MD_Y + 4)
#define MD_ARTIST_Y         (MD_Y + 22)
#define MD_VOL_BAR_Y        (MD_Y + 44)
#define MD_VOL_BAR_H        10
#define MD_VOL_BAR_X        8
#define MD_VOL_BAR_W        (SCREEN_W - 16)

// ── Media buttons ────────────────────────────────────────────
#define BTN_Y               (MD_Y + 60)
#define BTN_H               46
#define BTN_COUNT           5
#define BTN_MARGIN          4
#define BTN_W               ((SCREEN_W - BTN_MARGIN * (BTN_COUNT + 1)) / BTN_COUNT)

// Button indices
#define BTN_PREV            0
#define BTN_VOL_DOWN        1
#define BTN_PLAY_PAUSE      2
#define BTN_VOL_UP          3
#define BTN_NEXT            4

// ============================================================
// Scroll configuration (media title)
// ============================================================
#define SCROLL_SPEED_MS     40    // ms per pixel shift
#define SCROLL_PAUSE_MS     2000  // pause at start/end before scrolling
