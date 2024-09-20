#pragma once

#include "Enums.hpp"
#include "time.h"

#define MAX_SOIL_MOISTURE_NODE (30)

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
        bool result = tm_sec == time.tm_sec && tm_min == time.tm_min && tm_hour == time.tm_hour &&
                      getDay() == time.getDay() && getMonth() == time.getMonth() && getYear() == time.getYear();
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

struct SolenoidStates
{
    RelayState solenoid[NUMBER_OF_RELAYS];
    const uint8_t solenoidCnt = NUMBER_OF_RELAYS;

    bool valid = false;
};

struct SensorsData
{
    float externalTemp_C = NAN;
    float humidity       = NAN;
    float pressure_Pa    = NAN;
    uint8_t rainSensor   = 0;
    SoilMoisture soilMoisture[MAX_SOIL_MOISTURE_NODE];

    bool valid = false;
};