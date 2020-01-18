

#include "TestCases/TestLogger/include/DbgLoggerTestCases.h"

//
void DbgLoggerTestCases::DoSpecificTests(TestResults* results) {
	results->AddResult(true);
	results->AddResult(true);
	results->AddResult(true);
	results->AddResult(false);
	results->AddResult(false);
}