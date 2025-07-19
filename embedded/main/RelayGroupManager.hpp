#pragma once

#include "Enums.hpp"

class RelayGroupManager
{
public:
    RelayGroupManager();

    bool loadFromStr(const String& p_str);
    bool addRelay(RelayGroups p_group, RelayIds p_relay);
    bool removeRelay(RelayGroups p_group, RelayIds p_relay);

    bool isInGroup(RelayGroups p_group, RelayIds p_relay) const;
    String toJson() const;

protected:
    void static groupAndIdFromStirng(const String& p_cmd, RelayGroups& p_group, RelayIds& p_relayId);

    bool addRelay(const String& p_cmd);
    bool removeRelay(const String& p_cmd);

private:
    uint16_t m_groups[NUMBER_OF_RELAY_GROUPS];
};