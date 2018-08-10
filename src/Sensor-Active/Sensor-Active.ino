#include <SBNetwork.h>
#include "PowerHelper.h"
#include "Sensor.h"

SBNetwork networkDevice(true, 9, 10);

/*
   The active NRF24 device is sending sensor values in a specified time.
   Due to Radio.PowerDown, PowerSleep and ClockPreScaler, it's power consumption is 5.4uA in sleep mode.

   Uncomment and configure the sensor values in the function sendSensorValues
*/

void setup() {
  Serial.begin(115200);
  while (!Serial);

  randomSeed(getRandomSeed());
  SBMacAddress deviceMac(random(0, 256), random(0, 256), random(0, 256), random(0, 256), random(0, 256));
  networkDevice.initialize(deviceMac);

  Serial.println(F("*** PRESS 'N' to reset the device"));
}

void loop() {
  PowerHelper::setClockPrescaler(CLOCK_PRESCALER_1);
  PowerHelper::resetADC();

  receiveSerial();
  sendSensorValues();
  
  PowerHelper::setClockPrescaler(CLOCK_PRESCALER_16);
  PowerHelper::disableADC();
  PowerHelper::sleep(1175);
}

void receiveSerial() {
  if (Serial.available()) {
    char c = toupper(Serial.read());
    switch (c) {
      case 'N':
        networkDevice.resetData();
        break;
    }
  }
}

void sendSensorValues() {
  networkDevice.update();

  uint16_t voltage = Sensor::readVoltage(7, A6);
  sendNrf24(SENSOR_TYPE_VOLTAGE, (float)voltage);
  delay(10); // break to finish nrf24

  uint16_t humidity = Sensor::readHumidity(8, A0);
  sendNrf24(SENSOR_TYPE_HUMIDITY, (float)humidity);
  delay(10); // break to finish nrf24

  uint16_t photo = Sensor::readPhoto(8, A1);
  sendNrf24(SENSOR_TYPE_PHOTO, (float)photo);
  delay(10); // break to finish nrf24

  byte temp, hum = 0;
  Sensor::readDht11(7, 6, temp, hum);
  sendNrf24(SENSOR_TYPE_TEMPERATURE, (float)temp);
  sendNrf24(SENSOR_TYPE_HUMIDITYAIR, (float)hum);
  delay(10); // break to finish nrf24
}

void sendNrf24(uint8_t type, float value) {
  Serial.println(F("[NRF24] Send data... "));
  Serial.print(F("[NRF24] Type: "));
  Serial.println(type);
  Serial.print(F("[NRF24] Value: "));
  Serial.println(value);
  networkDevice.radio.powerUp();

  uint8_t version = 0x01;
  byte message[1 + 1 + 4];
  memcpy((void*)(message), &version, sizeof(version));
  memcpy((void*)(message + 1), &type, sizeof(type));
  memcpy((void*)(message + 2), &value, sizeof(value));
  networkDevice.sendToDevice(networkDevice.NetworkDevice.MasterMAC, message, 1 + 1 + 4);

  networkDevice.radio.powerDown();
  Serial.println(F("[NRF24] Send data... done"));
}

int getRandomSeed() {
  return analogRead(A0) + analogRead(A1) + analogRead(A2) + analogRead(A3) + analogRead(A4)
         + analogRead(A0) + analogRead(A1) + analogRead(A2) + analogRead(A3) + analogRead(A4);
}
