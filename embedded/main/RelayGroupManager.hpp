#pragma once

#include "Enums.hpp"

class RelayGroupManager
{
public:
    RelayGroupManager();

    bool addRelay(RelayGroups p_group, RelayIds p_relay);
    bool removeRelay(RelayGroups p_group, RelayIds p_relay);

    bool isInGroup(RelayGroups p_group, RelayIds p_relay) const;
    String toJson() const;

private:
    uint16_t m_groups[NUMBER_OF_RELAY_GROUPS];
};