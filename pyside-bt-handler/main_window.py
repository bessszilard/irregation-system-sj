import json
from datetime import datetime

from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QColor, QFont, QTextCharFormat, QTextCursor
from PySide6.QtWidgets import (
    QCheckBox,
    QComboBox,
    QFrame,
    QGridLayout,
    QGroupBox,
    QHBoxLayout,
    QHeaderView,
    QLabel,
    QLineEdit,
    QMainWindow,
    QPushButton,
    QScrollArea,
    QSizePolicy,
    QSplitter,
    QStatusBar,
    QTabWidget,
    QTableWidget,
    QTableWidgetItem,
    QTextEdit,
    QVBoxLayout,
    QWidget,
)

from ble_client import BleClient

_MONO = QFont("Monospace", 10)


# ────────────────────────────────────────────────────── helpers

def _ts() -> str:
    return datetime.now().strftime("%H:%M:%S")


def _fmt_json(text: str) -> str:
    try:
        return json.dumps(json.loads(text), indent=2)
    except Exception:
        return text


# ──────────────────────────────────────────────── connection bar

class ConnectionBar(QGroupBox):
    def __init__(self, parent=None):
        super().__init__("Bluetooth Connection", parent)
        layout = QHBoxLayout(self)

        self.device_combo = QComboBox()
        self.device_combo.setMinimumWidth(340)
        self.device_combo.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        self.device_combo.setPlaceholderText("— scan to find devices —")

        self.filter_check = QCheckBox("IrrigatorBT only")
        self.filter_check.setChecked(True)

        self.scan_btn = QPushButton("⟳  Scan")
        self.scan_btn.setFixedWidth(90)

        self.connect_btn = QPushButton("Connect")
        self.connect_btn.setFixedWidth(110)
        self.connect_btn.setEnabled(False)

        self.status_dot = QLabel("●")
        self.status_dot.setFixedWidth(24)
        self.status_dot.setAlignment(Qt.AlignCenter)
        self.status_dot.setStyleSheet("color: #9e9e9e; font-size: 20px;")

        layout.addWidget(QLabel("Device:"))
        layout.addWidget(self.device_combo, 1)
        layout.addWidget(self.filter_check)
        layout.addWidget(self.scan_btn)
        layout.addWidget(self.connect_btn)
        layout.addWidget(self.status_dot)


# ─────────────────────────────────────────────────────── log panel

_TOPIC_COLORS = {
    "sensors":           "#4fc3f7",
    "relays":            "#ffb74d",
    "localTime":         "#a5d6a7",
    "cmd/list":          "#ce93d8",
    "cmd/response":      "#f48fb1",
    "cmd/command-opt":   "#bcaaa4",
    "relay-groups/info": "#80deea",
    "system":            "#80cbc4",
    "sent":              "#fff176",
    "error":             "#ef9a9a",
    "raw":               "#bdbdbd",
}
_DEFAULT_MSG_COLOR = "#eeeeee"


class LogPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        self.text_edit.setFont(_MONO)
        self.text_edit.setStyleSheet(
            "QTextEdit { background-color: #1e1e1e; color: #eeeeee; border: none; }"
        )

        btn_row = QHBoxLayout()
        clear_btn = QPushButton("Clear")
        clear_btn.setFixedWidth(60)
        clear_btn.clicked.connect(self.text_edit.clear)
        btn_row.addStretch()
        btn_row.addWidget(clear_btn)

        layout.addWidget(self.text_edit)
        layout.addLayout(btn_row)

    def append(self, topic: str, payload: str):
        color = _TOPIC_COLORS.get(
            next((k for k in _TOPIC_COLORS if k in topic), ""), _DEFAULT_MSG_COLOR
        )
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)

        def _write(text, c):
            fmt = QTextCharFormat()
            fmt.setForeground(QColor(c))
            cursor.setCharFormat(fmt)
            cursor.insertText(text)

        _write(f"{_ts()} ", "#666666")
        _write(topic, color)
        _write(f"  {payload}\n", "#cccccc")

        self.text_edit.setTextCursor(cursor)
        self.text_edit.ensureCursorVisible()


