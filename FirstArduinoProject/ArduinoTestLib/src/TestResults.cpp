// 
// 
// 

#include "TestResults.h"


TestResults::TestResults() {
	this->Reset();
}


void TestResults::Reset() {
	this->failed = 0;
	this->ok = 0;
	this->total = 0;
}


void TestResults::AddResult(bool result) {
	if (result) {
		this->ok++;
	}
	else {
		this->failed++;
	}
	this->total = this->ok + this->failed;
}


void TestResults::Increment(TestResults* results) {
	this->failed += results->failed;
	this->ok += results->ok;
	this->total += results->total;
}


int TestResults::OK() {
	return this->ok;
}


int TestResults::Failed() {
	return this->failed;
}


int TestResults::Total() {
	return this->total;
}
