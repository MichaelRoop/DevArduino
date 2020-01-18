//#pragma once


#ifndef _ArduinoDbgLog_h
#define _ArduinoDbgLog_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#ifdef DBG_LOG
	char __LOG_data__[100];
	#define DBG_INIT(baud) Serial.begin(baud); while(!Serial){;}
	#define DBG_OUT(format,...) sprintf(__LOG_data__, format, ##__VA_ARGS__); Serial.print(__LOG_data__)
#else
	#define DBG_INIT(baud)
	#define DBG_OUT(format,...)
#endif // DBG_LOG

// test code for project structure
class TestSubCaller {
public:
	void InvokeSub();
};

#endif
