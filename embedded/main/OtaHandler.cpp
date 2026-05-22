#include "OtaHandler.hpp"
#include "Pinout.hpp"
#include <Update.h>
#include <BLEUtils.h>

static const uint32_t OTA_LED_BLINK_INTERVAL_MS = 250;

//---------------------------------------------------------------
class OtaCtrlCallbacks : public BLECharacteristicCallbacks
{
public:
    OtaCtrlCallbacks(OtaHandler* p_handler) : m_handler(p_handler) {}
    void onWrite(BLECharacteristic* p_char) override
    {
        m_handler->onCtrlWrite(p_char->getValue());
    }
private:
    OtaHandler* m_handler;
};

//---------------------------------------------------------------
class OtaDataCallbacks : public BLECharacteristicCallbacks
{
public:
    OtaDataCallbacks(OtaHandler* p_handler) : m_handler(p_handler) {}
    void onWrite(BLECharacteristic* p_char) override
    {
        String val = p_char->getValue();
        m_handler->onDataWrite(reinterpret_cast<const uint8_t*>(val.c_str()), val.length());
    }
private:
    OtaHandler* m_handler;
};

//---------------------------------------------------------------
OtaHandler::OtaHandler()
    : m_statusChar(nullptr)
    , m_inProgress(false)
    , m_transferComplete(false)
    , m_shouldReboot(false)
    , m_totalSize(0)
    , m_written(0)
    , m_lastProgressPct(0)
    , m_lastLedToggle_ms(0)
    , m_ledState(false)
//---------------------------------------------------------------
{
}

//---------------------------------------------------------------
void OtaHandler::registerService(BLEServer* p_server)
//---------------------------------------------------------------
{
    BLEService* svc = p_server->createService(OTA_SERVICE_UUID);

    // Control char: client writes "BEGIN:<total_bytes>" or "ABORT"
    BLECharacteristic* ctrlChar = svc->createCharacteristic(
        OTA_CTRL_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    ctrlChar->setCallbacks(new OtaCtrlCallbacks(this));

    // Data char: client writes raw binary firmware chunks
    BLECharacteristic* dataChar = svc->createCharacteristic(
        OTA_DATA_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    dataChar->setCallbacks(new OtaDataCallbacks(this));

    // Status char: ESP32 notifies with "READY", "PROGRESS:<pct>", "SUCCESS", "ERROR:<msg>"
    m_statusChar = svc->createCharacteristic(
        OTA_STATUS_CHAR_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    m_statusChar->addDescriptor(new BLE2902());

    svc->start();
    Serial.println("[OTA] BLE OTA service registered");
}

//---------------------------------------------------------------
// Called from the main loop — runs Update.end() outside the BLE task so the
// BLE stack remains free to transmit the SUCCESS notification before reboot.
void OtaHandler::loop()
//---------------------------------------------------------------
{
    if (!m_transferComplete)
        return;

    m_transferComplete = false;

    if (!Update.end(true))
    {
        abortOta(Update.errorString());
        return;
    }

    m_shouldReboot = true;
    sendStatus("SUCCESS");
    delay(500); // Give BLE stack time to transmit the notification before reboot
    Serial.println("[OTA] Flash complete — rebooting");
}

//---------------------------------------------------------------
bool OtaHandler::shouldReboot() const
//---------------------------------------------------------------
{
    return m_shouldReboot;
}

//---------------------------------------------------------------
bool OtaHandler::isInProgress() const
//---------------------------------------------------------------
{
    return m_inProgress || m_transferComplete;
}

//---------------------------------------------------------------
// Handles "BEGIN:<total_bytes>" and "ABORT" control commands.
void OtaHandler::onCtrlWrite(const String& p_data)
//---------------------------------------------------------------
{
    Serial.printf("[OTA] Ctrl: %s\n", p_data.c_str());

    if (p_data.startsWith("BEGIN:"))
    {
        if (m_inProgress)
        {
            Update.abort();
            m_inProgress = false;
        }

        size_t totalSize = (size_t)p_data.substring(6).toInt();
        if (totalSize == 0)
        {
            abortOta("Invalid size");
            return;
        }
        if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        {
            String err = Update.errorString();
            abortOta(err.c_str());
            return;
        }
        m_totalSize        = totalSize;
        m_written          = 0;
        m_inProgress       = true;
        m_transferComplete = false;
        m_lastProgressPct  = 0;
        m_shouldReboot     = false;
        m_lastLedToggle_ms = millis();
        m_ledState         = true;
        digitalWrite(LED_PIN, m_ledState);
        Serial.printf("[OTA] Started, expecting %u bytes\n", (unsigned)totalSize);
        sendStatus("READY");
    }
    else if (p_data == "ABORT")
    {
        if (m_inProgress)
        {
            Update.abort();
            m_inProgress = false;
            sendStatus("ABORTED");
            Serial.println("[OTA] Aborted by client");
        }
    }
}

//---------------------------------------------------------------
// Accumulates raw binary chunks and writes them to flash via Update.
void OtaHandler::onDataWrite(const uint8_t* p_data, size_t p_len)
//---------------------------------------------------------------
{
    if (!m_inProgress || p_len == 0)
        return;

    const uint32_t now = millis();
    if (now - m_lastLedToggle_ms >= OTA_LED_BLINK_INTERVAL_MS)
    {
        m_lastLedToggle_ms = now;
        m_ledState         = !m_ledState;
        digitalWrite(LED_PIN, m_ledState);
    }

    size_t written = Update.write(const_cast<uint8_t*>(p_data), p_len);
    if (written != p_len)
    {
        abortOta(Update.errorString());
        return;
    }
    m_written += written;

    // Notify progress every 5%
    uint8_t pct = (uint8_t)((m_written * 100) / m_totalSize);
    if (pct >= m_lastProgressPct + 5 || m_written >= m_totalSize)
    {
        m_lastProgressPct = pct;
        sendStatus("PROGRESS:" + String(pct));
    }

    if (m_written >= m_totalSize)
    {
        // All bytes received — finalise on the main loop task to avoid
        // blocking the BLE stack during the flash-write in Update.end().
        m_inProgress       = false;
        m_transferComplete = true;
        Serial.println("[OTA] All bytes received — finalising on main loop");
    }
}

//---------------------------------------------------------------
void OtaHandler::sendStatus(const String& p_msg)
//---------------------------------------------------------------
{
    if (m_statusChar == nullptr)
        return;
    m_statusChar->setValue(p_msg.c_str());
    m_statusChar->notify();
}

//---------------------------------------------------------------
void OtaHandler::abortOta(const char* p_reason)
//---------------------------------------------------------------
{
    Update.abort();
    m_inProgress       = false;
    m_transferComplete = false;
    String msg   = "ERROR:";
    msg         += p_reason;
    sendStatus(msg);
    Serial.printf("[OTA] Error: %s\n", p_reason);
}
