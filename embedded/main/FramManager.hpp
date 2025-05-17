#pragma once
// #include "FRAM.h"
#include "Adafruit_FRAM_I2C.h"

class FramManager {
public:
    FramManager(TwoWire *wire = &Wire);
    int begin();
    // bool isConnected();
    bool saveCommands(const String& cmdList);
    bool loadCommands(String& cmdList);
    // uint32_t clear();

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