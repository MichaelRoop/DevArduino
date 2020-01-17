/*
 Name:		ArduinoFileManager.h
 Created:	1/17/2020 4:30:59 PM
 Author:	Michael
 Editor:	http://www.visualmicro.com
*/

#ifndef _ArduinoFileManager_h
#define _ArduinoFileManager_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ArduinoFileManager {
public:
    ArduinoFileManager();
    
private:

};


#endif

