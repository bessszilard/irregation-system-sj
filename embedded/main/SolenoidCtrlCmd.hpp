#pragma once

#include "Enums.hpp"
#include "Structures.hpp"
// p_cmd example starts with [ ends with ]
// TODOsz add priority
// Man;R1;Close;1 << overridable by automation
// Aut;R2;Open;Temp;<15
// Aut;R3;Close;Flow;>10
// Aut;R4;Close;Hum;>10
// Aut;R5;Close;Time;>15:00
// Aut;RX;Close;Time;>15:00

// $Manua;P0;R01;C
// $ATime;P5;RXX;O15:00->20:00
// $ATime;P5;RXX;X15:00->20:00_O20m_C1h

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

    RelayState evaluate(const SensorData& sensors, const LocalTime& loctime);
};