#include <gtest/gtest.h>
#include "../embedded/main/SolenoidCtrlCmd.hpp"

const std::string ValidManualStr    = "Manua;R01;Closed;P0;F";
const std::string ValidAutomaticStr = "ATime;RXX;Opened;P5;>15:00";

TEST(SolenoidCtrlCmdTest, ManualValidStr)
{
    SolenoidCtrlCmd sm(ValidManualStr);
    ASSERT_EQ(sm.cmdType, CommandType::ManCtrl);
    ASSERT_EQ(sm.relayId, RelayIds::Relay1);
    ASSERT_EQ(sm.relayState, RelayState::Closed);
    ASSERT_EQ(sm.priority, CmdPriority::Priority0);
    ASSERT_EQ(sm.action, "F");
}

TEST(SolenoidCtrlCmdTest, AutomaticValidStr)
{
    SolenoidCtrlCmd sm(ValidAutomaticStr);
    ASSERT_EQ(sm.cmdType, CommandType::AutoTimeCtrl);
    ASSERT_EQ(sm.relayId, RelayIds::AllRelays);
    ASSERT_EQ(sm.relayState, RelayState::Opened);
    ASSERT_EQ(sm.priority, CmdPriority::Priority5);
    ASSERT_EQ(sm.action, ">15:00");
}