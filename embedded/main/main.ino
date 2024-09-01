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

RelayArray relayArray;
OneWire oneWireForTemp(TEMPERATURE_DATA_PIN);
DallasTemperature tempSensor(&oneWireForTemp);
DHT humSensor(HUMIDITY_DATA_PIN, 11);

BME280I2C bme; // Default : forced mode, standby time = 1000 ms
               // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

#define SEALEVELPRESSURE_HPA (1013.25)

void setup()
{
    int relayPins[NUMBER_OF_RELAYS] = {RELAY_1_PIN,
                                       RELAY_2_PIN,
                                       RELAY_3_PIN,
                                       RELAY_4_PIN,
                                       RELAY_5_PIN,
                                       RELAY_6_PIN,
                                       RELAY_7_PIN,
                                       RELAY_8_PIN,
                                       RELAY_9_PIN,
                                       RELAY_10_PIN,
                                       RELAY_11_PIN,
                                       RELAY_12_PIN};
    relayArray.init(relayPins);
    Serial.begin(115200);
    Wire.begin();

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

void loop()
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

    Serial.println("Good.");
    relayArray.setState(RelayIds::Relay1, RelayState::Closed);
    delay(1000);
    relayArray.setState(RelayIds::Relay1, RelayState::Opened);
    delay(100);
}
