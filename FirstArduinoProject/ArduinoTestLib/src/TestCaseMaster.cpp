// 
// 
// 

#include "TestCaseMaster.h"
#include "TestCases/TestLogger/include/DbgLoggerTestCases.h"
//#include "TestLogger/include/DbgLoggerTestCases.h"

TestCaseMaster::TestCaseMaster() {
}

TestResults* TestCaseMaster::ExecuteTests() {
	// List of test objects to execute
	DbgLoggerTestCases loggerTests;
	
	TestResults tmp;
	this->results.Reset();
	this->results.Increment(loggerTests.DoTests(&tmp));
}


void TestCaseMaster::PrintResultsToDebug() {
	Serial.println("----------------------------------");
	Serial.println("|----- Grand Totals --------------");
	Serial.println("----------------------------------");
	Serial.print("    OK: "); Serial.println(this->results.OK());
	Serial.print("Failed: "); Serial.println(this->results.Failed());
	Serial.print(" Total: "); Serial.println(this->results.Total());
	Serial.println("----------------------------------");
}