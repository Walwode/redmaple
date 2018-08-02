#ifndef SLEEPHELPER_H
#define SLEEPHELPER_H

#include "Arduino.h"

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

class SleepHelper {
  public:
    static void doSleepEightSeconds();
    
  private:
    // static bool httpRequest(String site);
};
#endif


