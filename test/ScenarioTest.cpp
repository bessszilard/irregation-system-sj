#include <gtest/gtest.h>
#include "../embedded/main/SolenoidManager.hpp"
#include "../embedded/main/Enums.hpp"
#include "../embedded/main/PredefinedCommands.hpp"

// TODOsz move this to a general folder
#define EXPECT_EQS(A, B) EXPECT_STREQ(ToString(A).c_str(), ToString(B).c_str())

void execInfoTest2(const RelayExeInfo& p_execInfo, CmdPriority p_cmdPriority, uint8_t p_cmdId, RelayState p_relayState,
                   const char* context = "")
{
    SCOPED_TRACE(context);  // adds context to GTest failure output

    EXPECT_EQS(p_cmdPriority, p_execInfo.priority);
    EXPECT_EQ(p_cmdId, p_execInfo.cmdIdx);
    EXPECT_EQS(p_relayState, p_execInfo.currentState);
}

TEST(ScenarioTest, OpenAllRelaysCloseGroupF)
{
    SolenoidManager sm;
    EXPECT_EQ(sm.appendCmd("$Manual;P00;RXX;C#"), CommandState::Added);
    EXPECT_EQ(sm.appendCmd("$SenThr;P05;R02;TESH_O>025.1#"), CommandState::Added);
    EXPECT_EQ(sm.appendCmd("$SenThr;P05;R01;SMLC_O<040.0#"), CommandState::Added);

    sm.sensors().set(SensorType::SoilMoistureLocal, 0);
    sm.updateRelayStates(true);

    RelayExeInfo relayExeInfo;
    sm.getRelayState(RelayIds::Relay1, relayExeInfo);
    execInfoTest2(relayExeInfo, CmdPriority::Priority5, 2, RelayState::Opened, "Relay1 check");

    for (RelayIds relayId = RelayIds::Relay2; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        sm.getRelayState(relayId, relayExeInfo);
        execInfoTest2(relayExeInfo, CmdPriority::Priority0, 0, RelayState::Closed, (ToString(relayId) + " check").c_str());
    }
    // sm.sensors().set(SensorType::TempInShadow, 30);

    // sm.getRelayState(RelayIds::Relay1, relayExeInfo);
    // execInfoTest2(relayExeInfo, CmdPriority::Priority5, 1, RelayState::Opened);
}
