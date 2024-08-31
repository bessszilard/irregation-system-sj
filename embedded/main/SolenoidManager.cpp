#include "SolenoidManager.hpp"

SolenoidManager::SolenoidManager() : m_currentCmdId(0)
{
    for (uint8_t id = 0; id < NUMBER_OF_RELAYS; id++)
    {
        m_relayCmdIndexes[id].set(0, CmdPriority::PriorityLowest, RelayState::Unknown);
    }
}

CommandState SolenoidManager::appendEvent(const String& p_eventStr)
{
    SolenoidCtrlCmd eventToAdd(p_eventStr);
    for (uint8_t i = 0; i < m_currentCmdId; ++i)
    {
        if (m_cmdList[i] == eventToAdd)
        {
            return CommandState::AlreadyPresent;
        }
    }
    if (MAX_NUMBER_OF_CMDS <= m_currentCmdId)
    {
        return CommandState::MemoryFull;
    }

    m_cmdList[m_currentCmdId] = eventToAdd;
    m_currentCmdId++;
    return CommandState::Added;
}

CommandState SolenoidManager::removeEvent(uint8_t p_id)
{
    if (m_currentCmdId < p_id)
    {
        return CommandState::CantRemove;
    }

    for (uint8_t i = p_id; i < m_currentCmdId - 1; i++)
    {
        m_cmdList[i] = m_cmdList[i + 1];
    }
    m_currentCmdId--;
    return CommandState::Removed;
}

String SolenoidManager::getEventString(uint8_t p_id) const
{
    if (m_currentCmdId < p_id)
    {
        return "";
    }
    return m_cmdList[p_id].toString();
}

RelayState SolenoidManager::updateRelayStatus(const SolenoidCtrlCmd& p_cmd)
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

bool SolenoidManager::updateRelayStates()
{
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        for (uint8_t cmdId = 0; cmdId < m_currentCmdId - 1; cmdId++)
        {
            RelayIds relayId = ToRelayId(relayIdu8);
            // skip if not the right relay selected
            if ((relayId != m_cmdList[cmdId].relayId) && (relayId != RelayIds::AllRelays))
            {
                continue;
            }

            // skip if the running priority is higher
            if (m_relayCmdIndexes[relayIdu8].priority >= m_cmdList[cmdId].priority)
            {
                continue;
            }

            RelayState state = updateRelayStatus(m_cmdList[cmdId]);
            m_relayCmdIndexes[relayIdu8].set(cmdId, m_cmdList[cmdId].priority, state);
        }
    }

    return false;
}

// bool SolenoidManager::publishAllEvents() {}
