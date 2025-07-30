#pragma once

#include "Enums.hpp"
#include "Structures.hpp"

struct SolenoidCtrlCmd
{
    SolenoidCtrlCmd(){};
    SolenoidCtrlCmd(const String& p_cmd);
    virtual ~SolenoidCtrlCmd(){};

    String toString(bool addChecksum = false) const;

    CommandType cmdType;
    CmdPriority priority; // larger value means lower priority
    RelayTarget relayTarget;
    RelayState relayState;
    String action;
    uint8_t checksum;
    bool valid;

    inline bool operator==(const SolenoidCtrlCmd& p_cmd) const
    {
        return cmdType == p_cmd.cmdType && relayTarget == p_cmd.relayTarget && relayState == p_cmd.relayState &&
               priority == p_cmd.priority && action == p_cmd.action;
    }

    uint8_t getChecksum(const String& p_cmd, bool checksumIncluded = true) const;

    RelayState evaluate(const SensorData& p_sensors, const LocalTime& p_now) const;

protected:
    // range in in "hh:mm->hh:mm" format
    static bool IsWithinTimeRange(const char* p_range, const LocalTime& p_now);
    static bool IsWithingRange(char p_operation, float p_value, float p_threshold);

    // TODOsz move to protected
    static RelayState RelayTimeSingleShotCtrl(const String& p_action, const LocalTime& p_now);
    static RelayState RelayTimeRepeatCtrl(const String& p_action, const LocalTime& p_now);
    static RelayState RelaySensorThresholdCtrl(const String& p_action, const SensorData& p_data);
    static RelayState RelaySensorRangeCtrl(const String& p_action, const SensorData& p_data);
    static RelayState RelaySenThTimeRangeRepCtrl(const String& p_action,
                                                 const SensorData& p_data,
                                                 const LocalTime& p_now);

    static RelayState ApplyRangeCtrl(const String& p_range, float p_value);
    static RelayState ApplyThresholdCtrl(const String& p_range, float p_value);
};