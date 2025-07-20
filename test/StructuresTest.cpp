#include <gtest/gtest.h>
#include "../embedded/main/Structures.hpp"

TEST(RelayTargetTest, SingleRelay)
{
    RelayTarget rt = RelayTarget::FromString("R12");
    EXPECT_EQ(ToString(rt.type), ToString(RelayTargetType::SingleRelay));
    EXPECT_EQ(ToString(rt.relayId), ToString(RelayIds::Relay12));
    EXPECT_EQ(ToString(rt.groupId), ToString(RelayGroups::Unknown));
}

TEST(RelayTargetTest, RelayGroup)
{
    RelayTarget rt = RelayTarget::FromString("RGF");
    EXPECT_EQ(ToString(rt.type), ToString(RelayTargetType::Group));
    EXPECT_EQ(ToString(rt.relayId), ToString(RelayIds::Unknown));
    EXPECT_EQ(ToString(rt.groupId), ToString(RelayGroups::F));
}

TEST(RelayTargetTest, AllRelays)
{
    RelayTarget rt = RelayTarget::FromString("RXX");
    EXPECT_EQ(ToString(rt.type), ToString(RelayTargetType::All));
    EXPECT_EQ(ToString(rt.relayId), ToString(RelayIds::AllRelays));
    EXPECT_EQ(ToString(rt.groupId), ToString(RelayGroups::Unknown));
}
