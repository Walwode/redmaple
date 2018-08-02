#include "Arduino.h"
#include "avr/sleep.h"
#include "avr/wdt.h"
#include "PowerHelper.h"

ISR (WDT_vect) { wdt_disable(); } // watchdog interrupt

void PowerHelper::sleep(byte sleepTime) {
  MCUSR = 0;
  WDTCSR = bit (WDCE) | bit (WDE);
  WDTCSR = sleepTime;
  wdt_reset();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  noInterrupts ();
  sleep_enable();
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS);
  interrupts ();
  sleep_cpu (); // start sleep until watchdog interrupt
  sleep_disable(); // end sleep
}

void PowerHelper::disableADC() {
  ADCSRA = (0x0);
}
void PowerHelper::resetADC() {
  ADCSRA = (0x87);
}

