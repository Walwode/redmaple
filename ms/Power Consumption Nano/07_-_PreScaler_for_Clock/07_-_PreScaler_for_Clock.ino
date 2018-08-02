#include "prescaler.h"

// Ardino Nano: 12mAh -> 5mAh

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }
  
  setClockPrescaler(0x08);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(4);
  digitalWrite(LED_BUILTIN, LOW);
  delay(4);
  // put your main code here, to run repeatedly:

}
