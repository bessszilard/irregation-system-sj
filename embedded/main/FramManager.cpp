#include "FramManager.hpp"
#include "FramMap.hpp"

FramManager::FramManager() : m_fram() {}

bool FramManager::begin()
{
    return m_fram.begin(0x50);
}

// TODOsz clean up

void FramManager::printId()
{
    uint16_t manufacturerID;
    uint16_t productID;

    m_fram.getDeviceID(&manufacturerID, &productID);
    Serial.printf("manufacturerID %d, SerialId %d", manufacturerID, productID);
}

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

    uint16_t cmdListLen = 0;
    if (false == read16(CMDS_MEM_LEN_ADDR, cmdListLen) || cmdListLen == 0)
    {
        Serial.printf("Failed to read cmd list length: %d\n", cmdListLen);
        return false;
    }
    Serial.printf("Command id len is %d\n", cmdListLen);

    String cmdListVerification;
    if (false == readString(CMDS_MEM_START_ADDR, cmdList, cmdListLen))
    {
        Serial.println("Failed to write the string to FRAM");
        return false;
    }

    // Serial.print("Read back string");
    // Serial.println(cmdList);
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
    Serial.println(">>>>>>>>>>>>>>>>> adasdsadasd a");
    Serial.printf("writing str len %d %s\n", strData.length(), strData.c_str());
    // if (false == m_fram.write(address, (uint8_t*)strData.c_str(), strData.length()+1))
    // {
    //     Serial.println("Failed to write string");
    //     return false;
    // }

    int chuckSize    = 16;
    const char* data = strData.c_str();
    int len          = strlen(data) + 1;
    for (int i = 0; i < len; i += chuckSize)
    {
        int chunkLen = min(chuckSize, len - i);
        m_fram.write(address + i, (uint8_t*)(data + i), chunkLen);
        // delay(5); // short pause for I2C to settle
    }
    // dumpFram(address, len);

    String strVerification;
    if (false == readString(address, strVerification, strData.length() + 1))
    {
        Serial.println("Failed to read back the string");
        return false;
    }

    bool everythingOkay = true;
    for (int i = 0; i < strData.length() + 1; ++i)
    {
        if (strData[i] != strVerification[i])
        {
            Serial.printf("Mismatch at index %d: wrote '%c' (0x%02X), read '%c' (0x%02X)\n",
                          i,
                          strData[i],
                          strData[i],
                          strVerification[i],
                          strVerification[i]);
            // return false;
            everythingOkay = false;
        }
    }
    Serial.println(" everything okay? >>>>>>");
    Serial.println(everythingOkay);

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
    uint8_t* buffer     = (uint8_t*)malloc(length);
    if (!buffer)
    {
        Serial.println("Failed to allocate buffer");
        return false;
    }

    for (uint16_t i = 0; i < length; i += chunkSize)
    {
        int thisChunk = (length - i < chunkSize) ? (length - i) : chunkSize;
        if (!m_fram.read(address + i, buffer + i, thisChunk))
        {
            Serial.println("Chunked read failed");
            free(buffer);
            return false;
        }
    }

    // Serial.printf("Buffer: %s\n", buffer);
    strData = String((char*)buffer); // null terminátorig konvertál
    free(buffer);

    return true;
}

bool FramManager::saveRelayGroups(uint16_t* p_data, uint16_t p_length)
{
    if (false == writeAndVerify16(RELAY_GROUPS_ID_ADDR, RELAY_GROUPS_ID))
    {
        Serial.println("Failed to write CMDS_ID_ADDR");
        return false;
    }

    for (uint16_t i = 0; i < p_length; ++i)
    {
        if (false == writeAndVerify16(RELAY_GROUPS_ADDR + (i * 2), p_data[i]))
        {
            Serial.println("Failed to write RELAY_GROUPS_ADDR");
            return false;
        }
        Serial.printf("FRAM Wrote %d %d", RELAY_GROUPS_ADDR + (i * 2), p_data[i]);
    }
    return true;
}

bool FramManager::loadRelayGroups(uint16_t* p_data, uint16_t p_length)
{
    uint16_t relayGroupId = 0;
    if (false == read16(RELAY_GROUPS_ID_ADDR, relayGroupId) && relayGroupId == RELAY_GROUPS_ID)
    {
        Serial.println("Failed to read RELAY_GROUPS_ID_ADDR");
        return false;
    }

    for (uint16_t i = 0; i < p_length; ++i)
    {
        if (false == read16(RELAY_GROUPS_ADDR + (i * 2), p_data[i]))
        {
            Serial.println("Failed to write RELAY_GROUPS_ADDR");
            return false;
        }
        Serial.printf("FRAM read %d %d", RELAY_GROUPS_ADDR + (i * 2), p_data[i]);
    }
    return true;
}