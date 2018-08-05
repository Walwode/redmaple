// VCCPIN -> 330Ohm + 10kOhm -> DATAPIN -> 5kOhm -> GRND
#ifndef VOLTAGESENSOR_H
#define VOLTAGESENSOR_H

#include "Sensor.h"

class VoltageSensor : public Sensor {
private:
  
public:
  virtual float read();
  VoltageSensor(byte vccPin, byte dataPin);
};
#endif

VoltageSensor::VoltageSensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_VOLTAGE;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

float VoltageSensor::read() {
  Serial.print(F("Read voltage... "));
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  analogReference(INTERNAL);    // set the ADC reference to 1.1V
  for (int i = 0; i < 8; i++) analogRead(dataPin); // burn
  delay(10);
  int voltageValue = analogRead(dataPin);
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  Serial.println(voltageValue);
  analogReference(DEFAULT);    // set the ADC reference to original
  return voltageValue;
}

