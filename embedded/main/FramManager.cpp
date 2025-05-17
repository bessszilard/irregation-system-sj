#include "FramManager.hpp"
#include "FramMap.hpp"

FramManager::FramManager(TwoWire* wire) : m_fram() {}

int FramManager::begin()
{
    // save to FRAM
    return m_fram.begin(0x50);
}

// bool FramManager::isConnected()
// {
//     return m_fram.isConnected();
// }

bool FramManager::saveCommands(const String& cmdList)
{
    if (false == writeAndVerify16(CMDS_ID_ADDR, CMDS_ID_NUMBER))
    {
        Serial.println("Failed to write CMDS_ID_ADDR");
        return false;
    }

    Serial.printf("Writing string %s to FRAM\n", cmdList.c_str());
    uint16_t stringLen = cmdList.length() + 1; // +1 is the \0
    if (false == writeAndVerify16(CMDS_MEM_LEN_ADDR, stringLen))
    {
        Serial.println("Failed to write CMDS_ID_ADDR");
        return false;
    }

    if (false == writeStringAndVerify(CMDS_MEM_START_ADDR, cmdList))
    {
        Serial.println("Failed to write the string to FRAM");
        return false;
    }
    return true;
}

bool FramManager::loadCommands(String& cmdList)
{
    uint16_t cmdId = 0;
    if (false == read16(CMDS_ID_ADDR, cmdId) || CMDS_ID_NUMBER != cmdId)
    {
        Serial.printf("Failed to read CMDS_ID_NUMBER: %d\n", cmdId);
        return false;
    }
    Serial.printf(">>> CMD ID is OK.\n");

    uint16_t cmdListLen = 0;
    if (false == read16(CMDS_MEM_LEN_ADDR, cmdListLen) || cmdListLen == 0)
    {
        Serial.printf("Failed to read cmd list length: %d\n", cmdListLen);
        return false;
    }
    Serial.printf(">>> Command id len is %d\n", cmdListLen);

    String cmdListVerification;
    if (false == readString(CMDS_MEM_START_ADDR, cmdList, cmdListLen))
    {
        Serial.println("Failed to write the string to FRAM");
        return false;
    }

    Serial.printf("Read back string %s\n", cmdList);
    return true;
}

bool FramManager::writeAndVerify16(uint16_t address, uint16_t data)
{
    if (false == m_fram.write(address, (uint8_t*)&data, sizeof(uint16_t)))
    {
        Serial.printf("Failed to write %d\n", data);
        return false;
    }

    uint16_t verifiedValue = 0;
    if (false == read16(address, verifiedValue))
    {
        Serial.printf("Failed to read back the value");
        return false;
    }

    if (verifiedValue != data)
    {
        Serial.printf("Failed to read back the same value %d != %d\n", data, verifiedValue);
        return false;
    }
    return true;
}

bool FramManager::writeStringAndVerify(uint16_t address, const String& strData)
{
    Serial.printf("writing str len %d %s\n", strData.length(), strData.c_str());
    int chuckSize = 64;
    const char* data = strData.c_str();
    int len          = strlen(data) + 1;
    for (int i = 0; i < len; i += chuckSize)
    {
        int chunkLen = min(chuckSize, len - i);
        m_fram.write(address + i, (uint8_t*)(data + i), chunkLen);
        // delay(5); // short pause for I2C to settle
    }

    String strVerification;
    if (false == readString(address, strVerification, strData.length()+1))
    {
        Serial.println("Failed to read back the string");
        return false;
    }

    bool everythingOkay = true;
    for(int i=0; i<strData.length() + 1; ++i)
    {
        if (strData[i] != strVerification[i])
        {
            Serial.printf("Mismatch at index %d: wrote '%c' (0x%02X), read '%c' (0x%02X)\n",
                          i, strData[i], strData[i], strVerification[i], strVerification[i]);
            // return false;
            everythingOkay = false;
        }
    }
    Serial.printf(">>>>> Everything okay? %d %s\n", everythingOkay, strVerification.c_str());
    return true;
}

bool FramManager::read16(uint16_t address, uint16_t& data)
{
    if (false == m_fram.read(address, (uint8_t*)&data, sizeof(uint16_t)))
    {
        Serial.printf("Failed to read back %d\n", data);
        return false;
    }
    return true;
}

bool FramManager::readString(uint16_t address, String& strData, uint16_t length)
{
    const int chunkSize = 64;
    uint8_t* buffer = (uint8_t*)malloc(length);
    if (!buffer) {
        Serial.println("Failed to allocate buffer");
        return false;
    }

    for (uint16_t i = 0; i < length; i += chunkSize) {
        int thisChunk = (length - i < chunkSize) ? (length - i) : chunkSize;
        if (!m_fram.read(address + i, buffer + i, thisChunk)) {
            Serial.println("Chunked read failed");
            free(buffer);
            return false;
        }
    }
    strData = String((char*)buffer);  // null terminátorig konvertál
    free(buffer);

    return true;
}

void FramManager::dumpFram(uint16_t address, uint16_t length) {
    uint8_t buf[64];
    for (uint16_t i = 0; i < length; i += 16) {
        uint16_t chunk = min(16, length - i);
        m_fram.read(address + i, buf, chunk);
        Serial.printf("0x%04X: ", address + i);
        for (int j = 0; j < chunk; ++j) {
            Serial.printf("%02X ", buf[j]);
        }
        Serial.println();
    }
}


// uint32_t FramManager::clear()
// {
//     return m_fram.clear();
// }

// bool FramManager::getInfo(String& infoStr)
// {
//     infoStr = String("ManufacturerID: ") + String(m_fram.getManufacturerID())
//             + " ProductID:" + String(m_fram.getProductID())
//             + " memory KB:" + String(m_fram.getSize());
//     return true;
// }

// void testRawWrite() {
//     uint8_t testData[16];
//     for (int i = 0; i < 16; ++i) testData[i] = i;

//     Serial.println("Writing raw bytes 0-15...");
//     if (!m_fram.write(0x0000, testData, 16)) {
//         Serial.println("Raw write failed!");
//         return;
//     }

//     Serial.println("Dumping FRAM...");
//     dumpFram(0x0000, 16);
// }