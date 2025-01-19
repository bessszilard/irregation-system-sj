#pragma once

#include "WiFiType.h"
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
    AutoMoistureCtrl,
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

enum class SignalStrength
{
    Strength9of9,
    Strength8of9,
    Strength7of9,
    Strength6of9,
    Strength5of9,
    Strength4of9,
    Strength3of9,
    Strength2of9,
    Strength1of9,
    Strength0of9,
    Unknown
};

CommandType CommandTypeFromString(const String& p_typeStr, int p_startId = 0, int p_endId = -1);
RelayIds RelayIdTypeFromString(const String& p_typeStr, int p_startId = 0, int p_endId = -1);
RelayState RelayStateFromString(const String& p_typeStr, int p_startId = 0, int p_endId = -1);
CmdPriority CmdPriorityFromString(const String& p_typeStr, int p_startId = 0, int p_endId = -1);

String ToString(RelayIds p_id);
String ToString(RelayState p_type);
String ToString(CmdPriority p_type);
String ToString(CommandType p_type);

// For LCD display
String ToShortString(wl_status_t p_status);
String ToShortString(SignalStrength p_strength);
String ToShortString(RelayState p_state);
// String ToString(SensorTypes type);
// String ToString(CommandState type);

SignalStrength ToSignalStrength(int8_t p_rssi);

RelayIds ToRelayId(uint8_t p_id);
inline uint8_t RelayIdToUInt(RelayIds p_id)
{
    static_cast<uint8_t>(p_id);
}