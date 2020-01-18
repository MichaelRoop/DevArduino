/*
 Name:		ArduinoTestLib.ino
 Created:	1/18/2020 11:14:44 AM
 Author:	Michael
*/

// the setup function runs once when you press reset or power the board
//#include <src/TestCaseBase.h>

#include "src/TestCaseMaster.h"

void setup() {

    Serial.begin(115400);
    while (!Serial) { ; }

    TestCaseMaster tcm;
    tcm.ExecuteTests();
    tcm.PrintResultsToDebug();
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
