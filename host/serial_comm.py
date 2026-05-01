"""serial_comm.py – thread-safe wrapper around pyserial for ESP32 communication."""
import serial
import threading
import queue
import time
import logging

logger = logging.getLogger(__name__)


class SerialComm:
    """Sends JSON lines to ESP32 and enqueues received CMD: lines."""

    def __init__(self, port: str, baud: int = 115200, timeout: float = 1.0):
        self._port = port
        self._baud = baud
        self._timeout = timeout
        self._ser: serial.Serial | None = None
        self._lock = threading.Lock()
        self._cmd_queue: queue.Queue[str] = queue.Queue()
        self._reader_thread: threading.Thread | None = None
        self._running = False

    # ------------------------------------------------------------------ #
    def open(self) -> bool:
        """Open the serial port and start the reader thread.  Returns True on success."""
        try:
            self._ser = serial.Serial(
                port=self._port,
                baudrate=self._baud,
                timeout=self._timeout,
            )
            self._running = True
            self._reader_thread = threading.Thread(
                target=self._reader, daemon=True, name="serial-reader"
            )
            self._reader_thread.start()
            logger.info("Opened serial port %s @ %d baud", self._port, self._baud)
            return True
        except serial.SerialException as exc:
            logger.error("Cannot open %s: %s", self._port, exc)
            return False

    def close(self) -> None:
        self._running = False
        if self._ser and self._ser.is_open:
            self._ser.close()

    @property
    def is_open(self) -> bool:
        return bool(self._ser and self._ser.is_open)

    # ------------------------------------------------------------------ #
    def send(self, line: str) -> bool:
        """Send a single newline-terminated line.  Returns False on error."""
        if not self.is_open:
            return False
        try:
            with self._lock:
                self._ser.write((line.rstrip("\n") + "\n").encode())
            return True
        except serial.SerialException as exc:
            logger.warning("Serial write error: %s", exc)
            return False

    def get_command(self) -> str | None:
        """Return the next CMD: line received from ESP32, or None."""
        try:
            return self._cmd_queue.get_nowait()
        except queue.Empty:
            return None

    # ------------------------------------------------------------------ #
    def _reader(self) -> None:
        buf = b""
        while self._running:
            try:
                if self._ser and self._ser.in_waiting:
                    chunk = self._ser.read(self._ser.in_waiting)
                    buf += chunk
                    while b"\n" in buf:
                        line, buf = buf.split(b"\n", 1)
                        decoded = line.strip().decode(errors="replace")
                        if decoded.startswith("CMD:"):
                            self._cmd_queue.put(decoded)
                        elif decoded:
                            logger.debug("ESP32 >> %s", decoded)
                else:
                    time.sleep(0.01)
            except (serial.SerialException, OSError) as exc:
                logger.warning("Serial read error: %s", exc)
                time.sleep(0.5)
