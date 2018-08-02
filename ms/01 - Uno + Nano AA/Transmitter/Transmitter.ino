#include "PreScaler.h"
#include "RadioHelper.h"
#include "PowerHelper.h"

void setup () {
  Serial.begin(115200);
  while(!Serial);

  setClockPrescaler(CLOCK_PRESCALER_16);
  RadioHelper::setupRadio();
}

void loop () 
{
  RadioHelper::sendHumidity(readHumidity());
  doSleep(4);  
}

void doSleep(int seconds) {
  Serial.print(F("Start sleep... "));
  // int cycles = (minutes * 60 / 8);
  int cycles = seconds;
  int currentCycle = 0;
  while (currentCycle < cycles) {
    Serial.print(cycles - currentCycle);
    PowerHelper::sleep(WDT_SLEEP_1S);
    currentCycle++;
  }
  Serial.println(F("done"));
}

int readHumidity() {
  Serial.print(F("Read Humdity... "));
  PowerHelper::enableADC();
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  delay(125);
  int humidity = analogRead(A0);
  digitalWrite(8, LOW);
  Serial.println(humidity);
  PowerHelper::disableADC();
  return humidity;
}

