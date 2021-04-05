/*
 Name:		DemoArduinoBT_BinaryMsg.ino
 Created:	4/3/2021 5:15:54 PM
 Author:	Michael
*/
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

#define IN_BUFF_SIZE 20

#endif // !SECTION_DEFINES

#ifndef SECTION_VARIABLES

int lastA0Value;
int lastA1Value;

// Just have the types you need in your application. One can be used for 
// multiple outgoing IOs by changing the ID and Value before send
MsgFloat32 outFloat;

// In buffer. Currently largest message is 12 bytes. We can just copy to 
// A specific message in a function to avoid reserving that memory
uint8_t buff[IN_BUFF_SIZE];
uint8_t currentPos = 0;

TemperatureProcessor tempProcessor;

// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX

#endif // !SECTION_VARIABLES


void setup() {
	Serial.begin(115200);
	while (!Serial) { }
	btSerial.begin(38400);
	while (!btSerial) {	}
	Serial.println("BT ready");
	Initialize();

	//DbgMsgs();
}

void loop() {
	ListenForData();
	CheckForSendBackData();
	delay(10);
}


#ifndef SECTION_PRIVATE_HELPERS

// First time initializations
void Initialize() {

	// Digital Pins for demo. Will differ per application
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);

	// Digital Pins with PWM for demo. Will differ per application
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);

	// Analog read values. Out of range forces first value to send
	lastA0Value = 0xFFFFFFFF;
	lastA1Value = 0xFFFFFFFF;
	ResetInBuff();
}


void ResetInBuff() {
	memset(buff, 0, IN_BUFF_SIZE);
	currentPos = 0;
}


void ListenForData() {
	int available = btSerial.available();
	if (available > 0) {
		if (currentPos == 0) {
			// New message arriving
			if (available >= MSG_HEADER_SIZE) {
				size_t count = btSerial.readBytes(buff, MSG_HEADER_SIZE);
				if (count == MSG_HEADER_SIZE) {
					if (ValidateHeader(buff)) {
						uint16_t size = GetSizeFromHeader(buff);
						if (size > 0) {
							uint16_t remaining = size - MSG_HEADER_SIZE;
							available = btSerial.available();
							if (available >= remaining) {
								// Copy in rest to buffer to make up complete message
								count = btSerial.readBytes(buff + MSG_HEADER_SIZE, remaining);
								if (count == remaining) {

								}
								else {
									ResetInBuff();
									Serial.println("Bad remaining read");
								}
							}
						}
						else {
							ResetInBuff();
							Serial.println("Bad msg size");
						}
					}
				}
				else {
					ResetInBuff();
					Serial.println("Bad header read");
				}




				//	ResetInMsg();
				//	if (ValidateIncomingMsg(btSerial.readBytes((uint8_t*)&inMsg, sizeof(MsgStruct)))) {
				//		if (ValidateIncomingValue()) {
				//			ApplyInMsgToIO();
				//		}
				//	}
			}
		}
		else {
			// another message part arriving
		}
	}
}


bool ValidateHeader(uint8_t* buff) {
	return
		(*(buff + SOH_POS) == _SOH) &&
		(*(buff + STX_POS) == _STX) &&
		(*(buff + TYPE_POS) > typeUndefined) &&
		(*(buff + TYPE_POS) < typeInvalid) &&
		// Following is validating the range of incoming
		// message IDs that you would have set in the dashboard
		// This can vary by dashboard configuration
		(*(buff + ID_POS) >= IN_MSG_ID_LED_RED_PIN) &&
		(*(buff + ID_POS) <= IN_MSG_ID_PMW_PIN_Y);
}


uint16_t GetSizeFromHeader(uint8_t* buff) {
	uint16_t size = 0;
	memcpy(&size, (buff + SIZE_POS), sizeof(uint16_t));
	// Message size must be at least big enough for header, footer and 1 byte payload
	if (size <= (MSG_HEADER_SIZE + MSG_FOOTER_SIZE + 1)) {
		size = 0;
	}
	return size;
}



// May have to put these in a stack where the send can happend when other sends complete
void CheckForSendBackData() {
	// In our demo, a KY-013 temperature sensor is attached to Analog pin A0
	if (DebounceValue(analogRead(A0), &lastA0Value, ANALOG_0_ID)) {
		SendTemperature(lastA0Value);
	}
	//DebounceValue(analogRead(A1), &lastA1Value, ANALOG_1_ID);
}


bool DebounceValue(int current, int* last, uint8_t pinId) {
	if ((current - ANALOG_DEBOUNCE_GAP) > *last ||
		(current + ANALOG_DEBOUNCE_GAP) < *last) {
		*last = current;
		return true;
	}
	return false;
}

#endif // !SECTION_PRIVATE_HELPERS

#ifndef SECTION_INPUT_MSG_HELPERS

//bool ValidateIncomingMsg(size_t readCount) {
//	if (readCount != sizeof(MsgStruct)) {
//		Serial.println("Misread in bytes");
//		return false;
//	}
//
//	// Validate the packet
//	if (inMsg.sDelimiter1 != SOH ||
//		inMsg.sDelimiter2 != STX ||
//		inMsg.eDelimiter1 != ETX ||
//		inMsg.eDelimiter2 != EOT) {
//		Serial.println("Delimiters not matching");
//		return false;
//	}
//
//	// Validate data type
//	if (inMsg.dataType == 0 || inMsg.dataType >= typeInvalid) {
//		Serial.println("Invalid data type");
//		return false;
//	}
//	return true;
//}


