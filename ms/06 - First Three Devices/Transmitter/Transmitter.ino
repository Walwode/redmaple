#include "PreScaler.h"
#include "RadioTransmitter.h"
#include "PowerHelper.h"
#include "HumiditySensor.h"

// 20.03.2018 - 2.893
// 21.03.2018 - 3.253
// 25.03.2018 - 3.240
// 27.03.2018 - 3.230
// 31.03.2018 - 3.211
// 09.04.2018 - 3.180
// 16.04.2018 - 3.272

HumiditySensor* sensorHumidity;
// TemperatureSensor sensorTemperature;
// PhotoSensor sensorPhoto;
RadioTransmitter* transmitter;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  setClockPrescaler(CLOCK_PRESCALER_16);
  
  sensorHumidity = new HumiditySensor(8, A0);
  // sensorTemperature = new TemperatureSensor(<VccPin>, <DataPin>);
  // sensorPhoto = new PhotoSensor(<VccPin>, <DataPin>);
  
  transmitter = new RadioTransmitter(9, 10, "1bd91bbe");
}

void loop() {
  PowerHelper::resetADC();
  
  int humidity = sensorHumidity->read();
  // int temperature = sensorTemperature.read();
  // int photo = sensorPhoto.read();
  
  transmitter->send(sensorHumidity->type, humidity);
  // transmitter.send(temperature.type(), temperature);
  // transmitter.send(sensorPhoto.type(), photo);
  
  PowerHelper::disableADC();
  doSleep(1199);
}

void doSleep(int seconds) {
  Serial.print(F("Start sleep... "));
  
  int cycles = seconds / 8;
  int currentCycle = 0;
  while (currentCycle < cycles) {
    PowerHelper::sleep(WDT_SLEEP_8S);
    currentCycle++;
  }
  
  Serial.println(F("done"));
}

