#include "SBNetwork.h"
#include "PowerHelper.h"
#include "Sensor.h"
#include <MemoryFree.h>

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
  // PowerHelper::sleep(16);
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
  uint16_t voltage = Sensor::readVoltage(5, A3);
  sendNrf24(SENSOR_TYPE_VOLTAGE, (float)voltage);
  delay(50); // break to finish nrf24

  uint16_t humidity = Sensor::readHumidity(7, A1);
  sendNrf24(SENSOR_TYPE_HUMIDITY, (float)humidity);
  delay(50); // break to finish nrf24

  /*
  uint16_t photo = Sensor::readPhoto(8, A1);
  sendNrf24(SENSOR_TYPE_PHOTO, (float)photo);
  delay(50); // break to finish nrf24

  byte temp, hum = 0;
  Sensor::readDht11(7, 6, temp, hum);
  sendNrf24(SENSOR_TYPE_TEMPERATURE, (float)temp);
  sendNrf24(SENSOR_TYPE_HUMIDITYAIR, (float)hum);
  delay(50); // break to finish nrf24
  */
}

void sendNrf24(uint8_t type, float value) {
  Serial.print(F("[NRF24] Send data: "));
  Serial.print(F("Type = "));
  Serial.print(type);
  Serial.print(F(", Value = "));
  Serial.print(value);
  Serial.print(F("..."));
  
  networkDevice.radio.powerUp();
  bool receivedConfirmation = false;
  int sendAttempt = 0;

  while (!receivedConfirmation && (sendAttempt < 3)) {
    sendAttempt++;
    Serial.print(F("."));
    
    doSendValue(type, value);
    unsigned long timeout = millis() + 5000;
    while ((millis() < timeout) && !receivedConfirmation) {
      receivedConfirmation = checkConfirmation(type);
      delay(100);
    }
  }

  networkDevice.radio.powerDown();
  receivedConfirmation ? Serial.println(F(" done")) : Serial.println(F(" timeout"));
}

void doSendValue(uint8_t type, float value) {
  uint8_t version = 0x02;
  byte message[1 + 1 + 4];
  memcpy((void*)(message), &version, sizeof(version));
  memcpy((void*)(message + 1), &type, sizeof(type));
  memcpy((void*)(message + 2), &value, sizeof(value));
  networkDevice.sendToDevice(networkDevice.NetworkDevice.MasterMAC, message, sizeof(message));
}

bool checkConfirmation(uint8_t typeSent) {
  networkDevice.update();
  uint8_t messageSize = networkDevice.available();
  if (messageSize > 0) {
    byte* message = (byte*)networkDevice.getMessage();
    uint8_t version;
    uint8_t type = 0xff;
    memcpy(&version, (void*)(message), sizeof(uint8_t));
    // Serial.print(F("Version: "));
    // Serial.println(version);
    switch (version) {
      case 0x2:
        memcpy(&type, (void*)(message + 1), sizeof(uint8_t));
        // Serial.print(F("Type: "));
        // Serial.println(type);
        return (type == typeSent);
        break;
      default:
        return false;
    }
  }
  return false;
  /*
  networkDevice.update();
  uint8_t messageSize = networkDevice.available();
  if (messageSize > 0) {
    Serial.print(F("Received Content: "));
    Serial.println((char*)networkDevice.getMessage());
    return true;
  }
  return false;  
  */
}

int getRandomSeed() {
  return analogRead(A0) + analogRead(A1) + analogRead(A2) + analogRead(A3) + analogRead(A4)
         + analogRead(A0) + analogRead(A1) + analogRead(A2) + analogRead(A3) + analogRead(A4);
}

