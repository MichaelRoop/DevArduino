// TestResults.h

#ifndef _TESTRESULTS_h
#define _TESTRESULTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class TestResults {
public:
	TestResults();
	void Reset();
	void Increment(TestResults* results);
	void AddResult(bool result);
	int Failed();
	int OK();
	int Total();
private:
	int failed;
	int ok;
	int total;
};

#endif

