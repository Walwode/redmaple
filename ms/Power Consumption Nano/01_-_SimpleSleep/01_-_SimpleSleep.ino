#include <avr/sleep.h>

// Arduino Nano: 1,57mAh

void setup () 
{
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
  }
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  sleep_cpu ();  
}  // end of setup

void loop () { }
