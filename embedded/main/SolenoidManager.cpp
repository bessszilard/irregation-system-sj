#include "SolenoidManager.hpp"

#define DELIMITER "_"

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

CommandState SolenoidManager::overrideCmd(const String& p_cmdStr)
{
    SolenoidCtrlCmd cmdToAdd(p_cmdStr);
    for (uint8_t i = 0; i < m_currentCmdId; ++i)
    {
        if ((m_cmdList[i].priority == cmdToAdd.priority) && (m_cmdList[i].relayId == cmdToAdd.relayId))
        {
            removeCmd(i);
        }
    }
    return appendCmd(p_cmdStr);
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
    strcpy(buffer, "{ \"cmdList\": [");
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

String SolenoidManager::getCmdListStr() const
{
    size_t estimatedSize = 0;

    for (uint8_t i = 0; i < m_currentCmdId; i++)
    {
        estimatedSize += m_cmdList[i].toString().length();
    }

    estimatedSize += m_currentCmdId - 1; // for DELIMITER characters

    String result;
    result.reserve(estimatedSize); // Preallocate memory

    for (uint8_t i = 0; i < m_currentCmdId; i++)
    {
        result += m_cmdList[i].toString();
        result += DELIMITER;
    }
    return result;
}

bool SolenoidManager::loadCmdsFromString(const String& p_cmds)
{
    m_currentCmdId = 0;

    int start = 0;
    int end   = p_cmds.indexOf(DELIMITER);

    while (end != -1 && m_currentCmdId < MAX_NUMBER_OF_CMDS)
    {
        String line = p_cmds.substring(start, end);

        if (CommandState::Added != appendCmd(line))
        {
            Serial.print("Failed to parse line: ");
            Serial.println(line);
            return false;
        }
        start = end + 1;
        end   = p_cmds.indexOf(DELIMITER, start);
    }

    // Handle last line (if not ending with newline)
    if (start < p_cmds.length() && m_currentCmdId < MAX_NUMBER_OF_CMDS)
    {
        String line = p_cmds.substring(start);
        line.trim();

        if (line.length() > 0)
        {
            if (CommandState::Added != appendCmd(line))
            {
                Serial.print("Failed to parse final line: ");
                Serial.println(line);
                return false;
            }
        }
    }

    return true;
}

// clang-format off
String SolenoidManager::getRelayStatesWithCmdIdsJson() const
{
    // First pass: Calculate required size
    size_t totalSize = 3; // "{\n" + "}"
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        RelayIds relayId = ToRelayId(relayIdu8);
        totalSize += ToString(relayId).length() + 10; // Quotes + `": {\n`
        totalSize += ToString(m_relayCmdIndexes[relayIdu8].currentState).length() + 13; // `"state": "XXX",\n`
        uint8_t currenCmdId = m_relayCmdIndexes[relayIdu8].cmdIdx;
        totalSize += m_cmdList[currenCmdId].toString().length() + 10; // `"cmd": "XXX",\n`
        totalSize += ToString(m_relayCmdIndexes[relayIdu8].priority).length() + 15; // `"priority": X,\n`
        totalSize += (relayIdu8 < NUMBER_OF_RELAYS - 1) ? 2 : 1; // ",\n" or "\n"
    }

    // Allocate memory
    char *buffer = new char[totalSize + 1]; // +1 for null terminator
    if (!buffer) return "{}"; // Memory allocation failed, return empty JSON

    // Second pass: Build JSON string
    size_t offset = 0;
    offset += snprintf(buffer + offset, totalSize - offset + 1, "{\n");
    
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        RelayIds relayId = ToRelayId(relayIdu8);
        offset += snprintf(buffer + offset, totalSize - offset + 1, "\"%s\": {\n", ToString(relayId).c_str());
        offset += snprintf(buffer + offset, totalSize - offset + 1, "\"state\": \"%s\",\n", 
                           ToString(m_relayCmdIndexes[relayIdu8].currentState).c_str());
        uint8_t currenCmdId = m_relayCmdIndexes[relayIdu8].cmdIdx;
        offset += snprintf(buffer + offset, totalSize - offset + 1, "\"cmd\": \"%s\",\n", 
                           m_cmdList[currenCmdId].toString().c_str());
        offset += snprintf(buffer + offset, totalSize - offset + 1, "\"priority\": \"%s\"\n", 
                           ToString(m_relayCmdIndexes[relayIdu8].priority).c_str());
        offset += snprintf(buffer + offset, totalSize - offset + 1, "}%s\n", 
                           (relayIdu8 < NUMBER_OF_RELAYS - 1) ? "," : "");
    }

    offset += snprintf(buffer + offset, totalSize - offset + 1, "}");

    // Convert to String, free memory, and return
    String jsonResult = String(buffer);
    delete[] buffer;
    return jsonResult;
}

// clang-format on

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
