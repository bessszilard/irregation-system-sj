#include "SolenoidCtrlCmd.hpp"
#include "PredefinedCommands.hpp"

#define START_CHAR "$"
#define TERMINATOR_CHAR "#"

SolenoidCtrlCmd::SolenoidCtrlCmd(const String& p_cmd) : valid(false)
{
    int idx = 1; // first char is $
    cmdType = CommandTypeFromString(p_cmd, idx, idx + CmdTypeStrLen);
    idx += CmdTypeStrLen + 1;

    priority = CmdPriorityFromString(p_cmd, idx, idx + PriorityStateLen);
    idx += PriorityStateLen + 1;

    relayTarget = RelayTarget::FromString(p_cmd, idx, idx + RelayIdStrLen);
    idx += RelayIdStrLen + 1;

    if (p_cmd.endsWith("#")) // no checksum
    {
        action   = Utils::GetSubStr(p_cmd, idx, -2); // skip last
        checksum = getChecksum(p_cmd);
    }
    else
    {
        action = Utils::GetSubStr(p_cmd, idx, -4); // skip last two
        idx    = strlen(p_cmd.c_str()) - 2; // last two characters

        String receivedChecksum = Utils::GetSubStr(p_cmd, idx, -3);

        checksum                  = getChecksum(p_cmd, true);
        String calculatedChecksum = String(checksum, HEX);
        if (receivedChecksum != calculatedChecksum)
        {
            Serial.printf("Invalid checksum. Received: %s vs calculated: %s",
                          receivedChecksum.c_str(),
                          calculatedChecksum.c_str());
            valid = false;
            return;
        }
    }
    valid = true;
}

String SolenoidCtrlCmd::toString(bool addChecksum) const
{
    if (addChecksum)
        return START_CHAR + ToString(cmdType) + ";" + ToString(priority) + ";" + relayTarget.toString() + ";" + action +
               TERMINATOR_CHAR + String(checksum, HEX);
    else
        return START_CHAR + ToString(cmdType) + ";" + ToString(priority) + ";" + relayTarget.toString() + ";" + action +
               TERMINATOR_CHAR;
}

uint8_t SolenoidCtrlCmd::getChecksum(const String& p_cmd, bool checksumIncluded) const
{
    uint8_t checksum = 0;
    // payload only
    uint8_t end = checksumIncluded ? strlen(p_cmd.c_str()) - 3 : strlen(p_cmd.c_str());
    for (uint8_t i = 1; i < end; i++)
    {
        // std::cout << std::hex << static_cast<int>(checksum) << "^";
        checksum ^= p_cmd[i];
        // std::cout << std::hex << static_cast<int>(p_cmd[i]) << "=" << static_cast<int>(checksum) << std::endl;
    }
    return checksum;
}
// TODOsz add evaluate command -> we need time and sensor data

bool SolenoidCtrlCmd::IsWithinTimeRange(const char* p_range, const LocalTime& p_now)
{
    if (!p_now.valid)
        return false;

    int startHour, startMin, endHour, endMin;
    // Parse string in format "hh:mm->hh:mm"
    if (sscanf(p_range, "%d:%d->%d:%d", &startHour, &startMin, &endHour, &endMin) != 4)
        return false;

    int startMinutes   = startHour * 60 + startMin;
    int endMinutes     = endHour * 60 + endMin;
    int currentMinutes = p_now.tm_hour * 60 + p_now.tm_min;

    return startMinutes <= currentMinutes && currentMinutes <= endMinutes;
}

RelayState SolenoidCtrlCmd::RelayTimeSingleShotCtr(const String& p_action, const LocalTime& p_now)
{
    if (!p_now.valid)
    {
        Serial.println("Invalid time");
        return RelayState::Unknown;
    }

    char type, relayStateStr;
    uint8_t startHour, startMin, endHour, endMin;
    if (sscanf(p_action.c_str(),
               "%c_%c%d:%d->%d:%d",
               &type,
               &relayStateStr,
               &startHour,
               &startMin,
               &endHour,
               &endMin) != 6)
    {
        Serial.printf("Failed to parse %s", p_action);
        return RelayState::Unknown;
    }

    RelayState state = ToRelayStateFromShortString(relayStateStr);
    if (type != 'S' || state == RelayState::Unknown)
    {
        Serial.printf("Failed to parse %s", p_action.c_str());
        return RelayState::Unknown;
    }
    uint16_t startMinutes   = startHour * 60 + startMin;
    uint16_t endMinutes     = endHour * 60 + endMin;
    uint16_t currentMinutes = p_now.tm_hour * 60 + p_now.tm_min;

    // within range
    if (startMinutes <= currentMinutes && currentMinutes <= endMinutes)
        return state;

    return RelayState::Unknown;
}

RelayState SolenoidCtrlCmd::RelayTimeRepeatCtrl(const String& p_action, const LocalTime& p_now)
{

    return RelayState::Unknown;
}

RelayState SolenoidCtrlCmd::RelayRangeCtrl(const String& p_range, float p_value)
{
    RelayState state = RelayThresholdCtrl(Utils::GetSubStr(p_range, 0, SensorFloatThresholdLength), p_value);
    if (state != RelayState::Unknown)
        return state;
    state = RelayThresholdCtrl(Utils::GetSubStr(p_range, SensorFloatThresholdLength + 1, -1), p_value);
    if (state != RelayState::Unknown)
        return state;
    return RelayState::Unknown;
}

RelayState SolenoidCtrlCmd::RelayThresholdCtrl(const String& p_range, float p_value)
{
    if (p_range.length() != SensorFloatThresholdLength)
    {
        Serial.printf("Invalid threshold string: %s\n", p_range.c_str());
    }

    char openOrClosed, operation;
    float threshold;
    if (sscanf(p_range.c_str(), "%c%c%f", &openOrClosed, &operation, &threshold) != 3)
    {
        Serial.printf("Failed to parse %s\n", p_range.c_str());
        return RelayState::Unknown;
    }

    RelayState targetState = ToRelayStateFromShortString(openOrClosed);

    if (operation == '>' && p_value > threshold)
        return targetState;
    if (operation == '<' && p_value < threshold)
        return targetState;

    return RelayState::Unknown;
}

RelayState SolenoidCtrlCmd::evaluate(const SensorData& p_sensors, const LocalTime& p_now) const
{
    switch (cmdType)
    {
        case CommandType::Manual:
            return ToRelayStateFromShortString(action);
        case CommandType::TimeSingleShot:
            if (action.length() == TimeSingleActionLength)
            {
                if (false == IsWithinTimeRange(action.c_str() + 2, p_now))
                    return RelayState::Unknown;
                else
                    return ToRelayStateFromShortString(action[0]);
            }
            else if (action.length() == TimeRepeatActionLength)
            {
                return RelayState::Unknown;
            }
            return RelayState::Unknown;

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
