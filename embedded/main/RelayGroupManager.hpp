#pragma once

#include "Enums.hpp"

class RelayGroupManager
{
public:
    RelayGroupManager();

    void clear();

    bool loadFromStr(const String& p_str);
    bool addRelay(RelayGroups p_group, RelayIds p_relay);

    bool isInGroup(RelayGroups p_group, RelayIds p_relay) const;
    String toJson() const;

    inline uint8_t getGroupNumber() const
    {
        return NUMBER_OF_RELAY_GROUPS;
    }

    void getFRAMArray(uint16_t (&groupsArray)[NUMBER_OF_RELAY_GROUPS]) const;
    void loadfromFRAMArray(uint16_t (&groupsArray)[NUMBER_OF_RELAY_GROUPS]);

protected:
    bool static groupAndIdFromStirng(const String& p_cmd, RelayGroups& p_group, RelayIds& p_relayId);

    bool addRelay(const String& p_cmd);

private:
    uint16_t m_groups[NUMBER_OF_RELAY_GROUPS];
};