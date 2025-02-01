#include <gtest/gtest.h>
#include "../embedded/main/SolenoidCtrlCmd.hpp"

const std::string ValidManualStr    = "Manua;R01;Closed;P00;F";
const std::string ValidAutomaticStr = "ATime;RXX;Opened;P05;15:00->20:00";

TEST(SolenoidCtrlCmdTest, ManualValidStr)
{
    SolenoidCtrlCmd sm(ValidManualStr);
    EXPECT_EQ(sm.cmdType, CommandType::ManCtrl);
    EXPECT_EQ(sm.relayId, RelayIds::Relay1);
    EXPECT_EQ(sm.relayState, RelayState::Closed);
    EXPECT_EQ(sm.priority, CmdPriority::Priority0);
    EXPECT_EQ(sm.action, "F");
}

TEST(SolenoidCtrlCmdTest, AutomaticValidStr)
{
    SolenoidCtrlCmd sm(ValidAutomaticStr);
    EXPECT_EQ(sm.cmdType, CommandType::AutoTimeCtrl);
    EXPECT_EQ(sm.relayId, RelayIds::AllRelays);
    EXPECT_EQ(sm.relayState, RelayState::Opened);
    EXPECT_EQ(sm.priority, CmdPriority::Priority5);
    EXPECT_EQ(sm.action, "15:00->20:00");
}

TEST(SolenoidCtrlCmdTest, toString)
{
    SolenoidCtrlCmd sm(ValidAutomaticStr);
    EXPECT_EQ(ValidAutomaticStr, sm.toString());
}