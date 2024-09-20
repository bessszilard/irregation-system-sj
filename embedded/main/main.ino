
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
#include "time.h"

#include "Structures.hpp"
#include "Pinout.hpp"
#include "RelayArray.hpp"
#include "YFG1FlowMeter.hpp"
#include "LcdLayouts.hpp"

#define SEALEVELPRESSURE_HPA (1013.25)

void localTimeSetup();
void sensorSetup();
void setupWifiAndMqtt();

bool localTimeUpdate(LocalTime& p_data);
bool sensorDataUpdate(SensorsData& p_data);

// Replace with your network credentials
const char* ssid     = "Bbox-F6C5B3B2";
const char* password = "eNv3xEW4SXu9AEMnsC";

const char* ntpServer        = "pool.ntp.org";
const long gmtOffset_sec     = 3600 * 2;
const int daylightOffset_sec = 3600 * 0;

RelayArray relayArray(RELAY_ARRAY_DATA, RELAY_ARRAY_CLOCK, RELAY_ARRAY_LATCH);
OneWire oneWireForTemp(TEMPERATURE_DATA_PIN);
DallasTemperature tempSensor(&oneWireForTemp);
DHT humSensor(HUMIDITY_DATA_PIN, 11);
LcdLayouts lcdLayout;
YFG1FlowMeter fm(FLOW_METER_PIN);
DS1307 rtc;

BME280I2C bme; // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
// Default : forced mode, standby time = 1000 ms

bool wifiAvailable = false;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB
    }

    Wire.begin();

    setupWifiAndMqtt();
    localTimeSetup();
    sensorSetup();

    lcdLayout.init();
    lcdLayout.selectKeyBoardMode("banan");

    relayArray.setState(RelayIds::AllRelays, RelayState::Opened);
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    // fast loop
    // Process MQTT updates
    // Store the messages from the moisture nodes

    // slow loop
    // Update time
    LocalTime locTime;
    if (false == localTimeUpdate(locTime))
    {
        Serial.println("Invalid local time");
        return;
    }

    // Update sensors
    SensorsData sensorData;
    if (false == sensorDataUpdate(sensorData))
    {
        Serial.println("Invalid local time");
        return;
    }

    // Set relay states based on the given rules

    // Update relay state

    // publish to MQTT

    // Update LCD

    // heart beat
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

    delay(1000);
}

//---------------------------------------------------------------

void setupWifiAndMqtt()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    int maxTryToConnect = 5;
    int attempt         = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        attempt++;
        delay(500);
        Serial.print(".");
        if (maxTryToConnect <= attempt)
        {
            wifiAvailable = false;
            return;
        }
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    wifiAvailable = true;
}

void localTimeSetup()
{
    if (wifiAvailable)
    {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }
    rtc.begin();
    rtc.stop();  /*stop/pause RTC*/
    rtc.start(); /*start RTC*/
}

bool localTimeUpdate(LocalTime& p_data)
{
    if (false == wifiAvailable)
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

void sensorSetup()
{
    while (!bme.begin())
    {
        Serial.println("Could not find BME280 sensor!");
        delay(1000);
    }

    switch (bme.chipModel())
    {
        case BME280::ChipModel_BME280:
            Serial.println("Found BME280 sensor! Success.");
            break;
        case BME280::ChipModel_BMP280:
            Serial.println("Found BMP280 sensor! No Humidity available.");
            break;
        default:
            Serial.println("Found UNKNOWN sensor! Error!");
    }
}

bool sensorDataUpdate(SensorsData& p_data)
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