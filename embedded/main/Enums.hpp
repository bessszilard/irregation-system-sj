#pragma once

enum class RelayIds
{
    Relay1,
    Relay2,
    Relay3,
    Relay4,
    Relay5,
    Relay6,
    Relay7,
    Relay8,
    Relay9,
    Relay10,
    Relay11,
    Relay12,
    NumberOfRelays,
    RelayAll,
    Unknown
};

enum class RelayState
{
    Open,
    Closed,
    Unknown
};

enum class CommandType
{
    ManCtrl,
    TempBasedCtrl,
    HumBasedCtrl,
    TimeBasedCtrl,
    FlowBasedCtrl,
    Unknown
};

enum class SensorTypes
{
    Temperature,
    Humidity,
    SoilMoisture,
    BarometricPressure
}

enum class CommandState
{
    Added,
    Removed,
    AlreadyPresent,
    CantRemove,
    MemoryFull,
    Unknown
};