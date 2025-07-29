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

TEST(SensorData, getSensorByType)
{
    SensorData sd;
    int i                = 0;
    sd.tempOnSun_C       = i++;
    sd.tempInShadow_C    = i++;
    sd.humidity_RH       = i++;
    sd.flowDaySum_Lit    = i++;
    sd.flowRate_LitMin   = i++;
    sd.rainSensor        = i++;
    sd.lightSensor       = i++;
    sd.waterPressure_bar = i++;
    sd.soilMoisture1     = i++;

    i = 0;
    EXPECT_EQ(sd.get("TESU"), i++);
    EXPECT_EQ(sd.get("TESH"), i++);
    EXPECT_EQ(sd.get("HUMI"), i++);
    EXPECT_EQ(sd.get("FRDS"), i++);
    EXPECT_EQ(sd.get("FRLM"), i++);
    EXPECT_EQ(sd.get("RAIN"), i++);
    EXPECT_EQ(sd.get("LIGH"), i++);
    EXPECT_EQ(sd.get("WAPR"), i++);
}