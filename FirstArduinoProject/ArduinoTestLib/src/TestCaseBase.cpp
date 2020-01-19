// 
// 
// 
// TODO why not src/ required
#include "TestCaseBase.h"


TestResults* TestCaseBase::DoTests(TestResults* results) {
	results->Reset();
	this->DoSpecificTests(results);
	return results;
}


void TestCaseBase::DoSpecificTests(TestResults* results) {
	//this->PrintTestSectionHeader();
	// by default it does nothings
}


void TestCaseBase::PrintTestSectionHeader(const char* msg) {
	Serial.println("----------------------------------");
	Serial.print("|--- ");
	Serial.println(msg);
	Serial.println("----------------------------------");
}


void TestCaseBase::PrintTestSectionFooter(TestResults* results) {
	Serial.println("----------------------------------");
	Serial.print("    OK: "); Serial.println(results->OK());
	Serial.print("Failed: "); Serial.println(results->Failed());
	Serial.print(" Total: "); Serial.println(results->Total());
	Serial.println("----------------------------------");
	Serial.println("");
}

bool TestCaseBase::PrintTestResult(bool isOk, const char* name) {
	if (isOk) {
		Serial.print("    OK: "); 
	}
	else {
		Serial.print("Failed: "); 
	}
	Serial.println(name);
	return isOk;
}