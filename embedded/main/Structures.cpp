#include "Structures.hpp"

//---------------------------------------------------------------
String RelayTarget::toString() const
//---------------------------------------------------------------
{
    if (type == RelayTargetType::All || type == RelayTargetType::SingleRelay)
    {
        return ToString(relayId);
    }
    else if (type == RelayTargetType::Group)
    {
        return ToString(groupId);
    }
    return "";
}

//---------------------------------------------------------------
RelayTarget RelayTarget::FromString(const String& p_rawMsg, int p_startId /*=0*/, int p_endId /*=-1*/)
//---------------------------------------------------------------
{
    String str = Utils::GetSubStr(p_rawMsg, p_startId, p_endId);
    if (str == "RXX")
        return {RelayTargetType::All, RelayIds::AllRelays, RelayGroups::Unknown};

    if (str.length() == 3 && str.startsWith("RG"))
    {
        return {RelayTargetType::Group, RelayIds::Unknown, RelayGroupsFromString(str)};
    }

    if (str.startsWith("R"))
    {
        return {RelayTargetType::SingleRelay, RelayIdTypeFromString(str), RelayGroups::Unknown};
    }
    return {RelayTargetType::Unknown, RelayIds::Unknown, RelayGroups::NumberOfGroups};
}

//---------------------------------------------------------------
bool RelayTarget::operator==(const RelayTarget& p_other) const
//---------------------------------------------------------------
{
    return type == p_other.type && relayId == p_other.relayId && groupId == p_other.groupId;
}

//---------------------------------------------------------------
bool LocalTime::eq(LocalTime& time)
//---------------------------------------------------------------
{
    bool result = tm_min == time.tm_min && tm_hour == time.tm_hour && getDay() == time.getDay() &&
                  getMonth() == time.getMonth() && getYear() == time.getYear() &&
                  (abs(tm_sec - time.tm_sec) < TOLERANCE_SEC);
    if (result)
        return true;
    notEqPrint(tm_sec, time.tm_sec);
    notEqPrint(tm_min, time.tm_min);
    notEqPrint(tm_hour, time.tm_hour);
    notEqPrint(getDay(), time.getDay());
    notEqPrint(getMonth(), time.getMonth());
    notEqPrint(getYear(), time.getYear());
    return false;
}

//---------------------------------------------------------------
String LocalTime::toString() const
//---------------------------------------------------------------
{
    return String(getYear()) + "-" + String(getMonth()) + "-" + String(getDay()) + " " + String(tm_hour) + ":" +
           String(tm_min) + ":" + String(tm_sec);
}

//---------------------------------------------------------------
RelayArrayStates::RelayArrayStates(RelayState p_state)
//---------------------------------------------------------------
{
    for (uint8_t relayId = 0; relayId < relayCnt; ++relayId)
    {
        states[relayId] = p_state;
    }
}

//---------------------------------------------------------------
RelayState RelayArrayStates::getState(RelayIds relayId)
//---------------------------------------------------------------
{
    uint8_t relayIdU8 = RelayIdToUInt(relayId);
    if (relayIdU8 >= NUMBER_OF_RELAYS)
    {
        Serial.print("Invalid relay Id");
        return RelayState::Unknown;
    }
    return states[relayIdU8];
}

//---------------------------------------------------------------
void RelayArrayStates::setState(RelayIds relayId, RelayState state)
//---------------------------------------------------------------
{
    uint8_t relayIdU8 = RelayIdToUInt(relayId);
    if (relayIdU8 >= NUMBER_OF_RELAYS)
    {
        Serial.print("Invalid relay Id");
        return;
    }
    states[relayIdU8] = state;
}

//---------------------------------------------------------------
String RelayArrayStates::toString() const
//---------------------------------------------------------------
{
    uint8_t spaceAfterId = 16;
    String str;
    if (NUMBER_OF_RELAYS < 16)
    {
        spaceAfterId = 5;
        str += "R:";
    }
    for (uint8_t relayId = 0; relayId < NUMBER_OF_RELAYS; ++relayId)
    {
        str += ToShortString(states[relayId]);
        if (relayId != 0 && (relayId % spaceAfterId == 0))
        {
            str += " ";
        }
    }
    return str;
}

// clang-format off
//---------------------------------------------------------------
String SensorData::toJSON() const
//---------------------------------------------------------------
{
    String json = "{";
    json += "\"tempOnSun_C\": "        + String(isnan(tempOnSun_C)      ? "null" : String(tempOnSun_C,      2)) + ",";
    json += "\"tempInShadow_C\": "     + String(isnan(tempInShadow_C)   ? "null" : String(tempInShadow_C,   2)) + ",";
    json += "\"humidity_%RH\": "       + String(isnan(humidity_RH)      ? "null" : String(humidity_RH,      2)) + ",";
    json += "\"flowRate_LitMin\": "    + String(isnan(flowDaySum_Lit)   ? "null" : String(flowDaySum_Lit,   2)) + ",";
    json += "\"flowDaySum_Min\": "     + String(isnan(flowRate_LitMin)  ? "null" : String(flowRate_LitMin ,  2)) + ",";
    json += "\"rainSensor_0-99\": "    + String(rainSensor) + ",";
    json += "\"light_0-99\": "         + String(lightSensor) + ",";
    json += "\"waterPressure_bar\": "  + String(isnan(waterPressure_bar) ? "null" : String(waterPressure_bar,  2)) + ",";
    json += "\"soilMoisture2_0-99\": " + String(soilMoisture1) + ",";
    json += "\"soilMoisture\": [";
    for (int i = 0; i < MAX_SOIL_MOISTURE_NODE; ++i)
    {
        json += isnan(soilMoisture[i].measurement) ? "null" : String(soilMoisture[i].measurement, 2);
        if (i < MAX_SOIL_MOISTURE_NODE - 1)
            json += ",";
    }
    json += "],";
    json += "\"valid\": " + String(valid ? "true" : "false");
    json += "}";
    return json;
}

//---------------------------------------------------------------
float SensorData::get(SensorType p_type) const
//---------------------------------------------------------------
{
    switch(p_type)
    {
        case SensorType::TempOnSun:            return tempOnSun_C;
        case SensorType::TempInShadow:         return tempInShadow_C;
        case SensorType::Humidity:             return humidity_RH;
        case SensorType::FlowRateDailySum:     return flowDaySum_Lit;
        case SensorType::FlowRateLitPerMin:    return flowRate_LitMin;
        case SensorType::Rain:                 return rainSensor;
        case SensorType::Light:                return lightSensor;
        case SensorType::WaterPressure:        return waterPressure_bar;
        case SensorType::SoilMoisture:         
        case SensorType::Unknown:              
    default:
        return NAN;
    }
}
// clang-format on

//---------------------------------------------------------------
float SensorData::get(const String& p_str) const
//---------------------------------------------------------------
{
    return get(ToSensorTypeFromString(p_str));
}
