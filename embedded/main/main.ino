#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include "Enums.hpp"
#include "Pinout.hpp"
#include "RelayArray.hpp"

RelayArray relayArray;
OneWire oneWireForTemp(TEMPERATURE_DATA);
DallasTemperature tempSensor(&oneWireForTemp);

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
}

void loop()
{
    tempSensor.requestTemperatures();
    Serial.print("Celsius temperature: ");
    Serial.print(tempSensor.getTempCByIndex(0));

    Serial.println("Good.");
    relayArray.setState(RelayIds::Relay1, RelayState::Closed);
    delay(1000);
    relayArray.setState(RelayIds::Relay1, RelayState::Opened);
    delay(100);
}
