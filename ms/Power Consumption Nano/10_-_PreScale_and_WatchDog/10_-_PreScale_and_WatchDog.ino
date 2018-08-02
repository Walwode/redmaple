#include <avr/sleep.h>
#include <avr/wdt.h>
#include "prescaler.h"

const byte LED = LED_BUILTIN;
int counter = 0;

// Arduino Nano Default (5V Vin):
// Deep Sleep: 3.6 mAh
// LED on: 8.9 mAh
// Idle: 6,75 mAh

// Arduino Nano NoLed NoVoltageRegulator (5V on +5V):
// Deep Sleep: 0.097 mAh
// LED on: 10.2 mAh
// Idle: 7,09 mAh

// Arduino Nano NoLed NoVoltageRegulator (AA on +5V):
// Deep Sleep: 0.02265 mAh
// LED on: 1.45 mAh
// Idle: 0,417 mAh

void flash ()
  {
  pinMode (LED, OUTPUT);
  for (byte i = 0; i < 2; i++)
    {
    digitalWrite (LED, HIGH);
    delay (8);
    digitalWrite (LED, LOW);
    delay (8);
    }
    
  pinMode (LED, INPUT);
    
  }  // end of flash
  
// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect
 
void setup () {
  setClockPrescaler(0x08);
  doSleepEightSeconds();
}

void loop () 
{
  // if ((counter % 8) == 0) flash ();
  // counter++;
  flash();

  doSleepEightSeconds();
} // end of loop

void doSleepEightSeconds() {
  ADCSRA = 0;
  MCUSR = 0;
  WDTCSR = bit (WDCE) | bit (WDE);
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);
  wdt_reset();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  noInterrupts ();
  sleep_enable();
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS);
  interrupts ();
  sleep_cpu ();
  
  sleep_disable();
}

