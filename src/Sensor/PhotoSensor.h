#ifndef PHOTOSENSOR_H
#define PHOTOSENSOR_H

#define SENSOR_TYPE_VOLTAGE       0
#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3 

// VCC -> Photo -> PIN -> 5k resitor -> GRND

class PhotoSensor {
private:
  
public:
  byte vccPin;
  byte dataPin;
  short type;
  
  int read();
  PhotoSensor(byte vccPin, byte dataPin);
  ~PhotoSensor() {} // Destructor
};
#endif

PhotoSensor::PhotoSensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_PHOTO;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

int PhotoSensor::read() {
  Serial.print(F("Read light... "));
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  // for (int i = 0; i < 8; i++) analogRead(dataPin); // burn
  delay(rescaleDuration(10));
  int photoValue = analogRead(dataPin);
  delay(rescaleDuration(10));
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  Serial.println(photoValue);
  return photoValue;
}

