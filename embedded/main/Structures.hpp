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

    static LocalTime Build(uint8_t p_hours, uint8_t p_minutes)
    {
        LocalTime time;
        time.tm_hour = p_hours;
        time.tm_min  = p_minutes;
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

    String toJSON() const;
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