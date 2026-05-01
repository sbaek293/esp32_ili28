"""media_control.py – Cross-platform media playback control.

Supported platforms:
  Linux  – playerctl (media info + control), pactl (volume)
  macOS  – AppleScript via osascript (Music / Spotify)
  Windows – pynput media keys + PowerShell SMTC (media info)
"""
import platform
import subprocess
import re
import logging
from typing import Optional

logger = logging.getLogger(__name__)

_PLATFORM = platform.system()   # "Linux" | "Darwin" | "Windows"


# ============================================================
# Internal helpers
# ============================================================
def _run(cmd: list[str], timeout: int = 3) -> Optional[str]:
    """Run a subprocess; return stdout string on success, None on failure."""
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        return r.stdout.strip() if r.returncode == 0 else None
    except (FileNotFoundError, subprocess.TimeoutExpired, OSError):
        return None


def _pynput_key(key_name: str) -> None:
    """Simulate a media key press using pynput (cross-platform fallback)."""
    try:
        from pynput.keyboard import Key, Controller
        kb  = Controller()
        key = getattr(Key, key_name, None)
        if key:
            kb.press(key)
            kb.release(key)
    except Exception as exc:
        logger.debug("pynput key %s failed: %s", key_name, exc)


# ============================================================
# Media control actions
# ============================================================
def play_pause() -> None:
    if _PLATFORM == "Linux":
        if _run(["playerctl", "play-pause"]) is not None:
            return
    elif _PLATFORM == "Darwin":
        _run(["osascript", "-e",
              'tell application "Music" to playpause'])
        return
    _pynput_key("media_play_pause")


def next_track() -> None:
    if _PLATFORM == "Linux":
        if _run(["playerctl", "next"]) is not None:
            return
    elif _PLATFORM == "Darwin":
        _run(["osascript", "-e",
              'tell application "Music" to next track'])
        return
    _pynput_key("media_next")


def prev_track() -> None:
    if _PLATFORM == "Linux":
        if _run(["playerctl", "previous"]) is not None:
            return
    elif _PLATFORM == "Darwin":
        _run(["osascript", "-e",
              'tell application "Music" to previous track'])
        return
    _pynput_key("media_previous")


def vol_up() -> None:
    if _PLATFORM == "Linux":
        _run(["pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%"])
    elif _PLATFORM == "Darwin":
        _run(["osascript", "-e",
              "set volume output volume "
              "(output volume of (get volume settings) + 5)"])
    else:
        _pynput_key("media_volume_up")


def vol_down() -> None:
    if _PLATFORM == "Linux":
        _run(["pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%"])
    elif _PLATFORM == "Darwin":
        _run(["osascript", "-e",
              "set volume output volume "
              "(output volume of (get volume settings) - 5)"])
    else:
        _pynput_key("media_volume_down")


# ============================================================
# Media info query
# ============================================================
def get_media_info() -> Optional[dict]:
    """Return {'title', 'artist', 'playing', 'vol'} or None."""
    if _PLATFORM == "Linux":
        return _info_linux()
    elif _PLATFORM == "Darwin":
        return _info_mac()
    elif _PLATFORM == "Windows":
        return _info_windows()
    return None


# ── Linux (playerctl + pactl) ────────────────────────────────
def _info_linux() -> Optional[dict]:
    status = _run(["playerctl", "status"])
    if status is None:
        return None
    title  = _run(["playerctl", "metadata", "title"])  or "Unknown"
    artist = _run(["playerctl", "metadata", "artist"]) or "Unknown"
    playing = status.lower() == "playing"

    vol = 50
    vol_out = _run(["pactl", "get-sink-volume", "@DEFAULT_SINK@"])
    if vol_out:
        m = re.search(r"(\d+)%", vol_out)
        if m:
            vol = int(m.group(1))
    return {"title": title, "artist": artist, "playing": playing, "vol": vol}


# ── macOS (osascript → Music / Spotify) ─────────────────────
def _info_mac() -> Optional[dict]:
    script = (
        'tell application "Music"\n'
        '  if it is running then\n'
        '    set t to name of current track\n'
        '    set a to artist of current track\n'
        '    set p to (player state is playing)\n'
        '    return t & "|" & a & "|" & p\n'
        '  end if\n'
        'end tell'
    )
    out = _run(["osascript", "-e", script])
    if not out or "|" not in out:
        return None
    parts = out.split("|")
    if len(parts) < 3:
        return None
    vol_script = "output volume of (get volume settings)"
    vol_out = _run(["osascript", "-e", vol_script])
    vol = int(vol_out) if vol_out and vol_out.isdigit() else 50
    return {
        "title":   parts[0].strip(),
        "artist":  parts[1].strip(),
        "playing": parts[2].strip().lower() == "true",
        "vol":     vol,
    }


# ── Windows (PowerShell SMTC) ───────────────────────────────
_PS_SMTC = r"""
$mgr = [Windows.Media.Control.GlobalSystemMediaTransportControlsSessionManager,
        Windows.Media.Control,ContentType=WindowsRuntime]
$s = $mgr::RequestAsync().GetAwaiter().GetResult().GetCurrentSession()
if ($s) {
    $p = $s.TryGetMediaPropertiesAsync().GetAwaiter().GetResult()
    $st = $s.GetPlaybackInfo().PlaybackStatus
    Write-Output ($p.Title + '|' + $p.Artist + '|' + ($st -eq 'Playing'))
}
"""


def _info_windows() -> Optional[dict]:
    out = _run(["powershell", "-NoProfile", "-Command", _PS_SMTC])
    if not out or "|" not in out:
        return None
    parts = out.strip().split("|")
    if len(parts) < 3:
        return None
    return {
        "title":   parts[0].strip(),
        "artist":  parts[1].strip(),
        "playing": parts[2].strip().lower() == "true",
        "vol":     50,   # Volume query via Windows API is complex; left as enhancement
    }
