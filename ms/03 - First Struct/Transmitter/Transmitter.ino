#include "PreScaler.h"
#include "RadioHelper.h"
#include "PowerHelper.h"

// 20.03.2015 - 2.893
// 21.03.2015 - 3.253

void setup () {
  Serial.begin(115200);
  while(!Serial);

  setClockPrescaler(CLOCK_PRESCALER_16);
  RadioHelper::setupRadio();
}

void loop () {
  RadioHelper::sendHumidity(readHumidity());
  doSleep(600);
}

void doSleep(int seconds) {
  Serial.print(F("Start sleep... "));
  
  // int cycles = seconds;
  int cycles = seconds / 8;
  int currentCycle = 0;
  while (currentCycle < cycles) {
    
    Serial.print(cycles - currentCycle);
    PowerHelper::sleep(WDT_SLEEP_8S);
    currentCycle++;
  }
  
  Serial.println(F("done"));
}

int readHumidity() {
  Serial.print(F("Read Humdity... "));
  PowerHelper::enableADC();
  
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  delay(64);
  int humidity = 1024 - analogRead(A0);
  digitalWrite(8, LOW);
  
  Serial.println(humidity);
  PowerHelper::disableADC();
  
  return humidity;
}

