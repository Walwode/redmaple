#ifndef PHOTOSENSOR_H
#define PHOTOSENSOR_H

#include "Sensor.h"

class PhotoSensor : public Sensor {
private:
  
public:
  virtual float read();
  PhotoSensor(byte vccPin, byte dataPin);
};
#endif

PhotoSensor::PhotoSensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_PHOTO;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

float PhotoSensor::read() {
  Serial.print(F("Read light... "));
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  // for (int i = 0; i < 8; i++) analogRead(dataPin); // burn
  delay(10);
  float photoValue = analogRead(dataPin);
  delay(10);
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  Serial.println(photoValue);
  return photoValue;
}

