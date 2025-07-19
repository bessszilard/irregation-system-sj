
#include <Wire.h>
#include <RtcDS3231.h>
#define MQTT_MAX_TRANSFER_SIZE 512 // or 256
#include <PubSubClient.h>
#include <WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>
#include <NTPClient.h> // https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
#include <WiFiUdp.h>
#include "uFire_SHT20.h"
#include "time.h"
#include "ADS1X15.h"

#include "Structures.hpp"
#include "Pinout.hpp"
#include "RelayArray.hpp"
#include "YFG1FlowMeter.hpp"
#include "LcdLayouts.hpp"
#include "MqttHandler.hpp"
#include "FramManager.hpp"

#include "SolenoidManager.hpp"

#define SEALEVELPRESSURE_HPA (1013.25)
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883

// TwoWire I2C_2 = TwoWire(0);
FramManager framM;

// Replace with your network credentials
const char* ssid     = "Bbox-F6C5B3B2";
const char* password = "eNv3xEW4SXu9AEMnsC";

const char* ntpServer        = "pool.ntp.org";
const long gmtOffset_sec     = 3600 * 2;
const int daylightOffset_sec = 3600 * 0;

RelayArray relayArray(RELAY_ARRAY_DATA, RELAY_ARRAY_CLOCK, RELAY_ARRAY_LATCH);
OneWire oneWireForTemp(TEMPERATURE_DATA_PIN);
DallasTemperature tempSensor(&oneWireForTemp);
uFire_SHT20 sht20;
DHT humSensor(HUMIDITY_DATA_PIN, 11);
LcdLayouts lcdLayout;
YFG1FlowMeter fm(FLOW_METER_PIN);
RtcDS3231<TwoWire> rtc(Wire);
ADS1115 ADS(0x48);

WifiSignalStrength wifiWifiSignalStrength = WifiSignalStrength::Unknown;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
MqttHandler mqttHd(&mqttClient);

// FRAM fram(&Wire);
// FramManager framM(&Wire);

SolenoidManager solM;
WifiSignalStrength filteredRssi = WifiSignalStrength::Unknown;

void localTimeSetup();
void sensorSetup();
bool setupWifiAndMqtt();

bool localTimeUpdate(LocalTime& p_data);
bool sensorDataUpdate(SensorData& p_data);
void reconnectMqtt();
void callback(char* topic, byte* message, unsigned int length);
void setupFRAM();

void setup()
{
    relayArray.setState(RelayIds::AllRelays, RelayState::Closed);

    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB
    }
    Wire.begin();
    lcdLayout.init();

    lcdLayout.connectingToSSID(ssid, false, 0);
    Serial.println("Setup started");

    sensorSetup();
    sht20.begin(); // TODOsz move to sensorSetup
    setupFRAM();

    ADS.begin(); // TODOsz move to sensorSetup

    // TODOsz move to the loop -> handle case if wifi not available at startup
    if (setupWifiAndMqtt())
    {
        lcdLayout.connectingToSSID(ssid, true);
    }
    localTimeSetup();

    // Serial2.begin(9600, SERIAL_8N1, HC12_RXD, HC12_TXD); // Hardware Serial of ESP32
}

LocalTime locTime;
SensorData sensorData;
RelayArrayStates relayStates(RelayState::Unknown);
wl_status_t oldWifiStatus              = WL_NO_SHIELD;
WifiSignalStrength oldRssi             = WifiSignalStrength::Unknown;
WifiSignalStrength filteredWifiSignals = WifiSignalStrength::Unknown;

char myData[50];

uint32_t slowLoopCalled_ms         = 0;     // Store the last time the code ran
uint32_t fastLoopCalled_ms         = 0;     // Store the last time the code ran
const uint32_t slowLoopInterval_ms = 20000; // 20 seconds in milliseconds
const uint32_t fastLoopInterval_ms = 1000;  // 1 seconds in milliseconds
bool storeCmdListToFRAMFlag        = false;
bool loadCmdListFromFRAMFlag       = false;

