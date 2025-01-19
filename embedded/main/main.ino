
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

#define SEALEVELPRESSURE_HPA (1013.25)
// #define MQTT_SERVER "test.mosquitto.org"
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883
char clientID[20];

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

SignalStrength wifiSignalStrength = SignalStrength::Unknown;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
MqttHandler mqttHd(&mqttClient);

FRAM fram(&Wire);

void localTimeSetup();
void sensorSetup();
bool setupWifiAndMqtt();

bool localTimeUpdate(LocalTime& p_data);
bool sensorDataUpdate(SensorData& p_data);
void reconnectMqtt();

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

    // Serial2.begin(9600, SERIAL_8N1, HC12_RXD, HC12_TXD); // Hardware Serial of ESP32
}

bool opened = false;
LocalTime locTime;
SensorData sensorData;
RelayArrayStates relayStates(RelayState::Opened);
char myData[50];

void loop()
{
    // Serial.println(" >> Free banana");

    if (Serial2.available() > 0)
    {
        byte m    = Serial2.readBytesUntil('\n', myData, 50);
        myData[m] = '\0';
        Serial.println(myData);
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    mqttHd.loop();

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
    // if (false == localTimeUpdate(locTime))
    // {
    //     Serial.println("Invalid local time");
    //     return;
    // }

    // // Update sensors
    // if (false == sensorDataUpdate(sensorData))
    // {
    //     Serial.println("Invalid local time");
    //     return;
    // }
    // // char humString[8];
    // // dtostrf(sensorData.humidity, 1, 2, humString);
    // // Serial.print("Humidity: ");
    // // Serial.println(humString);

    mqttHd.publish(sensorData);
    mqttHd.publish(relayStates);

    // Serial.println((String)sht20.humidity() + " %RH");
    // Serial.println();

    // delay(1000);

    // // Set relay states based on the given rules

    // relayStates.states[1]  = RelayState::Closed;
    // relayStates.states[8]  = opened ? RelayState::Opened : RelayState::Closed;
    // relayStates.states[0]  = (false == opened) ? RelayState::Opened : RelayState::Closed;
    // relayStates.states[2]  = (false == opened) ? RelayState::Opened : RelayState::Closed;
    // relayStates.states[3]  = (false == opened) ? RelayState::Opened : RelayState::Closed;
    // relayStates.states[4]  = (false == opened) ? RelayState::Opened : RelayState::Closed;
    // relayStates.states[9]  = (false == opened) ? RelayState::Opened : RelayState::Closed;
    // relayStates.states[15] = (false == opened) ? RelayState::Opened : RelayState::Closed;
    // opened                 = !opened;

    // // Update relay state
    // relayArray.update(relayStates);

    // // publish to MQTT

    // Update LCD
    lcdLayout.updateDef(WiFi.status(), WiFi.RSSI(), false, relayStates);
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(1000);
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
    if (false == mqttHd.init(MQTT_SERVER, MQTT_PORT))
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
    fm.updateFlowData();
    tempSensor.requestTemperatures();
    Serial.print("Celsius temperature: ");
    Serial.println(tempSensor.getTempCByIndex(0));
    Serial.print("Humidity: ");
    Serial.println(humSensor.readHumidity());

    float temp(NAN), hum(NAN), pres(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    bme.read(pres, temp, hum, tempUnit, presUnit);

    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print("°" + String(tempUnit == BME280::TempUnit_Celsius ? 'C' : 'F'));
    Serial.print("\t\tHumidity: ");
    Serial.print(hum);
    Serial.print("% RH");
    Serial.print("\t\tPressure: ");
    Serial.print(pres);
    Serial.println("Pa");

    return true;
}