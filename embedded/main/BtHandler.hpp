#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <functional>
#include "OtaHandler.hpp"

// Nordic UART Service UUIDs (supported by nRF Connect, LightBlue, etc. on iOS & Android)
#define NUS_SERVICE_UUID "6E400001-B5B3-F393-E0A9-E50E24DCCA9E"
#define NUS_TX_CHAR_UUID "6E400003-B5B3-F393-E0A9-E50E24DCCA9E" // ESP32 → client (notify)
#define NUS_RX_CHAR_UUID "6E400002-B5B3-F393-E0A9-E50E24DCCA9E" // client → ESP32 (write)

// Callback invoked when a complete "short_topic: payload\n" frame arrives over BLE RX.
// short_topic is the topic without the device-specific prefix, e.g. "cmd/add".
using BtCmdCallback = std::function<void(const String& shortTopic, const String& payload)>;

class BtHandler
{
public:
    BtHandler();

    void begin(const char* p_deviceName);
    bool isConnected() const;
    bool shouldReboot() const;
    bool isFirmwareUpdateActive() const;
    void publish(const char* p_topic, const String& p_message);

    // Call from the main loop — handles deferred publishes that must run outside BLE callbacks.
    void loop();

    // Register the callback that processes incoming BLE commands.
    void setCommandCallback(BtCmdCallback p_cb);

    // Called by internal BLE callbacks — not for external use.
    void setConnected(bool p_connected);
    void onRxData(const String& p_data);

private:
    BLEServer*         m_server;
    BLECharacteristic* m_txChar;
    bool               m_deviceConnected;
    BtCmdCallback      m_cmdCallback;
    String             m_rxBuffer;
    OtaHandler         m_otaHandler;

    static const size_t CHUNK_SIZE = 200;
};
