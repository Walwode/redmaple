#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include "SimpleDHT.h"
#include "Sensor.h"

class TemperatureSensor : public Sensor {
private:
  
public:
  virtual float read();
  TemperatureSensor(byte vccPin, byte dataPin);
};
#endif

TemperatureSensor::TemperatureSensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_TEMPERATURE;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

float TemperatureSensor::read() {
  Serial.print(F("Read temperature... "));
  SimpleDHT11 dht11;
  byte temp, hum = 0;
  byte data[40] = {0};
  float temperature, humidity;
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  while (dht11.read(dataPin, &temp, &hum, data)) {}
  temperature = (int)temp;
  humidity = (int)hum;
  Serial.print(temperature);
  Serial.print(F(",humidity... "));
  Serial.println(humidity);
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);
  return temperature;
}

