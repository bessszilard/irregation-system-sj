#pragma once
// #include "FRAM.h"
#include "Adafruit_FRAM_I2C.h"

class FramManager {
public:
    FramManager();
    bool begin();
    // bool isConnected();
    bool saveCommands(const String& cmdList);
    bool loadCommands(String& cmdList);
    // uint32_t clear();
    void printId();

    bool saveWifiConfig(const String& ssid, const String& password);
    bool loadWifiConfig(String& ssid, String& password);

    bool saveMqttConfig(const String& server, uint16_t port, const String& password);
    bool loadMqttConfig(String& server, uint16_t& port, String& password);

    bool saveRelayGroups(uint16_t* p_data, uint16_t p_length);
    bool loadRelayGroups(uint16_t* p_data, uint16_t p_length);

    // bool getInfo(String& infoStr);
protected:
    bool writeAndVerify16(uint16_t address, uint16_t data);
    bool writeStringAndVerify(uint16_t address, const String& strData);

    bool read16(uint16_t address, uint16_t& data);
    bool readString(uint16_t address, String& strData, uint16_t length);
    void dumpFram(uint16_t address, uint16_t length);
    
private:
    Adafruit_FRAM_I2C m_fram;
};