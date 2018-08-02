#include "PreScaler.h"
#include "RadioTransmitter.h"
#include "PowerHelper.h"
#include "HumiditySensor.h"
#include "VoltageSensor.h"
#include "TemperatureSensor.h"
#include "PhotoSensor.h"

// 20.03.2018 - 2.893
// 21.03.2018 - 3.253
// 25.03.2018 - 3.240
// 27.03.2018 - 3.230
// 31.03.2018 - 3.211
// 09.04.2018 - 3.180
// 16.04.2018 - 3.272

VoltageSensor* sensorVoltage;
HumiditySensor* sensorHumidity;
// TemperatureSensor* sensorTemperature;
// PhotoSensor* sensorPhoto;
RadioTransmitter* transmitter;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  // setClockPrescaler(CLOCK_PRESCALER_16);
  
  sensorVoltage = new VoltageSensor(8, A2);
  sensorHumidity = new HumiditySensor(7, A0);
  // sensorTemperature = new TemperatureSensor(7, 6);
  // sensorPhoto = new PhotoSensor(8, A1);
  
  transmitter = new RadioTransmitter(9, 10, "c50c018c");
}

void loop() {
  PowerHelper::resetADC();
  
  int voltage = sensorVoltage->read();
  int humidity = sensorHumidity->read();
  // int photo = sensorPhoto->read();
  // int temperature;
  // int humidityRoom;
  // sensorTemperature->read(temperature, humidityRoom);
  
  transmitter->send(sensorVoltage->type, voltage);
  transmitter->send(sensorHumidity->type, humidity);
  // transmitter->send(sensorPhoto->type, photo);
  // transmitter->send(sensorTemperature->type, temperature);
  
  delay(rescaleDuration(500)); // break to finish nrf24
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

