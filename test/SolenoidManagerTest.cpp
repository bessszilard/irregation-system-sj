#include <gtest/gtest.h>
#include "../embedded/main/SolenoidManager.hpp"
#include "../embedded/main/Enums.hpp"

const std::string ValidManualStr    = "Manua;R01;Closed;P0;F";
const std::string ValidManualAllOpenedStr = "Manua;RXX;Opened;P0;F";
const std::string ValidAutomaticStr = "ATime;RXX;Opened;P5;15:00->20:00";

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
    const std::string ValidManualAllClosedStr = "Manua;RXX;Closed;P0;F";

    EXPECT_EQ(CommandState::Added, sm.appendCmd(ValidManualAllClosedStr));

    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;
    for (uint8_t i = 0; i < NUMBER_OF_RELAYS; ++i)
    {
        sm.getRelayState(ToRelayId(i), relayExeInfo);
        // std::cout << relayExeInfo.toString() << std::endl;
        ASSERT_EQ(CmdPriority::Priority0, relayExeInfo.priority);
        ASSERT_EQ(0, relayExeInfo.cmdIdx);
        ASSERT_EQ(RelayState::Closed, relayExeInfo.currentState);
    }
}

TEST(SampleTest, ManualControlOpenAll)
{
    SolenoidManager sm;
    const std::string ValidManualAllClosedStr = "Manua;RXX;Opened;P0;F";

    EXPECT_EQ(CommandState::Added, sm.appendCmd(ValidManualAllClosedStr));

    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;
    for (uint8_t i = 0; i < NUMBER_OF_RELAYS; ++i)
    {
        sm.getRelayState(ToRelayId(i), relayExeInfo);
        EXPECT_EQ(CmdPriority::Priority0, relayExeInfo.priority);
        EXPECT_EQ(0, relayExeInfo.cmdIdx);
        EXPECT_EQ(RelayState::Opened, relayExeInfo.currentState);
    }
}