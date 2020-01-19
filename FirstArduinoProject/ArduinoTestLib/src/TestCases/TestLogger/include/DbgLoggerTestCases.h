//#pragma once

#ifndef _DBLOGGERTESTCASES_h
#define _DBLOGGERTESTCASES_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "TestCaseBase.h"

class DbgLoggerTestCases : public TestCaseBase {
protected:
	void DoSpecificTests(TestResults* results) override;
	//void PrintTestSectionHeader("") override;
};

#endif