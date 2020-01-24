/*
 Name:		ArduinoScratch.ino
 Created:	1/24/2020 4:24:13 PM
 Author:	Michael
*/

// the setup function runs once when you press reset or power the board
#include "FakeEnum.h"


void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    
    Serial.print("    Enum1 size:"); Serial.println(sizeof(STARTED));
    Serial.print("    Enum2 size:"); Serial.println(sizeof(STARTED2));
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
