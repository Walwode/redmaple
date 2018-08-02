#ifndef VOLTAGESENSOR_H
#define VOLTAGESENSOR_H

#define SENSOR_TYPE_VOLTAGE       0
#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3 

// VCCPIN -> 330Ohm + 10kOhm -> DATAPIN -> 5kOhm -> GRND

class VoltageSensor {
private:
  
public:
  byte vccPin;
  byte dataPin;
  short type;
  
  int read();
  VoltageSensor(byte vccPin, byte dataPin);
  ~VoltageSensor() {} // Destructor
};
#endif

VoltageSensor::VoltageSensor(byte vccPin, byte dataPin) {
  this->type = SENSOR_TYPE_VOLTAGE;
  this->vccPin = vccPin;
  this->dataPin = dataPin;
}

int VoltageSensor::read() {
  Serial.print(F("Read voltage... "));
  pinMode(vccPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(vccPin, HIGH);
  analogReference(INTERNAL);    // set the ADC reference to 1.1V
  for (int i = 0; i < 8; i++) analogRead(dataPin); // burn
  delay(rescaleDuration(10));
  int voltageValue = analogRead(dataPin);
  digitalWrite(vccPin, LOW);
  pinMode(vccPin, INPUT);  
  Serial.println(voltageValue);
  analogReference(DEFAULT);    // set the ADC reference to original
  return voltageValue;
}

