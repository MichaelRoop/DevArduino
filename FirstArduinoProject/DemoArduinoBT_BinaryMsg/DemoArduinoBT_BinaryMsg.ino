/*
 Name:		DemoArduinoBT_BinaryMsg.ino
 Created:	4/3/2021 5:15:54 PM
 Author:	Michael
*/
#include "MessageHelpers.h"
#include "MsgMessages.h"
#include "TempProcessing.h"
#include "MsgEnumerations.h"
#include "MsgDefines.h"
#include <SoftwareSerial.h>

#ifndef SECTION_DEFINES

// These are the IDs for the outgoing messages
#define ANALOG_0_ID 20
#define ANALOG_1_ID 21

// MSG IDs for incoming message
#define IN_MSG_ID_LED_RED_PIN 10
#define IN_MSG_ID_LED_BLUE_PIN 11
#define IN_MSG_ID_PMW_PIN_X 12
#define IN_MSG_ID_PMW_PIN_Y 13

// Arduino physical pins
#define LED_RED_PIN 1
#define LED_BLUE_PIN 2
#define PMW_PIN_X 9
#define PMW_PIN_Y 10

// Analog debounce limit
#define ANALOG_DEBOUNCE_GAP 5

// We never copy in more than the max current message size of 12
#define IN_BUFF_SIZE 15

#endif // !SECTION_DEFINES

#ifndef SECTION_VARIABLES

int lastA0Value;
int lastA1Value;

// Just have the types you need in your application. One can be used for 
// multiple outgoing IOs by changing the ID and Value before send
MsgFloat32 outFloat;

// Process the temperature
TemperatureProcessor temperatureProcessor;

// In buffer. Currently largest message is 12 bytes. We can just copy to 
// A specific message in a function to avoid reserving that memory
uint8_t buff[IN_BUFF_SIZE];
uint8_t currentPos = 0;
uint16_t currentRemaining = 0;

// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX

#endif // !SECTION_VARIABLES

#ifndef SECTION_ARDUINO_FUNCTIONS

void setup() {
	Serial.begin(115200);
	while (!Serial) { }
	btSerial.begin(38400);
	while (!btSerial) {	}
	Serial.println("BT ready");
	Initialize();
	//DbgMsgs();

	MsgHelpers::Execute();
}

void loop() {
	ListenForData();
	CheckForSendBackData();
	delay(10);
}

#endif // !SECTION_ARDUINO_FUNCTIONS

#ifndef SECTION_PRIVATE_HELPERS

// First time initializations
void Initialize() {

	// Digital Pins for demo. Will differ per application
	pinMode(LED_RED_PIN, OUTPUT);
	pinMode(LED_BLUE_PIN, OUTPUT);

	// Digital Pins with PWM for demo. Will differ per application
	pinMode(PMW_PIN_X, OUTPUT);
	pinMode(PMW_PIN_Y, OUTPUT);

	// Analog read values. Out of range forces first value to send
	lastA0Value = 0xFFFFFFFF;
	lastA1Value = 0xFFFFFFFF;
	ResetInBuff();

	//-------------------------------------------------------------
	// Register expected id/data type combination for incoming msgs
	// Expecting bool to turn LED on or off
	MsgHelpers::RegisterInIds(IN_MSG_ID_LED_BLUE_PIN, typeBool);
	MsgHelpers::RegisterInIds(IN_MSG_ID_LED_RED_PIN, typeBool);

	// Expectin Uint 8 msg with values 0-255 for PWM on digital pin
	MsgHelpers::RegisterInIds(IN_MSG_ID_PMW_PIN_X, typeUInt8);
	MsgHelpers::RegisterInIds(IN_MSG_ID_PMW_PIN_Y, typeUInt8);

	// Will be raised from in message parsed if msg value is of certain type
	// Register all required for your program
	MsgHelpers::RegisterFuncBool(&ExecuteBoolValue);
	MsgHelpers::RegisterFuncUInt8(&ExecuteUInt8Value);
	MsgHelpers::RegisterErrCallback(&ErrCallback);
	MsgHelpers::RegisterErrCallback(&ErrCallback);
}



void ErrCallback(MsgError err) {
	Serial.print("Err callback fired with value:"); Serial.println(err);
}



void ResetInBuff() {
	memset(buff, 0, IN_BUFF_SIZE);
	currentPos = 0;
	currentRemaining = 0;
}


// Purge all in the app buffer and Bluetooth serial buffer
void PurgeBuffAndBT() {
	int available = btSerial.available();
	if (available > 0) {
		btSerial.readBytes(buff, available);
	}
	ResetInBuff();
}

#endif // !SECTION_PRIVATE_HELPERS

#ifndef SECTION_INCOMING_MSGS

void ListenForData() {
	if (currentPos == 0) {
		GetNewMsg(btSerial.available());
	}
	else {
		GetRemainingMsgFragment(btSerial.available());
	}
}


