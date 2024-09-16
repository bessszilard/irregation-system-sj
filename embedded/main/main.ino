
#include <BME280I2C.h>
#include <Wire.h>
#include <DS1307.h>

#include <PubSubClient.h>
#include <WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#include "Enums.hpp"
#include "Pinout.hpp"
#include "RelayArray.hpp"
#include "YFG1FlowMeter.hpp"
#include "LcdLayouts.hpp"

RelayArray relayArray(RELAY_ARRAY_DATA, RELAY_ARRAY_CLOCK, RELAY_ARRAY_LATCH);
OneWire oneWireForTemp(TEMPERATURE_DATA_PIN);
DallasTemperature tempSensor(&oneWireForTemp);
DHT humSensor(HUMIDITY_DATA_PIN, 11);

LcdLayouts lcdLayout;
// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
BME280I2C bme; // Default : forced mode, standby time = 1000 ms

YFG1FlowMeter fm(FLOW_METER_PIN);

uint8_t set_Sec    = 0;    /* Set the Seconds */
uint8_t set_Minute = 47;   /* Set the Minutes */
uint8_t set_Hour   = 3;    /* Set the Hours */
uint8_t set_Day    = 13;   /* Set the Day */
uint8_t set_Month  = 05;   /* Set the Month */
uint16_t set_Year  = 2022; /* Set the Year */

uint8_t sec, minute, hour, day, month;
uint16_t year;

DS1307 rtc;

#define SEALEVELPRESSURE_HPA (1013.25)

void sensorSetup();
void updateSensorData();

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    sensorSetup();
    lcdLayout.init();
    lcdLayout.selectKeyBoardMode("banan");

    relayArray.setState(RelayIds::AllRelays, RelayState::Opened);
    pinMode(LED_PIN, OUTPUT);

    rtc.begin();
    /*03:47:00 13.05.2022 //sec, min, hour, day, month, year*/
    rtc.set(set_Sec, set_Minute, set_Hour, set_Day, set_Month, set_Year);
    rtc.stop(); /*stop/pause RTC*/

    rtc.start(); /*start RTC*/
    delay(1000); /*Wait for 1000mS*/
    Serial.print("You have set: ");
    Serial.print("\nTime: ");
    Serial.print(set_Hour, DEC);
    Serial.print(":");
    Serial.print(set_Minute, DEC);
    Serial.print(":");
    Serial.print(set_Sec, DEC);

    Serial.print("\nDate: ");
    Serial.print(set_Day, DEC);
    Serial.print(".");
    Serial.print(set_Month, DEC);
    Serial.print(".");
    Serial.print(set_Year, DEC);
    Serial.println("");
}

void loop()
{
    // relayArray.knTestIncr();
    // updateSensorData();

    // heart beat
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(400);

    /*get time from RTC*/
    rtc.get(&sec, &minute, &hour, &day, &month, &year);

    /*serial output*/
    Serial.print("\nTime: ");
    Serial.print(hour, DEC);
    Serial.print(":");
    Serial.print(minute, DEC);
    Serial.print(":");
    Serial.print(sec, DEC);

    Serial.print("\nDate: ");
    Serial.print(day, DEC);
    Serial.print(".");
    Serial.print(month, DEC);
    Serial.print(".");
    Serial.print(year, DEC);
    Serial.println("");
    /*wait a second*/
    delay(1000);
}

//---------------------------------------------------------------

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

void updateSensorData()
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
}