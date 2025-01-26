#pragma once

#include "Enums.hpp"
#include "SolenoidCtrlCmd.hpp"
#include "RelayArray.hpp"

#define MAX_NUMBER_OF_CMDS (50)

class SolenoidManager
{
public:
    SolenoidManager();

    CommandState appendCmd(const String& p_cmdStr);
    CommandState removeCmd(const String& p_cmdStr);
    CommandState removeCmd(uint8_t p_id);

    String getCmdString(uint8_t p_id) const;

    inline uint8_t getCmdNumber() const
    {
        return m_currentCmdId;
    }

    // bool publishAllCmds();
    RelayState updateAndGetRelayState(RelayIds p_relayId);

    void updateRelayStates(const SensorData& p_sensorData);

    RelayState applyCmd(const SolenoidCtrlCmd& p_cmd);

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

protected:
    // TODOsz implement
    bool isCommandActive(const SolenoidCtrlCmd& p_cmd)
    {
        return false;
    };

private:
    SolenoidCtrlCmd m_cmdList[MAX_NUMBER_OF_CMDS];
    RelayRunning m_relayCmdIndexes[NUMBER_OF_RELAYS];
    SensorData m_sensorData;

    uint8_t m_maxCmdId;
    uint8_t m_currentCmdId;
};