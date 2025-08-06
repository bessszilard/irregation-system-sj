#pragma once

#include "Enums.hpp"
#include "time.h"
#ifndef PC_BUILD
#include <RtcDS3231.h>
#endif

#define MAX_SOIL_MOISTURE_NODE (30)
#define TOLERANCE_SEC (10)

struct RelayTarget
{
    RelayTargetType type;
    RelayIds relayId;    // Valid if type == SingleRelay
    RelayGroups groupId; // Valid if type == Group

    String toString() const;

    static RelayTarget FromString(const String& p_rawMsg, int p_startId = 0, int p_endId = -1);

    bool operator==(const RelayTarget& p_other) const;
};

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

    bool eq(LocalTime& time);

    String toString() const;

    static LocalTime Build(uint8_t p_hours, uint8_t p_minutes, uint8_t p_seconds = 0)
    {
        LocalTime time;
        time.tm_hour = p_hours;
        time.tm_min  = p_minutes;
        time.tm_sec  = p_seconds;
        time.valid   = true;
        return time;
    }
};

struct RelayArrayStates
{
    RelayState states[NUMBER_OF_RELAYS];
    const uint8_t relayCnt = NUMBER_OF_RELAYS;
    bool valid             = false;

    RelayArrayStates(RelayState p_state);

    RelayState getState(RelayIds relayId);

    void setState(RelayIds relayId, RelayState state);

    String toString() const;
};

// TODOsz move to another file
struct SensorData
{
    float tempOnSun_C       = NAN;
    float tempInShadow_C    = NAN;
    float humidity_RH       = NAN;
    float flowDaySum_Lit    = NAN;
    float flowRate_LitMin   = NAN;
    float rainSensor        = NAN;
    float lightSensor       = NAN;
    float waterPressure_bar = NAN;
    float soilMoistureLocal = NAN;

    uint16_t rawRainSensor        = 0; // TODOsz create NAN also for these
    uint16_t rawLightSensor       = 0;
    uint16_t rawwaterPressure_bar = 0;
    uint16_t rawSoilMoisture1     = 0;

    SoilMoisture soilMoistureWl[MAX_SOIL_MOISTURE_NODE];

    bool valid = false;

    String toJSON() const;

    float get(SensorType p_type) const;
    float get(const String& p_str) const;

    void set(SensorType p_type, float p_value);
    void setFromADC(SensorType p_type, int16_t p_value);

    static float GetLightFromADC(int16_t p_adcValue);
    static float GetSoilMoistureFromADC(int16_t p_adcValue);
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