#include <gtest/gtest.h>
#include "../embedded/main/SolenoidManager.hpp"
#include "../embedded/main/Enums.hpp"

#define EXPECT_EQS(A, B) EXPECT_STREQ(ToString(A).c_str(), ToString(B).c_str())

const std::string ValidManualStr          = "Manua;R01;Closed;P00;F";
const std::string ValidManualAllOpenedStr = "Manua;RXX;Opened;P00;F";
const std::string ValidAutomaticStr       = "ATime;RXX;Opened;P05;15:00->20:00";

void execInfoTest(const RelayExeInfo& p_execInfo, CmdPriority p_cmdPriority, uint8_t p_cmdId, RelayState p_relayState)
{
    EXPECT_EQS(p_cmdPriority, p_execInfo.priority);
    EXPECT_EQ(p_cmdId, p_execInfo.cmdIdx);
    EXPECT_EQS(p_relayState, p_execInfo.currentState);
}

TEST(SampleTest, AddAndRemove)
{
    SolenoidManager sm;
    ASSERT_TRUE(true);

    EXPECT_EQ(CommandState::Added, sm.appendCmd(ValidManualStr));
    EXPECT_EQ(CommandState::Added, sm.appendCmd(ValidAutomaticStr));

    EXPECT_EQ(2, sm.getCmdNumber());
    EXPECT_EQ(ValidManualStr, sm.getCmdString(0));

    ASSERT_EQ(CommandState::CantRemove, sm.removeCmd(3));
    ASSERT_EQ(CommandState::Removed, sm.removeCmd(0));
    EXPECT_EQ(1, sm.getCmdNumber());
    EXPECT_EQ(ValidAutomaticStr, sm.getCmdString(0));
}

TEST(SampleTest, ManualControlCloseAll)
{
    SolenoidManager sm;
    const std::string ValidManualAllClosedStr = "Manua;RXX;Closed;P00;F";

    EXPECT_EQ(CommandState::Added, sm.appendCmd(ValidManualAllClosedStr));

    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;
    for (uint8_t i = 0; i < NUMBER_OF_RELAYS; ++i)
    {
        sm.getRelayState(ToRelayId(i), relayExeInfo);
        execInfoTest(relayExeInfo, CmdPriority::Priority0, 0, RelayState::Closed);
    }
}

TEST(SampleTest, ManualControlOpenAll)
{
    SolenoidManager sm;
    const std::string ValidManualAllClosedStr = "Manua;RXX;Opened;P00;F";

    EXPECT_EQ(CommandState::Added, sm.appendCmd(ValidManualAllClosedStr));

    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;
    for (uint8_t i = 0; i < NUMBER_OF_RELAYS; ++i)
    {
        sm.getRelayState(ToRelayId(i), relayExeInfo);
        execInfoTest(relayExeInfo, CmdPriority::Priority0, 0, RelayState::Opened);
    }
}

TEST(SampleTest, ManualControlCloseAllOpenTheFirst)
{
    SolenoidManager sm;
    const std::string ClosedCmdStr    = "Manua;RXX;Closed;P00;F";
    const std::string OpenFirstCmdStr = "Manua;R01;Opened;P01;F";

    EXPECT_EQS(CommandState::Added, sm.appendCmd(ClosedCmdStr));
    EXPECT_EQS(CommandState::Added, sm.appendCmd(OpenFirstCmdStr));

    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;

    sm.getRelayState(RelayIds::Relay1, relayExeInfo);
    execInfoTest(relayExeInfo, CmdPriority::Priority1, 1, RelayState::Opened);

    for (uint8_t i = 1; i < NUMBER_OF_RELAYS; ++i)
    {
        sm.getRelayState(ToRelayId(i), relayExeInfo);
        execInfoTest(relayExeInfo, CmdPriority::Priority0, 0, RelayState::Closed);
    }
}