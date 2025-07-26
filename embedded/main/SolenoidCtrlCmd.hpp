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

    // range in in "hh:mm->hh:mm" format
    static bool IsWithinTimeRange(const char* range, const LocalTime& now);

    static RelayState RelayThresholdCtrl(const String& p_range, float p_value);
};