// New message arriving. Don't pick up until the entire header is in BT buffer
void GetNewMsg(int available) {
	if (available >= MSG_HEADER_SIZE) {
		currentPos = btSerial.readBytes(buff, MSG_HEADER_SIZE);
		if (MsgHelpers::ValidateHeader(buff, currentPos)) {
			currentRemaining = (MsgHelpers::GetSizeFromHeader(buff) - MSG_HEADER_SIZE);
		}
		else {
			PurgeBuffAndBT();
			Serial.println("Bad header data");
		}
	}
}


// Get enough bytes to make a completed message and process result
void GetRemainingMsgFragment(int available) {
	if (available >= currentRemaining) {
		size_t count = btSerial.readBytes(buff + currentPos, currentRemaining);
		currentPos += count;
		if (MsgHelpers::ValidateMessage(buff, currentPos)) {
			ProcessInMsgBuff(buff, 0);
		}
		ResetInBuff();
	}
}


bool ProcessInMsgBuff(uint8_t* buff, uint16_t size) {
	MsgDataType dataType = MsgHelpers::GetDataTypeFromHeader(buff);
	switch (dataType) {
	case typeBool:
		MsgBool msgBool;
		memcpy(&msgBool, buff, sizeof(MsgBool));
		return ApplyMsgBool(&msgBool);
	case typeInt8:
		MsgInt8 msgInt8;
		memcpy(&msgInt8, buff, sizeof(MsgInt8));
		return ApplyMsgInt8(&msgInt8);
	case typeUInt8:
		MsgUInt8 msgUInt8;
		memcpy(&msgUInt8, buff, sizeof(MsgUInt8));
		return ApplyMsgUInt8(&msgUInt8);
	case typeInt16:
		MsgInt16 msgInt16;
		memcpy(&msgInt16, buff, sizeof(MsgInt16));
		return ApplyMsgInt16(&msgInt16);
	case typeUInt16:
		MsgUInt16 msgUInt16;
		memcpy(&msgUInt16, buff, sizeof(MsgUInt16));
		return ApplyMsgUInt16(&msgUInt16);
	case typeInt32:
		MsgInt32 msgInt32;
		memcpy(&msgInt32, buff, sizeof(MsgInt32));
		return ApplyMsgInt32(&msgInt32);
	case typeUInt32:
		MsgUInt32 msgUInt32;
		memcpy(&msgUInt32, buff, sizeof(MsgUInt32));
		return ApplyMsgUInt32(&msgUInt32);
	case typeFloat32:
		MsgFloat32 msgFloat32;
		memcpy(&msgFloat32, buff, sizeof(MsgFloat32));
		return ApplyMsgFloat32(&msgFloat32);
	case typeUndefined:
	case typeInvalid:
	default:
		// Should never happen, previously validate
		return false;
	}
}


void ExecuteBoolValue(uint8_t id, bool value) {
	Serial.print("ExecuteBoolValue id:"); Serial.print(id); Serial.print(" Value:"); Serial.println(value);
	switch (id) {
	case IN_MSG_ID_LED_RED_PIN:
		digitalWrite(LED_RED_PIN, value ? HIGH : LOW);
		break;
	case IN_MSG_ID_LED_BLUE_PIN:
		digitalWrite(LED_BLUE_PIN, value ? HIGH : LOW);
		break;
	default:
		// TODO - error msg if desired
		break;
	}
}


void ExecuteUInt8Value(uint8_t id, uint8_t value) {
	Serial.print("ExecuteUInt8Value id:"); Serial.print(id); Serial.print(" Value:"); Serial.println(value);

	// Analog writes from 0 - 255 (8 bits), so we use UInt8
	// Reads from 0 - 1023 (10 bits)
	switch (id) {
	case IN_MSG_ID_PMW_PIN_X:
		analogWrite(PMW_PIN_X, value);
		break;
	case IN_MSG_ID_PMW_PIN_Y:
		analogWrite(PMW_PIN_Y, value);
		break;
	default:
		// TODO - error msg if desired
		break;
	}
}




bool ApplyMsgBool(MsgBool* msg) {
	switch (msg->Id) {
	case IN_MSG_ID_LED_RED_PIN:
		digitalWrite(LED_RED_PIN, msg->Value ? HIGH : LOW);
		return true;
	case IN_MSG_ID_LED_BLUE_PIN:
		digitalWrite(LED_BLUE_PIN, msg->Value ? HIGH : LOW);
		return true;
	default:
		return false;
	}
}


bool ApplyMsgUInt8(MsgUInt8* msg) {
	// Analog writes from 0 - 255 (8 bits), so we use UInt8
	// Reads from 0 - 1023 (10 bits)
	switch (msg->Id) {
	case IN_MSG_ID_PMW_PIN_X:
		analogWrite(PMW_PIN_X, msg->Value);
		return true;
	case IN_MSG_ID_PMW_PIN_Y:
		analogWrite(PMW_PIN_Y, msg->Value);
		return true;
	default:
		return false;
	}
}


bool ApplyMsgInt8(MsgInt8* msg) {
	// Put in for demo completion. Not used in this implementation
	return true;
}


