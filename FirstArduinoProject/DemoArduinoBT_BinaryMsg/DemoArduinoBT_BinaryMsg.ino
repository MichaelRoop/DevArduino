/*
 Name:		DemoArduinoBT_BinaryMsg.ino
 Created:	4/3/2021 5:15:54 PM
 Author:	Michael
*/
#include "MsgDefines.h"
#include "MessageHelpers.h"
#include "MsgMessages.h"
#include "TempProcessing.h"
#include "MsgEnumerations.h"
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
#define IN_BUFF_SIZE 30

#endif // !SECTION_DEFINES

#ifndef SECTION_VARIABLES

int lastA0Value;
int lastPinX;

// Just have the types you need in your application. One can be used for 
// multiple outgoing IOs by changing the ID and Value before send
MsgFloat32 outFloat;
MsgBool outBool;
MsgUInt8 outUint8;

// Process the temperature
TemperatureProcessor temperatureProcessor;

// In buffer. Currently largest message is 12 bytes. We can just copy to 
// A specific message in a function to avoid reserving that memory
uint8_t buff[IN_BUFF_SIZE];
uint8_t currentPos = 0;
uint8_t currentRemaining = 0;

// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX

#endif // !SECTION_VARIABLES

#ifndef SECTION_ARDUINO_FUNCTIONS

void setup() {
	Serial.begin(115200);
	while (!Serial) { }
	// Must set the baud via an AT command AT+BAUD=115200,1,0
	btSerial.begin(115200);

	while (!btSerial) {	}
	Serial.println("...");
	Initialize();
	//DbgMsgs();
	//MsgHelpers::Execute();
}

void loop() {
	ListenForData();
	CheckForSendBackData();
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
	lastPinX = 0xFFFFFFFF;
	ResetInBuff();

	//-------------------------------------------------------------
	// Register expected id/data type combination for incoming msgs
	// Expecting bool to turn LED on or off
	MsgHelpers::RegisterInIds(IN_MSG_ID_LED_BLUE_PIN, typeBool);
	MsgHelpers::RegisterInIds(IN_MSG_ID_LED_RED_PIN, typeBool);

	// Expectin Uint 8 msg with values 0-255 for PWM on digital pin
	MsgHelpers::RegisterInIds(IN_MSG_ID_PMW_PIN_X, typeUInt8);
	MsgHelpers::RegisterInIds(IN_MSG_ID_PMW_PIN_Y, typeUInt8);

	// Will be raised from parsed incoming message if msg value is of certain type
	// Register all you require for your program
	MsgHelpers::RegisterFuncBool(&CallbackBoolValue);
	MsgHelpers::RegisterFuncUInt8(&CallbackUint8Value);
	MsgHelpers::RegisterErrCallback(&ErrCallback);
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
		//Serial.print("GetNewMsg:"); Serial.println(available);
		currentPos = btSerial.readBytes(buff, MSG_HEADER_SIZE);
		if (MsgHelpers::ValidateHeader(buff, currentPos)) {
			currentRemaining = (MsgHelpers::GetSizeFromHeader(buff) - MSG_HEADER_SIZE);
			if (btSerial.available() >= currentRemaining) {
				GetRemainingMsgFragment(btSerial.available());
			}
		}
		else {

			Serial.print("GetNewMsgERR- currentPos:"); Serial.println(currentPos);
			Serial.print("---:");
			Serial.print(buff[0]); Serial.print(",");
			Serial.print(buff[1]); Serial.print(",");
			Serial.print(buff[2]); Serial.print(",");
			Serial.print(buff[3]); Serial.print(",");
			Serial.print(buff[4]); Serial.print(",");
			Serial.print(buff[5]); Serial.print(",");
			Serial.print(buff[6]); Serial.print(",");
			Serial.println(buff[8]);

			PurgeBuffAndBT();
		}
	}
}


// Get enough bytes to make a completed message and process result
void GetRemainingMsgFragment(int available) {
	if (available >= currentRemaining) {
		size_t count = btSerial.readBytes(buff + currentPos, currentRemaining);
		//Serial.print("GetFrag:"); Serial.print(currentRemaining); Serial.print(":"); Serial.println(count);
		currentPos += count;
		MsgHelpers::ValidateMessage(buff, currentPos);
		ResetInBuff();
	}
}

#endif // !SECTION_INCOMING_MSGS

#ifndef SECTION_CALLBACKS


//#define VERBOSE_DEBUG 1
void ErrCallback(MsgError err) {
	// TODO - send msg back to client
	Serial.print("----Err:"); Serial.print(err);
#ifdef VERBOSE_DEBUG
	switch (err) {
	case err_NoErr:
		Serial.println(" no err");
		break;
	case err_InvalidType:
		Serial.println(" Invalid data type");
		break;
	case err_InvalidHeaderSize:
		Serial.println(" Invalid header size");
		break;
	case err_StartDelimiters:
		Serial.println(" Err with start delimiters");
		break;
	case err_InvalidSizeField:
		Serial.println(" Invalid msg size value");
		break;
	case err_InvalidPayloadSizeField:
		Serial.println(" Bad payload size value");
		break;
	case err_InvalidDataTypeForRegisteredId:
		Serial.println(" Invalid data type for msg ID");
		break;
	case err_CallbackNotRegisteredForId:
		Serial.println(" No callback for msg ID");
		break;
	default:
		Serial.println(" Unhandled");
		break;
	}

#else
	Serial.println("");
#endif // VERBOSE_DEBUG

}


