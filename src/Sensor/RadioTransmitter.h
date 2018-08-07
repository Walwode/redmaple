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
};
#endif

void RadioTransmitter::initialize(SBNetwork &networkDevice) {
  this->networkDevice = &networkDevice;
}

void RadioTransmitter::send(Sensor* sensor) {

  // radio->powerUp();
  uint8_t version = 0x01;
  uint8_t type = sensor->type;
  float value = sensor->read();
  
  byte message[1+1+4];
  memcpy((void*)(message), &version, sizeof(uint8_t));
  memcpy((void*)(message + 1), &type, sizeof(uint8_t));
  memcpy((void*)(message + 2), &value, sizeof(float));
  networkDevice->sendToDevice(networkDevice->NetworkDevice.MasterMAC, message, 1+1+4);

  // radio->powerDown();
}

