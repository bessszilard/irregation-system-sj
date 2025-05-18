#include "SolenoidCtrlCmd.hpp"

#define START_CHAR "$"
#define TERMINATOR_CHAR "#"

static const uint8_t CmdTypeStrLen    = 5;
static const uint8_t RelayIdStrLen    = 3;
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

    if (p_cmd.endsWith('#')) // no checksum
    {
        action   = Utils::GetSubStr(p_cmd, idx, -1);
        checksum = getChecksum(p_cmd);
    }
    else
    {
        action = Utils::GetSubStr(p_cmd, idx, -3);
        idx    = p_cmd.size() - 2; // last two characters

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
        return START_CHAR + ToString(cmdType) + ";" + ToString(priority) + ";" + ToString(relayId) + ";" + action +
               TERMINATOR_CHAR + String(checksum, HEX);
    else
        return START_CHAR + ToString(cmdType) + ";" + ToString(priority) + ";" + ToString(relayId) + ";" + action +
               TERMINATOR_CHAR;
}

uint8_t SolenoidCtrlCmd::getChecksum(const String& p_cmd, bool checksumIncluded) const
{
    uint8_t checksum = 0;
    // payload only
    uint8_t end = checksumIncluded ? p_cmd.size() - 3 : p_cmd.size();
    for (uint8_t i = 1; i < end; i++)
    {
        // std::cout << std::hex << static_cast<int>(checksum) << "^";
        checksum ^= p_cmd[i];
        // std::cout << std::hex << static_cast<int>(p_cmd[i]) << "=" << static_cast<int>(checksum) << std::endl;
    }
    return checksum;
}