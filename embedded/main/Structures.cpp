#include "Structures.hpp"

#define LIGHT_MIN_ADC (0)
#define LIGHT_MAX_ADC (26000)
#define LIGHT_BIAS (1)

#define SOIL_MOISTURE_MIN_ADC (0)
#define SOIL_MOISTURE_MAX_ADC (14000)
#define SOIL_MOISTURE_BIAS (-5000)

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
    // TODOsz make a function for this
    // TODOsz getUnit from type
    String json = "{";
    json += "\"tempOnSun_C\": "            + String(isnan(tempOnSun_C)       ? "null" : String(tempOnSun_C,       2)) + ",";
    json += "\"tempInShadow_C\": "         + String(isnan(tempInShadow_C)    ? "null" : String(tempInShadow_C,    2)) + ",";
    json += "\"humidity_%RH\": "           + String(isnan(humidity_RH)       ? "null" : String(humidity_RH,       2)) + ",";
    json += "\"flowRate_LitMin\": "        + String(isnan(flowDaySum_Lit)    ? "null" : String(flowDaySum_Lit,    2)) + ",";
    json += "\"flowDaySum_Min\": "         + String(isnan(flowRate_LitMin)   ? "null" : String(flowRate_LitMin ,  2)) + ",";
    json += "\"rainSensor_0-99\": "        + String(isnan(rainSensor)        ? "null" : String(rainSensor,        2)) + ",";
    json += "\"light_0-99\": "             + String(isnan(lightSensor)       ? "null" : String(lightSensor,       2)) + ",";
    json += "\"waterPressure_bar\": "      + String(isnan(waterPressure_bar) ? "null" : String(waterPressure_bar, 2)) + ",";
    json += "\"soilMoistureLocal_0-99\": " + String(isnan(soilMoistureLocal) ? "null" : String(soilMoistureLocal, 2)) + ",";
    json += "\"soilMoisture\": [";
    for (int i = 0; i < MAX_SOIL_MOISTURE_NODE; ++i)
    {
        json += isnan(soilMoistureWl[i].measurement) ? "null" : String(soilMoistureWl[i].measurement, 2);
        if (i < MAX_SOIL_MOISTURE_NODE - 1)
            json += ",";
    }
    json += "],";
    json += "\"valid\": " + String(valid ? "true" : "false");
    json += "}";
    return json;
}

//---------------------------------------------------------------
void SensorData::set(const String& p_typeStr, float p_value)
//---------------------------------------------------------------
{
    set(ToSensorTypeFromString(p_typeStr), p_value);
}

//---------------------------------------------------------------
void SensorData::set(SensorType p_type, float p_value)
//---------------------------------------------------------------
{
    switch(p_type)
    {
        case SensorType::TempOnSun:           tempOnSun_C         = p_value; break;
        case SensorType::TempInShadow:        tempInShadow_C      = p_value; break;
        case SensorType::Humidity:            humidity_RH         = p_value; break;
        case SensorType::FlowRateDailySum:    flowDaySum_Lit      = p_value; break;
        case SensorType::FlowRateLitPerMin:   flowRate_LitMin     = p_value; break;
        case SensorType::Rain:                rainSensor          = p_value; break;
        case SensorType::Light:               lightSensor         = p_value; break;
        case SensorType::WaterPressure:       waterPressure_bar   = p_value; break;
        case SensorType::SoilMoistureLocal:   soilMoistureLocal   = p_value; break;
        case SensorType::SoilMoistureWireless:         
        case SensorType::Unknown:              
            break;
    }
}

//---------------------------------------------------------------
void SensorData::setFromADC(SensorType p_type, int16_t p_value)
//---------------------------------------------------------------
{
    switch(p_type)
    {
        // TODOsz implement scaling
        case SensorType::TempOnSun:           tempOnSun_C         = p_value; break;
        case SensorType::TempInShadow:        tempInShadow_C      = p_value; break;
        case SensorType::Humidity:            humidity_RH         = p_value; break;
        case SensorType::FlowRateDailySum:    flowDaySum_Lit      = p_value; break;
        case SensorType::FlowRateLitPerMin:   flowRate_LitMin     = p_value; break;
        case SensorType::Rain:                rainSensor          = GetSoilMoistureFromADC(p_value); break;
        case SensorType::Light:               lightSensor         = GetLightFromADC(p_value); break;
        case SensorType::WaterPressure:       waterPressure_bar   = GetSoilMoistureFromADC(p_value); break;
        case SensorType::SoilMoistureLocal:   soilMoistureLocal   = GetSoilMoistureFromADC(p_value); break;
        case SensorType::SoilMoistureWireless:         
        case SensorType::Unknown:              
    default:
        Serial.printf("uint for %s not supported", ToString(p_type).c_str());
    }
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
        case SensorType::SoilMoistureLocal:    return soilMoistureLocal;         
        case SensorType::Unknown:              
    default:
        return NAN;
    }
}

//---------------------------------------------------------------
float SensorData::get(const String& p_str) const
//---------------------------------------------------------------
{
    return get(ToSensorTypeFromString(p_str));
}

//---------------------------------------------------------------
float SensorData::GetLightFromADC(int16_t p_adcValue)
//---------------------------------------------------------------
{
    if (p_adcValue == 0xFFFF)
    {
      return 0.0f;
    }

    if (p_adcValue < LIGHT_MIN_ADC || LIGHT_MAX_ADC < p_adcValue)
    {
       return NAN;
    }

    // scale + bias
    return (uint32_t)(p_adcValue + LIGHT_BIAS) * 99 / (LIGHT_MAX_ADC - LIGHT_MIN_ADC);
}

//---------------------------------------------------------------
float SensorData::GetSoilMoistureFromADC(int16_t p_adcValue)
//---------------------------------------------------------------
{
    if (p_adcValue < SOIL_MOISTURE_MIN_ADC || SOIL_MOISTURE_MAX_ADC < p_adcValue)
    {
       return NAN;
    }

    // scale + bias
    return 99 - (uint32_t)(p_adcValue + SOIL_MOISTURE_BIAS) * 99 /
                    (SOIL_MOISTURE_MAX_ADC - SOIL_MOISTURE_MIN_ADC + SOIL_MOISTURE_BIAS);
}

// clang-format on
