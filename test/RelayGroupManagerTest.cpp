#include <gtest/gtest.h>
#include "../embedded/main/RelayGroupManager.hpp"

class RelayGroupManagerTest : public RelayGroupManager
{
public:
    using RelayGroupManager::addRelay;
    using RelayGroupManager::groupAndIdFromStirng;
    using RelayGroupManager::removeRelay;
};

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
    RelayGroupManagerTest rm;
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
    RelayGroupManagerTest rm;
    rm.addRelay(RelayGroups::A, RelayIds::Relay1);

    EXPECT_TRUE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    rm.removeRelay(RelayGroups::A, RelayIds::Relay1);

    EXPECT_FALSE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
}

TEST(RelayGroupsTest, OnlyRelayIsOnlyInOneGroup)
{
    RelayGroupManagerTest rm;
    rm.addRelay(RelayGroups::A, RelayIds::Relay1);
    rm.addRelay(RelayGroups::B, RelayIds::Relay1);

    EXPECT_FALSE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    EXPECT_TRUE(rm.isInGroup(RelayGroups::B, RelayIds::Relay1));

    std::cout << rm.toJson() << std::endl;
}

TEST(RelayGroupsTest, StringToGroupIds)
{
    String cmd = "RGC;R13";

    RelayGroups group = RelayGroups::Unknown;
    RelayIds relayId  = RelayIds::Unknown;
    RelayGroupManagerTest::groupAndIdFromStirng(cmd, group, relayId);

    std::cout << ToString(group) << " vs " << ToString(relayId) << std::endl;

    EXPECT_EQ(group, RelayGroups::C);
    EXPECT_EQ(ToString(relayId), ToString(RelayIds::Relay13));
}