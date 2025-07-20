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

//---------------------------------------------------------------
String SensorData::toJSON() const
//---------------------------------------------------------------
{
    String json = "{";
    json += "\"externalTemp_C\": " + String(isnan(externalTemp_C) ? "null" : String(externalTemp_C, 2)) + ",";
    json += "\"humidity_%RH\": " + String(isnan(humidity_RH) ? "null" : String(humidity_RH, 2)) + ",";
    json += "\"flowRate_LitMin\": " + String(isnan(flowRate_LitMin) ? "null" : String(flowRate_LitMin, 2)) + ",";
    json += "\"pressure_Pa\": " + String(isnan(pressure_Pa) ? "null" : String(pressure_Pa, 2)) + ",";
    json += "\"rainSensor_0-99\": " + String(rainSensor) + ",";
    json += "\"light_0-99\": " + String(lightSensor) + ",";
    json += "\"soilMoisture1_0-99\": " + String(soilMoisture1) + ",";
    json += "\"soilMoisture2_0-99\": " + String(soilMoisture2) + ",";
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