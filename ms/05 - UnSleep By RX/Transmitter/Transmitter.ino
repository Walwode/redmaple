#include "PreScaler.h"
#include "RadioHelper.h"
#include "PowerHelper.h"

void setup () {
  Serial.begin(9600);
  while(!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  doBlink();

  // setClockPrescaler(CLOCK_PRESCALER_16);
  RadioHelper::setupRadio();
  
}

void loop () 
{
  Serial.println("all off");
  doSleep(1);
  Serial.println("powerup");
  doBlink();
  RadioHelper::powerUp();
  doSleep(1);
  Serial.println("startlistening");
  doBlink();
  RadioHelper::startListening();
  doSleep(1);
  Serial.println("powerdown");
  doBlink();
  RadioHelper::powerDown();
  doSleep(1);
  Serial.println("stoplistening");
  doBlink();
  RadioHelper::stopListening();
  
  // RadioHelper::sendHumidity(readHumidity());
  // doSleep(4);  
}

void doSleep(int seconds) {
  Serial.print(F("Start sleep... "));
  PowerHelper::disableADC();
  // int cycles = (minutes * 60 / 8);
  int cycles = seconds;
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
  delay(125);
  int humidity = analogRead(A0);
  digitalWrite(8, LOW);
  Serial.println(humidity);
  PowerHelper::disableADC();
  return humidity;
}

void doBlink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

