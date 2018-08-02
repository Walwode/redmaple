#include "Arduino.h"
#include "avr/sleep.h"
#include "avr/wdt.h"
#include "SleepHelper.h"

ISR (WDT_vect) { wdt_disable(); } // watchdog interrupt

void SleepHelper::doSleepEightSeconds() {
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
  sleep_cpu (); // start sleep until watchdog interrupt
  sleep_disable(); // end sleep

  // TODO: maybe re-switch off some components
}


