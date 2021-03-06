#ifndef RADIOTRANSMITTER_H
#define RADIOTRANSMITTER_H

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "Arduino.h"

class RadioTransmitter {
private:
  RF24* radio;
  void initialize();

public:
  RadioTransmitter(byte pinCE, byte pinCSN, String deviceId);
  send(int sensorType, int sensorValue);
  
};
#endif


