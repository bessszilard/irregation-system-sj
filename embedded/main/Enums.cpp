#include "Enums.hpp"

// clang-format off
CommandType CommandTypeFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "")       return CommandType::Unknown;
    if (subStr == "Manual") return CommandType::Manual;
    if (subStr == "TimSin") return CommandType::TimeSingleShot;
    if (subStr == "TimRep") return CommandType::TimeRepeat;
    if (subStr == "SenRan") return CommandType::SensorRange;
    if (subStr == "SenThr") return CommandType::SensorThreshold;
    if (subStr == "SeTTRe") return CommandType::SensorThresholdTimeRepeat;
    Serial.printf(">> %s is unknown\n", subStr.c_str());
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
    if (subStr == "R13")     return RelayIds::Relay13;
    if (subStr == "R14")     return RelayIds::Relay14;
    if (subStr == "R15")     return RelayIds::Relay15;
    if (subStr == "R16")     return RelayIds::Relay16;
    if (subStr == "RXX")     return RelayIds::AllRelays;
    Serial.printf(">> %s is unknown\n", subStr.c_str());
    return RelayIds::Unknown;
}

RelayGroups RelayGroupsFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "RGA") return RelayGroups::A;
    if (subStr == "RGB") return RelayGroups::B;
    if (subStr == "RGC") return RelayGroups::C;
    if (subStr == "RGD") return RelayGroups::D;
    if (subStr == "RGE") return RelayGroups::E;
    if (subStr == "RGF") return RelayGroups::F;
    if (subStr == "RGG") return RelayGroups::G;
    if (subStr == "RGH") return RelayGroups::H;
    return RelayGroups::Unknown;
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
    if (subStr == "PLW") return CmdPriority::PriorityLowest;
    if (subStr == "P00") return CmdPriority::Priority0;
    if (subStr == "P01") return CmdPriority::Priority1;
    if (subStr == "P02") return CmdPriority::Priority2;
    if (subStr == "P03") return CmdPriority::Priority3;
    if (subStr == "P04") return CmdPriority::Priority4;
    if (subStr == "P05") return CmdPriority::Priority5;
    if (subStr == "P06") return CmdPriority::Priority6;
    if (subStr == "P07") return CmdPriority::Priority7;
    if (subStr == "P08") return CmdPriority::Priority8;
    if (subStr == "P09") return CmdPriority::Priority9;
    if (subStr == "PTX") return CmdPriority::PriorityToggleAll;
    if (subStr == "PTO") return CmdPriority::PriorityToggleOne;
    if (subStr == "PHI") return CmdPriority::PriorityHighest;
    return CmdPriority::Unknown;    
}

SensorType ToSensorTypeFromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
{
    String subStr = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (subStr == "TESU") return SensorType::TempOnSun;
    if (subStr == "TESH") return SensorType::TempInShadow;
    if (subStr == "HUMI") return SensorType::Humidity;
    if (subStr == "FRDS") return SensorType::FlowRateDailySum;
    if (subStr == "FRLM") return SensorType::FlowRateLitPerMin;
    if (subStr == "RAIN") return SensorType::Rain;
    if (subStr == "LIGH") return SensorType::Light;
    if (subStr == "WAPR") return SensorType::WaterPressure;
    if (subStr == "SMLC") return SensorType::SoilMoistureLocal;
    if (subStr[0] == 'S' && subStr[1] == 'M') 
        return SensorType::SoilMoistureWireless; // SMXX <- index
    return SensorType::Unknown;
}

SerialCommands ToSerialCommands(const String& p_str)
{
    if (p_str == "SET_WIFI")  return SerialCommands::SetWifiParams;
    if (p_str == "GET_WIFI")  return SerialCommands::GetWifiParams;
    if (p_str == "SET_MQTT")  return SerialCommands::SetMqttParams;
    if (p_str == "GET_MQTT")  return SerialCommands::GetMqttParams;
    return SerialCommands::Unknown;
}

String ToString(RelayTargetType p_type)
{
    switch(p_type)
    {
        case RelayTargetType::SingleRelay:      return "SingleRelay";
        case RelayTargetType::Group:            return "Group";
        case RelayTargetType::All:              return "All";
        case RelayTargetType::Unknown: 
        default:
                 return "Unknown";
    }
    return "Unkown";
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
        case RelayIds::Relay13:      return "R13";
        case RelayIds::Relay14:      return "R14";
        case RelayIds::Relay15:      return "R15";
        case RelayIds::Relay16:      return "R16";
        case RelayIds::AllRelays:    return "RXX";
    }
    return "Uknown";
}

