#include "SolenoidCtrlCmd.hpp"

#define START_CHAR "$"
#define TERMINATOR_CHAR "#"

static const uint8_t CmdTypeStrLen = 5;
static const uint8_t RelayIdStrLen = 3;
static const uint8_t RelayStateStrLen = 6;
static const uint8_t PriorityStateLen = 3;

SolenoidCtrlCmd::SolenoidCtrlCmd(const String& p_cmd) : valid(false)
{
    int idx = 1; // first char is $
    cmdType = CommandTypeFromString(p_cmd, idx, idx + CmdTypeStrLen);
    idx += CmdTypeStrLen + 1;

    priority = CmdPriorityFromString(p_cmd, idx, idx + PriorityStateLen);
    idx += PriorityStateLen + 1;

    relayId = RelayIdTypeFromString(p_cmd, idx, idx + RelayIdStrLen);
    idx += RelayIdStrLen + 1;

    action = Utils::GetSubStr(p_cmd, idx, -1);
    valid  = true;
}

String SolenoidCtrlCmd::toStringShort() const {
  return ToString(cmdType) + ";" + ToString(relayId) + ";" +
         ToString(priority) + ";" + action;
}

String SolenoidCtrlCmd::toString() const
{
  return START_CHAR + ToString(cmdType) + ";" + ToString(priority) + ";" +
         ToString(relayId) + ";" + action + TERMINATOR_CHAR;
}