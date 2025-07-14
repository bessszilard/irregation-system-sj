#include <gtest/gtest.h>
#include "../embedded/main/RelayGroupManager.hpp"

TEST(RelayGroupsTest, NumberOfRelayGroups)
{
    EXPECT_EQ(NUMBER_OF_RELAY_GROUPS, 8);
}

TEST(RelayGroupsTest, RelayGroupToArrayPos)
{
    EXPECT_EQ(RelayGroupToArrayPos(RelayGroups::A), 0);
    EXPECT_EQ(RelayGroupToArrayPos(RelayGroups::H), 7);
}

TEST(RelayGroupsTest, AddRelay)
{
    RelayGroupManager rm;
    rm.addRelay(RelayGroups::A, RelayIds::Relay1);

    EXPECT_TRUE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));

    // rest should be empty
    for (RelayIds relayId = RelayIds::Relay2; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        EXPECT_FALSE(rm.isInGroup(RelayGroups::A, relayId));
    }

    for (RelayGroups group = RelayGroups::B; group < RelayGroups::NumberOfGroups; group = incRelayGroup(group))
    {
        for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
        {
            EXPECT_FALSE(rm.isInGroup(group, relayId));
        }
    }
}

TEST(RelayGroupsTest, RemoveRelay)
{
    RelayGroupManager rm;
    rm.addRelay(RelayGroups::A, RelayIds::Relay1);

    EXPECT_TRUE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    rm.removeRelay(RelayGroups::A, RelayIds::Relay1);

    EXPECT_FALSE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
}

TEST(RelayGroupsTest, OnlyRelayIsOnlyInOneGroup)
{
    RelayGroupManager rm;
    rm.addRelay(RelayGroups::A, RelayIds::Relay1);
    rm.addRelay(RelayGroups::B, RelayIds::Relay1);

    EXPECT_FALSE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    EXPECT_TRUE(rm.isInGroup(RelayGroups::B, RelayIds::Relay1));

    std::cout << rm.toJson() << std::endl;
}