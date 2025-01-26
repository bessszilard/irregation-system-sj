#pragma once

#include "Enums.hpp"

// p_cmd example starts with [ ends with ]
// TODOsz add priority
// Man;R1;Close;1 << overridable by automation
// Aut;R2;Open;Temp;<15
// Aut;R3;Close;Flow;>10
// Aut;R4;Close;Hum;>10
// Aut;R5;Close;Time;>15:00
// Aut;RX;Close;Time;>15:00

struct SolenoidCtrlCmd
{
    SolenoidCtrlCmd(){};
    SolenoidCtrlCmd(const String& p_cmd);
    virtual ~SolenoidCtrlCmd(){};

    String toString() const;

    CommandType cmdType;
    CmdPriority priority; // larger value means lower priority
    RelayIds relayId;
    RelayState relayState;
    String action;
    bool valid;

    inline bool operator==(const SolenoidCtrlCmd& p_cmd) const
    {
        return cmdType == p_cmd.cmdType && relayId == p_cmd.relayId && relayState == p_cmd.relayState &&
               action == p_cmd.action;
    }
};