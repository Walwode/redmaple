#ifndef HUMIDITYSENSOR_H
#define HUMIDITYSENSOR_H

#include "Sensor.h"

class HumiditySensor : public Sensor {
private:
  
public:
  virtual float read();
  HumiditySensor(byte vccPin, byte dataPin);
};
#endif

HumiditySensor::HumiditySensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_HUMIDITY;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

float HumiditySensor::read() {
  Serial.print(F("Read humidity... "));
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  delay(500);
  int humidityValue = analogRead(dataPin);
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  Serial.println(humidityValue);
  return humidityValue;
}

