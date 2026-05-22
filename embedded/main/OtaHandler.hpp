#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define OTA_SERVICE_UUID     "4C5F0001-E8C8-4B0D-AEB9-1C8B6D9A5E2F"
#define OTA_CTRL_CHAR_UUID   "4C5F0002-E8C8-4B0D-AEB9-1C8B6D9A5E2F"  // client → ESP32 (write)
#define OTA_DATA_CHAR_UUID   "4C5F0003-E8C8-4B0D-AEB9-1C8B6D9A5E2F"  // client → ESP32 (write, binary chunks)
#define OTA_STATUS_CHAR_UUID "4C5F0004-E8C8-4B0D-AEB9-1C8B6D9A5E2F"  // ESP32 → client (notify)

class OtaHandler
{
public:
    OtaHandler();

    void registerService(BLEServer* p_server);

    // Call from the main loop — finalises the update outside the BLE task.
    void loop();

    // Returns true once after a successful flash — caller should reboot.
    bool shouldReboot() const;
    bool isInProgress() const;

    // Called by BLE callbacks — not for external use.
    void onCtrlWrite(const String& p_data);
    void onDataWrite(const uint8_t* p_data, size_t p_len);

private:
    void sendStatus(const String& p_msg);
    void abortOta(const char* p_reason);

    BLECharacteristic* m_statusChar;
    volatile bool      m_inProgress;
    volatile bool      m_transferComplete; // set on BLE task, read on main task
    bool               m_shouldReboot;
    size_t             m_totalSize;
    size_t             m_written;
    uint8_t            m_lastProgressPct;
    uint32_t           m_lastLedToggle_ms;
    bool               m_ledState;
};
