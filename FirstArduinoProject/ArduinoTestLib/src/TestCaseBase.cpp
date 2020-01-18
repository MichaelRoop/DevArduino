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
	// by default it does nothing
}