# ──────────────────────────────────────────────────── sensor panel

_SENSOR_LABELS = {
    "tempOnSun_C":            ("Temp (sun)",      "°C"),
    "tempInShadow_C":         ("Temp (shadow)",   "°C"),
    "humidity_%RH":           ("Humidity",        "%RH"),
    "flowRate_LitMin":        ("Flow rate",       "L/min"),
    "flowDaySum_Min":         ("Flow today",      "L"),
    "rainSensor_0-99":        ("Rain sensor",     ""),
    "light_0-99":             ("Light",           ""),
    "waterPressure_bar":      ("Water pressure",  "bar"),
    "soilMoistureLocal_0-99": ("Soil moisture",   ""),
}


class SensorPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)

        self.table = QTableWidget(0, 3)
        self.table.setHorizontalHeaderLabels(["Sensor", "Value", "Unit"])
        hdr = self.table.horizontalHeader()
        hdr.setStretchLastSection(False)
        hdr.setSectionResizeMode(0, QHeaderView.ResizeMode.Stretch)
        self.table.setColumnWidth(1, 100)
        self.table.setColumnWidth(2, 80)
        self.table.setEditTriggers(QTableWidget.NoEditTriggers)
        self.table.setAlternatingRowColors(True)
        self.table.setSelectionMode(QTableWidget.NoSelection)
        self.table.verticalHeader().setVisible(False)

        self._rows: dict[str, int] = {}
        self._last = QLabel("—")
        self._last.setAlignment(Qt.AlignRight)
        self._last.setStyleSheet("color: gray; font-size: 10px;")

        layout.addWidget(self.table)
        layout.addWidget(self._last)

    def update_data(self, payload: str):
        try:
            data: dict = json.loads(payload)
        except json.JSONDecodeError:
            return
        for key, raw in data.items():
            if key == "valid":
                continue
            label, unit = _SENSOR_LABELS.get(key, (key, ""))
            display = "—" if raw is None else (f"{raw:.2f}" if isinstance(raw, float) else str(raw))
            if key not in self._rows:
                row = self.table.rowCount()
                self.table.insertRow(row)
                self.table.setItem(row, 0, QTableWidgetItem(label))
                self.table.setItem(row, 1, QTableWidgetItem(display))
                self.table.setItem(row, 2, QTableWidgetItem(unit))
                self._rows[key] = row
            else:
                self.table.item(self._rows[key], 1).setText(display)
        self._last.setText(f"Last update: {_ts()}")


# ──────────────────────────────────────────────────── relay panel

_RELAY_STYLES = {
    "On":      "background:#4caf50; color:white;",
    "Off":     "background:#f44336; color:white;",
    "Unknown": "background:#9e9e9e; color:white;",
}
_COLS = 4


class RelayCard(QFrame):
    def __init__(self, relay_id: str, parent=None):
        super().__init__(parent)
        self.setFrameShape(QFrame.StyledPanel)
        self.setFixedSize(130, 90)
        layout = QVBoxLayout(self)
        layout.setContentsMargins(6, 4, 6, 4)
        layout.setSpacing(2)

        self._id_lbl = QLabel(relay_id)
        self._id_lbl.setAlignment(Qt.AlignCenter)
        self._id_lbl.setFont(QFont("", 10, QFont.Bold))

        self._state_lbl = QLabel("—")
        self._state_lbl.setAlignment(Qt.AlignCenter)
        self._state_lbl.setStyleSheet("border-radius:4px; padding:3px; background:#607d8b; color:white;")

        self._cmd_lbl = QLabel("")
        self._cmd_lbl.setAlignment(Qt.AlignCenter)
        self._cmd_lbl.setStyleSheet("color:gray; font-size:9px;")

        layout.addWidget(self._id_lbl)
        layout.addWidget(self._state_lbl)
        layout.addWidget(self._cmd_lbl)

    def set_state(self, state: str, cmd: str):
        self._state_lbl.setText(state)
        style = _RELAY_STYLES.get(state, "background:#607d8b; color:white;")
        self._state_lbl.setStyleSheet(f"border-radius:4px; padding:3px; {style}")
        self._cmd_lbl.setText(f"cmd: {cmd}" if cmd else "")


class RelayPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        outer = QVBoxLayout(self)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        container = QWidget()
        self._grid = QGridLayout(container)
        self._grid.setSpacing(8)
        self._grid.setAlignment(Qt.AlignTop | Qt.AlignLeft)
        scroll.setWidget(container)

        self._last = QLabel("—")
        self._last.setAlignment(Qt.AlignRight)
        self._last.setStyleSheet("color:gray; font-size:10px;")

        outer.addWidget(scroll)
        outer.addWidget(self._last)

        self._cards: dict[str, RelayCard] = {}

    def _card(self, relay_id: str) -> RelayCard:
        if relay_id not in self._cards:
            idx = len(self._cards)
            c = RelayCard(relay_id)
            self._grid.addWidget(c, idx // _COLS, idx % _COLS)
            self._cards[relay_id] = c
        return self._cards[relay_id]

    def update_data(self, payload: str):
        # {"R1": {"state": "On", "cmd": "0"}, "R2": {...}, ...}
        try:
            data: dict = json.loads(payload)
        except json.JSONDecodeError:
            return
        for relay_id, info in data.items():
            if isinstance(info, dict):
                self._card(relay_id).set_state(info.get("state", "Unknown"), info.get("cmd", ""))
        self._last.setText(f"Last update: {_ts()}")


# ─────────────────────────────────────────────── commands panel

# Short topics the embedded device accepts over BLE RX
_COMMANDS = {
    # label                       : (short_topic,                 needs_payload)
    "Get All Info":                ("get-all-info",                False),
    "Get Command Options":         ("cmd/command-options/get",     False),
    "Save Commands to FRAM":       ("cmd/save_all",                False),
    "Load Commands from FRAM":     ("cmd/load_all",                False),
    "Reset Commands to Default":   ("cmd/reset_cmd_to_def",        False),
    "Load Relay Groups from FRAM": ("relay-groups/load",           False),
    "Add Command":                 ("cmd/add",                     True),
    "Remove Command":              ("cmd/remove",                  True),
    "Override Command":            ("cmd/override",                True),
    "Import Commands":             ("cmd/import",                  True),
    "Set Relay Groups":            ("relay-groups/set",            True),
}

_CMD_TEMPLATES = {
    "cmd/add":          '{\n  "name": "",\n  "relayId": 1,\n  "startTime": "08:00",\n  "durationMin": 10\n}',
    "cmd/remove":       '{"id": 0}',
    "cmd/override":     '{\n  "id": 0,\n  "durationMin": 5\n}',
    "cmd/import":       '[\n  {\n    "name": "",\n    "relayId": 1,\n    "startTime": "08:00",\n    "durationMin": 10\n  }\n]',
    "relay-groups/set": '{\n  "group1": [1, 2, 3],\n  "group2": [4, 5, 6]\n}',
}


class CommandsPanel(QWidget):
    # Signal emitted when user wants to send a command (connected to BleClient).
    # (short_topic, payload)
    from PySide6.QtCore import Signal
    command_requested = Signal(str, str)

    def __init__(self, parent=None):
        super().__init__(parent)
        outer = QVBoxLayout(self)
        outer.setSpacing(6)

        # ── Quick-action buttons ──────────────────────────────────
        quick_group = QGroupBox("Quick Actions (no payload)")
        quick_layout = QHBoxLayout(quick_group)
        quick_layout.setSpacing(6)

        no_payload_cmds = [(label, st) for label, (st, needs) in _COMMANDS.items() if not needs]
        for label, short_topic in no_payload_cmds:
            btn = QPushButton(label)
            btn.setEnabled(False)
            btn.clicked.connect(lambda _=False, t=short_topic: self.command_requested.emit(t, ""))
            quick_layout.addWidget(btn)
            self._quick_btns = getattr(self, "_quick_btns", [])
            self._quick_btns.append(btn)

        outer.addWidget(quick_group)

        # ── Splitter: command form | response/list ────────────────
        splitter = QSplitter(Qt.Horizontal)

        # Left: command form
        form_widget = QGroupBox("Send Command with Payload")
        form_layout = QVBoxLayout(form_widget)

        self._action_combo = QComboBox()
        payload_cmds = [(label, st) for label, (st, needs) in _COMMANDS.items() if needs]
        for label, short_topic in payload_cmds:
            self._action_combo.addItem(label, userData=short_topic)
        self._action_combo.currentIndexChanged.connect(self._on_action_changed)

        self._payload_edit = QTextEdit()
        self._payload_edit.setFont(_MONO)
        self._payload_edit.setPlaceholderText("JSON payload…")

        self._send_cmd_btn = QPushButton("Send")
        self._send_cmd_btn.setEnabled(False)
        self._send_cmd_btn.clicked.connect(self._on_send_cmd)

        form_layout.addWidget(QLabel("Action:"))
        form_layout.addWidget(self._action_combo)
        form_layout.addWidget(QLabel("Payload (JSON):"))
        form_layout.addWidget(self._payload_edit, 1)
        form_layout.addWidget(self._send_cmd_btn)

        # Right: response area (tabs within tab)
        response_widget = QWidget()
        resp_layout = QVBoxLayout(response_widget)
        resp_layout.setContentsMargins(0, 0, 0, 0)

        resp_tabs = QTabWidget()

        # cmd/response
        self._response_edit = QTextEdit()
        self._response_edit.setReadOnly(True)
        self._response_edit.setFont(_MONO)
        resp_tabs.addTab(self._response_edit, "Response")

        # cmd/list
        self._cmd_list_edit = QTextEdit()
        self._cmd_list_edit.setReadOnly(True)
        self._cmd_list_edit.setFont(_MONO)
        resp_tabs.addTab(self._cmd_list_edit, "Command List")

        # cmd/command-options
        self._options_edit = QTextEdit()
        self._options_edit.setReadOnly(True)
        self._options_edit.setFont(_MONO)
        resp_tabs.addTab(self._options_edit, "Command Options")

        # relay-groups/info
        self._groups_edit = QTextEdit()
        self._groups_edit.setReadOnly(True)
        self._groups_edit.setFont(_MONO)
        resp_tabs.addTab(self._groups_edit, "Relay Groups")

        resp_layout.addWidget(resp_tabs)
        self._resp_tabs = resp_tabs

        splitter.addWidget(form_widget)
        splitter.addWidget(response_widget)
        splitter.setStretchFactor(0, 1)
        splitter.setStretchFactor(1, 1)

        outer.addWidget(splitter, 1)

        # populate template for first action
        self._on_action_changed(0)

    def set_enabled(self, enabled: bool):
        for btn in getattr(self, "_quick_btns", []):
            btn.setEnabled(enabled)
        self._send_cmd_btn.setEnabled(enabled)

    def _on_action_changed(self, _index: int):
        short_topic = self._action_combo.currentData()
        template = _CMD_TEMPLATES.get(short_topic, "")
        self._payload_edit.setPlainText(template)

    def _on_send_cmd(self):
        short_topic = self._action_combo.currentData()
        payload = self._payload_edit.toPlainText().strip()
        self.command_requested.emit(short_topic, payload)

    def update_response(self, payload: str):
        self._response_edit.setPlainText(f"[{_ts()}]\n{_fmt_json(payload)}")
        self._resp_tabs.setCurrentIndex(0)

    def update_cmd_list(self, payload: str):
        self._cmd_list_edit.setPlainText(f"[{_ts()}]\n{_fmt_json(payload)}")
        self._resp_tabs.setCurrentIndex(1)

    def update_options(self, payload: str):
        self._options_edit.setPlainText(f"[{_ts()}]\n{_fmt_json(payload)}")
        self._resp_tabs.setCurrentIndex(2)

    def update_groups(self, payload: str):
        self._groups_edit.setPlainText(f"[{_ts()}]\n{_fmt_json(payload)}")
        self._resp_tabs.setCurrentIndex(3)


# ──────────────────────────────────────────────────── main window

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Irrigator BT Handler")
        self.resize(1050, 760)

        self._ble = BleClient()
        self._devices: dict[str, str] = {}
        self._connected = False
        self._version_queried = False

        self._setup_ui()
        self._connect_signals()

    def _setup_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        root = QVBoxLayout(central)
        root.setContentsMargins(8, 8, 8, 6)
        root.setSpacing(6)

        self.conn_bar = ConnectionBar()
        root.addWidget(self.conn_bar)

        fw_row = QHBoxLayout()
        self.fw_label = QLabel("Firmware: —")
        self.fw_label.setStyleSheet(
            "color: #9e9e9e; font-size: 11px; padding: 2px 4px;"
        )
        self._get_version_btn = QPushButton("Get Version")
        self._get_version_btn.setFixedWidth(100)
        self._get_version_btn.setEnabled(False)
        self._get_version_btn.clicked.connect(self._query_version)
        fw_row.addWidget(self.fw_label, 1)
        fw_row.addWidget(self._get_version_btn)
        root.addLayout(fw_row)

        sep = QFrame()
        sep.setFrameShape(QFrame.HLine)
        root.addWidget(sep)

        self.tabs = QTabWidget()
        self.log_panel = LogPanel()
        self.sensor_panel = SensorPanel()
        self.relay_panel = RelayPanel()
        self.cmd_panel = CommandsPanel()

        self.tabs.addTab(self.log_panel,    "📋  Log")
        self.tabs.addTab(self.sensor_panel, "🌡  Sensors")
        self.tabs.addTab(self.relay_panel,  "⚡  Relays")
        self.tabs.addTab(self.cmd_panel,    "🎛  Commands")
        root.addWidget(self.tabs, 1)

        # Raw send bar
        send_row = QHBoxLayout()
        self._send_input = QLineEdit()
        self._send_input.setPlaceholderText("Send raw text to device via BLE RX…")
        self._send_input.setEnabled(False)
        self._send_btn = QPushButton("Send")
        self._send_btn.setFixedWidth(70)
        self._send_btn.setEnabled(False)
        self._send_btn.clicked.connect(self._send_raw)
        self._send_input.returnPressed.connect(self._send_raw)
        send_row.addWidget(self._send_input)
        send_row.addWidget(self._send_btn)
        root.addLayout(send_row)

        self.setStatusBar(QStatusBar())
        self.statusBar().showMessage("Ready — click Scan to discover devices")

    def _connect_signals(self):
        self.conn_bar.scan_btn.clicked.connect(self._scan)
        self.conn_bar.connect_btn.clicked.connect(self._toggle_connection)
        self.conn_bar.device_combo.currentIndexChanged.connect(self._on_combo_changed)

        self._ble.device_found.connect(self._on_device_found)
        self._ble.scan_finished.connect(self._on_scan_finished)
        self._ble.connected.connect(self._on_connected)
        self._ble.disconnected.connect(self._on_disconnected)
        self._ble.message_received.connect(self._on_message)
        self._ble.error.connect(self._on_error)

        self.cmd_panel.command_requested.connect(self._send_command)

    # ─────────────── actions

    @Slot()
    def _scan(self):
        self.conn_bar.scan_btn.setEnabled(False)
        self.conn_bar.connect_btn.setEnabled(False)
        self.conn_bar.device_combo.clear()
        self._devices.clear()
        self.statusBar().showMessage("Scanning…")
        self._ble.scan()

    @Slot()
    def _toggle_connection(self):
        if self._connected:
            self.statusBar().showMessage("Disconnecting…")
            self.conn_bar.connect_btn.setEnabled(False)
            self._ble.disconnect_device()
        else:
            display = self.conn_bar.device_combo.currentText()
            address = self._devices.get(display)
            if address:
                self.statusBar().showMessage(f"Connecting to {display}…")
                self.conn_bar.connect_btn.setEnabled(False)
                self._ble.connect_device(address)

    @Slot()
    def _query_version(self):
        self._ble.send_command("system/version/get", "")
        self.log_panel.append("sent", "system/version/get: ")

    @Slot()
    def _send_raw(self):
        text = self._send_input.text().strip()
        if text:
            self._ble.send(text)
            self._send_input.clear()
            self.log_panel.append("sent", text)

    @Slot(str, str)
    def _send_command(self, short_topic: str, payload: str):
        self._ble.send_command(short_topic, payload)
        self.log_panel.append("sent", f"{short_topic}: {payload}")

    # ─────────────── BLE slots

    @Slot(str, str)
    def _on_device_found(self, name: str, address: str):
        if self.conn_bar.filter_check.isChecked() and "irrigator" not in name.lower():
            return
        display = f"{name}  [{address}]"
        self._devices[display] = address
        self.conn_bar.device_combo.addItem(display)

    @Slot(int)
    def _on_scan_finished(self, total: int):
        self.conn_bar.scan_btn.setEnabled(True)
        found = self.conn_bar.device_combo.count()
        if found > 0:
            self.conn_bar.device_combo.setCurrentIndex(0)
        suffix = f" (of {total} total)" if self.conn_bar.filter_check.isChecked() else ""
        self.statusBar().showMessage(f"Scan done — {found} device(s) shown{suffix}")
        self.conn_bar.connect_btn.setEnabled(found > 0)

    @Slot(str)
    def _on_connected(self, address: str):
        self._connected = True
        self.conn_bar.connect_btn.setText("Disconnect")
        self.conn_bar.connect_btn.setEnabled(True)
        self.conn_bar.status_dot.setStyleSheet("color:#4caf50; font-size:20px;")
        self._send_input.setEnabled(True)
        self._send_btn.setEnabled(True)
        self.cmd_panel.set_enabled(True)
        self._version_queried = False
        self._get_version_btn.setEnabled(True)
        self.statusBar().showMessage(f"Connected — {address}")
        self.log_panel.append("system", f"Connected to {address}")

    @Slot()
    def _on_disconnected(self):
        self._connected = False
        self.conn_bar.connect_btn.setText("Connect")
        self.conn_bar.connect_btn.setEnabled(True)
        self.conn_bar.status_dot.setStyleSheet("color:#9e9e9e; font-size:20px;")
        self._send_input.setEnabled(False)
        self._send_btn.setEnabled(False)
        self._get_version_btn.setEnabled(False)
        self.cmd_panel.set_enabled(False)
        self.fw_label.setText("Firmware: —")
        self.fw_label.setStyleSheet("color: #9e9e9e; font-size: 11px; padding: 2px 4px;")
        self.statusBar().showMessage("Disconnected")
        self.log_panel.append("system", "Disconnected")

    @Slot(str, str)
    def _on_message(self, topic: str, payload: str):
        self.log_panel.append(topic, payload)

        if "localTime" in topic and not self._version_queried:
            self._version_queried = True
            self._ble.send_command("system/version/get", "")
            self.log_panel.append("sent", "system/version/get: ")

        if "system/version" in topic:
            try:
                d = json.loads(payload)
                ver = d.get("version", "?")
                built = d.get("built", "?")
                self.fw_label.setText(f"Firmware: {ver}  |  built: {built}")
                self.fw_label.setStyleSheet("color: #a5d6a7; font-size: 11px; padding: 2px 4px;")
            except Exception:
                self.fw_label.setText(f"Firmware: {payload}")
        elif "sensors" in topic:
            self.sensor_panel.update_data(payload)
        elif "relays" in topic:
            self.relay_panel.update_data(payload)
        elif "cmd/response" in topic:
            self.cmd_panel.update_response(payload)
        elif "cmd/list" in topic:
            self.cmd_panel.update_cmd_list(payload)
        elif "cmd/command-options" in topic:
            self.cmd_panel.update_options(payload)
        elif "relay-groups/info" in topic:
            self.relay_panel.update_data  # relay groups shown separately
            self.cmd_panel.update_groups(payload)

    @Slot(str)
    def _on_error(self, msg: str):
        self.statusBar().showMessage(f"Error: {msg}")
        self.log_panel.append("error", msg)

    @Slot()
    def _on_combo_changed(self):
        self.conn_bar.connect_btn.setEnabled(
            not self._connected and self.conn_bar.device_combo.count() > 0
        )
