#include <BME280I2C.h>
#include <Wire.h>

#include <PubSubClient.h>
#include <WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#include "Enums.hpp"
#include "Pinout.hpp"
#include "RelayArray.hpp"

RelayArray relayArray(RELAY_ARRAY_DATA_1, RELAY_ARRAY_CLOCK, RELAY_ARRAY_LATCH);
OneWire oneWireForTemp(TEMPERATURE_DATA_PIN);
DallasTemperature tempSensor(&oneWireForTemp);
DHT humSensor(HUMIDITY_DATA_PIN, 11);

BME280I2C bme; // Default : forced mode, standby time = 1000 ms
               // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

#define SEALEVELPRESSURE_HPA (1013.25)

void sensorSetup();
void updateSensorData();

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    sensorSetup();
}

void loop()
{
    updateSensorData();

    Serial.println("Good.");
    relayArray.setState(RelayIds::Relay1, RelayState::Closed);
    relayArray.setState(RelayIds::Relay2, RelayState::Opened);
    delay(1000);
    relayArray.setState(RelayIds::Relay1, RelayState::Opened);
    relayArray.setState(RelayIds::Relay2, RelayState::Closed);
    delay(100);
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