String ToString(RelayGroups p_group)
{
    switch(p_group)
    {
        case RelayGroups::A: return "RGA";
        case RelayGroups::B: return "RGB";
        case RelayGroups::C: return "RGC";
        case RelayGroups::D: return "RGD";
        case RelayGroups::E: return "RGE";
        case RelayGroups::F: return "RGF";
        case RelayGroups::G: return "RGG";
        case RelayGroups::H: return "RGH";
    }
    return "Uknown";
}

String ToString(RelayState p_state)
{
    switch(p_state)
    {
        case RelayState::Opened: return "Opened";
        case RelayState::Closed: return "Closed";
    }
    return "Uknown";
}

String ToString(CmdPriority p_priority)
{
    switch(p_priority)
    {
        case CmdPriority::Unknown:            return "PUN";
        case CmdPriority::PriorityLowest:     return "PLW";
        case CmdPriority::Priority0:          return "P00";
        case CmdPriority::Priority1:          return "P01";
        case CmdPriority::Priority2:          return "P02";
        case CmdPriority::Priority3:          return "P03";
        case CmdPriority::Priority4:          return "P04";
        case CmdPriority::Priority5:          return "P05";
        case CmdPriority::Priority6:          return "P06";
        case CmdPriority::Priority7:          return "P07";
        case CmdPriority::Priority8:          return "P08";
        case CmdPriority::Priority9:          return "P09";
        case CmdPriority::PriorityToggleAll:  return "PTX";
        case CmdPriority::PriorityToggleOne:  return "PTO";
        case CmdPriority::PriorityHighest:    return "PHI";
    }
    return "Unknown";
}

String ToString(CommandType p_type)
{
    switch(p_type) 
    {
        case CommandType::Manual:                    return "Manual";
        case CommandType::TimeSingleShot:            return "TimSin";
        case CommandType::TimeRepeat:                return "TimRep";
        case CommandType::SensorRange:               return "SenRan";
        case CommandType::SensorThreshold:           return "SenThr";
        case CommandType::SensorThresholdTimeRepeat: return "SeTTRe";
    }
    return "Unknown";
}

String ToString(CommandState p_state)
{
    switch(p_state) 
    {
        case CommandState::Added:            return "Added";
        case CommandState::Removed:          return "Removed";
        case CommandState::Overriden:        return "Overriden";
        case CommandState::AlreadyPresent:   return "AlreadyPresent";
        case CommandState::CantRemove:       return "CantRemove";
        case CommandState::MemoryFull:       return "MemoryFull";
        case CommandState::Unknown:          return "Unknown";
    }
    return "Unknown";
}

String ToString(SensorType p_type)
{
    switch(p_type)
    {
        case SensorType::TempOnSun:             return "TESU";
        case SensorType::TempInShadow:          return "TESH";
        case SensorType::Humidity:              return "HUMI";
        case SensorType::FlowRateDailySum:      return "FRDS";
        case SensorType::FlowRateLitPerMin:     return "FRLM";
        case SensorType::Rain:                  return "RAIN";
        case SensorType::Light:                 return "LIGH";
        case SensorType::WaterPressure:         return "WAPR";
        case SensorType::SoilMoistureLocal:     return "SMLC";
        case SensorType::SoilMoistureWireless:  return "SM"; // SMXX <- index
        case SensorType::Unknown:
        default:
            return "Unknown";
    }
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
    return "Unknown";
}

String ToShortString(WifiSignalStrength p_strength)
{
    switch(p_strength)
    {
        case WifiSignalStrength::Strength9of9: return "9";
        case WifiSignalStrength::Strength8of9: return "8";
        case WifiSignalStrength::Strength7of9: return "7";
        case WifiSignalStrength::Strength6of9: return "6";
        case WifiSignalStrength::Strength5of9: return "5";
        case WifiSignalStrength::Strength4of9: return "4";
        case WifiSignalStrength::Strength3of9: return "3";
        case WifiSignalStrength::Strength2of9: return "2";
        case WifiSignalStrength::Strength1of9: return "1";
        case WifiSignalStrength::Strength0of9: return "0";
    }
    return "U";
}


String ToShortString(RelayState p_state)
{
    switch(p_state)
    {
        case RelayState::Opened: return "O";
        case RelayState::Closed: return "C";
    }
    return "U";
}


RelayState ToRelayStateFromShortString(const String& p_str)
{
    if (p_str == "O") return RelayState::Opened;
    if (p_str == "C") return RelayState::Closed;
    Serial.printf("Failed to parse %s", p_str);
    return RelayState::Unknown;
}


