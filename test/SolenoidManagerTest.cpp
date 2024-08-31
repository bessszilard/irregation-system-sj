#include <gtest/gtest.h>
#include "../embedded/main/SolenoidManager.hpp"

const std::string ValidManualStr    = "Manua;R01;Closed;P0;F";
const std::string ValidAutomaticStr = "ATime;RXX;Opened;P5;15:00->20:00";

TEST(SampleTest, AddAndRemove)
{
    SolenoidManager sm;
    ASSERT_TRUE(true);

    sm.appendEvent(ValidManualStr);
    sm.appendEvent(ValidAutomaticStr);

    EXPECT_EQ(2, sm.getEventNumber());
    EXPECT_EQ(ValidManualStr, sm.getEventString(0));

    ASSERT_EQ(CommandState::CantRemove, sm.removeEvent(3));
    ASSERT_EQ(CommandState::Removed, sm.removeEvent(0));
    EXPECT_EQ(1, sm.getEventNumber());
    EXPECT_EQ(ValidAutomaticStr, sm.getEventString(0));
}