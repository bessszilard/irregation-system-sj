#include <gtest/gtest.h>
#include "../embedded/main/SolenoidCtrlCmd.hpp"
#include "../embedded/main/PredefinedCommands.hpp"

TEST(SolenoidCtrlCmdTest, ManualValidStr)
{
    SolenoidCtrlCmd sm(CMD_MANUAL_CLOSE_ALL_RELAYS);
    EXPECT_EQ(sm.cmdType, CommandType::Manual);
    EXPECT_EQ(ToString(sm.relayTarget.relayId), ToString(RelayIds::AllRelays));
    EXPECT_EQ(sm.priority, CmdPriority::Priority0);
    EXPECT_EQ(sm.action, "C");
}

TEST(SolenoidCtrlCmdTest, AutoTimeStr)
{
    SolenoidCtrlCmd sm(CMD_ATIME_SINGLE);
    EXPECT_EQ(sm.cmdType, CommandType::TimeSingleShot);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "S_C07:00->20:00");
}

TEST(SolenoidCtrlCmdTest, AutoTimeReapeatStr)
{
    SolenoidCtrlCmd sm(CMD_ATIME_REPEAT);
    EXPECT_EQ(sm.cmdType, CommandType::TimeRepeat);
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
    EXPECT_EQ(sm.cmdType, CommandType::Manual);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority1);
    EXPECT_EQ(sm.checksum, 0x38);
    EXPECT_EQ(sm.action, "C");
}

TEST(SolenoidCtrlCmdTest, IsBetweenRangeTime)
{
    LocalTime time1 = LocalTime::Build(8, 0);
    LocalTime time2 = LocalTime::Build(23, 12);

    EXPECT_TRUE(SolenoidCtrlCmd::IsWithinTimeRange("07:00->12:00", time1));
    EXPECT_FALSE(SolenoidCtrlCmd::IsWithinTimeRange("07:00->12:00", time2));
    EXPECT_TRUE(SolenoidCtrlCmd::IsWithinTimeRange("23:10->23:12", time2));
}

TEST(SolenoidCtrlCmdTest, RelayThresholdCtrlFloatClosed)
{
    String exampleCmd = "C>010.0";
    EXPECT_EQ(SolenoidCtrlCmd::RelayThresholdCtrl(exampleCmd, 11), RelayState::Closed);
    EXPECT_EQ(SolenoidCtrlCmd::RelayThresholdCtrl(exampleCmd, 9), RelayState::Unknown);
}

TEST(SolenoidCtrlCmdTest, RelayThresholdCtrlFloatOpen)
{
    String exampleCmd = "O<050.0";
    EXPECT_EQ(SolenoidCtrlCmd::RelayThresholdCtrl(exampleCmd, 11), RelayState::Opened);
    EXPECT_EQ(SolenoidCtrlCmd::RelayThresholdCtrl(exampleCmd, 51), RelayState::Unknown);
}