RelayState ToRelayStateFromShortString(char p_char)
{
    if (p_char == 'O') return RelayState::Opened;
    if (p_char == 'C') return RelayState::Closed;
    Serial.printf("Failed to parse %c", p_char);
    return RelayState::Unknown;
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
        case 12 : return RelayIds::Relay13;
        case 13 : return RelayIds::Relay14;
        case 14 : return RelayIds::Relay15;
        case 15 : return RelayIds::Relay16;
        case 17 : return RelayIds::AllRelays;
    }
    return RelayIds::Unknown;
}

TimeUnit ToTimeUnit(char p_char)
{
    if (p_char == 'h' || p_char == 'H') return TimeUnit::Hour;
    if (p_char == 'm' || p_char == 'M') return TimeUnit::Minute;
    if (p_char == 's' || p_char == 'S') return TimeUnit::Seconds;
    return TimeUnit::Unknown;
}

uint16_t TimeUnitToSeconds(TimeUnit p_unit)
{
    switch(p_unit)
    {
        case TimeUnit::Hour:    return 3600;
        case TimeUnit::Minute:  return 60;
        case TimeUnit::Seconds: return 1;
    }
    return 0;
}

WifiSignalStrength ToWifiSignalStrength(int8_t p_rssi)
{
    if (p_rssi ==  0) return WifiSignalStrength::Unknown;
    if (p_rssi > -50) return WifiSignalStrength::Strength9of9;
    if (p_rssi > -55) return WifiSignalStrength::Strength8of9;
    if (p_rssi > -60) return WifiSignalStrength::Strength7of9;
    if (p_rssi > -65) return WifiSignalStrength::Strength6of9;
    if (p_rssi > -70) return WifiSignalStrength::Strength5of9;
    if (p_rssi > -75) return WifiSignalStrength::Strength4of9;
    if (p_rssi > -80) return WifiSignalStrength::Strength3of9;
    if (p_rssi > -85) return WifiSignalStrength::Strength2of9;
    if (p_rssi > -90) return WifiSignalStrength::Strength1of9;
    return WifiSignalStrength::Strength0of9;
}

RelayIds& operator++(RelayIds& c)
{
    if (c == RelayIds::Unknown)
    {
        return c;
    }
    c = static_cast<RelayIds>(static_cast<int>(c) + 1);
    return c;
}

CommandType& operator++(CommandType& c)
{
    if (c == CommandType::Unknown)
    {
        return c;
    }
    c = static_cast<CommandType>(static_cast<int>(c) + 1);
    return c;
}

RelayState& operator++(RelayState& c)
{
    if (c == RelayState::Unknown)
    {
        return c;
    }
    c = static_cast<RelayState>(static_cast<int>(c) + 1);
    return c;
}

CmdPriority& operator++(CmdPriority& c)
{
    if (c == CmdPriority::Unknown)
    {
        return c;
    }
    c = static_cast<CmdPriority>(static_cast<int>(c) + 1);
    return c;
}

// clang-format on
// $ATime;P05;RXX;O15:00->20:00#
void GetCommandBuilderJSON(String& json)
{
    json       = " {\"startChar\": \"$\", \"endChar\": \"#\",  \"CommandType\": [";
    bool first = true;
    for (CommandType i = CommandType::Manual; i < CommandType::Unknown; ++i)
    {
        if (!first)
        {
            json += ", ";
        }
        first = false;
        json += "\"" + ToString(i) + "\"";
    }
    json += "],";

    first = true;
    json += " \"CmdPriority\": [";
    for (CmdPriority i = CmdPriority::PriorityLowest; i < CmdPriority::Unknown; ++i)
    {
        if (!first)
        {
            json += ", ";
        }
        first = false;
        json += "\"" + ToString(i) + "\"";
    }
    json += "],";

    first = true;
    json += " \"RelayIds\": [";
    for (RelayIds i = RelayIds::Relay1; i <= RelayIds::AllRelays; ++i)
    {
        if (i == RelayIds::NumberOfRelays)
        {
            continue;
        }

        if (!first)
        {
            json += ", ";
        }
        first = false;
        json += "\"" + ToString(i) + "\"";
    }
    for (RelayGroups i = RelayGroups::A; i < RelayGroups::NumberOfGroups; incRelayGroup(i))
    {
        if (!first)
        {
            json += ", ";
        }
        first = false;
        json += "\"" + ToString(i) + "\"";
    }
    json += "]";

    json += "}";
}