#pragma once

#include "Utils.hpp"

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
    Relay13,
    Relay14,
    Relay15,
    Relay16,
    NumberOfRelays,
    AllRelays,
    Unknown = 0xFF
};

#define NUMBER_OF_RELAYS (static_cast<int>(RelayIds::NumberOfRelays))

enum class RelayState
{
    Opened,
    Closed,
    Unknown
};

enum class CmdPriority
{
    Priority0,
    Priority1,
    Priority2,
    Priority3,
    Priority4,
    Priority5,
    Priority6,
    Priority7,
    Priority8,
    Priority9,
    PriorityLowest,
    Unknown
};

enum class CommandType
{
    ManCtrl,
    AutoTemperatureCtrl,
    AutoHumidityCtrl,
    AutoTimeCtrl,
    AutoFlowCtrl,
    Unknown
};

enum class SensorTypes
{
    Temperature,
    Humidity,
    SoilMoisture,
    BarometricPressure
};

enum class CommandState
{
    Added,
    Removed,
    AlreadyPresent,
    CantRemove,
    MemoryFull,
    Unknown
};

CommandType CommandTypeFromString(const String& typeStr, int startId = 0, int endId = -1);
RelayIds RelayIdTypeFromString(const String& typeStr, int startId = 0, int endId = -1);
RelayState RelayStateFromString(const String& typeStr, int startId = 0, int endId = -1);
CmdPriority CmdPriorityFromString(const String& typeStr, int startId = 0, int endId = -1);

String ToString(RelayIds id);
String ToString(RelayState type);
String ToString(CmdPriority type);
String ToString(CommandType type);
// String ToString(SensorTypes type);
// String ToString(CommandState type);

RelayIds ToRelayId(uint8_t p_id);
inline uint8_t RelayIdToUInt(RelayIds p_id)
{
    static_cast<uint8_t>(p_id);
}