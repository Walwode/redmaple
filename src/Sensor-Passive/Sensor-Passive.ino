#include <SBNetwork.h>
#include "PowerHelper.h"
#include "Sensor.h"

SBNetwork networkDevice(true, 9, 10);

void setup() {
  Serial.begin(115200);
  while(!Serial);

  randomSeed(analogRead(A0) + analogRead(A1) + analogRead(A2) + analogRead(A3) + analogRead(A4));
  SBMacAddress deviceMac(random(0,256), random(0,256), random(0,256), random(0,256), random(0,256));
  networkDevice.initialize(deviceMac);

  Serial.println(F("*** PRESS 'N' to reset the device"));
}

void loop() {
  PowerHelper::setClockPrescaler(CLOCK_PRESCALER_1);
  PowerHelper::resetADC();

  if (Serial.available()) { char c = toupper(Serial.read()); if (c == 'N') { networkDevice.resetData(); } }
  networkDevice.update();
  
  uint16_t voltage = Sensor::readVoltage(7, A6);
  sendNrf24(SENSOR_TYPE_VOLTAGE, (float)voltage);
  PowerHelper::sleep(8); // break to finish nrf24
  
  uint16_t humidity = Sensor::readHumidity(8, A0);
  sendNrf24(SENSOR_TYPE_HUMIDITY, (float)humidity);
  PowerHelper::sleep(8); // break to finish nrf24

  /*
  uint16_t photo = Sensor::readPhoto(8, A1);
  sendNrf24(SENSOR_TYPE_PHOTO, (float)photo);
  PowerHelper::sleep(8); // break to finish nrf24
  
  byte temp, hum = 0;
  Sensor::readDht11(7, 6, temp, hum);
  sendNrf24(SENSOR_TYPE_TEMPERATURE, (float)temp);
  sendNrf24(SENSOR_TYPE_HUMIDITYAIR, (float)hum);
  PowerHelper::sleep(8); // break to finish nrf24
  */
  
  PowerHelper::setClockPrescaler(CLOCK_PRESCALER_16);
  PowerHelper::disableADC();
  // PowerHelper::sleep(32);
  PowerHelper::sleep(1175);
}

void sendNrf24(uint8_t type, float value) {
  Serial.println(F("[NRF24] Send data... "));
  Serial.print(F("[NRF24] Type: "));
  Serial.println(type);
  Serial.print(F("[NRF24] Value: "));
  Serial.println(value);
  networkDevice.radio.powerUp();
  
  uint8_t version = 0x01;
  byte message[1+1+4];
  memcpy((void*)(message), &version, sizeof(version));
  memcpy((void*)(message + 1), &type, sizeof(type));
  memcpy((void*)(message + 2), &value, sizeof(value));
  networkDevice.sendToDevice(networkDevice.NetworkDevice.MasterMAC, message, 1+1+4);

  networkDevice.radio.powerDown();
  Serial.println(F("[NRF24] Send data... done"));
  yield();
}