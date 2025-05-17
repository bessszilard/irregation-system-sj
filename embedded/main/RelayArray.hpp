#pragma once

#include "Enums.hpp"
#include "Structures.hpp"

#ifdef PC_BUILD
#include "../mock/Shifty.h"
#else
#include <Shifty.h>
#endif

class RelayArray
{
public:
    RelayArray(int dataPin, int clockPin, int latchPin);

    ~RelayArray(){};

    bool setState(RelayIds p_id, RelayState p_state);

    void update(const RelayArrayStates& p_states);

    inline RelayState getState(RelayIds p_id)
    {
        if (p_id == RelayIds::AllRelays || p_id == RelayIds::NumberOfRelays)
            return RelayState::Unknown;
        return m_states[RelayIdToUInt(p_id)];
    }

    void knTestIncr();

private:
    bool handleAllRelays(RelayState p_state);
    bool handleRelay(int p_id, RelayState p_state);

private:
    RelayState m_states[NUMBER_OF_RELAYS];
    Shifty m_shift;
};