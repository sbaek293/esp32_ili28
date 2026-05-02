"""main.py – ESP32 ILI9341 Dongle Display host application.

Usage:
    python main.py [--config config.json] [--port COM3]

The script:
  • Sends the current time to the ESP32 every second.
  • Fetches weather data every N minutes and forwards it.
  • Queries local media player state every 2 seconds.
  • Listens for CMD: touch events from the ESP32 and acts on them.
"""

import argparse
import json
import logging
import os
import platform
import sys
import time
from datetime import datetime
from pathlib import Path

import serial.tools.list_ports

import media_control as mc
from serial_comm import SerialComm
from weather import WeatherFetcher

# Must stay in sync with firmware/src/config.h buffer sizes (size - 1)
MAX_TITLE_LEN  = 63
MAX_ARTIST_LEN = 47

# ============================================================
# Logging
# ============================================================
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s  %(levelname)-7s  %(message)s",
    datefmt="%H:%M:%S",
)
logger = logging.getLogger(__name__)

# ============================================================
# Config helpers
# ============================================================
DEFAULT_CONFIG = {
    "port": "",
    "baud": 115200,
    "weather": {
        "api_key": "YOUR_OPENWEATHERMAP_API_KEY",
        "city":    "Seoul",
        "units":   "metric",
        "update_interval_sec": 300,
    },
    "media": {
        "update_interval_sec": 2,
    },
}


def _load_config(path: str) -> dict:
    cfg = dict(DEFAULT_CONFIG)
    if os.path.exists(path):
        with open(path, encoding="utf-8") as f:
            user = json.load(f)
        # Merge top-level keys
        for k, v in user.items():
            if k.startswith("_"):
                continue
            if isinstance(v, dict) and isinstance(cfg.get(k), dict):
                cfg[k] = {**cfg[k], **v}
            else:
                cfg[k] = v
    return cfg


def _auto_detect_port() -> str:
    """Return the first USB-serial port that looks like an ESP32."""
    keywords = ("CP210", "CH340", "FTDI", "USB Serial", "USB-SERIAL", "USB CDC")
    for p in serial.tools.list_ports.comports():
        desc = (p.description or "") + (p.manufacturer or "")
        if any(k.lower() in desc.lower() for k in keywords):
            logger.info("Auto-detected port: %s (%s)", p.device, p.description)
            return p.device
    # Fall back to first available port
    ports = serial.tools.list_ports.comports()
    if ports:
        logger.warning("Could not auto-detect ESP32 port; using %s", ports[0].device)
        return ports[0].device
    return ""


# ============================================================
# Time message builder
# ============================================================
def _build_time_msg() -> str:
    now = datetime.now()
    return json.dumps({
        "t":    "T",
        "hh":   now.strftime("%H"),
        "mm":   now.strftime("%M"),
        "ss":   now.strftime("%S"),
        "date": now.strftime("%a %b %d"),
        "year": now.strftime("%Y"),
    }, ensure_ascii=False)


# ============================================================
# CMD: dispatch
# ============================================================
def _dispatch(cmd: str) -> None:
    logger.info("Touch command: %s", cmd)
    action = cmd.removeprefix("CMD:").strip()
    dispatch_map = {
        "play_pause": mc.play_pause,
        "next":       mc.next_track,
        "prev":       mc.prev_track,
        "vol_up":     mc.vol_up,
        "vol_down":   mc.vol_down,
    }
    fn = dispatch_map.get(action)
    if fn:
        fn()
    else:
        logger.warning("Unknown command: %s", action)


# ============================================================
# Main loop
# ============================================================
def run(cfg: dict) -> None:
    port = cfg["port"] or _auto_detect_port()
    if not port:
        logger.error("No serial port found.  Set 'port' in config.json or connect the device.")
        sys.exit(1)

    comm = SerialComm(port, cfg["baud"])
    if not comm.open():
        sys.exit(1)

    wx_cfg = cfg["weather"]
    fetcher = WeatherFetcher(wx_cfg["api_key"], wx_cfg["city"], wx_cfg["units"])

    wx_interval  = wx_cfg["update_interval_sec"]
    med_interval = cfg["media"]["update_interval_sec"]

    last_wx_time  = 0.0
    last_med_time = 0.0

    logger.info("Host running.  Press Ctrl-C to quit.")

    while True:
        now = time.monotonic()

        # ── 1. Send time every second ──────────────────────────
        comm.send(_build_time_msg())

        # ── 2. Send weather periodically ──────────────────────
        if now - last_wx_time >= wx_interval:
            data = fetcher.fetch()
            if data:
                comm.send(json.dumps(data, ensure_ascii=False))
                logger.debug("Weather sent: %s", data)
            last_wx_time = now

        # ── 3. Send media info periodically ───────────────────
        if now - last_med_time >= med_interval:
            info = mc.get_media_info()
            if info:
                msg = {
                    "t":       "M",
                    "title":   info["title"][:MAX_TITLE_LEN],
                    "artist":  info["artist"][:MAX_ARTIST_LEN],
                    "playing": int(info["playing"]),
                    "vol":     info["vol"],
                }
                comm.send(json.dumps(msg, ensure_ascii=False))
            last_med_time = now

        # ── 4. Process touch commands from ESP32 ───────────────
        while True:
            cmd = comm.get_command()
            if cmd is None:
                break
            _dispatch(cmd)

        time.sleep(1)


# ============================================================
# Entry point
# ============================================================
def _check_platform() -> None:
    """Exit early on unsupported platforms (Windows, macOS, and Linux are supported)."""
    supported = ("Windows", "Darwin", "Linux")
    current = platform.system()
    if current not in supported:
        print(
            f"[ERROR] 이 호스트 앱은 Windows, macOS, Linux에서만 지원됩니다. "
            f"현재 플랫폼: {current}",
            file=sys.stderr,
        )
        sys.exit(1)


def main() -> None:
    _check_platform()
    parser = argparse.ArgumentParser(description="ESP32 ILI9341 Dongle Display host")
    parser.add_argument("--config", default="config.json",
                        help="Path to config JSON (default: config.json)")
    parser.add_argument("--port",   default="",
                        help="Serial port override (e.g. COM3 or /dev/ttyUSB0)")
    args = parser.parse_args()

    cfg = _load_config(args.config)
    if args.port:
        cfg["port"] = args.port

    try:
        run(cfg)
    except KeyboardInterrupt:
        logger.info("Shutting down.")


if __name__ == "__main__":
    main()