bool ApplyMsgUInt16(MsgUInt16* msg) {
	// Put in for demo completion. Not used in this implementation
	return true;
}


bool ApplyMsgInt16(MsgInt16* msg) {
	// Put in for demo completion. Not used in this implementation
	return true;
}


bool ApplyMsgUInt32(MsgUInt32* msg) {
	// Put in for demo completion. Not used in this implementation
	return true;
}


bool ApplyMsgInt32(MsgInt32* msg) {
	// Put in for demo completion. Not used in this implementation
	return true;
}


bool ApplyMsgFloat32(MsgFloat32* msg) {
	// Put in for demo completion. Not used in this implementation
	return true;
}

#endif // !SECTION_INCOMING_MSGS

#ifndef SECTION_OUTGOING_MSGS

// May have to put these in a stack where the send can happend when other sends complete
void CheckForSendBackData() {
	// In our demo, a KY-013 temperature sensor is attached to Analog pin A0
	if (ChatterFiltered(analogRead(A0), &lastA0Value, ANALOG_0_ID)) {
		SendTemperature(lastA0Value);
	}
	//ChatterFiltered(analogRead(A1), &lastA1Value, ANALOG_1_ID);
}


bool ChatterFiltered(int current, int* last, uint8_t pinId) {
	if ((current - ANALOG_DEBOUNCE_GAP) > *last ||
		(current + ANALOG_DEBOUNCE_GAP) < *last) {
		*last = current;
		return true;
	}
	return false;
}


//void SendBoolMsg(uint8_t id, bool value) {
//}

//void SendInt8Msg(uint8_t id, int8_t value) {
//}

//void SendInt16Msg(uint8_t id, int16_t value) {
//}

//void SendInt32Msg(uint8_t id, int32_t value) {
//}

//void SendUInt8Msg(uint8_t id, uint8_t value) {
//}

//void SendUInt16Msg(uint8_t id, uint16_t value) {
//}

//void SendUInt32Msg(uint8_t id, uint32_t value) {
//}



void SendFloatMsg(uint8_t id, float value) {
	outFloat.Id = id;
	outFloat.Value = value;
	Serial.println(value);
	SendMsg(&outFloat, outFloat.Size);
}


void SendMsg(void* msg, int size) {
	size_t sent = btSerial.write((uint8_t*)msg, size);
	//Serial.print("Size sent:"); Serial.println(sent);
}


// Convert raw sensor to temperature
void SendTemperature(int sensorValue) {
	temperatureProcessor.ProcessRaw(sensorValue);
	SendFloatMsg(ANALOG_0_ID, temperatureProcessor.Celcius());
	//SendFloatMsg(ANALOG_0_ID, tempProcessor.Kelvin());
	//SendFloatMsg(ANALOG_0_ID, tempProcessor.Farenheit());
}

#endif // !SECTION_OUTGOING_MSGS

#ifndef SECTION_DBG

//void DbgMsgs() {
//	MsgFloat32 f;
//	f.Id = 123;
//	f.Value = 321.2;
//	Serial.println(f.SOH);
//	Serial.println(f.STX);
//	Serial.println(f.Size);
//	Serial.println(f.DataType);
//	Serial.println(f.Id);
//	Serial.println(f.Value);
//	Serial.println(f.ETX);
//	Serial.println(f.EOT);
//
//	MsgBool b;
//	MsgInt8 i8;
//	MsgUInt8 u8;
//	MsgInt16 i16;
//	MsgUInt16 u16;
//	MsgInt32 i32;
//	MsgUInt32 u32;
//
//	Serial.print("   bool: "); Serial.print(b.DataType); Serial.print(" - "); Serial.print(b.Size); Serial.print(" - "); Serial.println(sizeof(b));
//	Serial.print("  UInt8: "); Serial.print(i8.DataType); Serial.print(" - "); Serial.print(i8.Size); Serial.print(" - "); Serial.println(sizeof(i8));
//	Serial.print("   Int8: "); Serial.print(u8.DataType); Serial.print(" - "); Serial.print(u8.Size); Serial.print(" - "); Serial.println(sizeof(u8));
//	Serial.print(" Uint16: "); Serial.print(i16.DataType); Serial.print(" - "); Serial.print(i16.Size); Serial.print(" - "); Serial.println(sizeof(i16));
//	Serial.print("  Int16: "); Serial.print(u16.DataType); Serial.print(" - "); Serial.print(u16.Size); Serial.print(" - "); Serial.println(sizeof(u16));
//	Serial.print(" UInt32: "); Serial.print(i32.DataType); Serial.print(" - "); Serial.print(i32.Size); Serial.print(" - "); Serial.println(sizeof(i32));
//	Serial.print("  Int32: "); Serial.print(u32.DataType); Serial.print(" - "); Serial.print(u32.Size); Serial.print(" - "); Serial.println(sizeof(u32));
//	Serial.print("Float32: "); Serial.print(f.DataType); Serial.print(" - "); Serial.print(f.Size); Serial.print(" - "); Serial.println(sizeof(f));
//}

#endif // !SECTION_DBG