//// Validate that the correct data type for incoming IO point ID
//bool ValidateIncomingValue() {
//	bool result = false;
//	switch (inMsg.id) {
//	case LED_RED_PIN_ID:
//	case LED_BLUE_PIN_ID:
//		// Arduino UNO digital pins. Validate data type
//		result = inMsg.dataType = typeBool;
//		break;
//	case PMW_PIN_X_ID:
//	case PMW_PIN_Y_ID:
//		// UNO pins with PMW. Validate data type
//		result = inMsg.dataType = typeUInt8;
//		break;
//		// TODO - add analog
//	default:
//		Serial.print("Unhandled ID:"); Serial.println(inMsg.id);
//		return false;
//	}
//
//	if (result == false) {
//		Serial.print("Type:"); Serial.print(inMsg.dataType); Serial.print(" invalid for Id:"); Serial.print(inMsg.id);
//	}
//	return result;
//}


//// Apply the value to the correct IO
//void ApplyInMsgToIO() {
//	switch (inMsg.id) {
//	case LED_RED_PIN_ID:
//		digitalWrite(LED_RED_PIN, (inMsg.value.boolVal == true) ? HIGH : LOW);
//		break;
//	case LED_BLUE_PIN_ID:
//		digitalWrite(LED_BLUE_PIN, (inMsg.value.boolVal == true) ? HIGH : LOW);
//		break;
//	case 3:
//		analogWrite(PMW_PIN_X, inMsg.value.uint8Val);
//		break;
//	case 4:
//		analogWrite(PMW_PIN_Y, inMsg.value.uint8Val);
//		break;
//		// TODO Add analog
//	default:
//		// Should never happen. Already validated
//		break;
//	}
//}

#endif // !SECTION_INPUT_MSG_HELPERS

#ifndef SECTION_OUTPUT_MSG_HELPERS

//void SendBoolMsg(uint8_t id, bool value) {
//	outMsg.id = id;
//	outMsg.dataType = typeBool;
//	outMsg.value.boolVal = value;
//	SendMsg(typeBool);
//}
//
//
//void SendInt16Msg(uint8_t id, int value) {
//	outMsg.id = id;
//	outMsg.dataType = typeInt16;
//	outMsg.value.int16Val = (int16_t)value;
//	SendMsg(typeInt16);
//}
//
//
void SendFloatMsg(uint8_t id, float value) {
	outFloat.Id = id;
	outFloat.Value = value;
	Serial.println(value);
	SendMsg(&outFloat, outFloat.Size);
}


//
//
//
//void SendUint32Msg(uint8_t id, uint32_t value) {
//	outMsg.id = id;
//	outMsg.dataType = typeInt32;
//	outMsg.value.uint32Val = value;
//	SendMsg(typeInt32);
//}



void SendMsg(void* msg, int size) {
	size_t sent = btSerial.write((uint8_t*)msg, size);
	//Serial.print("Size sent:"); Serial.println(sent);
}


//void SendMsg(uint8_t dataType) {
//	DbgDumpOutMsg(dataType);
//	size_t sent = btSerial.write((uint8_t*)&outMsg, sizeof(MsgStruct));
//	//Serial.print("Size sent:"); Serial.println(sent);
//}

//<template typename T>
//void DbgDumpOutMsg<T>(MsgBinary<T> msg) {
//
//}



//void DbgDumpOutMsg(uint8_t dataType) {
//	//Serial.println("");
//	//Serial.print("sDel1:"); Serial.println(outMsg.sDelimiter1);
//	//Serial.print("sDel2:"); Serial.println(outMsg.sDelimiter2);
//	//Serial.print("   Id:"); Serial.println(outMsg.id);
//	//// The demo just sends back int16 so we will dump that value
//	//Serial.print(" Type:"); Serial.println(outMsg.dataType);
//	//Serial.print("Value:"); Serial.println(outMsg.value.int16Val);
//	//Serial.print("eDel1:"); Serial.println(outMsg.eDelimiter1);
//	//Serial.print("eDel2:"); Serial.println(outMsg.eDelimiter2);
//
//
//	Serial.print("Id:"); Serial.print(outMsg.id); Serial.print(", Value:"); 
//
//	switch (dataType) {
//	case typeBool:
//		Serial.println(outMsg.value.boolVal);
//		break;
//	case typeUInt16:
//		Serial.println(outMsg.value.int16Val);
//		break;
//	case typeInt32:
//		Serial.println(outMsg.value.int32Val);
//		break;
//	case typeFloat32:
//		Serial.println(outMsg.value.floatVal);
//		break;
//	default:
//		Serial.print("UNHANDLED for type:"); Serial.println(dataType);
//		break;
//	}
//
//	//Serial.print("Id:"); Serial.print(outMsg.id); Serial.print(", Value:"); Serial.println(outMsg.value.int16Val);
//
//}


// Convert raw sensor to temperature
void SendTemperature(int sensorValue) {
	tempProcessor.ProcessRaw(sensorValue);
	SendFloatMsg(ANALOG_0_ID, tempProcessor.Celcius());

	//SendFloatMsg(ANALOG_0_ID, tempProcessor.Kelvin());
	//SendFloatMsg(ANALOG_0_ID, tempProcessor.Farenheit());

	//SendFloatMsg(ANALOG_0_ID, TemperatureKelvin(sensorValue));
	//SendFloatMsg(ANALOG_0_ID, KelvinToCelcius(TemperatureKelvin(sensorValue)));
	//SendFloatMsg(ANALOG_0_ID, KelvinToFarenheit(TemperatureKelvin(sensorValue)));
}

#endif // !SECTION_OUTPUT_MSG_HELPERS



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








