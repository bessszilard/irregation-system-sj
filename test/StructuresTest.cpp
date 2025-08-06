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
    sd.soilMoistureLocal = i++;

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

TEST(SensorData, setSensorByType)
{
    SensorData sd;
    float i = 0.0;
    sd.set(SensorType::TempOnSun, i++);
    sd.set(SensorType::TempInShadow, i++);
    sd.set(SensorType::Humidity, i++);
    sd.set(SensorType::FlowRateDailySum, i++);
    sd.set(SensorType::FlowRateLitPerMin, i++);
    sd.set(SensorType::Rain, i++);
    sd.set(SensorType::Light, i++);
    sd.set(SensorType::WaterPressure, i++);
    sd.set(SensorType::SoilMoistureLocal, i++);

    i = 0;
    EXPECT_EQ(sd.get(SensorType::TempOnSun), i++);
    EXPECT_EQ(sd.get(SensorType::TempInShadow), i++);
    EXPECT_EQ(sd.get(SensorType::Humidity), i++);
    EXPECT_EQ(sd.get(SensorType::FlowRateDailySum), i++);
    EXPECT_EQ(sd.get(SensorType::FlowRateLitPerMin), i++);
    EXPECT_EQ(sd.get(SensorType::Rain), i++);
    EXPECT_EQ(sd.get(SensorType::Light), i++);
    EXPECT_EQ(sd.get(SensorType::WaterPressure), i++);
    EXPECT_EQ(sd.get(SensorType::SoilMoistureLocal), i++);
}

TEST(SensorData, GetLightFromADC)
{
    EXPECT_EQ(SensorData::GetLightFromADC(1), 0);
    EXPECT_EQ(SensorData::GetLightFromADC(1), 0);
    EXPECT_EQ(SensorData::GetLightFromADC(26000), 99);
}

TEST(SensorData, GetSoilMoistureFromADC)
{
    EXPECT_EQ(SensorData::GetSoilMoistureFromADC(5000), 99);
    EXPECT_EQ(SensorData::GetSoilMoistureFromADC(14000), 0);
}