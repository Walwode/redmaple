#ifndef HUMIDITYSENSOR_H
#define HUMIDITYSENSOR_H

#define SENSOR_TYPE_VOLTAGE       0
#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3 

class HumiditySensor {
private:
  
public:
  byte vccPin;
  byte dataPin;
  short type;
  
  int read();
  HumiditySensor(byte vccPin, byte dataPin);
  ~HumiditySensor() {} // Destructor
};
#endif

HumiditySensor::HumiditySensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_HUMIDITY;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

int HumiditySensor::read() {
  Serial.print(F("Read humidity... "));
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  delay(rescaleDuration(500));
  int humidityValue = analogRead(dataPin);
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  Serial.println(humidityValue);
  return humidityValue;
}

