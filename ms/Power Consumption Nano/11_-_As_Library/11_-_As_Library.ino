#include "PowerHelper.h"
#include "PreScaler.h"
#include "RadioHelper.h"

void setup () {
  Serial.begin(115200);
  while(!Serial);
  
  pinMode(LED_BUILTIN, OUTPUT);
  PowerHelper::disableADC();
  RadioHelper::setupRadio();
}

void loop () {
  setClockPrescaler(CLOCK_PRESCALER_1);
  RadioHelper::sendHumidity(1);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_2);
  RadioHelper::sendHumidity(2);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_4);
  RadioHelper::sendHumidity(4);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_8);
  RadioHelper::sendHumidity(8);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_16);
  RadioHelper::sendHumidity(16);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_32);
  RadioHelper::sendHumidity(32);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_64);
  RadioHelper::sendHumidity(64);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_128);
  RadioHelper::sendHumidity(128);
  PowerHelper::sleep(WDT_SLEEP_4S);
  
  setClockPrescaler(CLOCK_PRESCALER_256);
  RadioHelper::sendHumidity(256);
  PowerHelper::sleep(WDT_SLEEP_4S);
}




