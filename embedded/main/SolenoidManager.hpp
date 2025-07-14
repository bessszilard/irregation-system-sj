#pragma once

#include "Enums.hpp"
#include "SolenoidCtrlCmd.hpp"
#include "RelayArray.hpp"
#include "RelayGroupManager.hpp"

#define MAX_NUMBER_OF_CMDS (50)

class SolenoidManager
{
public:
    SolenoidManager();

    CommandState appendCmd(const String& p_cmdStr);
    CommandState removeCmd(const String& p_cmdStr);
    CommandState overrideCmd(const String& p_cmdStr);
    CommandState removeCmd(uint8_t p_id);

    String getCmdListInJson() const;
    String getCmdListStr() const;
    bool loadCmdsFromString(const String& p_cmds);

    void getRelayStatesWithCmdIdsJson(String& jsonResult) const;

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

    RelayGroupManager& relayGroups()
    {
        return m_relayGroups;
    }

    const RelayGroupManager& relayGroups() const
    {
        return m_relayGroups;
    }

protected:
    // TODOsz remove this since it's inside the commands
    bool isCommandActive(const SolenoidCtrlCmd& p_cmd)
    {
        return false;
    };

private:
    SolenoidCtrlCmd m_cmdList[MAX_NUMBER_OF_CMDS];
    RelayExeInfo m_relayCmdIndexes[NUMBER_OF_RELAYS];
    SensorData m_sensorData;
    LocalTime m_localTime;
    RelayGroupManager m_relayGroups;

    uint8_t m_maxCmdId;
    uint8_t m_currentCmdId;
};