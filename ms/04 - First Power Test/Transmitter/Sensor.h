#ifndef SENSOR_H
#define SENSOR_H

#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3

class Sensor {
private:
  
public:
  byte vccPin;
  byte dataPin;
  short type;
  
  virtual void read();
  virtual Sensor();
  virtual Sensor(int type, byte vccPin, byte dataPin);
  virtual ~Sensor() {} // Destructor
  int sensorType();
};
#endif

int Sensor::sensorType() {
  return type;
}

