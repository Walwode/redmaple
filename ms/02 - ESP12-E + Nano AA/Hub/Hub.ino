#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP8266WiFi.h>

// SPI pin configuration figured out from here:
// https://gist.github.com/crcastle/6bab21445c857993d1d8
// http://d.av.id.au/blog/esp8266-hardware-spi-hspi-general-info-and-pinout/
RF24 radio(2, 15);    // Set up nRF24L01 radio on SPI bus plus pins 2 for CE and 15 for CSN

// Topology
const uint64_t myAddress = 0xF0F0F0F0E1LL;
const uint64_t transmitterAddress = 0xF0F0F0F0D2LL;

struct dataStruct{
  unsigned long _salt;
  float temp;
  int volt;
}sensor_data;

void setup() {
  yield();
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("starting......");
  restartRadio();                    // turn on and configure radio  
  Serial.println("restarting radio");
  radio.startListening();
  Serial.println("Listening for sensor values..."); 
  connectWifi();
}

void loop() {
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
    sendHumidity(humidity);
  }
}

void restartRadio(){
  yield();
  radio.begin(); // Start up the radio
  radio.setChannel(1);                    // Set the channel
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);  
  radio.openReadingPipe(1,transmitterAddress);
  radio.openWritingPipe(myAddress);  
  radio.stopListening();

  /*
  radio.begin();                          // Setup and configure rf radio
  radio.setChannel(1);                    // Set the channel
  radio.setPALevel(RF24_PA_LOW);          // Set PA LOW for this demonstration. We want the radio to be as lossy as possible for this example.
  radio.setDataRate(RF24_1MBPS);          // Raise the data rate to reduce transmission distance and increase lossiness
  // radio.setAutoAck(1);                    // Ensure autoACK is enabled
  // radio.setRetries(2,15);                 // Optionally, increase the delay between retries. Want the number of auto-retries as high as possible (15)
  radio.setCRCLength(RF24_CRC_16);        // Set CRC length to 16-bit to ensure quality of data
  radio.openWritingPipe(myAddress);       // Open the default reading and writing pipe
  radio.openReadingPipe(1,transmitterAddress);       
  // radio.setChannel(4); // TODO
  
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging

  
  radio.begin(); // Start up the radio
  radio.setChannel(1);                    // Set the channel
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,transmitterAddress);
  radio.openWritingPipe(myAddress);  
  radio.stopListening();
  */
  
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


void sendHumidity(int humidity) {
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
  String url = "/whitewalnut/plant-arduino.php?action=humidity&code=EXTERNAL&humidity=" + String(humidity);
  
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
