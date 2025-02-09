
#include <BME280I2C.h>
#include <Wire.h>
#include <DS1307.h> // https://www.electronicwings.com/esp32/rtc-ds1307-interfacing-with-esp32

#include <PubSubClient.h>
#include <WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>
#include <NTPClient.h> // https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
#include <WiFiUdp.h>
#include "FRAM.h"
#include "uFire_SHT20.h"
#include "time.h"

#include "Structures.hpp"
#include "Pinout.hpp"
#include "RelayArray.hpp"
#include "YFG1FlowMeter.hpp"
#include "LcdLayouts.hpp"
#include "MqttHandler.hpp"

#include "SolenoidManager.hpp"

#define SEALEVELPRESSURE_HPA (1013.25)
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883

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
DS1307 rtc;

BME280I2C bme; // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
// Default : forced mode, standby time = 1000 ms

WifiSignalStrength wifiWifiSignalStrength = WifiSignalStrength::Unknown;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
MqttHandler mqttHd(&mqttClient);

FRAM fram(&Wire);

SolenoidManager solM;
WifiSignalStrength filteredRssi = WifiSignalStrength::Unknown;

void localTimeSetup();
void sensorSetup();
bool setupWifiAndMqtt();

bool localTimeUpdate(LocalTime& p_data);
bool sensorDataUpdate(SensorData& p_data);
void reconnectMqtt();
void callback(char* topic, byte* message, unsigned int length);

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB
    }
    Wire.begin();
    lcdLayout.init();

    lcdLayout.connectingToSSID(ssid, false, 0);
    if (setupWifiAndMqtt())
    {
        lcdLayout.connectingToSSID(ssid, true);
    }
    localTimeSetup();
    sensorSetup();
    sht20.begin();

    relayArray.setState(RelayIds::AllRelays, RelayState::Opened);
    pinMode(LED_PIN, OUTPUT);

    solM.appendCmd("Manua;RXX;Closed;P00;F");

    // Serial2.begin(9600, SERIAL_8N1, HC12_RXD, HC12_TXD); // Hardware Serial of ESP32
}

bool opened = false;
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

void loop()
{
    if (Serial2.available() > 0)
    {
        byte m    = Serial2.readBytesUntil('\n', myData, 50);
        myData[m] = '\0';
        Serial.println(myData);
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    uint32_t currentTime_ms = millis();

    // each each time -------------------------------------------------------------

    // we only check mqtt if Wifi is connected.
    if (WL_CONNECTED == WiFi.status())
    {
        mqttHd.loop();
    }

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
    if (atLeastOneRelayChanged)
    {
        mqttHd.publishRelayInfo(solM.getRelayStatesWithCmdIdsJson());
    }

    // Fast loop - each seconds ----------------------------------------------------
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
    }

    // Slow loop
    if (currentTime_ms - slowLoopCalled_ms >= slowLoopInterval_ms)
    {
        slowLoopCalled_ms = currentTime_ms;
        mqttHd.publish(sensorData);
    }

    // int rv = fram.begin(0x50);
    // if (rv != 0)
    // {
    //     Serial.println(rv);
    // }
    // else
    // {
    //     Serial.println();
    //     Serial.println(__FUNCTION__);
    //     Serial.print("ManufacturerID: ");
    //     Serial.println(fram.getManufacturerID());
    //     Serial.print("     ProductID: ");
    //     Serial.println(fram.getProductID());
    //     Serial.print("     memory KB: ");
    //     Serial.println(fram.getSize());

    //     Serial.println();
    // }
    // Serial.println("done...");

    // fast loop
    // Process MQTT updates
    // Store the messages from the moisture nodes

    // slow loop
    // Update time

    // atLeastOneRelayChanged will be false next loop
    if (atLeastOneRelayChanged || oldRssi != filteredRssi || oldWifiStatus != WiFi.status())
    {
        Serial.println(ToShortString(oldRssi) + " != " + ToShortString(filteredRssi));
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
    rtc.begin();
    rtc.stop();  /*stop/pause RTC*/
    rtc.start(); /*start RTC*/
}

//---------------------------------------------------------------
bool localTimeUpdate(LocalTime& p_data)
//---------------------------------------------------------------
{
    if (WiFi.status() != WL_CONNECTED)
    {
        LocalTime rtcTime;
        int year, month;
        rtc.get(&rtcTime.tm_sec, &rtcTime.tm_min, &rtcTime.tm_hour, &rtcTime.tm_mday, &month, &year);
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
        LocalTime rtcTime;
        rtc.get(&rtcTime.tm_sec, &rtcTime.tm_min, &rtcTime.tm_hour, &rtcTime.tm_mday, &month, &year);
        rtcTime.tm_mon  = month - 1;
        rtcTime.tm_year = year - 1900;

        if (ntpTime.eq(rtcTime))
        {
            return true;
        }

        rtc.set(ntpTime.tm_sec,
                ntpTime.tm_min,
                ntpTime.tm_hour,
                ntpTime.tm_mday,
                ntpTime.tm_mon + 1,
                ntpTime.tm_year + 1900);

        rtc.get(&rtcTime.tm_sec, &rtcTime.tm_min, &rtcTime.tm_hour, &rtcTime.tm_mday, &month, &year);
        rtcTime.tm_mon  = month - 1;
        rtcTime.tm_year = year - 1900;
        Serial.println("Update RTC");
        Serial.println("Rtc time: " + rtcTime.toString());
    }
    else
    {
        Serial.println("Failed to obtain time from NTP");
        LocalTime rtcTime;
        int year, month;
        rtc.get(&rtcTime.tm_sec, &rtcTime.tm_min, &rtcTime.tm_hour, &rtcTime.tm_mday, &month, &year);
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
    // TODOsz update
    // while (!bme.begin())
    // {
    //     Serial.println("Could not find BME280 sensor!");
    //     delay(1000);
    // }

    // switch (bme.chipModel())
    // {
    //     case BME280::ChipModel_BME280:
    //         Serial.println("Found BME280 sensor! Success.");
    //         break;
    //     case BME280::ChipModel_BMP280:
    //         Serial.println("Found BMP280 sensor! No Humidity available.");
    //         break;
    //     default:
    //         Serial.println("Found UNKNOWN sensor! Error!");
    // }
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

    p_data.humidity = sht20.humidity();

    p_data.valid = true;
    return true;
}

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
}