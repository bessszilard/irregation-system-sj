#include "BtHandler.hpp"
#include <BLEUtils.h>

//---------------------------------------------------------------
class BtServerCallbacks : public BLEServerCallbacks
{
public:
    BtServerCallbacks(BtHandler* p_handler)
        : m_handler(p_handler)
    {
    }

    void onConnect(BLEServer* pServer) override
    {
        m_handler->setConnected(true);
        Serial.println("[BT] Client connected");
    }

    void onDisconnect(BLEServer* pServer) override
    {
        m_handler->setConnected(false);
        Serial.println("[BT] Client disconnected, restarting advertising");
        BLEDevice::startAdvertising();
    }

private:
    BtHandler* m_handler;
};

//---------------------------------------------------------------
class BtRxCallbacks : public BLECharacteristicCallbacks
{
public:
    BtRxCallbacks(BtHandler* p_handler)
        : m_handler(p_handler)
    {
    }

    void onWrite(BLECharacteristic* p_char) override
    {
        m_handler->onRxData(p_char->getValue());
    }

private:
    BtHandler* m_handler;
};

//---------------------------------------------------------------
BtHandler::BtHandler()
    : m_server(nullptr)
    , m_txChar(nullptr)
    , m_deviceConnected(false)
//---------------------------------------------------------------
{
}

//---------------------------------------------------------------
void BtHandler::begin(const char* p_deviceName)
//---------------------------------------------------------------
{
    BLEDevice::init(p_deviceName);

    m_server = BLEDevice::createServer();
    m_server->setCallbacks(new BtServerCallbacks(this));

    BLEService* pService = m_server->createService(NUS_SERVICE_UUID);

    // TX: ESP32 → client (notify)
    m_txChar = pService->createCharacteristic(NUS_TX_CHAR_UUID, BLECharacteristic::PROPERTY_NOTIFY);
    m_txChar->addDescriptor(new BLE2902());

    // RX: client → ESP32 (write)
    BLECharacteristic* rxChar = pService->createCharacteristic(
        NUS_RX_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    rxChar->setCallbacks(new BtRxCallbacks(this));

    pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(NUS_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    Serial.printf("[BT] BLE advertising as '%s'\n", p_deviceName);
}

//---------------------------------------------------------------
bool BtHandler::isConnected() const
//---------------------------------------------------------------
{
    return m_deviceConnected;
}

//---------------------------------------------------------------
void BtHandler::setConnected(bool p_connected)
//---------------------------------------------------------------
{
    m_deviceConnected = p_connected;
}

//---------------------------------------------------------------
void BtHandler::loop()
//---------------------------------------------------------------
{
}

//---------------------------------------------------------------
void BtHandler::setCommandCallback(BtCmdCallback p_cb)
//---------------------------------------------------------------
{
    m_cmdCallback = p_cb;
}

//---------------------------------------------------------------
// Called from the BLE task — accumulates data into a line buffer,
// then fires m_cmdCallback for each complete "shortTopic: payload\n" frame.
void BtHandler::onRxData(const String& p_data)
//---------------------------------------------------------------
{
    m_rxBuffer += p_data;

    int nlIdx;
    while ((nlIdx = m_rxBuffer.indexOf('\n')) >= 0)
    {
        String line = m_rxBuffer.substring(0, nlIdx);
        m_rxBuffer  = m_rxBuffer.substring(nlIdx + 1);
        line.trim();
        if (line.length() == 0)
            continue;

        int sep = line.indexOf(':');
        if (sep < 0)
        {
            Serial.printf("[BT RX] Malformed frame (no ':'): %s\n", line.c_str());
            continue;
        }

        String shortTopic = line.substring(0, sep);
        String payload    = line.substring(sep + 1);
        shortTopic.trim();
        payload.trim();

        Serial.printf("[BT RX] topic=%s payload=%s\n", shortTopic.c_str(), payload.c_str());

        if (m_cmdCallback)
        {
            m_cmdCallback(shortTopic, payload);
        }
    }
}

//---------------------------------------------------------------
void BtHandler::publish(const char* p_topic, const String& p_message)
//---------------------------------------------------------------
{
    if (!m_deviceConnected || m_txChar == nullptr)
    {
        return;
    }

    // Format: "topic: message\n"
    String packet = String(p_topic) + ": " + p_message + "\n";
    size_t len    = packet.length();
    size_t offset = 0;

    while (offset < len)
    {
        size_t chunkLen = min(CHUNK_SIZE, len - offset);
        m_txChar->setValue((uint8_t*)(packet.c_str() + offset), chunkLen);
        m_txChar->notify();
        offset += chunkLen;
        if (offset < len)
        {
            delay(10); // Give BLE stack time between chunks
        }
    }
}
