#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#define SENSOR_TYPE_VOLTAGE       0
#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3 

#include "SimpleDHT.h"

class TemperatureSensor {
private:
  
public:
  byte vccPin;
  byte dataPin;
  short type;
  
  bool read(int &temp, int &humi);
  TemperatureSensor(byte vccPin, byte dataPin);
  ~TemperatureSensor() {} // Destructor
};
#endif

TemperatureSensor::TemperatureSensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_TEMPERATURE;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

bool TemperatureSensor::read(int &temperature, int &humidity) {
  Serial.print(F("Read temperature... "));
  SimpleDHT11 dht11;
  byte temp = 0;
  byte hum = 0;
  byte data[40] = {0};
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
  return true;
}

