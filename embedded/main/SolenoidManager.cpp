#include "SolenoidManager.hpp"

#define DELIMITER "_"

SolenoidManager::SolenoidManager() : m_currentCmdId(0), m_relayGroups()
{
    resetPriorities();
}

void SolenoidManager::resetPriorities()
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
        if ((m_cmdList[i].priority == cmdToAdd.priority) && (m_cmdList[i].relayTarget == cmdToAdd.relayTarget))
        {
            removeCmd(i);
        }
    }
    CommandState result = appendCmd(p_cmdStr);

    return result == CommandState::Added ? CommandState::Overriden : result;
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
        resetPriorities();
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
    resetPriorities();

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
void SolenoidManager::getRelayStatesWithCmdIdsJson(String& jsonResult) const
{
    // First pass: Calculate required size
    size_t totalSize = 3; // "{\n" + "}"
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        RelayIds relayId = ToRelayId(relayIdu8);
        totalSize += ToString(relayId).length() + 10; // Quotes + `": {\n`
        totalSize += ToString(m_relayCmdIndexes[relayIdu8].currentState).length() + 13; // `"state": "XXX",\n`
        uint8_t currenCmdId = m_relayCmdIndexes[relayIdu8].cmdIdx;
        totalSize += 3; // `"cmd": "XXX",\n`
        totalSize += ToString(m_relayCmdIndexes[relayIdu8].priority).length() + 15; // `"priority": X,\n`
        totalSize += (relayIdu8 < NUMBER_OF_RELAYS - 1) ? 2 : 1; // ",\n" or "\n"
    }
    // totalSize += 200;
    // Allocate memory
    char *buffer = new char[totalSize + 1]; // +1 for null terminator
    if (!buffer) return;

    // Second pass: Build JSON string
    size_t offset = 0;
    offset += snprintf(buffer + offset, totalSize - offset + 1, "{");
    
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        RelayIds relayId = ToRelayId(relayIdu8);
        offset += snprintf(buffer + offset, totalSize - offset + 1, "\"%s\":{", ToString(relayId).c_str());
        offset += snprintf(buffer + offset, totalSize - offset + 1, "\"state\":\"%s\",", 
                           ToString(m_relayCmdIndexes[relayIdu8].currentState).c_str());
        uint8_t currenCmdId = m_relayCmdIndexes[relayIdu8].cmdIdx;
        offset += snprintf(buffer + offset, totalSize - offset + 1, "\"cmd\":\"%d\"", 
                          currenCmdId);
        // offset += snprintf(buffer + offset, totalSize - offset + 1, "\"priority\":\"%s\"", 
        //                    ToString(m_relayCmdIndexes[relayIdu8].priority).c_str());
        offset += snprintf(buffer + offset, totalSize - offset + 1, "}%s", 
                           (relayIdu8 < NUMBER_OF_RELAYS - 1) ? "," : "");
    }

    offset += snprintf(buffer + offset, totalSize - offset + 1, "}");

    Serial.printf(">>>>>>>>>>>> total %d vs %d", totalSize, offset);

    // Convert to String, free memory, and return
    jsonResult = String(buffer);
    delete[] buffer;
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

bool SolenoidManager::updateRelayStates(bool verbose /*=false*/)
{
    bool atLeastOneChanged = false;
    for (uint8_t relayIdu8 = 0; relayIdu8 < NUMBER_OF_RELAYS; relayIdu8++)
    {
        for (uint8_t cmdId = 0; cmdId < m_currentCmdId; cmdId++)
        {
            RelayIds relayId = ToRelayId(relayIdu8);
            const SolenoidCtrlCmd& cmd = m_cmdList[cmdId];

            if (verbose)
            {
                Serial.printf("%s: %s -> ", ToString(relayId).c_str(), cmd.toString().c_str());
            }

            // skip if not the right relay selected
            if (cmd.relayTarget.type == RelayTargetType::SingleRelay)
            {
                if (cmd.relayTarget.relayId != relayId)
                {
                    if ((verbose))
                        Serial.printf("%s != %s -> skipped\n",
                                      ToString(relayId).c_str(),
                                      ToString(cmd.relayTarget.relayId).c_str());
                    continue;
                }
            }
            // skip if command effects on a group, and relay not selected
            else if (cmd.relayTarget.type == RelayTargetType::Group)
            {
                if (false == m_relayGroups.isInGroup(cmd.relayTarget.groupId, relayId))
                {
                    if (verbose)
                        Serial.printf("%s not in %s group -> skipped\n",
                                      ToString(relayId).c_str(),
                                      ToString(cmd.relayTarget.groupId).c_str());
                    continue;
                }
            }

            // skip if the running priority is higher
            if (m_relayCmdIndexes[relayIdu8].priority >= m_cmdList[cmdId].priority)
            {
                if (verbose)
                    Serial.print("Skipped since higher priority was already applied");
                continue;
            }

            if (verbose)
            {
                Serial.println("Applied");
            }

            RelayState state = m_cmdList[cmdId].evaluate(m_sensorData, m_localTime);
            if (state == RelayState::Unknown)
            {
                continue;
            }
            RelayState oldState = m_relayCmdIndexes[relayIdu8].currentState;
            m_relayCmdIndexes[relayIdu8].set(cmdId, m_cmdList[cmdId].priority, state);
            if (m_relayCmdIndexes[relayIdu8].currentState != oldState)
            {
                atLeastOneChanged = true;
            }
        }
    }
    return atLeastOneChanged;
}

// bool SolenoidManager::publishAllCmds() {}
