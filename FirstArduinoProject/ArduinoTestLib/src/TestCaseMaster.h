#ifndef _TESTCASEMASTER_h
#define _TESTCASEMASTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TestResults.h"

class TestCaseMaster {
public:
	TestCaseMaster();
	TestResults* ExecuteTests();
	void PrintResultsToDebug();

private:
	TestResults results;
};


#endif

