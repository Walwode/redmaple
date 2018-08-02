#include <ArduinoJson.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // SPI + pins 9 for CE and 10 for CSN

const uint64_t espAddress 	= 0xE8E8F0F0A1LL;
const uint64_t nanoAddress	= 0xE8E8F0F0A2LL;  
const uint64_t unoAddress	= 0xE8E8F0F0A3LL;
// const uint64_t pipe04 = 0xE8E8F0F0A4LL;
// const uint64_t pipe05 = 0xE8E8F0F0A5LL;
// const uint64_t pipe06 = 0xE8E8F0F0A6LL;

struct RadioData { // max 32byte
  int version = 1.0; // 2 byte
  int humidity; // 2 byte
  char deviceId[9] = "2a1adf7d";
} data;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  restartRadio();
  radio.powerUp();
}

void loop() {
  sendHumidity();
  delay(3000);
}

int getHumidity() {
  return 200;
}

void sendHumidity() {
  Serial.print(F("Send data... "));
  
  data.humidity = getHumidity();
  toBinary(&data, 13);  
  if (!radio.write(&data, 13)) {
    Serial.println("failed");
    return;
  }
  Serial.println("done");
  Serial.println(data.version);
  Serial.println(data.deviceId);
  Serial.println(data.humidity);
  
}

void restartRadio() {
  Serial.print("Restart radio... ");
  yield();
  
  radio.begin();
  radio.setChannel(2);
  radio.setPALevel(RF24_PA_LOW);       // RF24_PA_MIN = -18dBm, RF24_PA_LOW = -12dBm, RF24_PA_HIGH = -6dBm, RF24_PA_MAX = 0dBm
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  // radio.setAutoAck(1);                   // Ensure autoACK is enabled
  // radio.setRetries(2, 15);               // Optionally, increase the delay between retries. Want the number of auto-retries as high a  
  
  radio.openWritingPipe(nanoAddress);
  radio.openReadingPipe(1, espAddress);
  radio.openReadingPipe(2, unoAddress);
  // radio.enableDynamicPayloads();     // must have for multi pipe receiving
  
  radio.stopListening();
  radio.powerDown();
  Serial.println("done");
}

void toBinary(const void* buf, uint8_t data_len) {
  Serial.print(F("Data Size: "));
  Serial.println(data_len);
  
  Serial.print(F("Binary: "));
  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);
  for(int i = 0; i < data_len; i++)
  {
    Serial.print(current[i],BIN);
  }
  Serial.println();
}

