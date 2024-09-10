#include "RelayArray.hpp"
#include <Arduino.h>

RelayArray::RelayArray(int dataPin, int clockPin, int latchPin)
{
    m_shift.setBitCount(8);
    m_shift.setPins(dataPin, clockPin, latchPin);
}

bool RelayArray::handleRelay(int p_id, RelayState p_state)
{
    if (NUMBER_OF_RELAYS < p_id)
    {
        Serial.println("Invalid relay id " + String(p_id));
        return false;
    }
    switch (p_state)
    {
        case RelayState::Opened:
            m_shift.writeBit(p_id, HIGH);
            return true;
        case RelayState::Closed:
            m_shift.writeBit(p_id, LOW);
            return true;
    }
    return false;
}

bool RelayArray::setState(RelayIds p_relayId, RelayState p_state)
{
    switch (p_relayId)
    {
        case RelayIds::Relay1:
        case RelayIds::Relay2:
        case RelayIds::Relay3:
        case RelayIds::Relay4:
        case RelayIds::Relay5:
        case RelayIds::Relay6:
        case RelayIds::Relay7:
        case RelayIds::Relay8:
        case RelayIds::Relay9:
        case RelayIds::Relay10:
        case RelayIds::Relay11:
        case RelayIds::Relay12:
        {
            int id       = static_cast<int>(p_relayId);
            m_states[id] = p_state;
            return handleRelay(id, p_state);
        }
        case RelayIds::AllRelays:
        {
            for (int id = 0; id < NUMBER_OF_RELAYS; ++id)
            {
                handleRelay(id, p_state);
            }
            return true;
        }
        case RelayIds::NumberOfRelays:
            // TODOsz error
            return false;
        case RelayIds::Unknown:
            // TODOsz error
            return false;
    }
}