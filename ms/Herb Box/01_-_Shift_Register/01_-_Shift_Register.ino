// Arduino-Pin verbunden mit SH_CP des 74HC595
int shiftPin = 4;
// Arduino-Pin verbunden mit ST_CP des 74HC595
int storePin = 5;
// Arduino-Pin verbunden mit DS des 74HC595
int dataPin = 16;
 
// Dieses Muster soll ausgegeben werden
// VLEFT, MRIGHT, MLEFT, MCENTER, VRIGHT, LED, VCENTER, PUMP
int muster[8] = {0,0,1,0,1,0,1,1};

 
void setup() {
 
 // Pins 8,9,10 auf Ausgabe
 pinMode(storePin, OUTPUT);
 pinMode(shiftPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
 
 // storePin sicherheitshalber auf LOW
 digitalWrite(storePin, LOW); 
 
 for (int i=0; i<8; i++) {
   // Zuerst immer alle 3 Pins auf LOW
   // Aktion passiert bei Wechsel von LOW auf HIGH
   digitalWrite(shiftPin, LOW);
   // Jetzt den Wert der aktuellen Stelle ans Datenpin DS anlegen 
   digitalWrite(dataPin, muster[i]);
   // Dann ShiftPin SHCP von LOW auf HIGH, damit wird der Wert
   // am Datenpin ins Register geschoben. 
   digitalWrite(shiftPin, HIGH);
 }
 
 // Wenn alle 8 Stellen im Register sind, jetzt das StorePin STCP
 // von LOW auf HIGH, damit wird Registerinhalt an Ausgabepins
 // kopiert und der Zustand an den LEDs sichtbar
 
 digitalWrite(storePin, HIGH);
}
 
void loop () {
 // Hier passiert nichts.
}
