#include <ArduinoDbgLog.h>
#include <stdio.h>
//#include <stdarg.h>

#include <TestSub/include/TestSub.h>

//void DBG_OUTPUT_HELPER::DO_IT(const char* format, ... ts) {
//	char data[100];
//	sprintf(data, format, ts);
//	Serial.print(data);
//
//	//int value = 0;
//	//va_list argList;
//	//va_start(argList, num);
//	//for (; num; num--) {
//	//	value = va_arg(argList, int);
//	//	Serial.print()
//	//}
//
//}

void TestSubCaller::InvokeSub() {
	TestSub ts;
	ts.PrintMsg();
}