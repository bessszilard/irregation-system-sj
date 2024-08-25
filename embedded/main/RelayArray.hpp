#include "Enums.hpp"

#define NUMBER_OF_RELAYS (static_cast<int>(RelayIds::NumberOfRelays))

class RelayArray
{
public:
    RelayArray();

    bool init(int pins[NUMBER_OF_RELAYS]);
    ~RelayArray(){};

    bool setState(RelayIds p_id, RelayState p_state);

    // TODOsz fix this
    // inline RelayState getState(RelayIds p_id) { return m_states; }

private:
    bool handleRelay(int p_id, RelayState p_state);

private:
    int m_pins[NUMBER_OF_RELAYS];
    RelayState m_states[NUMBER_OF_RELAYS];
};