#ifndef SENSOR_H
#define SENSOR_H

#define SENSOR_TYPE_VOLTAGE       0
#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3 
#define SENSOR_TYPE_GAS           4 

class Sensor {
private:
  
public:
  uint8_t type;
  byte vccPin;
  byte dataPin;
  
  // Sensor(uint8_t type, byte vccPin, byte dataPin);
  virtual ~Sensor() {} // Destructor
  
  virtual float read() = 0;
};
#endif
