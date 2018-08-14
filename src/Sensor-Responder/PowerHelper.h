#ifndef SLEEPHELPER_H
#define SLEEPHELPER_H

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

// WDIE = Interrupt
// WDE  = System Reset (reset)

#define WDT_SLEEP_16MS    bit (WDIE)
#define WDT_SLEEP_32MS    bit (WDIE) | bit (WDP0)
#define WDT_SLEEP_64MS    bit (WDIE) | bit (WDP1)
#define WDT_SLEEP_125MS   bit (WDIE) | bit (WDP1) | bit (WDP0)
#define WDT_SLEEP_250MS   bit (WDIE) | bit (WDP2)
#define WDT_SLEEP_500MS   bit (WDIE) | bit (WDP2) | bit (WDP0)
#define WDT_SLEEP_1S      bit (WDIE) | bit (WDP2) | bit (WDP1)
#define WDT_SLEEP_2S      bit (WDIE) | bit (WDP2) | bit (WDP1) | bit (WDP0)
#define WDT_SLEEP_4S      bit (WDIE) | bit (WDP3)
#define WDT_SLEEP_8S      bit (WDIE) | bit (WDP3) | bit (WDP0)

#define CLOCK_PRESCALER_1   (0x0)
#define CLOCK_PRESCALER_2   (0x1)
#define CLOCK_PRESCALER_4   (0x2)
#define CLOCK_PRESCALER_8   (0x3)
#define CLOCK_PRESCALER_16  (0x4)
#define CLOCK_PRESCALER_32  (0x5)
#define CLOCK_PRESCALER_64  (0x6)
#define CLOCK_PRESCALER_128 (0x7)
#define CLOCK_PRESCALER_256 (0x8)

static uint8_t __clock_prescaler = (CLKPR & (_BV(CLKPS0) | _BV(CLKPS1) | _BV(CLKPS2) | _BV(CLKPS3)));
ISR (WDT_vect) { wdt_disable(); } // watchdog interrupt

class PowerHelper {
private:
  static uint8_t getClockPrescaler() {return (__clock_prescaler);}
  static uint16_t getClockDivisionFactor() {return ((uint16_t)(1 << __clock_prescaler));}
  
public:
  static void sleep(int seconds);
  static void doSleep(byte sleepTime);
  
  static void setClockPrescaler(uint8_t clockPrescaler);
  static unsigned long trueMillis() {return millis() * getClockDivisionFactor();}
  static void trueDelay(unsigned long ms) {unsigned long start = trueMillis(); while (trueMillis() - start < ms);}
  static unsigned long rescaleDuration(unsigned long d) {return (d / getClockDivisionFactor());}
  static unsigned long rescaleTime(unsigned long t) {return (t * getClockDivisionFactor());}
  
  static void disableADC();
  static void resetADC();
};
#endif

void PowerHelper::doSleep(byte sleepTime) {
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

void PowerHelper::sleep(int seconds) {
  Serial.print(F("[POWER] PowerSleep..."));
  yield();
  
  int cycles = (seconds / 8) + 1;
  int currentCycle = 0;
  while (currentCycle < cycles) {
    doSleep(WDT_SLEEP_8S);
    currentCycle++;
    Serial.print(F("."));
  }
  
  Serial.println(F(" done"));
  yield();
}

void PowerHelper::disableADC() {
  ADCSRA = (0x0);
}

void PowerHelper::resetADC() {
  ADCSRA = (0x87);
}

/*
 * Results:
 * 
 * Arduino Nano (-LED, -5VReg, -ADC, 5V)
 * Delay  13.69mAh
 * LED    16.69mAh
 * Sleep  98.00uAh
 * 
 * Arduino Nano (-LED, -5VReg, -ADC, AA)
 * Delay  04.73mAh
 * LED    05.69mAh
 * Sleep  22.00uAh
*/

void PowerHelper::setClockPrescaler(uint8_t clockPrescaler) {
  if (clockPrescaler <= CLOCK_PRESCALER_256) {
    // Disable interrupts.
    uint8_t oldSREG = SREG;
    cli();

    // Enable change.
    CLKPR = _BV(CLKPCE); // write the CLKPCE bit to one and all the other to zero

    // Change clock division.
    CLKPR = clockPrescaler; // write the CLKPS0..3 bits while writing the CLKPE bit to zero

    // Copy for fast access.
    __clock_prescaler = clockPrescaler;

    // Recopy interrupt register.
    SREG = oldSREG;
  }
}
