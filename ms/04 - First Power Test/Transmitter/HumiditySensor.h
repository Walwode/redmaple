#ifndef HUMIDITYSENSOR_H
#define HUMIDITYSENSOR_H

#include "Sensor.h"

class HumiditySensor : Sensor {
private:
  
public:
  void read();
  HumiditySensor(byte vccPin, byte dataPin);
  ~HumiditySensor() {} // Destructor
};
#endif

HumiditySensor::HumiditySensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_HUMIDITY;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

