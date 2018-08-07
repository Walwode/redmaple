#ifndef SENSOR_H
#define SENSOR_H

#define SENSOR_TYPE_VOLTAGE       0
#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3 
#define SENSOR_TYPE_GAS           4 

#include "SimpleDHT.h"

class Sensor {
private:
  
public:
  static uint16_t readVoltage(byte vccPin, byte dataPin);
  static uint16_t readHumidity(byte vccPin, byte dataPin);
  static uint16_t readPhoto(byte vccPin, byte dataPin);
  static void readDht11(byte vccPin, byte dataPin, byte &temp, byte &hum) {
};
#endif

uint16_t Sensor::readVoltage(byte vccPin, byte dataPin) {
  Serial.print(F("[SENSOR] Read voltage... "));
  
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  analogReference(INTERNAL);    // set the ADC reference to 1.1V
  
  for (int i = 0; i < 8; i++) analogRead(dataPin); // burn
  delay(10);
  uint16_t voltageValue = analogRead(dataPin);
  
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  analogReference(DEFAULT);    // set the ADC reference to original
  
  Serial.println(voltageValue);
  return voltageValue;
}

uint16_t Sensor::readHumidity(byte vccPin, byte dataPin) {
  Serial.print(F("[SENSOR] Read humidity... "));
  
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  
  for (int i = 0; i < 8; i++) analogRead(dataPin); // burn
  delay(500);
  uint16_t humidityValue = 1023 - analogRead(dataPin);
  
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);
  
  Serial.println(humidityValue);
  return humidityValue;
}

uint16_t Sensor::readPhoto(byte vccPin, byte dataPin) {
  Serial.print(F("[SENSOR] Read photo... "));
  
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  
  for (int i = 0; i < 8; i++) analogRead(dataPin); // burn
  delay(10);
  uint16_t photoValue = analogRead(dataPin);
  delay(10);
  
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  
  Serial.println(photoValue);
  return photoValue;
}

void Sensor::readDht11(byte vccPin, byte dataPin, byte &temp, byte &hum) {
  Serial.println(F("[SENSOR] Read DHT11... "));
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  
  temp, hum = 0;
  byte data[40] = {0};
  SimpleDHT11 dht11;
  while (dht11.read(dataPin, &temp, &hum, data)) {}
  
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);
  
  Serial.print(F("[SENSOR] Temperature... "));
  Serial.println((int)temp);
  Serial.print(F("[SENSOR] Humidity... "));
  Serial.println((int)hum);
}