void loop()
{
    // if (Serial2.available() > 0)
    // {
    //     byte m    = Serial2.readBytesUntil('\n', myData, 50);
    //     myData[m] = '\0';
    //     Serial.println(myData);
    //     digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    // }

    uint32_t currentTime_ms = millis();

    // each each time -------------------------------------------------------------

    // we only check mqtt if Wifi is connected.
    if (WL_CONNECTED == WiFi.status())
    {
        mqttHd.loop();
    }

    // TODOsz move to separate function as updateRelayStateAndApply
    solM.updateRelayStates();
    bool atLeastOneRelayChanged = false;
    RelayExeInfo exeInfo;
    for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        solM.getRelayState(relayId, exeInfo);
        if (relayStates.getState(relayId) == exeInfo.currentState)
        {
            continue;
        }
        atLeastOneRelayChanged = true;
        // Only apply if change happened
        relayStates.setState(relayId, exeInfo.currentState);
        relayArray.setState(relayId, exeInfo.currentState);
    }
    // <<<

    if (atLeastOneRelayChanged)
    {
        String json;
        solM.getRelayStatesWithCmdIdsJson(json);
        mqttHd.publishRelayInfo(json);
    }

    if (storeCmdListToFRAMFlag)
    {
        storeCmdListToFRAMFlag = false;
        framM.saveCommands(solM.getCmdListStr());
    }
    if (loadCmdListFromFRAMFlag)
    {
        loadCmdListFromFRAMFlag = false;
        String cmdList;
        if (framM.loadCommands(cmdList))
        {
            Serial.print("Loadded commands: ");
            solM.loadCmdsFromString(cmdList);
            Serial.println(cmdList);
            mqttHd.publish(solM);
        }
        else
            Serial.println("Failed to load commands from FRAM");
    }

    // Fast loop - each seconds ----------------------------------------------------
    // TODOsz move to function as fastLoopEach_1sec
    if (currentTime_ms - fastLoopCalled_ms >= fastLoopInterval_ms)
    {
        fastLoopCalled_ms = currentTime_ms;
        if (false == localTimeUpdate(locTime))
        {
            Serial.println("Invalid local time");
            return;
        }
        mqttHd.publish(locTime);

        // Update sensors
        if (false == sensorDataUpdate(sensorData))
        {
            Serial.println("Failed to updateSensor data");
            return;
        }

        filteredRssi = ToWifiSignalStrength(Utils::GetSmoothedRSSI(WiFi.RSSI()));

        // Update LCD
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        // mqttHd.publish(sensorData);
        // String json;
        // solM.getRelayStatesWithCmdIdsJson(json);
        // mqttHd.publishRelayInfo(json);
        // mqttHd.publish(sensorData);
    }

    // Slow loop
    // TODOsz move to function as slowLoopEach_10sec
    if (currentTime_ms - slowLoopCalled_ms >= slowLoopInterval_ms)
    {
        slowLoopCalled_ms = currentTime_ms;
        mqttHd.publish(sensorData);
    }

    // Update LCD if needed
    // atLeastOneRelayChanged will be false next loop
    if (atLeastOneRelayChanged || oldRssi != filteredRssi || oldWifiStatus != WiFi.status())
    {
        oldWifiStatus = WiFi.status();
        oldRssi       = filteredRssi;
        lcdLayout.updateDef(WiFi.status(), WiFi.RSSI(), mqttHd.connected(), relayStates);
    }
}

//---------------------------------------------------------------
bool setupWifiAndMqtt()
//---------------------------------------------------------------

{
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    int maxTryToConnect = 5;
    int attempt         = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        lcdLayout.connectingToSSID(ssid, attempt);
        attempt++;
        delay(500);
        Serial.print(".");
        if (maxTryToConnect <= attempt)
        {
            return false;
        }
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Setting up MQTT connection");
    if (false == mqttHd.init(MQTT_SERVER, MQTT_PORT, callback))
    {
        Serial.println("Failed to set up the Mqtt server");
        return false;
    }
    return true;
}

