#pragma once

#include "Enums.hpp"
#include "SolenoidCtrlCmd.hpp"
#include "RelayArray.hpp"

#define MAX_NUMBER_OF_CMDS (50)

class SolenoidManager
{
public:
    SolenoidManager();

    CommandState appendEvent(const String& p_eventStr);
    CommandState removeEvent(uint8_t p_id);
    String getEventString(uint8_t p_id) const;

    inline uint8_t getEventNumber() const
    {
        return m_currentCmdId;
    }

    // bool publishAllEvents();
    bool updateRelayStates();

    RelayState updateAndGetRelayState(RelayIds p_relayId);

    void setTemperature(int8_t p_temp);
    void setHumidity(int8_t p_temp);
    void setMoisture(uint8_t p_moisture, uint8_t p_id);
    RelayState updateRelayStatus(const SolenoidCtrlCmd& p_cmd);

    struct RelayRunning
    {
        CmdPriority priority;
        uint8_t cmdIdx;
        RelayState currentState;

        inline void set(uint8_t p_cmdIdx, CmdPriority p_priority, RelayState p_state)
        {
            priority     = p_priority;
            cmdIdx       = p_cmdIdx;
            currentState = p_state;
        }
    };

private:
    SolenoidCtrlCmd m_cmdList[MAX_NUMBER_OF_CMDS];
    RelayRunning m_relayCmdIndexes[NUMBER_OF_RELAYS];

    uint8_t m_maxEventId;
    uint8_t m_currentCmdId;
};