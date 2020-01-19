

#include "TestCases/TestLogger/include/DbgLoggerTestCases.h"

//
void DbgLoggerTestCases::DoSpecificTests(TestResults* results) {
	this->PrintTestSectionHeader("Bogus test cases");
	results->AddResult(this->PrintTestResult(true, "Test1"));
	results->AddResult(this->PrintTestResult(true, "Test2"));
	results->AddResult(this->PrintTestResult(true, "Test3"));
	results->AddResult(this->PrintTestResult(false, "Test4"));
	results->AddResult(this->PrintTestResult(false, "Test5"));
	this->PrintTestSectionFooter(results);
}