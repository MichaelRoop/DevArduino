/*
 Name:		DemoArduinoBT_BinaryMsg.ino
 Created:	4/3/2021 5:15:54 PM
 Author:	Michael
*/
#include <SoftwareSerial.h>

#ifndef SECTION_TYPES_AND_ENUMS

// Holds any value in one variable
typedef __attribute__((packed)) union  {
	bool boolVal;
	int8_t int8Val;
	uint8_t uint8Val;
	int16_t int16Val;
	uint16_t uint16Val;
	int32_t int32Val;
	uint32_t uint32Val;
	// The string must include the null terminator within the 32 bytes
	char stringVal[sizeof(uint32_t)];
} MsgValue;


typedef __attribute__((packed)) struct  {
	uint8_t sDelimiter1;
	uint8_t sDelimiter2;
	uint8_t id;
	// DataType enum value identifies type held in value
	uint8_t dataType;
	// The payload
	MsgValue value;
	uint8_t eDelimiter1;
	uint8_t eDelimiter2;
} MsgStruct;


enum DataType {
	tyepUndefined = 0,
	typeBool = 1,
	typeInt8 = 2,
	typeUInt8 = 3,
	typeInt16 = 4,
	typeUInt16 = 5,
	typeInt32 = 6,
	typeUInt32 = 7,
	typeString = 8,
	typeInvalid = 9
};


// Start of heading, start of text
#define SOH 0x01
#define STX 0x02
// End of text end of transmission
#define ETX 0x03
#define EOT 0x04

// These are the IDs for the outgoing messages
#define ANALOG_1_ID 20
#define ANALOG_2_ID 21


#endif // !SECTION_TYPES_AND_ENUMS

#ifndef SECTION_VARIABLES

MsgStruct inMsg;
MsgStruct outMsg;
int lastA0Value;
int lastA1Value;

// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX

#endif // !SECTION_VARIABLES

void setup() {
	Serial.begin(115200);

	while (!Serial) { }
	Serial.println("Dbg serial");
	btSerial.begin(38400);
	while (!btSerial) {	}
	Serial.println("BT running");
	Initialize();

	Serial.print(" Size of Msg:"); Serial.println(sizeof(inMsg));
	SendBoolMsg(111, false);
	SendUint32Msg(32, 31000);

}

// the loop function runs over and over again until power down or reset
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

	// Analog read values
	lastA0Value = 0;
	lastA1Value = 0;

	// The out messagea
	memset(&outMsg, 0, sizeof(MsgStruct));
	outMsg.sDelimiter1 = SOH;
	outMsg.sDelimiter2 = STX;
	outMsg.eDelimiter1 = ETX;
	outMsg.eDelimiter2 = EOT;
	
	ResetInMsg();
}


void ResetInMsg() {
	memset(&inMsg, 0, sizeof(MsgStruct));
}


void ListenForData() {
	int available = btSerial.available();
	if (available > 0) {
		if (available >= sizeof(MsgStruct)) {
			ResetInMsg();
			if (ValidateIncomingMsg(btSerial.readBytes((uint8_t*)&inMsg, sizeof(MsgStruct)))) {
				if (ValidateIncomingValue()) {
					ApplyInMsgToIO();
				}
			}
		}
	}
}


// May have to put these in a stack where the send can happend when other sends complete
void CheckForSendBackData() {
	int tmp = analogRead(A0);
	if (tmp != lastA0Value) {
		lastA0Value = tmp;
		SendInt16Msg(ANALOG_1_ID, lastA0Value);
	}
	tmp = analogRead(A1);
	if (tmp != lastA1Value) {
		lastA1Value = tmp;
		SendInt16Msg(ANALOG_2_ID, lastA1Value);
	}
}


#endif // !SECTION_PRIVATE_HELPERS

#ifndef SECTION_INPUT_MSG_HELPERS

bool ValidateIncomingMsg(size_t readCount) {
	if (readCount != sizeof(MsgStruct)) {
		Serial.println("Misread in bytes");
		return false;
	}

	// Validate the packet
	if (inMsg.sDelimiter1 != SOH ||
		inMsg.sDelimiter2 != STX ||
		inMsg.eDelimiter1 != ETX ||
		inMsg.eDelimiter2 != EOT) {
		Serial.println("Delimiters not matching");
		return false;
	}

	// Validate data type
	if (inMsg.dataType == 0 || inMsg.dataType >= typeInvalid) {
		Serial.println("Invalid data type");
		return false;
	}
	return true;
}


//Validate that the correct data type for your IO point
bool ValidateIncomingValue() {
	
	// Note: The id does not need to match the pin number. The id is whatever 
	// you set it to.  You can map id to pin here. Just using same for simplicity
	bool result = false;
	switch (inMsg.id) {
	case 1:
	case 2:
		// Uno digital pins
		result = inMsg.dataType = typeBool;
		break;
	case 3:
	case 4:
		// Uno pins with PMW
		result = inMsg.dataType = typeUInt8;
		break;
		// TODO - add analog
	default:
		Serial.print("Unhandled ID:"); Serial.println(inMsg.id);
		return false;
	}

	if (result == false) {
		Serial.print("Type:"); Serial.print(inMsg.dataType); Serial.print(" invalid for Id:"); Serial.print(inMsg.id);
	}
	return result;
}


// Apply the value to the correct IO
void ApplyInMsgToIO() {
	switch (inMsg.id) {
	case 1:
		digitalWrite(1, (inMsg.value.boolVal == true) ? HIGH : LOW);
		break;
	case 2:
		digitalWrite(2, (inMsg.value.boolVal == true) ? HIGH : LOW);
		break;
	case 3:
		analogWrite(9, inMsg.value.uint8Val);
		break;
	case 4:
		analogWrite(10, inMsg.value.uint8Val);
		break;
		// TODO Add analog
	default:
		// Should never happen. Already validated
		break;
	}

}


#endif // !SECTION_INPUT_MSG_HELPERS

#ifndef SECTION_OUTPUT_MSG_HELPERS

void SendBoolMsg(uint8_t id, bool value) {
	outMsg.id = id;
	outMsg.dataType = typeBool;
	outMsg.value.boolVal = value;
	SendMsg();
}


void SendInt16Msg(uint8_t id, int value) {
	outMsg.id = id;
	outMsg.dataType = typeInt16;
	outMsg.value.int16Val = (int16_t)value;
	SendMsg();
}


void SendUint32Msg(uint8_t id, uint32_t value) {
	outMsg.id = id;
	outMsg.dataType = typeInt32;
	outMsg.value.uint32Val = value;
	SendMsg();
}






void SendMsg() {
	DbgDumpOutMsg();
	size_t sent = btSerial.write((uint8_t*)&outMsg, sizeof(MsgStruct));
	Serial.print("Size sent:"); Serial.println(sent);
}




void DbgDumpOutMsg() {
	Serial.println("");
	Serial.print("sDel1:"); Serial.println(outMsg.sDelimiter1);
	Serial.print("sDel2:"); Serial.println(outMsg.sDelimiter2);
	Serial.print("   Id:"); Serial.println(outMsg.id);
	Serial.print(" Type:"); Serial.println(outMsg.dataType);
	Serial.print("Value b:"); Serial.println(outMsg.value.boolVal);
	Serial.print("Value:"); Serial.println(outMsg.value.uint32Val);
	Serial.print("eDel1:"); Serial.println(outMsg.eDelimiter1);
	Serial.print("eDel2:"); Serial.println(outMsg.eDelimiter2);
}

#endif // !SECTION_OUTPUT_MSG_HELPERS






