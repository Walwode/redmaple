#include "HumiditySensor.h"
#include "PowerHelper.h"
#include "PhotoSensor.h"
#include "RadioTransmitter.h"
#include "TemperatureSensor.h"
#include "VoltageSensor.h"

SBNetwork networkDevice(true, 9, 10);
RadioTransmitter transmitter;
PhotoSensor photoSensor(8, A1);
HumiditySensor humiditySensor(5, A0);
VoltageSensor voltageSensor(3, A2);
TemperatureSensor temperatureSensor(6, 7);

void setup() {
  Serial.begin(115200);
  while(!Serial);

  transmitter.initialize(networkDevice);
  
  randomSeed(analogRead(0) + millis());
  SBMacAddress deviceMac(random(0,256), random(0,256), random(0,256), random(0,256), random(0,256));
  networkDevice.initialize(deviceMac);

  Serial.println(F("*** PRESS 'N' to reset the device"));
}

void printBinary(const void* buf, uint8_t len) {
  Serial.print(F("Data Size: "));
  Serial.println(len);
  Serial.print(F("Binary: "));
  const uint8_t* curr = reinterpret_cast<const uint8_t*>(buf);
  for (int i = 0; i < len; i++) {  for (int j = 7; j >= 0; j--) Serial.print(bitRead(curr[i],j)); }
  Serial.println();
}

void loop() {
  PowerHelper::resetADC();

  if (Serial.available()) { char c = toupper(Serial.read()); if (c == 'N') { networkDevice.resetData(); } }
  networkDevice.update();
  
  transmitter.send(&photoSensor);
  doSleep(8); // break to finish nrf24
  transmitter.send(&humiditySensor);
  doSleep(8); // break to finish nrf24
  transmitter.send(&voltageSensor);
  doSleep(8); // break to finish nrf24
  transmitter.send(&temperatureSensor);
  
  PowerHelper::disableADC();
  // doSleep(1199);
  doSleep(16);
}

void doSleep(int seconds) {
  Serial.print(F("[SLEEP] Start sleep..."));
  delay(500);
  
  int cycles = seconds / 8;
  int currentCycle = 0;
  while (currentCycle < cycles) {
    PowerHelper::sleep(WDT_SLEEP_8S);
    currentCycle++;
    delay(500);
    Serial.print(F("."));
  }
  
  delay(500);
  Serial.println(F(" Done"));
}

