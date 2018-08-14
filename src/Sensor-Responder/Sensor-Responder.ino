#include <SBNetwork.h>
#include "PowerHelper.h"
#include "Sensor.h"

SBNetwork networkDevice(true, 9, 10);
volatile int count = 0; //tracks the number of interrupts from IRQ
int pCount = 0; //tracks what last count value was so know when count has been updated

/*
   The passive NRF24 device is receiving the request to check for a sensor value.
   There is no Radio.PowerDown. The power consumption with PowerSleep and ClockPreScaler is 0.35mA in sleep mode.

   Uncomment and configure the sensor values in the function receiveRadio
*/

void setup() {
  Serial.begin(115200);
  while (!Serial);

  randomSeed(getRandomSeed());
  SBMacAddress deviceMac(random(0, 256), random(0, 256), random(0, 256), random(0, 256), random(0, 256));
  // networkDevice.resetData();
  networkDevice.initialize(deviceMac);
  networkDevice.radio.maskIRQ(1,1,0); //mask all IRQ triggers except for receive (1 is mask, 0 is no mask)
  networkDevice.radio.setPALevel(RF24_PA_LOW); //Set power level to low, won't work well at higher levels (interfer with receiver)
  attachInterrupt(digitalPinToInterrupt(2), interruptFunction, FALLING);  // Create interrupt: 0 for pin 2 or 1 for pin 3, the name of the interrupt function or ISR, and condition to trigger interrupt
  
  Serial.println(F("*** PRESS 'N' to reset the device ***"));
}

void loop() {
  PowerHelper::setClockPrescaler(CLOCK_PRESCALER_16);
  PowerHelper::disableADC();
  PowerHelper::sleep(8);
  
  if(pCount < count) { //If this is true it means count was interated and another interrupt occurred
    Serial.begin(115200);  //start serial to communicate process
    Serial.print("Receive packet number ");
    Serial.println(count);
    Serial.end(); //have to end serial since it uses interrupts
    pCount = count;
  }
}

void receiveSerial() {
  if (Serial.available()) {
    char c = toupper(Serial.read());
    switch (c) {
      case  'N':
        networkDevice.resetData();
        break;
      case 'T':
        Serial.println("Still running...");
        break;
    }
  }
}

void receiveRadio() {
  networkDevice.update();
  uint8_t messageSize = networkDevice.available();
  
  while (messageSize > 0) {
    byte* message = (byte*)networkDevice.getMessage();
    SBMacAddress sender = networkDevice.getLastReceivedMac();
    uint8_t version = 0x0;

    Serial.print(F("Sensor value request... "));

    memcpy(&version, (void*)(message), sizeof(uint8_t));
    if (version == 0x01) {
      uint8_t type, temp1, temp2 = 0x0;
      float value = 0x0;

      memcpy(&type, (void*)(message + 1), sizeof(uint8_t));
      Serial.println(type);
      switch (type) {
        case SENSOR_TYPE_VOLTAGE:
          value = (float)Sensor::readVoltage(2, A2);
          break;
        case SENSOR_TYPE_HUMIDITY:
          value = Sensor::readHumidity(8, A0);
          break;
        /*
        case SENSOR_TYPE_PHOTO:
          value = Sensor::readPhoto(8, A1);
          break;
        case SENSOR_TYPE_TEMPERATURE:
          Sensor::readDht11(7, 6, temp1, temp2);
          value = (float)temp1;
          break;
        case SENSOR_TYPE_HUMIDITYAIR:
          Sensor::readDht11(7, 6, temp1, temp2);
          value = (float)temp2;
          break;
        case SENSOR_TYPE_GAS:
          // not implemented
          break;
        */
      }
      if (value != 0x0) sendNrf24(sender, type, value);
      delay(50);
    }

    networkDevice.update();
    messageSize = networkDevice.available();
  }
}

void sendNrf24(SBMacAddress &sendToMac, uint8_t type, float value) {
  Serial.println(F("[NRF24] Send data... "));
  Serial.print(F("[NRF24] Type: "));
  Serial.println(type);
  Serial.print(F("[NRF24] Value: "));
  Serial.println(value);

  uint8_t version = 0x01;
  byte message[1 + 1 + 4];
  memcpy((void*)(message), &version, sizeof(version));
  memcpy((void*)(message + 1), &type, sizeof(type));
  memcpy((void*)(message + 2), &value, sizeof(value));
  networkDevice.sendToDevice(sendToMac, message, 1 + 1 + 4);

  Serial.println(F("[NRF24] Send data... done"));
}

int getRandomSeed() {
  return analogRead(A0) + analogRead(A1) + analogRead(A2) + analogRead(A3) + analogRead(A4)
         + analogRead(A0) + analogRead(A1) + analogRead(A2) + analogRead(A3) + analogRead(A4);
}

// This is the function called when the interrupt occurs (pin 2 goes high)
// this is often referred to as the interrupt service routine or ISR
// This cannot take any input arguments or return anything
void interruptFunction() {
  count++; //up the receive counter
  
  PowerHelper::setClockPrescaler(CLOCK_PRESCALER_1);
  PowerHelper::resetADC();
  receiveRadio();
}