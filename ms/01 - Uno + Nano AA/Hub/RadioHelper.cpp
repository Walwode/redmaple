#include "Arduino.h"
#include "RadioHelper.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN for Arduino Uno

const uint64_t myAddress = 0xF0F0F0F0E1LL;
const uint64_t transmitterAddress = 0xF0F0F0F0D2LL;

void RadioHelper::setupRadio() {
  Serial.print(F("Setup radio..."));
  
  radio.begin();                          // Setup and configure rf radio
  radio.setChannel(1);                    // Set the channel
  radio.setPALevel(RF24_PA_LOW);          // Set PA LOW for this demonstration. We want the radio to be as lossy as possible for this example.
  radio.setDataRate(RF24_1MBPS);          // Raise the data rate to reduce transmission distance and increase lossiness
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.setRetries(2,15);                 // Optionally, increase the delay between retries. Want the number of auto-retries as high as possible (15)
  radio.setCRCLength(RF24_CRC_16);        // Set CRC length to 16-bit to ensure quality of data
  radio.openWritingPipe(myAddress);       // Open the default reading and writing pipe
  radio.openReadingPipe(1,transmitterAddress);       
  // radio.setChannel(4); // TODO
  
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  // radio.stopListening();                  // Stop listening
  
  // radio.powerUp();                        // Power up the radio
  // radio.powerDown();
  
  Serial.println(F("Done"));
}

void RadioHelper::receiveHumidity() {
  // Serial.print(".");
  if (radio.available()) {
    
    int humidity;
    while (radio.available()) {                                 // While there is data ready
      radio.read(&humidity, sizeof(int));                       // Get the payload
    }
    Serial.print(F("Read humidity "));
    Serial.println(humidity);  
    
    radio.stopListening();                                      // First, stop listening so we can talk   
    radio.write(&humidity, sizeof(int));                        // Send the final one back.      
    radio.startListening();                                     // Now, resume listening so we catch the next packets.     
    Serial.print(F("Sent response "));
    Serial.println(humidity);  
  }
}

