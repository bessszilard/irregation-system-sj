#include "Enums.hpp"

// clang-format off
CommandType CommandTypeFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "")      return CommandType::Unknown;
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

RelayState RelayStateFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "Opened") return RelayState::Opened;
    if (subStr == "Closed") return RelayState::Closed;
    return RelayState::Unknown;
}

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

String ToString(RelayIds p_id)
{
    switch(p_id)
    {
        case RelayIds::Relay1:       return "R01";
        case RelayIds::Relay2:       return "R02";
        case RelayIds::Relay3:       return "R03";
        case RelayIds::Relay4:       return "R04";
        case RelayIds::Relay5:       return "R05";
        case RelayIds::Relay6:       return "R06";
        case RelayIds::Relay7:       return "R07";
        case RelayIds::Relay8:       return "R08";
        case RelayIds::Relay9:       return "R09";
        case RelayIds::Relay10:      return "R10";
        case RelayIds::Relay11:      return "R11";
        case RelayIds::Relay12:      return "R12";
        case RelayIds::AllRelays:    return "RXX";
    }
    return "";
}

String ToString(RelayState p_state)
{
    switch(p_state)
    {
        case RelayState::Opened: return "Opened";
        case RelayState::Closed: return "Closed";
    }
    return "";
}

String ToString(CmdPriority p_priority)
{
    switch(p_priority)
    {
        case CmdPriority::Priority0: return "P0";
        case CmdPriority::Priority1: return "P1";
        case CmdPriority::Priority2: return "P2";
        case CmdPriority::Priority3: return "P3";
        case CmdPriority::Priority4: return "P4";
        case CmdPriority::Priority5: return "P5";
        case CmdPriority::Priority6: return "P6";
        case CmdPriority::Priority7: return "P7";
        case CmdPriority::Priority8: return "P8";
        case CmdPriority::Priority9: return "P9";
    }
    return "";
}

String ToString(CommandType p_type)
{
    switch(p_type) 
    {
        case CommandType::ManCtrl:              return "Manua";
        case CommandType::AutoTemperatureCtrl:  return "ATemp";
        case CommandType::AutoFlowCtrl:         return "AFlow";
        case CommandType::AutoHumidityCtrl:     return "AHumi";
        case CommandType::AutoTimeCtrl:         return "ATime";
        case CommandType::AutoMoistureCtrl:     return "AMost";
    }
    return "";
}

String ToString(CommandState p_state)
{
    switch(p_state) 
    {
        case CommandState::Added:            return "Added";
        case CommandState::Removed:          return "Removed";
        case CommandState::AlreadyPresent:   return "AlreadyPresent";
        case CommandState::CantRemove:       return "CantRemove";
        case CommandState::MemoryFull:       return "MemoryFull";
        case CommandState::Unknown:          return "Unknown";
    }
    return "";
}

String ToShortString(wl_status_t p_status)
{
    switch(p_status)
    {
        case WL_NO_SHIELD:      return "NS";
        case WL_STOPPED:        return "ST";
        case WL_IDLE_STATUS:    return "ID";
        case WL_NO_SSID_AVAIL:  return "NO";
        case WL_SCAN_COMPLETED: return "SC";
        case WL_CONNECTED:      return "OK";
        case WL_CONNECT_FAILED: return "CF";
        case WL_CONNECTION_LOST:return "CL";
        case WL_DISCONNECTED:   return "DI";
    }
    return "";
}

String ToShortString(SignalStrength p_strength)
{
    switch(p_strength)
    {
        case SignalStrength::Strength9of9: return "9";
        case SignalStrength::Strength8of9: return "8";
        case SignalStrength::Strength7of9: return "7";
        case SignalStrength::Strength6of9: return "6";
        case SignalStrength::Strength5of9: return "5";
        case SignalStrength::Strength4of9: return "4";
        case SignalStrength::Strength3of9: return "3";
        case SignalStrength::Strength2of9: return "2";
        case SignalStrength::Strength1of9: return "1";
        case SignalStrength::Strength0of9: return "0";
    }
    return "U";
}


String ToShortString(RelayState p_state)
{
    switch(p_state)
    {
        case RelayState::Opened: return "0";
        case RelayState::Closed: return "1";
    }
    return "U";
}


RelayIds ToRelayId(uint8_t p_id)
{
    switch(p_id)
    {
        case 0  : return RelayIds::Relay1;
        case 1  : return RelayIds::Relay2;
        case 2  : return RelayIds::Relay3;
        case 3  : return RelayIds::Relay4;
        case 4  : return RelayIds::Relay5;
        case 5  : return RelayIds::Relay6;
        case 6  : return RelayIds::Relay7;
        case 7  : return RelayIds::Relay8;
        case 8  : return RelayIds::Relay9;
        case 9  : return RelayIds::Relay10;
        case 10 : return RelayIds::Relay11;
        case 11 : return RelayIds::Relay12;
        case 14 : return RelayIds::AllRelays;
    }
    return RelayIds::Unknown;
}

SignalStrength ToSignalStrength(int8_t p_rssi)
{
    if (p_rssi ==  0) return SignalStrength::Unknown;
    if (p_rssi > -50) return SignalStrength::Strength9of9;
    if (p_rssi > -55) return SignalStrength::Strength8of9;
    if (p_rssi > -60) return SignalStrength::Strength7of9;
    if (p_rssi > -65) return SignalStrength::Strength6of9;
    if (p_rssi > -70) return SignalStrength::Strength5of9;
    if (p_rssi > -75) return SignalStrength::Strength4of9;
    if (p_rssi > -80) return SignalStrength::Strength3of9;
    if (p_rssi > -85) return SignalStrength::Strength2of9;
    if (p_rssi > -90) return SignalStrength::Strength1of9;
    return SignalStrength::Strength0of9;
}

// clang-format on