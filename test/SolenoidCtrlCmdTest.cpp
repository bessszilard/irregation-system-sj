#include <gtest/gtest.h>
#include "../embedded/main/SolenoidCtrlCmd.hpp"
#include "../embedded/main/PredefinedCommands.hpp"

using SCC = SolenoidCtrlCmd;
using LC  = LocalTime;

TEST(SolenoidCtrlCmdTest, ManualValidStr)
{
    SCC sm(CMD_MANUAL_CLOSE_ALL_RELAYS);
    EXPECT_EQ(sm.cmdType, CommandType::Manual);
    EXPECT_EQ(ToString(sm.relayTarget.relayId), ToString(RelayIds::AllRelays));
    EXPECT_EQ(sm.priority, CmdPriority::Priority0);
    EXPECT_EQ(sm.action, "C");
}

TEST(SolenoidCtrlCmdTest, AutoTimeStr)
{
    SCC sm(CMD_ATIME_SINGLE);
    EXPECT_EQ(sm.cmdType, CommandType::TimeSingleShot);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "S_C07:00->20:00");
}

TEST(SolenoidCtrlCmdTest, AutoTimeReapeatStr)
{
    SCC sm(CMD_ATIME_REPEAT);
    EXPECT_EQ(sm.cmdType, CommandType::TimeRepeat);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "R_X06:00->20:00_O01h_C20m");
}

TEST(SolenoidCtrlCmdTest, toString)
{
    for (const auto str : {CMD_MANUAL_CLOSE_ALL_RELAYS, CMD_ATIME_SINGLE, CMD_ATIME_SINGLE})
    {
        SCC sm(str);
        EXPECT_EQ(str, sm.toString());
    }
}

TEST(SolenoidCtrlCmdTest, ValidChecksum)
{
    SCC sm(CMD_MANUAL_CLOSE_RELAY1);
    EXPECT_EQ(sm.cmdType, CommandType::Manual);
    EXPECT_EQ(sm.relayTarget.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.priority, CmdPriority::Priority1);
    EXPECT_EQ(sm.checksum, 0x38);
    EXPECT_EQ(sm.action, "C");
}

TEST(SolenoidCtrlCmdTest, IsBetweenTimeRange)
{
    LC time1 = LC::Build(8, 0);
    LC time2 = LC::Build(23, 12);

    EXPECT_TRUE(SCC::IsWithinTimeRange("07:00->12:00", time1));
    EXPECT_FALSE(SCC::IsWithinTimeRange("07:00->12:00", time2));
    EXPECT_TRUE(SCC::IsWithinTimeRange("23:10->23:12", time2));
}

TEST(SolenoidCtrlCmdTest, RelayTimeRepeatCtrl)
{
    LC time1 = LC::Build(8, 0);
    LC time2 = LC::Build(23, 12);

    EXPECT_TRUE(SCC::IsWithinTimeRange("07:00->12:00", time1));
    EXPECT_FALSE(SCC::IsWithinTimeRange("07:00->12:00", time2));
    EXPECT_TRUE(SCC::IsWithinTimeRange("23:10->23:12", time2));
}

TEST(SolenoidCtrlCmdTest, RelayTimeSingleShotCtr)
{
    // RelayTimeSingleShotCtr
    LC time1           = LC::Build(1, 2);
    LC time2           = LC::Build(4, 12);
    String exampleCmd1 = "S_C00:00->04:00";
    String exampleCmd2 = "S_O04:00->23:00";

    EXPECT_EQ(SCC::RelayTimeSingleShotCtr(exampleCmd1, time1), RelayState::Closed);
    EXPECT_EQ(SCC::RelayTimeSingleShotCtr(exampleCmd1, time2), RelayState::Unknown);

    EXPECT_EQ(SCC::RelayTimeSingleShotCtr(exampleCmd2, time1), RelayState::Unknown);
    EXPECT_EQ(SCC::RelayTimeSingleShotCtr(exampleCmd2, time2), RelayState::Opened);
}

TEST(SolenoidCtrlCmdTest, RelayTimeRepeatCtr)
{
    String exampleCmd = "R_X06:00->20:00_O01h_C20m";

    EXPECT_EQ(SCC::RelayTimeRepeatCtrl(exampleCmd, LC::Build(1, 2)), RelayState::Unknown);
    EXPECT_EQ(SCC::RelayTimeRepeatCtrl(exampleCmd, LC::Build(6, 1)), RelayState::Opened);
    EXPECT_EQ(SCC::RelayTimeRepeatCtrl(exampleCmd, LC::Build(7, 0)), RelayState::Closed);
    EXPECT_EQ(SCC::RelayTimeRepeatCtrl(exampleCmd, LC::Build(7, 20)), RelayState::Opened);
    EXPECT_EQ(SCC::RelayTimeRepeatCtrl(exampleCmd, LC::Build(8, 21)), RelayState::Closed);
    EXPECT_EQ(SCC::RelayTimeRepeatCtrl(exampleCmd, LC::Build(9, 21)), RelayState::Opened);
    EXPECT_EQ(SCC::RelayTimeRepeatCtrl(exampleCmd, LC::Build(20, 01)), RelayState::Unknown);
}

TEST(SolenoidCtrlCmdTest, RelayThresholdCtrlFloatClosed)
{
    String exampleCmd = "C>010.0";
    EXPECT_EQ(SCC::RelayThresholdCtrl(exampleCmd, 11), RelayState::Closed);
    EXPECT_EQ(SCC::RelayThresholdCtrl(exampleCmd, 9), RelayState::Unknown);
}

TEST(SolenoidCtrlCmdTest, RelayThresholdCtrlFloatOpen)
{
    String exampleCmd = "O<050.0";
    EXPECT_EQ(SCC::RelayThresholdCtrl(exampleCmd, 11), RelayState::Opened);
    EXPECT_EQ(SCC::RelayThresholdCtrl(exampleCmd, 51), RelayState::Unknown);
}

TEST(SolenoidCtrlCmdTest, RelayRangeCtrl)
{
    String exampleCmd = "O<050.0_C>090.0";
    EXPECT_EQ(SCC::RelayRangeCtrl(exampleCmd, 11), RelayState::Opened);
    EXPECT_EQ(SCC::RelayRangeCtrl(exampleCmd, 51), RelayState::Unknown);
    EXPECT_EQ(SCC::RelayRangeCtrl(exampleCmd, 91), RelayState::Closed);
}
