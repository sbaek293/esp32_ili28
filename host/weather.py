"""weather.py – Fetch current weather from OpenWeatherMap free tier."""
import requests
import logging
from typing import Optional

logger = logging.getLogger(__name__)

OWM_URL = "https://api.openweathermap.org/data/2.5/weather"

# Must stay in sync with firmware/src/config.h buffer sizes
MAX_CITY_LEN = 31
MAX_DESC_LEN = 31


class WeatherFetcher:
    def __init__(self, api_key: str, city: str, units: str = "metric"):
        self._api_key = api_key
        self._city    = city
        self._units   = units          # "metric" (°C) | "imperial" (°F) | "standard" (K)

    def fetch(self) -> Optional[dict]:
        """Returns a dict ready to serialise into the W: JSON message, or None on error."""
        if not self._api_key or self._api_key == "YOUR_OPENWEATHERMAP_API_KEY":
            logger.warning("Weather API key not set – skipping fetch.")
            return None
        try:
            resp = requests.get(
                OWM_URL,
                params={
                    "q":     self._city,
                    "appid": self._api_key,
                    "units": self._units,
                },
                timeout=10,
            )
            resp.raise_for_status()
            data = resp.json()

            temp   = round(data["main"]["temp"])
            feels  = round(data["main"]["feels_like"])
            hum    = data["main"]["humidity"]
            desc   = data["weather"][0]["description"].title()
            city   = data.get("name", self._city)

            return {
                "t":     "W",
                "city":  city[:MAX_CITY_LEN],
                "temp":  str(temp),
                "feels": str(feels),
                "desc":  desc[:MAX_DESC_LEN],
                "hum":   f"{hum}%",
            }
        except requests.RequestException as exc:
            logger.warning("Weather fetch failed: %s", exc)
            return None