void CallbackBoolValue(uint8_t id, bool value) {
	Serial.print("bool-id:"); Serial.print(id); Serial.print(" Val:"); Serial.println(value);
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


void CallbackUint8Value(uint8_t id, uint8_t value) {
	// Debug only 
	//Serial.print("U8-id:"); Serial.print(id); Serial.print(" Val:"); Serial.println(value);
	// Analog writes from 0 - 255 (8 bits), so we use UInt8
	// Reads from 0 - 1023 (10 bits)
	switch (id) {
	case IN_MSG_ID_PMW_PIN_X:
		analogWrite(PMW_PIN_X, value);
		// For demo, bounce back the value
		SendUint8Msg(IN_MSG_ID_PMW_PIN_X, value);
		break;
	case IN_MSG_ID_PMW_PIN_Y:
		analogWrite(PMW_PIN_Y, value);
		break;
	default:
		// TODO - error msg if desired
		break;
	}
}


#endif // !SECTION_CALLBACKS

#ifndef SECTION_OUTGOING_MSGS

// May have to put these in a stack where the send can happend when other sends complete
void CheckForSendBackData() {
	// In our demo, a KY-013 temperature sensor is attached to Analog pin A0
	if (ChatterFiltered(analogRead(A0), &lastA0Value, ANALOG_0_ID)) {
		SendTemperature(lastA0Value);
	}


	//if (ChatterFiltered(analogRead(PMW_PIN_X), &lastPinX, IN_MSG_ID_PMW_PIN_X)) {
	//	SendUint8Msg(IN_MSG_ID_PMW_PIN_X, lastPinX);
	//}
	//int val = analogRead(PMW_PIN_X);
	//if (val != lastPinX) {
	//	lastPinX = val;
	//	SendUint8Msg(IN_MSG_ID_PMW_PIN_X, val);
	//}

	// TODO - put the PWM 12 here

}


bool ChatterFiltered(int current, int* last, uint8_t pinId) {
	if ((current - ANALOG_DEBOUNCE_GAP) > *last ||
		(current + ANALOG_DEBOUNCE_GAP) < *last) {
		*last = current;
		return true;
	}
	return false;
}

/*
// Use any of the following with the specified messge to send info to Dashboard
void SendBoolMsg(uint8_t id, bool value) {
	outBool.Id = id;
	outBool.Value = value;
	SendMsg(&outBool, outBool.Size);
}

void SendInt8Msg(uint8_t id, int8_t value) {
}

void SendInt16Msg(uint8_t id, int16_t value) {
}

void SendInt32Msg(uint8_t id, int32_t value) {
}

void SendUInt8Msg(uint8_t id, uint8_t value) {
}

void SendUInt16Msg(uint8_t id, uint16_t value) {
}

void SendUInt32Msg(uint8_t id, uint32_t value) {
}
*/

void SendFloatMsg(uint8_t id, float value) {
	outFloat.Id = id;
	outFloat.Value = value;
	Serial.println(value);
	SendMsg(&outFloat, outFloat.Size);
}


void SendUint8Msg(uint8_t id, uint8_t value) {
	outUint8.Id = id;
	outUint8.Value = value;
	//Serial.println(value);
	SendMsg(&outUint8, outUint8.Size);
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
//	MsgInt16 i16;
//	MsgInt32 i32;
//	MsgUInt8 u8;
//	MsgUInt16 u16;
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
//
//	Serial.println("\n\n");
//	MsgBool bx(1, true);
//	MsgInt8 i8x(2, -12);
//	MsgInt16 i16x(4, -13);
//	MsgInt32 i32x(6, -14);
//	MsgUInt8 u8x(3, 15);
//	MsgUInt16 u16x(5, 16);
//	MsgUInt32 u32x(7, 17);
//	MsgFloat32 fx(7, 187.77);
//
//	Serial.print("   bool: "); Serial.print(bx.DataType); Serial.print(" - "); Serial.print(bx.Size); Serial.print(" : "); Serial.println(bx.Value);
//	Serial.print("   Int8: "); Serial.print(i8x.DataType); Serial.print(" - "); Serial.print(i8x.Size); Serial.print(" : "); Serial.println(i8x.Value);
//	Serial.print("  Int16: "); Serial.print(i16x.DataType); Serial.print(" - "); Serial.print(i16x.Size); Serial.print(" : "); Serial.println(i16x.Value);
//	Serial.print("  Int32: "); Serial.print(i32x.DataType); Serial.print(" - "); Serial.print(i32x.Size); Serial.print(" : "); Serial.println(i32x.Value);
//	Serial.print("  UInt8: "); Serial.print(u8x.DataType); Serial.print(" - "); Serial.print(u8x.Size); Serial.print(" : "); Serial.println(u8x.Value);
//	Serial.print(" UInt16: "); Serial.print(u16x.DataType); Serial.print(" - "); Serial.print(u16x.Size); Serial.print(" : "); Serial.println(u16x.Value);
//	Serial.print(" UInt32: "); Serial.print(u32x.DataType); Serial.print(" - "); Serial.print(u32x.Size); Serial.print(" : "); Serial.println(u32x.Value);
//	Serial.print("Float32: "); Serial.print(fx.DataType); Serial.print(" - "); Serial.print(fx.Size); Serial.print(" : "); Serial.println(fx.Value);
//
//
//
//}

#endif // !SECTION_DBG



