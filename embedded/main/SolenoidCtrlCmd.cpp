#include "SolenoidCtrlCmd.hpp"

// Manua;R1;Closed;P00;F << not overridable by automation
// Manua;R1;Closed;P00;T << overridable by automation
// ATemp;R2;Opened;P01;<15
// AFlow;R3;Closed;P02;>10
// AHumi;R4;Closed;P03;>10
// ATime;R5;Closed;P04;15:00->20:00
// ATime;RX;Closed;P05;>15:00

SolenoidCtrlCmd::SolenoidCtrlCmd(const String& p_cmd) : valid(false)
{
    cmdType    = CommandTypeFromString(p_cmd, 0, 5);
    relayId    = RelayIdTypeFromString(p_cmd, 6, 9);
    relayState = RelayStateFromString(p_cmd, 10, 16);
    priority   = CmdPriorityFromString(p_cmd, 17, 20);
    action     = Utils::GetSubStr(p_cmd, 21, -1);

    valid = cmdType != CommandType::Unknown && relayId != RelayIds::Unknown && relayState != RelayState::Unknown &&
            priority != CmdPriority::Unknown;
}

String SolenoidCtrlCmd::toString() const
{
    return ToString(cmdType) + ";" + ToString(relayId) + ";" + ToString(relayState) + ";" + ToString(priority) + ";" +
           action;
}