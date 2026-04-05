import asyncio
import threading

from bleak import BleakClient, BleakScanner
from PySide6.QtCore import QObject, Signal

NUS_SERVICE_UUID = "6e400001-b5b3-f393-e0a9-e50e24dcca9e"
NUS_TX_CHAR_UUID = "6e400003-b5b3-f393-e0a9-e50e24dcca9e"  # ESP32 → client (notify)
NUS_RX_CHAR_UUID = "6e400002-b5b3-f393-e0a9-e50e24dcca9e"  # client → ESP32 (write)

CHUNK_SIZE = 200


class BleClient(QObject):
    device_found = Signal(str, str)       # name, address
    scan_finished = Signal(int)           # total devices found
    connected = Signal(str)              # device name/address
    disconnected = Signal()
    message_received = Signal(str, str)  # topic, payload
    error = Signal(str)

    def __init__(self):
        super().__init__()
        self._client: BleakClient | None = None
        self._rx_char = None
        self._buffer = ""
        self._loop = asyncio.new_event_loop()
        self._thread = threading.Thread(target=self._run_loop, daemon=True)
        self._thread.start()

    # ------------------------------------------------------------------ loop --

    def _run_loop(self):
        asyncio.set_event_loop(self._loop)
        self._loop.run_forever()

    def _submit(self, coro):
        return asyncio.run_coroutine_threadsafe(coro, self._loop)

    # ----------------------------------------------------------------- public --

    def scan(self, timeout: float = 5.0):
        self._submit(self._scan(timeout))

    def connect_device(self, address: str):
        self._submit(self._connect(address))

    def disconnect_device(self):
        self._submit(self._disconnect())

    def send(self, text: str):
        self._submit(self._send(text))

    def send_command(self, short_topic: str, payload: str = ""):
        """Send a command in 'short_topic: payload\\n' format understood by BtHandler."""
        self.send(f"{short_topic}: {payload}\n")

    # --------------------------------------------------------------- privates --

    async def _scan(self, timeout: float):
        try:
            devices = await BleakScanner.discover(timeout=timeout)
            for d in devices:
                self.device_found.emit(d.name or "Unknown", d.address)
            self.scan_finished.emit(len(devices))
        except Exception as exc:
            msg = str(exc)
            if "POWERED_OFF" in msg or "No powered Bluetooth" in msg:
                self.error.emit(
                    "Bluetooth is powered off. "
                    "Run: sudo systemctl start bluetooth && bluetoothctl power on"
                )
            else:
                self.error.emit(f"Scan error: {exc}")
            self.scan_finished.emit(0)

    def _notification_handler(self, _sender, data: bytearray):
        self._buffer += data.decode("utf-8", errors="replace")
        while "\n" in self._buffer:
            line, self._buffer = self._buffer.split("\n", 1)
            line = line.strip()
            if not line:
                continue
            if ": " in line:
                topic, payload = line.split(": ", 1)
                self.message_received.emit(topic.strip(), payload.strip())
            else:
                self.message_received.emit("raw", line)

    def _on_ble_disconnect(self, _client: BleakClient):
        self._client = None
        self._rx_char = None
        self.disconnected.emit()

    async def _connect(self, address: str):
        try:
            self._client = BleakClient(address, disconnected_callback=self._on_ble_disconnect)
            await self._client.connect()

            # Locate the RX characteristic for later writes
            for service in self._client.services:
                for char in service.characteristics:
                    if char.uuid.lower() == NUS_RX_CHAR_UUID:
                        self._rx_char = char

            await self._client.start_notify(NUS_TX_CHAR_UUID, self._notification_handler)
            self.connected.emit(address)
        except Exception as exc:
            self._client = None
            self.error.emit(f"Connection failed: {exc}")

    async def _disconnect(self):
        if self._client and self._client.is_connected:
            await self._client.disconnect()
        # disconnected signal fires via _on_ble_disconnect callback

    async def _send(self, text: str):
        if not (self._client and self._client.is_connected and self._rx_char):
            self.error.emit("Cannot send: not connected or RX characteristic unavailable")
            return
        data = text.encode("utf-8")
        for i in range(0, len(data), CHUNK_SIZE):
            await self._client.write_gatt_char(self._rx_char, data[i : i + CHUNK_SIZE])
