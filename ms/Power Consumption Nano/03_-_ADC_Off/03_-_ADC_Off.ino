#include <avr/sleep.h>
#include <avr/power.h>

// Arduino Nano: 1.416mAh

void setup () 
{
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
  }
  
  // disable ADC
  ADCSRA = 0;

  for (byte i = 0; i <= A5; i++)
  {
    pinMode (i, INPUT);    // changed as per below
    digitalWrite (i, LOW);  //     ditto
  }  
   
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  noInterrupts ();           // timed sequence follows
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);  // turn on brown-out enable select
  MCUCR = bit (BODS);        // this must be done within 4 clock cycles of above
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();              // sleep within 3 clock cycles of above
}  // end of setup

void loop () { }
