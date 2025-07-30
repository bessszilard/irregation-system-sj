#include <gtest/gtest.h>
#include "../embedded/main/SolenoidCtrlCmd.hpp"
#include "../embedded/main/PredefinedCommands.hpp"

class SolenoidCtrlCmdTest : public SolenoidCtrlCmd
{
public:
    using SolenoidCtrlCmd::ApplyRangeCtrl;
    using SolenoidCtrlCmd::ApplyThresholdCtrl;
    using SolenoidCtrlCmd::IsWithingRange;
    using SolenoidCtrlCmd::IsWithinTimeRange;
    using SolenoidCtrlCmd::RelaySensorRangeCtrl;
    using SolenoidCtrlCmd::RelaySensorThresholdCtrl;
    using SolenoidCtrlCmd::RelaySenThTimeRangeRepCtrl;
    using SolenoidCtrlCmd::RelayTimeRepeatCtrl;
    using SolenoidCtrlCmd::RelayTimeSingleShotCtrl;
    using SolenoidCtrlCmd::SolenoidCtrlCmd;
};

using SCC = SolenoidCtrlCmdTest;
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

TEST(SolenoidCtrlCmdTest, IsWithingRange)
{
    EXPECT_TRUE(SCC::IsWithingRange('>', 25.0, 24.0));
    EXPECT_FALSE(SCC::IsWithingRange('<', 25.0, 24.0));

    EXPECT_TRUE(SCC::IsWithingRange('<', 25.0, 26.0));
    EXPECT_FALSE(SCC::IsWithingRange('>', 25.0, 26.0));
}

TEST(SolenoidCtrlCmdTest, RelayTimeRepeatCtrl)
{
    LC time1 = LC::Build(8, 0);
    LC time2 = LC::Build(23, 12);

    EXPECT_TRUE(SCC::IsWithinTimeRange("07:00->12:00", time1));
    EXPECT_FALSE(SCC::IsWithinTimeRange("07:00->12:00", time2));
    EXPECT_TRUE(SCC::IsWithinTimeRange("23:10->23:12", time2));
}

TEST(SolenoidCtrlCmdTest, RelayTimeSingleShotCtrl)
{
    // RelayTimeSingleShotCtrl
    LC time1           = LC::Build(1, 2);
    LC time2           = LC::Build(4, 12);
    String exampleCmd1 = "S_C00:00->04:00";
    String exampleCmd2 = "S_O04:00->23:00";

    EXPECT_EQ(SCC::RelayTimeSingleShotCtrl(exampleCmd1, time1), RelayState::Closed);
    EXPECT_EQ(SCC::RelayTimeSingleShotCtrl(exampleCmd1, time2), RelayState::Unknown);

    EXPECT_EQ(SCC::RelayTimeSingleShotCtrl(exampleCmd2, time1), RelayState::Unknown);
    EXPECT_EQ(SCC::RelayTimeSingleShotCtrl(exampleCmd2, time2), RelayState::Opened);
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

TEST(SolenoidCtrlCmdTest, ApplyThresholdCtrlFloatClosed)
{
    String exampleCmd = "C>010.0";
    EXPECT_EQ(SCC::ApplyThresholdCtrl(exampleCmd, 11), RelayState::Closed);
    EXPECT_EQ(SCC::ApplyThresholdCtrl(exampleCmd, 9), RelayState::Unknown);
}

TEST(SolenoidCtrlCmdTest, ApplyThresholdCtrlFloatOpen)
{
    String exampleCmd = "O<050.0";
    EXPECT_EQ(SCC::ApplyThresholdCtrl(exampleCmd, 11), RelayState::Opened);
    EXPECT_EQ(SCC::ApplyThresholdCtrl(exampleCmd, 51), RelayState::Unknown);
}

TEST(SolenoidCtrlCmdTest, ApplyRangeCtrl)
{
    String exampleCmd = "O<050.0_C>090.0";
    EXPECT_EQ(SCC::ApplyRangeCtrl(exampleCmd, 11), RelayState::Opened);
    EXPECT_EQ(SCC::ApplyRangeCtrl(exampleCmd, 51), RelayState::Unknown);
    EXPECT_EQ(SCC::ApplyRangeCtrl(exampleCmd, 91), RelayState::Closed);
}

TEST(SolenoidCtrlCmdTest, RelaySensorThresholdCtrl)
{
    String exampleCmd = "RAIN_C>040.0";
    SensorData sd;
    sd.rainSensor = 0;
    EXPECT_EQ(SCC::RelaySensorThresholdCtrl(exampleCmd, sd), RelayState::Unknown);
    sd.rainSensor = 41;
    EXPECT_EQ(SCC::RelaySensorThresholdCtrl(exampleCmd, sd), RelayState::Closed);
}

TEST(SolenoidCtrlCmdTest, RelaySensorRangeCtrl)
{
    String exampleCmd = "TESU_O>025.0_C<010.5";
    SensorData sd;
    sd.tempOnSun_C = 24;
    EXPECT_EQ(SCC::RelaySensorRangeCtrl(exampleCmd, sd), RelayState::Unknown);

    sd.tempOnSun_C = 26;
    EXPECT_EQ(SCC::RelaySensorRangeCtrl(exampleCmd, sd), RelayState::Opened);

    sd.tempOnSun_C = 11;
    EXPECT_EQ(SCC::RelaySensorRangeCtrl(exampleCmd, sd), RelayState::Unknown);

    sd.tempOnSun_C = 9;
    EXPECT_EQ(SCC::RelaySensorRangeCtrl(exampleCmd, sd), RelayState::Closed);
}

TEST(SolenoidCtrlCmdTest, RelaySenThTimeRangeRepCtrl)
{
    String exampleCmd = "TESH_X>030.0_R_X06:00->20:00_O01h_C20m";

    SensorData sd;
    sd.tempInShadow_C = 31;

    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(1, 2)), RelayState::Unknown);
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(6, 1)), RelayState::Opened);
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(7, 0)), RelayState::Closed);
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(7, 20)), RelayState::Opened);
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(8, 21)), RelayState::Closed);
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(9, 21)), RelayState::Opened);
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(20, 01)), RelayState::Unknown);

    sd.tempInShadow_C = 29;
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(6, 1)), RelayState::Unknown);
    EXPECT_EQ(SCC::RelaySenThTimeRangeRepCtrl(exampleCmd, sd, LC::Build(7, 0)), RelayState::Unknown);
}
