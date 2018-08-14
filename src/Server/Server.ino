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

void PassiveScheduler::setup() {}
void PassiveScheduler::loop() {
  receiveSerial();
  sendRadio();
  // delay(30000);
  delay(5*60*1000);
  // delay(19*60*1000); // 19 minutes
}

void ActiveScheduler::setup() {}
void ActiveScheduler::loop() {
  receiveSerial();
  receiveRadio();
}

void receiveSerial() {
  if (Serial.available()) {
    char c = toupper(Serial.read());
    switch (c) {
      case 'N': networkDevice.resetData(); break;
      case 'E':
      if (!networkDevice.RunAsClient) { Serial.println("*****");
        if (networkDevice.isAutomaticClientAddingEnabled()) { Serial.println("Deactivating AutomaticClientAdding"); }
        else { Serial.println("Activating AutomaticClientAdding"); }
        Serial.println("*****");
        networkDevice.enableAutomaticClientAdding(!networkDevice.isAutomaticClientAddingEnabled());        
      }
        break;
      case 'S': sendRadio(); break;
    }
  }
}

void receiveRadio() {
  networkDevice.update();
  uint8_t messageSize = networkDevice.available();
  if (messageSize > 0) {
    byte* message = (byte*)networkDevice.getMessage();
    Serial.print(F("Received data from sensor: "));
    Serial.println(getLastMacString());
    
    uint8_t version;
    memcpy(&version, (void*)(message), sizeof(uint8_t));
    if (version == 0x01) {
      uint8_t type;
      float value;
      
      memcpy(&type, (void*)(message + 1), sizeof(uint8_t));
      memcpy(&value, (void*)(message + 2), sizeof(float));
      sendDataToApi(getLastMacString(), type, value);
    }
  }
}

void sendDataToApi(char* mac, uint8_t type, float value) {
  while (WiFi.status() != WL_CONNECTED) { connectWifi(); }
  
  // Serial.println(F(">> Send by WiFi start"));
  
  // PROGMEM const char* site = "This string is in PROGMEM";
  // PROGMEM const char* accessToken = "<token>";
  // sprintf(url, "%S?action=sensor&access=%S&device=%S&type=%02d&value=%02d",site,accessToken,mac,type,value);

  /*
  String url;
  url += site;
  url += F("?action=sensor&access=");
  url += accessToken;
  url += "&device=" + String(mac);
  url += "&type=" + String(type);
  url += "&value=" + String(value);
  Serial.print(F("Requesting URL: "));
  Serial.println(url);
  */

  char url[250];
  sprintf(url, "http://%s%s?action=sensor&access=%s&device=%s&type=%d&value=%f",server,site,accessToken,mac,type,value);
  Serial.println(url);
  
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) { Serial.println(http.getString()); } // print payload
  http.end();
  
  // Serial.println(F("<< Send by WiFi end"));
}

void sendRadio() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (!networkDevice.MasterStorage.Slaves[i].isEquals(EMPTY_MAC)) {
      Serial.print("Send sensor request to ");
      Serial.print(macToString(networkDevice.MasterStorage.Slaves[i]));
      Serial.print(": ");

      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_VOLTAGE);
      delay(10000);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_HUMIDITY);
      delay(10000);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_TEMPERATURE);
      delay(10000);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_PHOTO);
      delay(10000);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_GAS);
      delay(10000);
      sendRadioRequest(networkDevice.MasterStorage.Slaves[i], SENSOR_TYPE_HUMIDITYAIR);
      delay(10000);
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

  delay(100);
  receiveRadio();
}

char* getLastMacString() {
  return macToString(networkDevice.getLastReceivedMac());
}

char* macToString(SBMacAddress mac) {
  char temp[9];
  sprintf(temp, "%02x.%02x.%02x.%02x.%02x",mac.Bytes[0],mac.Bytes[1],mac.Bytes[2],mac.Bytes[3],mac.Bytes[4]);
  return temp;
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
