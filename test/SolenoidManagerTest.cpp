#include <gtest/gtest.h>
#include "../embedded/main/SolenoidManager.hpp"
#include "../embedded/main/Enums.hpp"
#include "../embedded/main/PredefinedCommands.hpp"

#include <vector>
#include <algorithm>

#define EXPECT_EQS(A, B) EXPECT_STREQ(ToString(A).c_str(), ToString(B).c_str())

const String CmdsRaw =
    String(CMD_MANUAL_CLOSE_ALL_RELAYS) + "_" + CMD_MANUAL_OPEN_ALL_RELAYS + "_" + CMD_ATIME_SINGLE + "_";

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

    EXPECT_EQ(CommandState::Added, sm.appendCmd(CMD_MANUAL_CLOSE_ALL_RELAYS));
    EXPECT_EQ(CommandState::Added, sm.appendCmd(CMD_ATIME_SINGLE));

    EXPECT_EQ(2, sm.getCmdNumber());
    EXPECT_EQ(CMD_MANUAL_CLOSE_ALL_RELAYS, sm.getCmdString(0));

    ASSERT_EQ(CommandState::CantRemove, sm.removeCmd(3));
    ASSERT_EQ(CommandState::Removed, sm.removeCmd(0));
    EXPECT_EQ(1, sm.getCmdNumber());
    EXPECT_EQ(CMD_ATIME_SINGLE, sm.getCmdString(0));
}

TEST(SampleTest, ManualControlCloseAll)
{
    SolenoidManager sm;
    EXPECT_EQ(CommandState::Added, sm.appendCmd(CMD_MANUAL_CLOSE_ALL_RELAYS));

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
    EXPECT_EQ(CommandState::Added, sm.appendCmd(CMD_MANUAL_OPEN_ALL_RELAYS));

    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;
    for (uint8_t i = 0; i < NUMBER_OF_RELAYS; ++i)
    {
        sm.getRelayState(ToRelayId(i), relayExeInfo);
        execInfoTest(relayExeInfo, CmdPriority::Priority1, 0, RelayState::Opened);
    }
}

TEST(SampleTest, ManualControlCloseAllOpenTheFirst)
{
    SolenoidManager sm;
    const std::string OpenFirstCmdStr = "$Manual;P01;R01;O#";

    EXPECT_EQS(CommandState::Added, sm.appendCmd(CMD_MANUAL_CLOSE_ALL_RELAYS));
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

TEST(SampleTest, CloseAllOpenGroupBRelays)
{
    SolenoidManager sm;
    ASSERT_TRUE(true);

    sm.appendCmd(CMD_MANUAL_CLOSE_ALL_RELAYS);
    sm.updateRelayStates();
}

TEST(SolenoidManager, AddRemoveRelayToGroup) {
    SolenoidManager sm;

    RelayIds targetRelay = RelayIds::Relay16;
    RelayGroups targetGroup = RelayGroups::C;
    sm.relayGroups().addRelay(targetGroup, targetRelay);

    // only target group and target relay
    for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        for (RelayGroups group = RelayGroups::A; group < RelayGroups::NumberOfGroups; group = incRelayGroup(group))
        {
            EXPECT_EQ(sm.relayGroups().isInGroup(group, relayId), targetRelay == relayId && targetGroup == group);
        }
    }
}

TEST(SampleTest, CloseAllRelaysOpenGroupB)
{
    const std::string OpenGroupB             = "$Manual;P01;RGB;O#";
    const std::vector<RelayIds> targetRelays = {RelayIds::Relay1, RelayIds::Relay5, RelayIds::Relay6};
    const RelayGroups targetGroup            = RelayGroups::B;

    SolenoidManager sm;
    for (const RelayIds& relayId : targetRelays)
    {
        sm.relayGroups().addRelay(targetGroup, relayId);
    }

    sm.appendCmd(CMD_MANUAL_CLOSE_ALL_RELAYS);
    sm.appendCmd(OpenGroupB);
    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;
    for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        sm.getRelayState(relayId, relayExeInfo);

        if (std::find(targetRelays.begin(), targetRelays.end(), relayId) != targetRelays.end())
        {
            execInfoTest(relayExeInfo, CmdPriority::Priority1, 1, RelayState::Opened);
        }
        else
        {
            execInfoTest(relayExeInfo, CmdPriority::Priority0, 0, RelayState::Closed);
        }
    }
}

TEST(SampleTest, OpenAllRelaysCloseGroupF)
{
    const std::string CloseGroupF            = "$Manual;P09;RGF;C#";
    const std::vector<RelayIds> targetRelays = {RelayIds::Relay3, RelayIds::Relay7, RelayIds::Relay14};
    const RelayGroups targetGroup            = RelayGroups::F;

    SolenoidManager sm;
    for (const RelayIds& relayId : targetRelays)
    {
        sm.relayGroups().addRelay(targetGroup, relayId);
    }

    sm.appendCmd(CMD_MANUAL_OPEN_ALL_RELAYS);
    sm.appendCmd(CloseGroupF);
    sm.updateRelayStates();

    RelayExeInfo relayExeInfo;
    for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        sm.getRelayState(relayId, relayExeInfo);
        if (std::find(targetRelays.begin(), targetRelays.end(), relayId) != targetRelays.end())
        {
            execInfoTest(relayExeInfo, CmdPriority::Priority9, 1, RelayState::Closed);
        }
        else
        {
            execInfoTest(relayExeInfo, CmdPriority::Priority1, 0, RelayState::Opened);
        }
    }
}

TEST(SolenoidManager, LoadCmdsFromString)
{
    SolenoidManager sm;
    EXPECT_TRUE(sm.loadCmdsFromString(AllCommandExamples));
    EXPECT_EQ(sm.getCmdNumber(), AllCommandExamplesCnt);
    EXPECT_EQ(sm.getCmdListStr(), AllCommandExamples);
}

TEST(SolenoidManager, ResetCommands)
{
    SolenoidManager sm;
    EXPECT_TRUE(sm.loadCmdsFromString(AllCommandExamples));
    EXPECT_EQ(sm.getCmdNumber(), AllCommandExamplesCnt);
    EXPECT_EQ(sm.getCmdListStr(), AllCommandExamples);

    EXPECT_TRUE(sm.loadCmdsFromString(CMD_MANUAL_CLOSE_ALL_RELAYS));
    EXPECT_EQ(sm.getCmdNumber(), 1);

    sm.updateRelayStates();

    // EXPECT_EQ(sm.getCmdListStr(), CMD_MANUAL_CLOSE_ALL_RELAYS);
}