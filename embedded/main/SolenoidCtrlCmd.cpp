#include "SolenoidCtrlCmd.hpp"

// Manua;R1;Closed;P0;F << not overridable by automation
// Manua;R1;Closed;P0;T << overridable by automation
// ATemp;R2;Opened;P1;<15
// AFlow;R3;Closed;P2;>10
// AHumi;R4;Closed;P3;>10
// ATime;R5;Closed;P4;>15:00
// ATime;RX;Closed;P5;>15:00

SolenoidCtrlCmd::SolenoidCtrlCmd(const String& p_cmd)
{
    cmdType    = CommandTypeFromString(p_cmd, 0, 5);
    relayId    = RelayIdTypeFromString(p_cmd, 6, 9);
    relayState = RelayStateFromString(p_cmd, 10, 16);
    priority   = CmdPriorityFromString(p_cmd, 17, 19);
    action     = Utils::GetSubStr(p_cmd, 20, -1);
}