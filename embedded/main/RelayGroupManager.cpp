#include "RelayGroupManager.hpp"

RelayGroupManager::RelayGroupManager()
{
    // reset all values
    for (int i = 0; i < NUMBER_OF_RELAY_GROUPS; ++i)
    {
        m_groups[i] = 0;
    }
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

bool RelayGroupManager::removeRelay(RelayGroups p_group, RelayIds p_relay)
{
    uint8_t groupArrayPos = RelayGroupToArrayPos(p_group);
    if (NUMBER_OF_RELAY_GROUPS <= groupArrayPos)
    {
        return false;
    }
    m_groups[groupArrayPos] &= (~(1 << RelayIdToUInt(p_relay)));
    return true;
}

bool RelayGroupManager::isInGroup(RelayGroups p_group, RelayIds p_relay) const
{
    return (m_groups[RelayGroupToArrayPos(p_group)] & (1 << RelayIdToUInt(p_relay))) != 0;
}

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

        result += "\"" + ToString(relayId) + "\":" + "\"" + groupStr + "\"";
        if (false == isLastElement(relayId))
        {
            result += ",";
        }
    }

    result += "}}";
    return result;
}