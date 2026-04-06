import asyncio
import threading

from bleak import BleakClient, BleakScanner
from PySide6.QtCore import QObject, Signal

NUS_SERVICE_UUID = "6e400001-b5b3-f393-e0a9-e50e24dcca9e"
NUS_TX_CHAR_UUID = "6e400003-b5b3-f393-e0a9-e50e24dcca9e"  # ESP32 → client (notify)
NUS_RX_CHAR_UUID = "6e400002-b5b3-f393-e0a9-e50e24dcca9e"  # client → ESP32 (write)

OTA_SERVICE_UUID     = "4c5f0001-e8c8-4b0d-aeb9-1c8b6d9a5e2f"
OTA_CTRL_CHAR_UUID   = "4c5f0002-e8c8-4b0d-aeb9-1c8b6d9a5e2f"  # client → ESP32 (write)
OTA_DATA_CHAR_UUID   = "4c5f0003-e8c8-4b0d-aeb9-1c8b6d9a5e2f"  # client → ESP32 (write, binary)
OTA_STATUS_CHAR_UUID = "4c5f0004-e8c8-4b0d-aeb9-1c8b6d9a5e2f"  # ESP32 → client (notify)

CHUNK_SIZE = 200


class BleClient(QObject):
    device_found = Signal(str, str)       # name, address
    scan_finished = Signal(int)           # total devices found
    connected = Signal(str)              # device name/address
    disconnected = Signal()
    message_received = Signal(str, str)  # topic, payload
    error = Signal(str)
    ota_progress = Signal(int, int)       # bytes_written, total
    ota_status = Signal(str)              # "READY" | "PROGRESS:<pct>" | "SUCCESS" | "ERROR:<msg>" | "ABORTED"

    def __init__(self):
        super().__init__()
        self._client: BleakClient | None = None
        self._rx_char = None
        self._ota_ctrl_char = None
        self._ota_data_char = None
        self._ota_status_char = None
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

    def send_ota(self, filepath: str, chunk_size: int = 512):
        """Flash firmware over BLE OTA. Emits ota_progress and ota_status signals."""
        self._submit(self._send_ota(filepath, chunk_size))

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

            # Request larger MTU for faster OTA transfers
            try:
                await self._client.exchange_mtu(517)
            except Exception:
                pass  # Not all platforms support explicit MTU exchange; negotiation still happens

            # Locate NUS and OTA characteristics
            self._rx_char = None
            self._ota_ctrl_char = None
            self._ota_data_char = None
            self._ota_status_char = None
            for service in self._client.services:
                for char in service.characteristics:
                    uuid = char.uuid.lower()
                    if uuid == NUS_RX_CHAR_UUID:
                        self._rx_char = char
                    elif uuid == OTA_CTRL_CHAR_UUID:
                        self._ota_ctrl_char = char
                    elif uuid == OTA_DATA_CHAR_UUID:
                        self._ota_data_char = char
                    elif uuid == OTA_STATUS_CHAR_UUID:
                        self._ota_status_char = char

            await self._client.start_notify(NUS_TX_CHAR_UUID, self._notification_handler)
            self.connected.emit(address)
        except Exception as exc:
            self._client = None
            self.error.emit(f"Connection failed: {exc}")

    async def _disconnect(self):
        if self._client and self._client.is_connected:
            await self._client.disconnect()
        # disconnected signal fires via _on_ble_disconnect callback

    async def _send_ota(self, filepath: str, chunk_size: int):
        if not (self._client and self._client.is_connected):
            self.ota_status.emit("ERROR:Not connected")
            return
        if not (self._ota_ctrl_char and self._ota_data_char and self._ota_status_char):
            self.ota_status.emit("ERROR:OTA characteristics not found — is the firmware up to date?")
            return

        try:
            with open(filepath, "rb") as f:
                firmware = f.read()
        except OSError as exc:
            self.ota_status.emit(f"ERROR:Cannot read file: {exc}")
            return

        total = len(firmware)
        ready_event = asyncio.Event()
        complete_event = asyncio.Event()
        last_status: list[str] = []

        def _ota_notify(_sender, data: bytearray):
            msg = data.decode("utf-8", errors="replace").strip()
            self.ota_status.emit(msg)
            last_status.clear()
            last_status.append(msg)
            if msg == "READY":
                ready_event.set()
            elif msg.startswith("PROGRESS:"):
                try:
                    pct = int(msg.split(":")[1])
                    self.ota_progress.emit(int(pct * total / 100), total)
                except ValueError:
                    pass
            elif msg in ("SUCCESS", "ABORTED") or msg.startswith("ERROR:"):
                ready_event.set()   # unblock READY wait if error arrives first
                complete_event.set()

        await self._client.start_notify(self._ota_status_char, _ota_notify)
        await asyncio.sleep(0.1)  # Allow CCCD subscription to settle
        try:
            # 1. Begin
            await self._client.write_gatt_char(
                self._ota_ctrl_char,
                f"BEGIN:{total}".encode(),
                response=True,
            )

            # 2. Wait for READY (or an early ERROR from the device)
            await asyncio.wait_for(ready_event.wait(), timeout=15.0)

            # Stop if device reported an error instead of READY
            if last_status and last_status[0] != "READY":
                return

            # 3. Send firmware in chunks
            for offset in range(0, total, chunk_size):
                chunk = firmware[offset:offset + chunk_size]
                await self._client.write_gatt_char(self._ota_data_char, chunk, response=True)
                self.ota_progress.emit(min(offset + chunk_size, total), total)

            # 4. Wait for SUCCESS / ERROR — Update.end() does SHA256 which takes a few seconds
            await asyncio.wait_for(complete_event.wait(), timeout=60.0)

        except asyncio.TimeoutError:
            self.ota_status.emit("ERROR:Timeout — no response from device")
        except Exception as exc:
            self.ota_status.emit(f"ERROR:{exc}")
        finally:
            try:
                await self._client.stop_notify(self._ota_status_char)
            except Exception:
                pass

    async def _send(self, text: str):
        if not (self._client and self._client.is_connected and self._rx_char):
            self.error.emit("Cannot send: not connected or RX characteristic unavailable")
            return
        data = text.encode("utf-8")
        for i in range(0, len(data), CHUNK_SIZE):
            await self._client.write_gatt_char(self._rx_char, data[i : i + CHUNK_SIZE])
