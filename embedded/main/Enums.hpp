#pragma once

#ifdef PC_BUILD
#include "../mock/EnumsAndConstants.h"
#include <cstring>
#else
#include "WiFiType.h"
#endif

#include "Utils.hpp"

enum class RelayTargetType
{
    SingleRelay,
    Group,
    All,
    Unknown
};

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
    Unknown
};

#define NUMBER_OF_RELAYS (static_cast<int>(RelayIds::NumberOfRelays))

// Define operator++
inline RelayIds incRelayId(RelayIds& relayId)
{
    relayId = static_cast<RelayIds>(static_cast<int>(relayId) + 1);
    return relayId;
}

inline bool isLastElement(RelayIds relayId)
{
    return relayId == RelayIds::Relay16;
}

enum class RelayGroups
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    NumberOfGroups,
    Unknown
};

#define NUMBER_OF_RELAY_GROUPS (static_cast<int>(RelayGroups::NumberOfGroups))

inline RelayGroups incRelayGroup(RelayGroups& p_group)
{
    // TODOsz simplify
    p_group = static_cast<RelayGroups>(static_cast<int>(p_group) + 1);
    return p_group;
}

inline bool isLastElement(RelayGroups p_group)
{
    return p_group == RelayGroups::H;
}

enum class RelayState
{
    Opened,
    Closed,
    Unknown
};

enum class CmdPriority
{
    PriorityLowest,
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
    PriorityToggleAll,
    PriorityToggleOne,
    PriorityHighest,
    Unknown
};

// TODOsz 3 cases: MA, AT, AS
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
    Overriden,
    AlreadyPresent,
    CantRemove,
    MemoryFull,
    Unknown
};

enum class WifiSignalStrength
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
RelayGroups RelayGroupsFromString(const String& p_rawMsg, int p_startId = 0, int p_endId = -1);
RelayState RelayStateFromString(const String& p_typeStr, int p_startId = 0, int p_endId = -1);
CmdPriority CmdPriorityFromString(const String& p_typeStr, int p_startId = 0, int p_endId = -1);

String ToString(RelayTargetType p_type);
String ToString(RelayIds p_id);
String ToString(RelayGroups p_group);
String ToString(RelayState p_type);
String ToString(CmdPriority p_type);
String ToString(CommandType p_type);

// For LCD display
String ToShortString(wl_status_t p_status);
String ToShortString(WifiSignalStrength p_strength);
String ToShortString(RelayState p_state);
// String ToString(SensorTypes type);
String ToString(CommandState p_state);

RelayState ToRelayStateFromShortString(const String& p_str);

WifiSignalStrength ToWifiSignalStrength(int8_t p_rssi);

RelayIds ToRelayId(uint8_t p_id);
inline uint8_t RelayIdToUInt(RelayIds p_id)
{
    return static_cast<uint8_t>(p_id);
}

void GetCommandBuilderJSON(String& json);

RelayIds& operator++(RelayIds& c);
CommandType& operator++(CommandType& c);
RelayState& operator++(RelayState& c);
CmdPriority& operator++(CmdPriority& c);

// Define operator++
inline uint8_t RelayGroupToArrayPos(RelayGroups p_group)
{
    return static_cast<int>(p_group);
}
