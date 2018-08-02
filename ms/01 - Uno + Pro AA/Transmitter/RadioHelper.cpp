#include "Arduino.h"
#include "RadioHelper.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN for Arduino Nano Atmega168/326

const uint64_t myAddress = 0xF0F0F0F0D2LL;
const uint64_t hubAddress = 0xF0F0F0F0E1LL;

void RadioHelper::setupRadio() {
  Serial.print(F("Setup radio... "));
  
  radio.begin();                          // Setup and configure rf radio
  radio.setChannel(1);                    // Set the channel
  radio.setPALevel(RF24_PA_LOW);          // Set PA LOW for this demonstration. We want the radio to be as lossy as possible for this example.
  radio.setDataRate(RF24_1MBPS);          // Raise the data rate to reduce transmission distance and increase lossiness
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.setRetries(2,15);                 // Optionally, increase the delay between retries. Want the number of auto-retries as high as possible (15)
  radio.setCRCLength(RF24_CRC_16);        // Set CRC length to 16-bit to ensure quality of data
  radio.openWritingPipe(myAddress);       // Open the default reading and writing pipe
  radio.openReadingPipe(1,hubAddress);       
  
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  radio.stopListening();                  // Stop listening
  
  radio.powerDown();
  
  Serial.println(F("done"));
}

bool RadioHelper::sendHumidity(int humidity) {
  Serial.print(F("Send humidity "));
  Serial.print(humidity);
  Serial.print(F("... "));
  
  radio.powerUp();
  bool timeout = false;

  radio.stopListening();
  int requestTimeout = micros() + 200000;
  while(!radio.write(&humidity, sizeof(int)) && !timeout) {
    if (micros() > requestTimeout) timeout = true;
  }

  if (timeout) {
    Serial.println(F("timeout"));
    radio.powerDown();
    return false;
  } else {
    
    Serial.println(F("done"));
    Serial.print(F("Response... "));

    radio.startListening();
    long responseTimeout = micros() + 200000;
    while (!radio.available() && !timeout) {
      if (micros() > responseTimeout) timeout = true;
    }

    if (timeout) {
      Serial.println(F("timeout"));
      radio.powerDown();
      return false;
    } else {
      
      int response;
      radio.read(&response, sizeof(int));
      if (response != humidity) {
        Serial.println(F("com. error"));
        radio.powerDown();
        return false;        
      }
    }
  }

  Serial.println(F("done"));
  radio.powerDown();
  return true;
}

