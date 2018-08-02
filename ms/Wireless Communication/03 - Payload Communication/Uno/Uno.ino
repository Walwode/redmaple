#include <ArduinoJson.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // SPI + pins 7 for CE and 8 for CSN

const uint64_t espAddress 	= 0xE8E8F0F0A1LL;
const uint64_t nanoAddress	= 0xE8E8F0F0A2LL;
const uint64_t unoAddress	= 0xE8E8F0F0A3LL;
// const uint64_t pipe04 = 0xE8E8F0F0A4LL;
// const uint64_t pipe05 = 0xE8E8F0F0A5LL;
// const uint64_t pipe06 = 0xE8E8F0F0A6LL;

struct HumidityData {
  float version;
  char deviceId[9];
  float humidity;
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  restartRadio();
  radio.powerUp();
  radio.startListening();
}

void loop() {
  listenRadio();
}

void listenRadio() {
  if (radio.available()) {

    uint8_t pipe;
    while (radio.available(&pipe)) {
      
      // char data[32];
      HumidityData data;
      radio.read(&data, sizeof(data));
      Serial.println(data.humidity);
      
      // StaticJsonBuffer<32> jsonBuffer;
      // JsonObject& root = jsonBuffer.parseObject(data);
      // root.printTo(Serial);
      // Serial.println();
    }
  }
}

void restartRadio() {
  Serial.print("Restart radio... ");
  yield();

  radio.begin();
  radio.setChannel(2);
  radio.setPALevel(RF24_PA_LOW); 			// RF24_PA_MIN = -18dBm, RF24_PA_LOW = -12dBm, RF24_PA_HIGH = -6dBm, RF24_PA_MAX = 0dBm
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  // radio.setAutoAck(1);                   // Ensure autoACK is enabled
  // radio.setRetries(2, 15);               // Optionally, increase the delay between retries. Want the number of auto-retries as high a

  radio.openWritingPipe(unoAddress);
  radio.openReadingPipe(1, espAddress);
  radio.openReadingPipe(2, nanoAddress);
  // radio.enableDynamicPayloads(); 		// must have for multi pipe receiving

  radio.stopListening();
  radio.powerDown();
  Serial.println("done");
}

