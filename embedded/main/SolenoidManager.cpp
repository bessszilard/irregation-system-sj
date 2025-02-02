#include "SolenoidManager.hpp"

SolenoidManager::SolenoidManager() : m_currentCmdId(0)
{
    for (uint8_t id = 0; id < NUMBER_OF_RELAYS; id++)
    {
        m_relayCmdIndexes[id].set(0, CmdPriority::PriorityLowest, RelayState::Unknown);
    }
}

CommandState SolenoidManager::appendCmd(const String& p_cmdStr)
{
    SolenoidCtrlCmd cmdToAdd(p_cmdStr);
    if (false == cmdToAdd.valid)
    {
        Serial.println(p_cmdStr + " is unknown");
        return CommandState::Unknown;
    }
    for (uint8_t i = 0; i < m_currentCmdId; ++i)
    {
        if (m_cmdList[i] == cmdToAdd)
        {
            return CommandState::AlreadyPresent;
        }
    }
    if (MAX_NUMBER_OF_CMDS <= m_currentCmdId)
    {
        return CommandState::MemoryFull;
    }

    m_cmdList[m_currentCmdId] = cmdToAdd;
    m_currentCmdId++;
    return CommandState::Added;
}

CommandState SolenoidManager::removeCmd(const String& p_cmdStr)
{
    SolenoidCtrlCmd cmdToRemove(p_cmdStr);
    for (uint8_t i = 0; i < m_currentCmdId; ++i)
    {
        if (m_cmdList[i] == cmdToRemove)
        {
            return removeCmd(i);
        }
    }
    return CommandState::CantRemove;
}

// add remove based on string
// invalidate current command, and update relayStates
CommandState SolenoidManager::removeCmd(uint8_t p_id)
{
    if (m_currentCmdId < p_id)
    {
        return CommandState::CantRemove;
    }

    bool removedCmdControlledARelay = false;
    // Check if this command controlled one of the relays
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        // relay controlled by this command
        if (m_relayCmdIndexes[relayIdu8].cmdIdx == p_id)
        {
            m_relayCmdIndexes[relayIdu8].priority     = CmdPriority::PriorityLowest;
            m_relayCmdIndexes[relayIdu8].currentState = RelayState::Unknown;
            removedCmdControlledARelay                = true;
        }
    }

    // >>> invalidate in this case
    for (uint8_t i = p_id; i < m_currentCmdId - 1; i++)
    {
        m_cmdList[i] = m_cmdList[i + 1];
    }
    m_currentCmdId--;

    if (removedCmdControlledARelay)
    {
        updateRelayStates();
    }

    return CommandState::Removed;
}

String SolenoidManager::getCmdListInJson() const
{
    // Calculate the approximate buffer size needed.
    size_t estimatedSize = 16; // Initial size for "{ cmdList: [ ] }" and other fixed parts
    for (uint8_t i = 0; i < m_currentCmdId; i++)
    {
        estimatedSize += m_cmdList[i].toString().length() + 4; // Quotes and commas
    }

    // Allocate a buffer on the heap
    char* buffer = (char*)malloc(estimatedSize);
    if (!buffer)
    {
        return "{}"; // Return empty JSON if memory allocation fails
    }

    // Start building the JSON
    strcpy(buffer, "{ cmdList: [");
    for (uint8_t i = 0; i < m_currentCmdId; i++)
    {
        strcat(buffer, "\"");
        strcat(buffer, m_cmdList[i].toString().c_str());
        strcat(buffer, "\"");
        if (i < m_currentCmdId - 1)
        {
            strcat(buffer, ", ");
        }
    }
    strcat(buffer, "] }");
    // Create a String object from the buffer and free the buffer
    String result(buffer);
    free(buffer);

    return result;
}

String SolenoidManager::getCmdString(uint8_t p_id) const
{
    if (m_currentCmdId < p_id)
    {
        return "";
    }
    return m_cmdList[p_id].toString();
}

RelayState SolenoidManager::applyCmd(const SolenoidCtrlCmd& p_cmd)
{
    switch (p_cmd.cmdType)
    {
        case CommandType::ManCtrl:
            return p_cmd.relayState;
            // case CommandType::AutoTemperatureCtrl:
            //     return p_cmd.relayState;
            // case CommandType::AutoHumidityCtrl:
            //     return p_cmd.relayState;
            // case CommandType::AutoTimeCtrl:
            //     return p_cmd.relayState;
            // case CommandType::AutoFlowCtrl:
            //     return p_cmd.relayState;
    }
    return RelayState::Unknown;
}

void SolenoidManager::updateRelayStates() // const SensorData& p_sensorData)
{
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        for (uint8_t cmdId = 0; cmdId < m_currentCmdId; cmdId++)
        {
            RelayIds relayId = ToRelayId(relayIdu8);
            // skip if not the right relay selected
            if ((relayId != m_cmdList[cmdId].relayId) && (m_cmdList[cmdId].relayId != RelayIds::AllRelays))
            {
                continue;
            }

            // skip if the running priority is higher
            if (m_relayCmdIndexes[relayIdu8].priority >= m_cmdList[cmdId].priority)
            {
                continue;
            }

            // TODOsz Only update the state if the command is active.
            RelayState state = applyCmd(m_cmdList[cmdId]);
            m_relayCmdIndexes[relayIdu8].set(cmdId, m_cmdList[cmdId].priority, state);
        }
    }
}

// bool SolenoidManager::publishAllCmds() {}
