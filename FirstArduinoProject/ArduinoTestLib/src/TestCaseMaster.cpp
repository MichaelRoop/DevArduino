// 
// 
// 

#include "TestCaseMaster.h"
// TODO - find out why it is not finding the headers
//#include "src/TestCases/TestLogger/include/DbgLoggerTestCases.h"
#include "D:\Dev\Arduino\FirstArduinoProject\ArduinoTestLib\src\TestCases\TestLogger\include/DbgLoggerTestCases.h"

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
	Serial.print("    OK: "); Serial.println(this->results.OK());
	Serial.print("Failed: "); Serial.println(this->results.Failed());
	Serial.print(" Total: "); Serial.println(this->results.Total());
}