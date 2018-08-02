#include "Arduino.h"
#include "RadioTransmitter.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t espAddress   = 0xE8E8F0F0A1LL;
const uint64_t nanoAddress  = 0xE8E8F0F0A2LL;  
const uint64_t unoAddress = 0xE8E8F0F0A3LL;
// const uint64_t pipe04 = 0xE8E8F0F0A4LL;
// const uint64_t pipe05 = 0xE8E8F0F0A5LL;
// const uint64_t pipe06 = 0xE8E8F0F0A6LL;

struct SensorData { // max 32byte
  int version = 1.0; // 2 byte
  int sensorData; // 2 byte
  char deviceId[9];
} data;

RadioTransmitter::RadioTransmitter(byte pinCE, byte pinCSN, char* deviceId) {
  radio = new RF24(pinCE, pinCSN);
  initialize();
}

RadioTransmitter::send(int sensorType, double sensorData) {
  Serial.print(F("Send data... "));
  
  radio->powerUp();
  data.sensorData = sensorData;
  // toBinary(&data, 13);
  if (!radio->write(&data, sizeof(data))) {
    Serial.println("failed");
    return false;
  }
  Serial.println("done");
  Serial.println(data.version);
  Serial.println(data.deviceId);
  Serial.println(data.sensorData);
  radio->powerDown();
  delay(1000);
  return true;
}

void RadioTransmitter::initialize() {
  Serial.print("Restart radio... ");
  yield();
  
  radio->begin();
  radio->setChannel(2);
  radio->setPALevel(RF24_PA_LOW);       // RF24_PA_MIN = -18dBm, RF24_PA_LOW = -12dBm, RF24_PA_HIGH = -6dBm, RF24_PA_MAX = 0dBm
  radio->setDataRate(RF24_1MBPS);
  radio->setCRCLength(RF24_CRC_16);
  // radio.setAutoAck(1);                   // Ensure autoACK is enabled
  // radio.setRetries(2, 15);               // Optionally, increase the delay between retries. Want the number of auto-retries as high a  
  
  radio->openWritingPipe(nanoAddress);
  radio->openReadingPipe(1, espAddress);
  radio->openReadingPipe(2, unoAddress);
  // radio.enableDynamicPayloads();     // must have for multi pipe receiving
  
  radio->stopListening();
  radio->powerDown();
  Serial.println("done");
}

