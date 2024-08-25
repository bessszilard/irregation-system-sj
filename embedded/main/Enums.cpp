#include "Enums.hpp"

// clang-format off
CommandType CommandTypeFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "")       return CommandType::Unknown;
    if (subStr == "Manua") return CommandType::ManCtrl;
    if (subStr == "ATemp") return CommandType::AutoTemperatureCtrl;
    if (subStr == "AFlow") return CommandType::AutoFlowCtrl;
    if (subStr == "AHumi") return CommandType::AutoHumidityCtrl;
    if (subStr == "ATime") return CommandType::AutoTimeCtrl;
    return CommandType::Unknown;
}

RelayIds RelayIdTypeFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "")        return RelayIds::Unknown;
    if (subStr == "R01")     return RelayIds::Relay1;
    if (subStr == "R02")     return RelayIds::Relay2;
    if (subStr == "R03")     return RelayIds::Relay3;
    if (subStr == "R04")     return RelayIds::Relay4;
    if (subStr == "R05")     return RelayIds::Relay5;
    if (subStr == "R06")     return RelayIds::Relay6;
    if (subStr == "R07")     return RelayIds::Relay7;
    if (subStr == "R08")     return RelayIds::Relay8;
    if (subStr == "R09")     return RelayIds::Relay9;
    if (subStr == "R10")     return RelayIds::Relay10;
    if (subStr == "R11")     return RelayIds::Relay11;
    if (subStr == "R12")     return RelayIds::Relay12;
    if (subStr == "RXX")     return RelayIds::AllRelays;
    return RelayIds::Unknown;
}

// clang-format off
RelayState RelayStateFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "Opened") return RelayState::Opened;
    if (subStr == "Closed") return RelayState::Closed;
    return RelayState::Unknown;
}

// clang-format off
CmdPriority CmdPriorityFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "P0") return CmdPriority::Priority0;
    if (subStr == "P1") return CmdPriority::Priority1;
    if (subStr == "P2") return CmdPriority::Priority2;
    if (subStr == "P3") return CmdPriority::Priority3;
    if (subStr == "P4") return CmdPriority::Priority4;
    if (subStr == "P5") return CmdPriority::Priority5;
    if (subStr == "P6") return CmdPriority::Priority6;
    if (subStr == "P7") return CmdPriority::Priority7;
    if (subStr == "P8") return CmdPriority::Priority8;
    if (subStr == "P9") return CmdPriority::Priority9;
    return CmdPriority::Unknown;    
}

// clang-format on