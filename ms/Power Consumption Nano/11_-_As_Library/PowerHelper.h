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
 *
 * Arduino Pro Mini 368 (-LED, +5VReg, -ADC, AA)
 * Sleep LED   153.00uAh
 * Sleep -LED   83.00uAh
 *
 * Arduino Pro Mini 368 (-LED, -5VReg, -ADC, AA)
 * Sleep LED    77.00uAh
 * Sleep -LED    5.41uAh
*/

#ifndef SLEEPHELPER_H
#define SLEEPHELPER_H

#include "Arduino.h"

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

class PowerHelper {
  public:
    static void sleep(byte sleepTime);
    static void disableADC();
    
  private:
};
#endif


