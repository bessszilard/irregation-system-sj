#include "SolenoidCtrlCmd.hpp"
#include "PredefinedCommands.hpp"

#define START_CHAR "$"
#define TERMINATOR_CHAR "#"

// TODOsz add decoration
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

bool SolenoidCtrlCmd::IsWithingRange(char p_operation, float p_value, float p_threshold)
{
    if (p_operation == '>' && p_value > p_threshold)
        return true;
    if (p_operation == '<' && p_value < p_threshold)
        return true;
    return false;
}

RelayState SolenoidCtrlCmd::RelayTimeSingleShotCtrl(const String& p_action, const LocalTime& p_now)
{
    if (!p_now.valid)
    {
        Serial.println("Invalid time");
        return RelayState::Unknown;
    }

    char type, relayStateStr;
    int startHour, startMin, endHour, endMin;
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
    if (!p_now.valid)
    {
        Serial.println("Invalid time");
        return RelayState::Unknown;
    }
    char type, phase1StateChar, timeUnitChar1, phase2StateChar, timeUnitChar2;
    int startHour, startMin, endHour, endMin, phase1dur, phase2dur;
    int count = sscanf(p_action.c_str(),
                       "%c_X%d:%d->%d:%d_%c%d%c_%c%d%c",
                       &type,
                       &startHour,
                       &startMin,
                       &endHour,
                       &endMin,
                       &phase1StateChar,
                       &phase1dur,
                       &timeUnitChar1,
                       &phase2StateChar,
                       &phase2dur,
                       &timeUnitChar2);
    if (count != 11)
    {
        Serial.printf("Failed to parse %s", p_action);
        return RelayState::Unknown;
    }
    RelayState phase1state = ToRelayStateFromShortString(phase1StateChar);
    RelayState phase2state = ToRelayStateFromShortString(phase2StateChar);
    TimeUnit timeUnit1     = ToTimeUnit(timeUnitChar1);
    TimeUnit timeUnit2     = ToTimeUnit(timeUnitChar2);

    if (type != 'R' || phase1state == RelayState::Unknown || phase2state == RelayState::Unknown ||
        timeUnit1 == TimeUnit::Unknown || timeUnit2 == TimeUnit::Unknown)
    {
        Serial.printf("Failed to parse %s", p_action.c_str());
        return RelayState::Unknown;
    }

    uint16_t startMinutes   = startHour * 60 + startMin;
    uint16_t endMinutes     = endHour * 60 + endMin;
    uint16_t currentMinutes = p_now.tm_hour * 60 + p_now.tm_min;

    // within range
    if (currentMinutes < startMinutes || endMinutes < currentMinutes)
        return RelayState::Unknown;

    uint16_t phase1_sec = phase1dur * TimeUnitToSeconds(timeUnit1);
    uint16_t phase2_sec = phase2dur * TimeUnitToSeconds(timeUnit2);

    uint16_t period_sec                = phase1_sec + phase2_sec;
    uint32_t currentPhaseTime_sec      = (currentMinutes - startMinutes) * 60 + p_now.tm_sec;
    uint16_t currentPeriodFragment_sec = currentPhaseTime_sec % period_sec;

    if (currentPeriodFragment_sec < phase1_sec)
    {
        return phase1state;
    }
    else
    {
        return phase2state;
    }
}

RelayState SolenoidCtrlCmd::RelaySensorThresholdCtrl(const String& p_action, const SensorData& p_data)
{
    // TESU_O>025.0_C<010.5
    SensorType sensorType = ToSensorTypeFromString(Utils::GetSubStr(p_action, 0, 4));
    if (sensorType == SensorType::Unknown)
    {
        Serial.printf("Failed to get sensor type from %s\n", p_action.c_str());
        return RelayState::Unknown;
    }
    String thresholdStr = Utils::GetSubStr(p_action, 5, -1);
    return ApplyThresholdCtrl(thresholdStr, p_data.get(sensorType));
}

RelayState SolenoidCtrlCmd::RelaySensorRangeCtrl(const String& p_action, const SensorData& p_data)
{
    // TESU_O>025.0_C<010.5
    SensorType sensorType = ToSensorTypeFromString(Utils::GetSubStr(p_action, 0, 4));
    if (sensorType == SensorType::Unknown)
    {
        Serial.printf("Failed to get sensor type from %s\n", p_action.c_str());
        return RelayState::Unknown;
    }
    String rangeStr = Utils::GetSubStr(p_action, 5, -1);
    return ApplyRangeCtrl(rangeStr, p_data.get(sensorType));
}

RelayState SolenoidCtrlCmd::ApplyRangeCtrl(const String& p_range, float p_value)
{
    RelayState state = ApplyThresholdCtrl(Utils::GetSubStr(p_range, 0, SensorFloatThresholdLength), p_value);
    if (state != RelayState::Unknown)
        return state;
    state = ApplyThresholdCtrl(Utils::GetSubStr(p_range, SensorFloatThresholdLength + 1, -1), p_value);
    if (state != RelayState::Unknown)
        return state;
    return RelayState::Unknown;
}

RelayState SolenoidCtrlCmd::ApplyThresholdCtrl(const String& p_range, float p_value)
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

    if (false == IsWithingRange(operation, p_value, threshold))
        return RelayState::Unknown;
    return targetState;
}

RelayState SolenoidCtrlCmd::RelaySenThTimeRangeRepCtrl(const String& p_action,
                                                       const SensorData& p_data,
                                                       const LocalTime& p_now)
{
    SensorType sensorType = ToSensorTypeFromString(Utils::GetSubStr(p_action, 0, 4));
    if (sensorType == SensorType::Unknown)
    {
        Serial.printf("Failed to get sensor type from %s\n", p_action.c_str());
        return RelayState::Unknown;
    }
    String thresholdStr = Utils::GetSubStr(p_action, 5, -1);
    char operation;
    float threshold;
    if (sscanf(thresholdStr.c_str(), "X%c%f", &operation, &threshold) != 2)
    {
        Serial.printf("Failed to parse %s\n", p_action.c_str());
        return RelayState::Unknown;
    }

    if (false == IsWithingRange(operation, p_data.get(sensorType), threshold))
        return RelayState::Unknown;

    String timeAction = Utils::GetSubStr(p_action, 13, -1);
    return RelayTimeRepeatCtrl(timeAction, p_now);
}

RelayState SolenoidCtrlCmd::evaluate(const SensorData& p_sensors, const LocalTime& p_now) const
{
    switch (cmdType)
    {
        case CommandType::Manual:
            return ToRelayStateFromShortString(action);
        case CommandType::TimeSingleShot:
            return RelayTimeSingleShotCtrl(action, p_now);
        case CommandType::TimeRepeat:
            return RelayTimeRepeatCtrl(action, p_now);
        case CommandType::SensorRange:
            return RelaySensorRangeCtrl(action, p_sensors);
        case CommandType::SensorThreshold:
            return RelaySensorThresholdCtrl(action, p_sensors);
        case CommandType::SensorThresholdTimeRepeat:
            return RelaySenThTimeRangeRepCtrl(action, p_sensors, p_now);
    }
    return RelayState::Unknown;
}
