#include <SoftwareSerial.h>

#define LED_PIN 10
#define HC_TX 8
#define HC_RX 9
#define MOISTURE_INPUT A0

SoftwareSerial HC12(HC_TX, HC_RX); // HC-12 TX Pin, HC-12 RX Pin
String testString = "FieldNode1";

long measureBatteryVoltage_mV(); // https://forum.arduino.cc/t/can-an-arduino-measure-the-voltage-of-its-own-power-source/669954/4

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOISTURE_INPUT, INPUT);
}

int id = 0;
void loop() {
  // while (HC12.available()) {        // If HC-12 has data
  //   Serial.write(HC12.read());      // Send the data to Serial monitor
  // }
  // while (Serial.available()) {      // If Serial monitor has data
  //   HC12.write(Serial.read());      // Send that data to HC-12
  // }

  int value = analogRead(MOISTURE_INPUT);

  long batteryLevel_mV = measureBatteryVoltage_mV();
  String sendString = testString + " " + String(id++) + " " + String(value) + " " + String(batteryLevel_mV) + "\n";
  HC12.write(sendString.c_str());
  Serial.println(sendString);
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  delay(1000);  
}

//------------------------------------
long measureBatteryVoltage_mV() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}


