#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP8266WiFi.h>

RF24 radio(2, 15); // HSPI + pins 2 for CE and 15 for CSN

const uint64_t espAddress   = 0xE8E8F0F0A1LL;
const uint64_t nanoAddress  = 0xE8E8F0F0A2LL;
const uint64_t unoAddress = 0xE8E8F0F0A3LL;
// const uint64_t pipe04 = 0xE8E8F0F0A4LL;
// const uint64_t pipe05 = 0xE8E8F0F0A5LL;
// const uint64_t pipe06 = 0xE8E8F0F0A6LL;

struct SensorData {
  short version;
  short humidity;
  char deviceId[9]; // esp char[] != nano char[] -> last in struct!
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  restartRadio();
  radio.powerUp();
  radio.startListening();
  
  connectWifi();
}

void loop() {
  listenRadio();
}

void listenRadio() {
  if (radio.available()) {

    uint8_t pipe;
    while (radio.available(&pipe)) {
      
      HumidityData data;
      radio.read(&data, sizeof(data));
      Serial.println(data.version);
      Serial.println(data.deviceId);
      Serial.println(data.humidity);
      sendHumidity(&data);
    }
  }
}

void restartRadio() {
  Serial.print("Restart radio... ");
  yield();

  radio.begin();
  radio.setChannel(2);
  radio.setPALevel(RF24_PA_LOW);      // RF24_PA_MIN = -18dBm, RF24_PA_LOW = -12dBm, RF24_PA_HIGH = -6dBm, RF24_PA_MAX = 0dBm
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  // radio.setAutoAck(1);                   // Ensure autoACK is enabled
  // radio.setRetries(2, 15);               // Optionally, increase the delay between retries. Want the number of auto-retries as high a

  radio.openWritingPipe(espAddress);
  radio.openReadingPipe(1, unoAddress);
  radio.openReadingPipe(1, nanoAddress);
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
  for(int i = 0; i < data_len; i++) Serial.print(current[i],BIN);
  Serial.println();
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("WLAN-9PGVPK", "6763142139087153");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

void sendHumidity(HumidityData* data) {
  Serial.print("connecting to ");
  Serial.println("api.walterheger.de");
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect("api.walterheger.de", httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  // String url;
  // url += "/redmaple/arduino.php?action=humidity";
  // url += "&access=d548434a-2c80-11e8-8c45-00248c77bce5";
  // url += "&device=" + String(data->deviceId);
  // url += "&humidity=" + String(data->humidity);
  String url = "/whitewalnut/plant-arduino.php?action=humidity&code=EXTERNAL&humidity=" + String(data->humidity); 
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: api.walterheger.de\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

