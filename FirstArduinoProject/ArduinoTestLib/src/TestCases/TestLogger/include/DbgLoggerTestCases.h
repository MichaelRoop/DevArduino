//#pragma once

#ifndef _DBLOGGERTESTCASES_h
#define _DBLOGGERTESTCASES_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

//#include "src/TestCaseBase.h"
#include "D:\Dev\Arduino\FirstArduinoProject\ArduinoTestLib\src/TestCaseBase.h"

class DbgLoggerTestCases : public TestCaseBase {
protected:
	void DoSpecificTests(TestResults* results) override {
		results->AddResult(true);
		results->AddResult(true);
		results->AddResult(true);
		results->AddResult(false);
		results->AddResult(false);
	}
};

//D:\Dev\Arduino\FirstArduinoProject\ArduinoTestLib\src\TestCases\TestLogger\source

#endif