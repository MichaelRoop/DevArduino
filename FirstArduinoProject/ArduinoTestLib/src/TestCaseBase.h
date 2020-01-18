// TestCaseBase.h

#ifndef _TESTCASEBASE_h
#define _TESTCASEBASE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TestResults.h"

class TestCaseBase {
public:
	TestResults* DoTests(TestResults* results);
protected:
	virtual void DoSpecificTests(TestResults* results);
};


#endif

