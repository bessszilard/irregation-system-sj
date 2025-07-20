#include <gtest/gtest.h>
#include "../embedded/main/SolenoidCtrlCmd.hpp"
#include "../embedded/main/SolenoidCtrlCmdExamples.hpp"

TEST(SolenoidCtrlCmdTest, ManualValidStr)
{
    SolenoidCtrlCmd sm(CMD_MANUAL_CLOSE_ALL_RELAYS);
    EXPECT_EQ(sm.cmdType, CommandType::ManCtrl);
    EXPECT_EQ(ToString(sm.relayTarget.relayId), ToString(RelayIds::AllRelays));
    EXPECT_EQ(sm.priority, CmdPriority::PriorityLowest);
    EXPECT_EQ(sm.action, "C");
}

TEST(SolenoidCtrlCmdTest, AutoTimeStr)
{
    SolenoidCtrlCmd sm(CMD_ATIME_SINGLE);
    EXPECT_EQ(sm.cmdType, CommandType::AutoTimeCtrl);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "S_C07:00->20:00");
}

TEST(SolenoidCtrlCmdTest, AutoTimeReapeatStr)
{
    SolenoidCtrlCmd sm(CMD_ATIME_REPEAT);
    EXPECT_EQ(sm.cmdType, CommandType::AutoTimeCtrl);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "R_X06:00->20:00_O01h_C20m");
}

TEST(SolenoidCtrlCmdTest, toString)
{
    for (const auto str : {CMD_MANUAL_CLOSE_ALL_RELAYS, CMD_ATIME_SINGLE, CMD_ATIME_SINGLE})
    {
        SolenoidCtrlCmd sm(str);
        EXPECT_EQ(str, sm.toString());
    }
}

TEST(SolenoidCtrlCmdTest, ValidChecksum)
{
    SolenoidCtrlCmd sm(CMD_MANUAL_CLOSE_RELAY1);
    EXPECT_EQ(sm.cmdType, CommandType::ManCtrl);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::PriorityLowest);
    EXPECT_EQ(sm.checksum, 0x4E);
    EXPECT_EQ(sm.action, "C");
}