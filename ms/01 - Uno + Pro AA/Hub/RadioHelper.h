#ifndef RADIOHELPER_H
#define RADIOHELPER_H

#include "Arduino.h"

class RadioHelper {
  public:
    static void setupRadio();
    static void receiveHumidity();
    
  private:
    // static bool httpRequest(String site);
};
#endif