//---------------------------------------------------------------
void localTimeSetup()
//---------------------------------------------------------------
{
    if (WiFi.status() == WL_CONNECTED)
    {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }
    rtc.Begin();
    // rtc.stop();  /*stop/pause RTC*/
    // rtc.start(); /*start RTC*/
}

//---------------------------------------------------------------
bool localTimeUpdate(LocalTime& p_data)
//---------------------------------------------------------------
{
    if (WiFi.status() != WL_CONNECTED)
    {
        LocalTime rtcTime(rtc.GetDateTime());
        // int year, month;
        // rtc.get(&rtcTime.tm_sec, &rtcTime.tm_min, &rtcTime.tm_hour, &rtcTime.tm_mday, &month, &year);
        p_data = rtcTime;
        Serial.println("Rtc time: " + rtcTime.toString());
        return true;
    }

    LocalTime ntpTime;
    if (getLocalTime(&ntpTime))
    {
        p_data = ntpTime;
        Serial.println("ntp time: " + ntpTime.toString());

        // RTC
        int year, month;
        LocalTime rtcTime(rtc.GetDateTime());
        // rtc.get(&rtcTime.tm_sec, &rtcTime.tm_min, &rtcTime.tm_hour, &rtcTime.tm_mday, &month, &year);
        // rtcTime.tm_mon  = month - 1;
        // rtcTime.tm_year = year - 1900;

        if (ntpTime.eq(rtcTime))
        {
            return true;
        }
        // not eq
        rtc.SetDateTime(ntpTime.toDt());

        // rtc.set(ntpTime.tm_sec,
        //         ntpTime.tm_min,
        //         ntpTime.tm_hour,
        //         ntpTime.tm_mday,
        //         ntpTime.tm_mon + 1,
        //         ntpTime.tm_year + 1900);

        // rtc.get(&rtcTime.tm_sec, &rtcTime.tm_min, &rtcTime.tm_hour, &rtcTime.tm_mday, &month, &year);
        // rtcTime.tm_mon  = month - 1;
        // rtcTime.tm_year = year - 1900;
        rtcTime = LocalTime(rtc.GetDateTime());
        Serial.printf("ntp = rtc time %s vs %s\n", ntpTime.toString().c_str(), rtcTime.toString().c_str());
        Serial.println("Update RTC");
        Serial.println("Rtc time: " + rtcTime.toString());
    }
    else
    {
        Serial.println("Failed to obtain time from NTP");
        LocalTime rtcTime(rtc.GetDateTime());
        Serial.println("Update RTC");
        Serial.println("Rtc time: " + rtcTime.toString());

        p_data = rtcTime;
    }
    return true;
}

//---------------------------------------------------------------
void sensorSetup()
//---------------------------------------------------------------
{
}

//---------------------------------------------------------------
bool sensorDataUpdate(SensorData& p_data)
//---------------------------------------------------------------
{
    p_data.valid = false;
    fm.updateFlowData();
    p_data.flowRate_LitMin = fm.getData().flowRate_LitMin;

    tempSensor.requestTemperatures();
    p_data.externalTemp_C = tempSensor.getTempCByIndex(0);
    p_data.humidity_RH    = sht20.humidity();

    ADS.setGain(0);
    p_data.rainSensor    = Utils::scaleTo99(ADS.readADC(0));
    p_data.lightSensor   = Utils::scaleTo99(ADS.readADC(1));
    p_data.soilMoisture1 = Utils::scaleTo99(ADS.readADC(2));
    p_data.soilMoisture2 = Utils::scaleTo99(ADS.readADC(3));

    p_data.valid = true;
    return true;
}

