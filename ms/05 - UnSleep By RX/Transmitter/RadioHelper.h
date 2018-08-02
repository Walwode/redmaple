#ifndef RADIOHELPER_H
#define RADIOHELPER_H

#include "Arduino.h"

class RadioHelper {
  public:
    static void setupRadio();
    static bool sendHumidity(int humidity);
    static void startListening();
    static void stopListening();
    static void powerUp();
    static void powerDown();
    
  private:
    // static bool httpRequest(String site);
};
#endif


