#pragma once

#include "Enums.hpp"
#include "time.h"
#ifndef PC_BUILD
#include <RtcDS3231.h>
#endif

#define MAX_SOIL_MOISTURE_NODE (30)
#define TOLERANCE_SEC (10)

struct SoilMoisture
{
    uint8_t id               = 0;
    uint8_t measurement      = 0;
    uint64_t lastMeasurement = 0; // time stamp vs hour, minute, sec

    bool valid = false;
};

struct LocalTime : tm
{
    bool valid = false;
    LocalTime(){};
#ifndef PC_BUILD
    LocalTime(const RtcDateTime& dt)
    {
        tm_mon  = dt.Month();
        tm_mday = dt.Day();
        tm_year = dt.Year() - 2000;
        tm_hour = dt.Hour();
        tm_min  = dt.Minute();
        tm_sec  = dt.Second();
        valid   = true;
    }

    RtcDateTime toDt()
    {
        return RtcDateTime(tm_year, tm_mon, getDay(), tm_hour, tm_min, tm_sec);
    }
#endif
    inline uint8_t getDay() const
    {
        return tm_mday;
    }

    inline uint8_t getMonth() const
    {
        return tm_mon + 1;
    }

    inline uint16_t getYear() const
    {
        return tm_year + 1900;
    }

    void notEqPrint(int a, int b)
    {
        if (a != b)
            Serial.println(String(a) + "!=" + String(b));
    }

    bool eq(LocalTime& time)
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

    String toString() const
    {
        return String(getYear()) + "-" + String(getMonth()) + "-" + String(getDay()) + " " + String(tm_hour) + ":" +
               String(tm_min) + ":" + String(tm_sec);
    }
};

struct RelayArrayStates
{
    RelayState states[NUMBER_OF_RELAYS];
    const uint8_t relayCnt = NUMBER_OF_RELAYS;

    RelayArrayStates(RelayState p_state)
    {
        for (uint8_t relayId = 0; relayId < relayCnt; ++relayId)
        {
            states[relayId] = p_state;
        }
    }

    RelayState getState(RelayIds relayId)
    {
        uint8_t relayIdU8 = RelayIdToUInt(relayId);
        if (relayIdU8 >= NUMBER_OF_RELAYS)
        {
            Serial.print("Invalid relay Id");
            return RelayState::Unknown;
        }
        return states[relayIdU8];
    }

    void setState(RelayIds relayId, RelayState state)
    {
        uint8_t relayIdU8 = RelayIdToUInt(relayId);
        if (relayIdU8 >= NUMBER_OF_RELAYS)
        {
            Serial.print("Invalid relay Id");
            return;
        }
        states[relayIdU8] = state;
    }

    String toString() const
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
    bool valid = false;
};

struct SensorData
{
    float externalTemp_C  = NAN;
    float humidity_RH     = NAN;
    float pressure_Pa     = NAN;
    float flowRate_LitMin = NAN;
    int16_t rainSensor    = 0;
    int16_t lightSensor   = 0;
    int16_t soilMoisture1 = 0;
    int16_t soilMoisture2 = 0;
    SoilMoisture soilMoisture[MAX_SOIL_MOISTURE_NODE];

    bool valid = false;

    String toJSON() const
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
};

struct RelayExeInfo
{
    CmdPriority priority    = CmdPriority::Unknown;
    uint8_t cmdIdx          = 0;
    RelayState currentState = RelayState::Unknown;

    inline void set(uint8_t p_cmdIdx, CmdPriority p_priority, RelayState p_state)
    {
        priority     = p_priority;
        cmdIdx       = p_cmdIdx;
        currentState = p_state;
    }

    inline String toString() const
    {
        return ToString(priority) + ";" + String(cmdIdx) + ";" + ToString(currentState) + ";";
    }
};