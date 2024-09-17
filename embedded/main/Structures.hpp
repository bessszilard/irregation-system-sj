#pragma once

#include "Enums.hpp"

#define MAX_SOIL_MOISTURE_NODE (30)

struct SoilMoisture
{
    uint8_t id               = 0;
    uint8_t measurement      = 0;
    uint64_t lastMeasurement = 0; // time stamp vs hour, minute, sec

    bool valid = false;
};

struct LocalTime
{
    uint8_t sec    = 0;
    uint8_t minute = 0;
    uint8_t hour   = 0;
    uint8_t day    = 0;
    uint8_t month  = 0;
    uint16_t year  = 0;

    bool valid = false;
    bool operator==(LocalTime& time)
    {
        return sec == time.sec && minute == time.minute && hour == time.hour && day == time.day &&
               month == time.month && year == time.year;
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