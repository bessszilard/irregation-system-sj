#include <gtest/gtest.h>
#include "../embedded/main/SolenoidManager.hpp"

const std::string ValidManualStr    = "Manua;R01;Closed;P0;F";
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