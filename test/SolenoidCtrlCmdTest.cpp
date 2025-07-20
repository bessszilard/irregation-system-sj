#include <gtest/gtest.h>
#include "../embedded/main/SolenoidCtrlCmd.hpp"

const std::string ValidManualStr       = "$Manua;P00;R01;C#";
const std::string ValidAutomaticStr    = "$ATime;P05;RXX;O15:00->20:00#";
const std::string ValidAutomaticRepStr = "$ATime;P05;RXX;X15:00->20:00_O01h_C20m#";

TEST(SolenoidCtrlCmdTest, ManualValidStr)
{
    SolenoidCtrlCmd sm(ValidManualStr);
    EXPECT_EQ(sm.cmdType, CommandType::ManCtrl);
    EXPECT_EQ(ToString(sm.relayTarget.relayId), ToString(RelayIds::Relay1));
    EXPECT_EQ(sm.priority, CmdPriority::Priority0);
    EXPECT_EQ(sm.action, "C");
}

TEST(SolenoidCtrlCmdTest, AutoTimeStr)
{
    SolenoidCtrlCmd sm(ValidAutomaticStr);
    EXPECT_EQ(sm.cmdType, CommandType::AutoTimeCtrl);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::AllRelays);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "O15:00->20:00");
}

TEST(SolenoidCtrlCmdTest, AutoTimeReapeatStr)
{
    SolenoidCtrlCmd sm(ValidAutomaticRepStr);
    EXPECT_EQ(sm.cmdType, CommandType::AutoTimeCtrl);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::AllRelays);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "X15:00->20:00_O01h_C20m");
}

TEST(SolenoidCtrlCmdTest, toString)
{
    for (const auto str : {ValidManualStr, ValidAutomaticStr, ValidAutomaticStr})
    {
        SolenoidCtrlCmd sm(str);
        EXPECT_EQ(str, sm.toString());
    }
}

TEST(SolenoidCtrlCmdTest, ValidChecksum)
{
    SolenoidCtrlCmd sm(ValidManualStr);
    EXPECT_EQ(sm.cmdType, CommandType::ManCtrl);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority0);
    EXPECT_EQ(sm.checksum, 0x55);
    EXPECT_EQ(sm.action, "C");
}