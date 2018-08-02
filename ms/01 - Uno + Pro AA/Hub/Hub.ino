#include "PreScaler.h"
#include "RadioHelper.h"
#include "SleepHelper.h"

void setup () {
  Serial.begin(9600);
  while(!Serial);
  
  RadioHelper::setupRadio();
}

void loop () 
{
  RadioHelper::receiveHumidity();
  delay(200);
}
