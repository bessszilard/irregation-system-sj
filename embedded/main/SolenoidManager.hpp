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

    String getCmdListInJson() const;

    String getCmdString(uint8_t p_id) const;

    inline uint8_t getCmdNumber() const
    {
        return m_currentCmdId;
    }

    // bool publishAllCmds();
    RelayState updateAndGetRelayState(RelayIds p_relayId);

    void updateRelayStates(); // const SensorData& p_sensorData);

    RelayState applyCmd(const SolenoidCtrlCmd& p_cmd);

    void getRelayState(RelayIds id, RelayExeInfo& relayState)
    {
        // TODOsz striction
        relayState = m_relayCmdIndexes[RelayIdToUInt(id)];
    }

protected:
    // TODOsz implement
    bool isCommandActive(const SolenoidCtrlCmd& p_cmd)
    {
        return false;
    };

private:
    SolenoidCtrlCmd m_cmdList[MAX_NUMBER_OF_CMDS];
    RelayExeInfo m_relayCmdIndexes[NUMBER_OF_RELAYS];
    SensorData m_sensorData;

    uint8_t m_maxCmdId;
    uint8_t m_currentCmdId;
};