// TODOsz rename
//---------------------------------------------------------------
void callback(char* topic, byte* message, unsigned int length)
//---------------------------------------------------------------
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }

    // Feel free to add more if statements to control more GPIOs with MQTT

    // TODOsz String(topic) move to variable
    // Subscribed topics
    if (String(topic) == MQTT_SUB_ADD_CMD)
    {
        CommandState cmdState = solM.appendCmd(messageTemp);
        Serial.println(messageTemp + ">> " + ToString(cmdState));
        mqttHd.publish(cmdState);
        mqttHd.publish(solM);
    }
    else if (String(topic) == MQTT_SUB_REMOVE_CMD)
    {
        CommandState cmdState = solM.removeCmd(messageTemp);
        mqttHd.publish(cmdState);
        Serial.println(messageTemp + ">> " + ToString(cmdState));
        mqttHd.publish(solM);
    }
    else if (String(topic) == MQTT_SUB_OVERRIDE_CMD)
    {
        CommandState cmdState = solM.overrideCmd(messageTemp);
        mqttHd.publish(cmdState);
        Serial.println(messageTemp + ">> " + ToString(cmdState));
        mqttHd.publish(solM);
    }
    else if (String(topic) == MQTT_SUB_GET_COMMAND_OPTIONS)
    {
        String json;
        GetCommandBuilderJSON(json);
        mqttHd.publishCmdOptions(json);
    }
    else if (String(topic) == MQTT_SUB_GET_ALL_INFO)
    {
        String json;
        GetCommandBuilderJSON(json);
        mqttHd.publishCmdOptions(json);
        mqttHd.publish(solM);
        json = "";
        solM.getRelayStatesWithCmdIdsJson(json);
        mqttHd.publishRelayInfo(json);
        mqttHd.publish(solM.relayGroups());
        mqttHd.publish(sensorData);
    }
    else if (String(topic) == MQTT_SUB_SAVE_ALL_CMDS)
    {
        Serial.println("Save all commands");
        storeCmdListToFRAMFlag = true;
    }
    else if (String(topic) == MQTT_SUB_LOAD_ALL_CMDS)
    {
        Serial.println("MQTT_SUB_LOAD_ALL_CMDS");
        loadCmdListFromFRAMFlag = true;
    }
    else if (String(topic) == MQTT_SUB_RESET_CMDS_TO_DEFAULT)
    {
        Serial.println("MQTT_SUB_RESET_CMDS_TO_DEFAULT");
    }
    else if (String(topic) == MQTT_RELAY_GROUPS_SET)
    {
        // TODOsz remove this topic print later
        solM.relayGroups().loadFromStr(messageTemp);
        Serial.printf("RelayGroups: %s\n", solM.relayGroups().toJson().c_str());
        saveRelayGroupsFormFRAM();
        mqttHd.publish(solM.relayGroups());
    }
    else if (String(topic) == MQTT_RELAY_GROUPS_LOAD)
    {
        Serial.println(MQTT_RELAY_GROUPS_LOAD);
        loadRelayGroupsFormFRAM();
        mqttHd.publish(solM.relayGroups());
    }
}

// FRAM functions
void loadRelayGroupsFormFRAM()
{
    uint16_t relayGroupArray[NUMBER_OF_RELAY_GROUPS] = {0};
    if (framM.loadRelayGroups(relayGroupArray, NUMBER_OF_RELAY_GROUPS))
    {
        solM.relayGroups().loadfromFRAMArray(relayGroupArray);
    }
    else
    {
        Serial.println("Relay group array FRAM is empty");
    }
}

void saveRelayGroupsFormFRAM()
{
    uint16_t relayGroupArray[NUMBER_OF_RELAY_GROUPS] = {0};
    solM.relayGroups().getFRAMArray(relayGroupArray);
    if (framM.saveRelayGroups(relayGroupArray, NUMBER_OF_RELAY_GROUPS))
    {
        Serial.println("Save to group array");
    }
    else
    {
        Serial.println("Relay group array FRAM is empty");
    }
}

void setupFRAM()
{
    if (framM.begin())
    {
        Serial.println("Connected to FRAM");
    }
    else
    {
        Serial.println("Failed to connect to FRAM");
    }

    // TODOsz move to a function
    String cmdList;
    if (framM.loadCommands(cmdList))
    {
        Serial.print("Loadded commands: ");
        solM.loadCmdsFromString(cmdList);
    }
    else
    {
        solM.appendCmd("$Manua;P00;RXX;C#"); // Default
    }
    loadRelayGroupsFormFRAM();
}