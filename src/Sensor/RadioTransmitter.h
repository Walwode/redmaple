#ifndef RADIOTRANSMITTER_H
#define RADIOTRANSMITTER_H

#include <SBNetwork_config.h>
#include <SBNetwork.h>
#include "Sensor.h"

class RadioTransmitter {
private:
  SBNetwork *networkDevice;

public:
  RadioTransmitter() {};
  void initialize(SBNetwork &networkDevice);
  void send(Sensor *sensor);
  void resetGuid();
  // String bytesToString(byte bytes[], bool hex);
};
#endif

void RadioTransmitter::initialize(SBNetwork &networkDevice) {
  this->networkDevice = &networkDevice;

  /*
  randomSeed(analogRead(0) + millis());
  
  EEPROM.get(SB_NETWORK_FLASH_SIZE, guid);
  if (guid[0] == 0xFF || guid[0] == 0x00) {
    guid[0] = random(1,255);
    guid[1] = random(1,255);
    guid[2] = random(1,255);
    guid[3] = random(1,255);
    guid[4] = random(1,255);
    EEPROM.put(SB_NETWORK_FLASH_SIZE, guid);
    Serial.println(F("Created Sensor GUID"));
  }
  Serial.print(F("GUID: "));
  Serial.print(bytesToString(guid,false));
  Serial.print(" (");
  Serial.print(bytesToString(guid,true));
  Serial.println(" )");
  */
}

/*
String RadioTransmitter::bytesToString(byte bytes[], bool hex = false) {
  char temp[5+1];
  hex ? sprintf(temp, "%02x.%02x.%02x.%02x",bytes[0],bytes[1],bytes[2],bytes[3],bytes[4]) : sprintf(temp, "%02u.%02u.%02u.%02u",bytes[0],bytes[1],bytes[2],bytes[3],bytes[4]);
  String str(temp);
  return str;
}
*/

void RadioTransmitter::send(Sensor* sensor) {

  // radio->powerUp();
  uint8_t version = 0x01;
  uint8_t type = sensor->type;
  float value = sensor->read();
  
  byte message[1+1+4];
  memcpy((void*)(message), &version, sizeof(uint8_t));
  memcpy((void*)(message + 1), &type, sizeof(uint8_t));
  memcpy((void*)(message + 2), &value, sizeof(float));
  Serial.print(F("[RADIO] Send data..."));
  networkDevice->sendToDevice(networkDevice->NetworkDevice.MasterMAC, message, 1+1+4);
  Serial.println(F(" Done"));

  // radio->powerDown();
}

void RadioTransmitter::resetGuid() {
  /*
  for(uint16_t i = 0; i < sizeof(guid); i++){
    EEPROM.write(i + SB_NETWORK_FLASH_SIZE, 0);
  }
  */
}

