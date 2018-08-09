#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SBNetwork_config.h>
#include <SBNetwork.h>
#include <Scheduler.h>

#define SENSOR_TYPE_VOLTAGE       0
#define SENSOR_TYPE_HUMIDITY      1
#define SENSOR_TYPE_TEMPERATURE   2
#define SENSOR_TYPE_PHOTO         3 
#define SENSOR_TYPE_GAS           4 
#define SENSOR_TYPE_HUMIDITYAIR   5

class PassiveScheduler : public Task {
public:
  PassiveScheduler() {};
  ~PassiveScheduler() {};
protected:
  void loop();
  void setup();
};

class ActiveScheduler : public Task {
public:
  ActiveScheduler() {};
  ~ActiveScheduler() {};
protected:
  void loop();
  void setup();
};

SBNetwork networkDevice(false, D4, D8);
PassiveScheduler passiveScheduler;
ActiveScheduler activeScheduler;

const char* server = "api.walterheger.de";
const char* site = "/redmaple/arduino.php";
const char* accessToken = "d548434a-2c80-11e8-8c45-00248c77bce5";
const char* deviceId = "1ea587d3";

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  SBMacAddress deviceMac(0x01, 0x02, 0x03, 0x04, 0x05);
  networkDevice.initialize(deviceMac);
  Serial.println(F("*** PRESS 'N' to reset the device"));
  
  Scheduler.start(&passiveScheduler);
  Scheduler.start(&activeScheduler);
  
  connectWifi();
}

void loop() {
  Scheduler.begin();
  Serial.println("unreachable code");
}

void receiveSerial() {
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
}

void receiveRadio() {
  networkDevice.update();
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

      /*
      Serial.println("Send received message back to client...");
      char* returnMessage = "Hello client, yes I can hear you well!";
      networkDevice.sendToDevice(networkDevice.getLastReceivedMac(), returnMessage, strlen(returnMessage) + 1);
      */
    }
  }
}

String getLastMacString() {
  return getMacString(networkDevice.getLastReceivedMac());
}

String getMacString(SBMacAddress mac) {
  char temp[6+1];
  sprintf(temp, "%02x.%02x.%02x.%02x.%02x",mac.Bytes[0],mac.Bytes[1],mac.Bytes[2],mac.Bytes[3],mac.Bytes[4]);
  String str(temp);
  return str;
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

  /*
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(server, httpPort)) {
    Serial.println(F("Connection failed"));
    return;
  }
  */
  
  // We now create a URI for the request
  String url;
  url += site;
  url += F("?action=sensor&access=");
  url += accessToken;
  url += "&device=" + String(mac);
  url += "&type=" + String(type);
  url += "&value=" + String(value);
  // String url = "/whitewalnut/plant-arduino.php?action=humidity&code=EXTERNAL&humidity=" + String(data->humidity); 

  Serial.print(F("Requesting URL: "));
  Serial.println(url);

  
  HTTPClient http;  //Declare an object of class HTTPClient

  http.begin(url);  //Specify request destination
  int httpCode = http.GET();                                                                  //Send the request

  if (httpCode > 0) { //Check the returning code

    String payload = http.getString();   //Get the request response payload
    Serial.println(payload);                     //Print the response payload

  }

  http.end();   //Close connection

  /*
  // This will send the request to the server
  client.print(F("GET "));
  client.print(url);
  client.print(F(" HTTP/1.1\r\n"));
  client.print(F("Host: api.walterheger.de\r\n"));
  // client.print(F("Connection: close\r\n\r\n"));
  client.print(F("Connection: keep-alive\r\n\r\n"));
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
  
  client.stop();
  Serial.println();
  Serial.println(F("Closing connection"));
  */
  
  Serial.println(F("<< Send by WiFi end"));
}

void sendRadio() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (!networkDevice.MasterStorage.Slaves[i].isEquals(EMPTY_MAC)) {
      Serial.print("Send sensor request to ");
      Serial.print(String(getMacString(networkDevice.MasterStorage.Slaves[i])));
      Serial.print(": ");

      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_VOLTAGE);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_HUMIDITY);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_TEMPERATURE);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i],SENSOR_TYPE_PHOTO);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i],SENSOR_TYPE_GAS);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_HUMIDITYAIR);
      Serial.println("...done");
    }
  }
}

void sendRadioRequest(SBMacAddress mac, uint8_t type) {
  Serial.print(type);
  uint8_t version = 0x01;
  byte message[1 + 1];
  memcpy((void*)(message), &version, sizeof(uint8_t));
  memcpy((void*)(message + 1), &type, sizeof(uint8_t));
  networkDevice.sendToDevice(mac, message, sizeof(message));
}

void PassiveScheduler::setup() {}
void PassiveScheduler::loop() {
  receiveSerial();
  sendRadio();
  delay(30000);
}

void ActiveScheduler::setup() {}
void ActiveScheduler::loop() {
  receiveSerial();
  receiveRadio();
}

