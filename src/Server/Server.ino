#include <ESP8266WiFi.h>
#include <SBNetwork_config.h>
#include <SBNetwork.h>

SBMacAddress deviceMac(0x01, 0x02, 0x03, 0x04, 0x05);
SBNetwork networkDevice(false, D4, D8);

const char* server = "api.walterheger.de";
const char* site = "/redmaple/arduino.php";
const char* accessToken = "d548434a-2c80-11e8-8c45-00248c77bce5";
const char* deviceId = "1ea587d3";

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  networkDevice.initialize(deviceMac);
  Serial.println(F("*** PRESS 'N' to reset the device"));
  
  connectWifi();
}

void loop() {
  if (Serial.available()) {
    char c = toupper(Serial.read());
    if (c == 'N') { networkDevice.resetData(); }
    if (c == 'E') {
      if (!networkDevice.RunAsClient) { Serial.println("*****");
        if (networkDevice.isAutomaticClientAddingEnabled()) { Serial.println("Deactivating AutomaticClientAdding"); }
        else { Serial.println("Activating AutomaticClientAdding"); }
        Serial.println("*****");
        networkDevice.enableAutomaticClientAdding(!networkDevice.isAutomaticClientAddingEnabled());        
      }
    }
  }
  networkDevice.update();
  
  listenRadio();
}

void listenRadio() {
  uint8_t messageSize = networkDevice.available();
  if (messageSize > 0) {
    byte* message = (byte*)networkDevice.getMessage();
    Serial.print(F("Received Content from sensor: "));
    Serial.println((char*)message);
    
    uint8_t version;
    memcpy(&version, (void*)(message), sizeof(uint8_t));
    if (version == 0x01) {
      uint8_t type;
      float value;
      
      memcpy(&type, (void*)(message + 1), sizeof(uint8_t));
      memcpy(&value, (void*)(message + 2), sizeof(float));
      sendDataToApi(getLastMacString(), type, value);

      Serial.println("Send received message back to client...");
      char* returnMessage = "Hello client, yes I can hear you well!";
      networkDevice.sendToDevice(networkDevice.getLastReceivedMac(), returnMessage, strlen(returnMessage) + 1);
    }
  }
}

char* to3Digit(uint8_t value) {
  char str[4];
  snprintf(str, 4, "%03d", value);
  return str;
}

String getLastMacString() {
  SBMacAddress mac = networkDevice.getLastReceivedMac();
  
  char temp[6+1];
  sprintf(temp, "%02x.%02x.%02x.%02x.%02x",mac.Bytes[0],mac.Bytes[1],mac.Bytes[2],mac.Bytes[3],mac.Bytes[4]);
  String str(temp);
  return str;
  
  // return (String)mac.Bytes[0] + '.' +(String)mac.Bytes[1] + '.' + (String)mac.Bytes[2] + '.' + (String)mac.Bytes[3] + '.' + (String)mac.Bytes[4];
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

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());  
}

void sendDataToApi(String mac, uint8_t type, float value) {
  Serial.println(F(">> Send by WiFi start"));
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(server, httpPort)) {
    Serial.println(F("Connection failed"));
    return;
  }
  
  // We now create a URI for the request
  String url;
  url += site;
  url += F("?action=sensor&access=");
  url += accessToken;
  url += "&device=" + mac;
  url += "&type=" + String(type);
  url += "&value=" + String(value);
  // String url = "/whitewalnut/plant-arduino.php?action=humidity&code=EXTERNAL&humidity=" + String(data->humidity); 
  
  Serial.print(F("Requesting URL: "));
  Serial.println(url);
  
  // This will send the request to the server
  client.print(F("GET "));
  client.print(url);
  client.print(F(" HTTP/1.1\r\n"));
  client.print(F("Host: api.walterheger.de\r\n"));
  client.print(F("Connection: close\r\n\r\n"));
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(F("Client Timeout !"));
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
  Serial.println(F("Closing connection"));
  
  Serial.println(F("<< Send by WiFi end"));
}

