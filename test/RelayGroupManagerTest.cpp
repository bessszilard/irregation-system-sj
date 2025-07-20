#include <gtest/gtest.h>
#include "../embedded/main/RelayGroupManager.hpp"

class RelayGroupManagerTest : public RelayGroupManager
{
public:
    using RelayGroupManager::addRelay;
    using RelayGroupManager::groupAndIdFromStirng;
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

TEST(RelayGroupsTest, OnlyRelayIsOnlyInOneGroup)
{
    RelayGroupManagerTest rm;
    rm.addRelay(RelayGroups::A, RelayIds::Relay1);
    rm.addRelay(RelayGroups::B, RelayIds::Relay1);

    EXPECT_FALSE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    EXPECT_TRUE(rm.isInGroup(RelayGroups::B, RelayIds::Relay1));

    // std::cout << rm.toJson() << std::endl;
}

TEST(RelayGroupsTest, toJsonNoCrash)
{
    RelayGroupManagerTest rm;
    rm.addRelay(RelayGroups::A, RelayIds::Relay1);
    rm.addRelay(RelayGroups::B, RelayIds::Relay1);

    EXPECT_FALSE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    EXPECT_TRUE(rm.isInGroup(RelayGroups::B, RelayIds::Relay1));
    String json = rm.toJson();

    EXPECT_NE(json.length(), 0);
}

TEST(RelayGroupsTest, StringToGroupIds)
{
    String cmd = "RGC:R13;";

    RelayGroups group = RelayGroups::Unknown;
    RelayIds relayId  = RelayIds::Unknown;
    EXPECT_TRUE(RelayGroupManagerTest::groupAndIdFromStirng(cmd, group, relayId));

    EXPECT_EQ(group, RelayGroups::C);
    EXPECT_EQ(ToString(relayId), ToString(RelayIds::Relay13));
}

TEST(RelayGroupsTest, LoadFromString)
{
    String cmd = "RGA:R01;RGB:R02;RGC:R13;";

    RelayGroupManagerTest rm;
    EXPECT_TRUE(rm.loadFromStr(cmd));

    EXPECT_TRUE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    EXPECT_TRUE(rm.isInGroup(RelayGroups::B, RelayIds::Relay2));
    EXPECT_TRUE(rm.isInGroup(RelayGroups::C, RelayIds::Relay13));
}

TEST(RelayGroupsTest, LoadFromARray)
{
    uint16_t groupsArray[NUMBER_OF_RELAY_GROUPS] = {0};

    groupsArray[0] = 0x1;
    groupsArray[2] = 0x2;

    RelayGroupManagerTest rm;
    rm.loadfromFRAMArray(groupsArray);

    EXPECT_TRUE(rm.isInGroup(RelayGroups::A, RelayIds::Relay1));
    EXPECT_TRUE(rm.isInGroup(RelayGroups::C, RelayIds::Relay2));

    uint16_t groupsArrayOut[NUMBER_OF_RELAY_GROUPS] = {0};
    rm.getFRAMArray(groupsArrayOut);

    for (uint8_t i = 0; i < NUMBER_OF_RELAY_GROUPS; i++)
    {
        EXPECT_EQ(groupsArray[i], groupsArrayOut[i]);
    }
}
