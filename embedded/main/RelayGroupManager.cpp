#include "RelayGroupManager.hpp"

// RGA:R01;
static const uint8_t RelayGroupStrLen = 3;
static const uint8_t RelayIdStrLen    = 3;
static const uint8_t SliceLength       = RelayGroupStrLen + RelayIdStrLen + 2;
static const uint8_t SeparatorCharPos  = RelayGroupStrLen;
static const uint8_t TerminatorCharPos = SliceLength - 1;
static const char SeparatorChar        = ':';
static const char TerminatorChar       = ';';

RelayGroupManager::RelayGroupManager()
{
    // reset all values
    clear();
}

void RelayGroupManager::clear()
{
    for (int i = 0; i < NUMBER_OF_RELAY_GROUPS; ++i)
    {
        m_groups[i] = 0;
    }
}

bool RelayGroupManager::loadFromStr(const String& p_str)
{
    clear();

    // reset only
    if (p_str.length() == 0)
    {
        return true;
    }

    uint16_t id = 0;
    while ((id + SliceLength) <= p_str.length())
    {
        String groupAndId = Utils::GetSubStr(p_str, id, id + SliceLength);
        if (false == addRelay(groupAndId))
        {
            Serial.printf("Failed to add %s\n", groupAndId.c_str());
            return false;
        }
        id += SliceLength;
    }
    return true;
}

bool RelayGroupManager::addRelay(RelayGroups p_group, RelayIds p_relay)
{
    uint8_t groupArrayPos = RelayGroupToArrayPos(p_group);
    if (NUMBER_OF_RELAY_GROUPS <= groupArrayPos)
    {
        return false;
    }
    // Remove from all groups
    for (RelayGroups group = RelayGroups::A; group < RelayGroups::NumberOfGroups; group = incRelayGroup(group))
    {
        m_groups[RelayGroupToArrayPos(group)] &= (~(1 << RelayIdToUInt(p_relay)));
    }
    m_groups[groupArrayPos] |= (1 << RelayIdToUInt(p_relay));
    return true;
}

bool RelayGroupManager::groupAndIdFromStirng(const String& p_cmd, RelayGroups& p_group, RelayIds& p_relayId)
{
    if (p_cmd.length() != SliceLength || p_cmd[SeparatorCharPos] != SeparatorChar ||
        p_cmd[TerminatorCharPos] != TerminatorChar)
    {
        Serial.printf("Invalid group and Id string: %s\n", p_cmd.c_str());
        return false;
    }

    int idx = 0;
    p_group = RelayGroupsFromString(p_cmd, idx, idx + RelayGroupStrLen);
    idx += RelayGroupStrLen + 1;
    p_relayId = RelayIdTypeFromString(p_cmd, idx, idx + RelayIdStrLen);
    return true;
}

bool RelayGroupManager::addRelay(const String& p_cmd)
{
    RelayGroups group;
    RelayIds relayId;
    if (false == groupAndIdFromStirng(p_cmd, group, relayId))
    {
        return false;
    }
    return addRelay(group, relayId);
}

// RGA;R01
bool RelayGroupManager::isInGroup(RelayGroups p_group, RelayIds p_relay) const
{
    return (m_groups[RelayGroupToArrayPos(p_group)] & (1 << RelayIdToUInt(p_relay))) != 0;
}

// TODOsz arduino JSON
String RelayGroupManager::toJson() const
{
    String result = "{ \"RelayGroups\": [";
    for (RelayGroups group = RelayGroups::A; group < RelayGroups::NumberOfGroups; group = incRelayGroup(group))
    {
        result += "\"" + ToString(group) + "\"";
        if (false == isLastElement(group))
        {
            result += ",";
        }
    }
    result += "], ";
    result += "\"Relays\": {";

    for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        String groupStr = "";
        for (RelayGroups group = RelayGroups::A; group < RelayGroups::NumberOfGroups; group = incRelayGroup(group))
        {
            if (isInGroup(group, relayId))
            {
                groupStr = ToString(group);
                break;
            }
        }

        // TODOsz optimize -> only show what needed
        result += "\"" + ToString(relayId) + "\":" + "\"" + groupStr + "\"";
        if (false == isLastElement(relayId))
        {
            result += ",";
        }
    }

    result += "}}";
    return result;
}

void RelayGroupManager::getFRAMArray(uint16_t (&groupsArray)[NUMBER_OF_RELAY_GROUPS]) const
{
    for (uint8_t i = 0; i < NUMBER_OF_RELAY_GROUPS; i++)
    {
        groupsArray[i] = m_groups[i];
    }
}

void RelayGroupManager::loadfromFRAMArray(uint16_t (&groupsArray)[NUMBER_OF_RELAY_GROUPS])
{
    for (uint8_t i = 0; i < NUMBER_OF_RELAY_GROUPS; i++)
    {
        m_groups[i] = groupsArray[i];
    }
}
