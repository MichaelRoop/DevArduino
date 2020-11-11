
// Name:		ArduinoBluetoothDataTests.ino
// Created:	12/12/2019 3:14:05 PM
// Author:	Michael
//
// Sample to send and receive date to and from Arduino Bluetooth shield via serial monitor
//
// Written and tested in Visual Studio using Visual Micro
// Tested against Arduino Uno and itea Bluetooth shield with HC-05 module
//
// MUST HAVE BOARD TOGGLED TO DATA
// MUST HAVE BOARD JUMPERS SET FOLLOWING for IO4 TX and IO5 RX
//
//      0    6
//		같�|같  TX
//		같|같�
//		같같같  RX
//
// MUST HAVE DEBUG SERIAL SET TO 9600 Baud
// MUST HAVE BT SERIAL SET TO 38400 Baud
//
// Use a Bluetooth terminal program to send data. For example, BT Serial Terminal                                                                 
// on Windows
//
// Must pair with Bluetooth first
#include <SoftwareSerial.h>

#ifndef SECTION_DATA

// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX


#define IN_BUFF_SIZE 50
#define MSG_COMP_BUFF 50
char inBuff[IN_BUFF_SIZE];
char msgCmpBuff[IN_BUFF_SIZE];
int msgSize = 0;
unsigned char inIndex = 0;

char OPEN_DOOR_CMD_REV[] = "OpenDoor";
char CLOSE_DOOR_CMD_REV[] = "CloseDoor";
char OPEN_DOOR_CMD[] = "OpenDoor";
char CLOSE_DOOR_CMD[] = "CloseDoor";
int  OPEN_CMD_LEN = 8;
int CLOSE_CMD_LEN = 9;

#endif // !SECTION_DATA


// the setup function runs once when you press reset or power the board
void setup() {
	// There is some strange behaviour when using different baud rates
	// TODO - determine how to communicate in other baud rates for BT. Seems board may be fixed to 38400
	SetupCommunications(9600, 38400);
}

// the loop function runs over and over again until power down or reset
void loop() {
	//ListenToBTData();
	ListenForData();
	delay(10);
}

void ListenForData() {
	int available = btSerial.available();
	if (available > 0) {
		msgSize = 0;

		// Error check to avoid overrun of buffer
		if ((inIndex + available) > IN_BUFF_SIZE) {
			Blink();
			btSerial.write("ERROR-PURGING INPUT\r\n");
			Serial.write("ERROR-PURGING INPUT\r\n");
			inIndex = 0;
			return;
		}
		Serial.print("Received bytes:"); Serial.println(available);
		size_t count = btSerial.readBytes(&inBuff[inIndex], available);
		inIndex += count;

		for (int i = 0; i < inIndex; i++) {
			if (inBuff[i] == '\r') {
				Serial.print("CR");
			}
			else if (inBuff[i] == '\n') {
				Serial.print("LN");
			}
			else {
				Serial.print(inBuff[i]);
			}


			// Make assumption that \n\r comming in so look for \r for end
			if (i > 1) {
				if (inBuff[i - 1] == '\r' && inBuff[i] == '\n') {
					msgSize = i - 1;
					memset(msgCmpBuff, 0, MSG_COMP_BUFF);
					memcpy(msgCmpBuff, inBuff, msgSize);
					memmove(inBuff, &inBuff[i + 1], (inIndex + count) - (msgSize + 2));
					inIndex -= msgSize + 2;
					memset(&inBuff[inIndex], 0, (IN_BUFF_SIZE - inIndex));
					CompareForResponse(msgSize);
				}
			}
		}
	}
}


/// <summary>Compare the incoming message to carry out IO actions</summary>
/// <param name="msgSize">Size of the incoming message</param>
void CompareForResponse(int msgSize) {

	Serial.println("Doing comparison");
	Serial.print(msgCmpBuff);

	// Compare from start of buffer. Garbage at end of Command
	// and before terminator is ignored (OpenDoorlsdlfkdjdflj)
	if (strncmp(msgCmpBuff, OPEN_DOOR_CMD, OPEN_CMD_LEN) == 0) {
		Blink();
		btSerial.write("OPENING\r\n");
		btSerial.flush();
		Serial.write("OPENING\r\n");
		OpenGarageDoor();
	}
	else if (strncmp(msgCmpBuff, CLOSE_DOOR_CMD, CLOSE_CMD_LEN) == 0) {
		Blink();
		btSerial.write("CLOSING\r\n");
		btSerial.flush();
		Serial.write("CLOSING\r\n");
		CloseGarageDoor();
	}
	else {
		// Reverse the incoming buffer. This will ignore garbage 
		// at start of legit command (sdfsdfsOpenDoor)
		strrev(msgCmpBuff);
		if (strncmp(msgCmpBuff, OPEN_DOOR_CMD_REV, OPEN_CMD_LEN) == 0) {
			btSerial.write("OPENING\r\n");
			btSerial.flush();
			Serial.write("OPENING\r\n");
			OpenGarageDoor();
		}
		else if (strncmp(msgCmpBuff, CLOSE_DOOR_CMD_REV, CLOSE_CMD_LEN) == 0) {
			Blink();
			btSerial.write("CLOSING\r\n");
			btSerial.flush();
			Serial.write("CLOSING\r\n");
			CloseGarageDoor();
		}
		else {
			Blink();
			btSerial.write("NOT_HANDLED\r\n");
			btSerial.flush();
			Serial.write("NOT_HANDLED\r\n");
		}
	}
}


void OpenGarageDoor() {
	// Do you IO stuff here to open the door
}


void CloseGarageDoor() {
	// Do you IO stuff here to close the door
}

// So user can tell device is sending back stuff
void Blink() {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(1);
	digitalWrite(LED_BUILTIN, LOW);
}





// Private helpers
void SetupCommunications(long dbgBaud, long btBaud) {
	Serial.begin(dbgBaud);

	while (!Serial) {
		; // wait for serial port to connect. Needed for Native USB only
	}
	Serial.println("Debug serial active");
	// example had pin 9 set low, then high but does not seem necessary

	btSerial.begin(btBaud);
	while (!btSerial) {
	}
	Serial.println("Bluetooth DATA Mode up and running");
}


//void ListenToBTData() {
//	if (btSerial.available() > 0) {
//		if (inIndex >= IN_BUFF_SIZE) {
//			inIndex = 0;
//			Serial.println("Corrupt BT input. Purge buffer");
//		}
//
//		char c = (char)btSerial.read();
//		btSerial.write(c);
//
//		buff[inIndex] = c;
//		if (buff[inIndex] == '\r') {
//			Serial.write("CR");
//		}
//		else if (buff[inIndex] == '\n') {
//			Serial.write("LN");
//		}
//		else {
//			Serial.write(buff[inIndex]);
//		}
//
//		// Doing \n\r
//		if (buff[inIndex] == '\r') {
//			Serial.println("");
//			Serial.println("Printing msg in buffer");
//			hasInput = true;
//			Serial.write(buff, inIndex + 1);
//			btSerial.write(buff, inIndex + 1);
//			memset(buff, 0, IN_BUFF_SIZE);
//			inIndex = 0;
//		}
//		else {
//			inIndex++;
//			// Wipe out buffer if too long
//			if (inIndex >= IN_BUFF_SIZE) {
//				inIndex = 0;
//				Serial.println("Corrupt BT input. Purge buffer");
//			}
//			else {
//
//
//			}
//
//		}
//	}
//	else {
//		if (hasInput) {
//			hasInput = false;
//			//Serial.println("");
//			// Bounce a message back to the BT terminal
//			//btSerial.write("Finished processing input\n");
//		}
//
//		if (i % 500 == 0) {
//			Serial.print("No BT cmds# ");
//			Serial.print((i / 10));
//			Serial.println("");
//		}
//		i++;
//		delay(10);
//	}
//
//		/*
//
//		hasInput = true;
//		// Just bounce it to the serial output for debugging 
//		int in = btSerial.read();
//
//		// bounce back to both
//		btSerial.write(in);
//		Serial.write(in);
//
//		//// debug incoming
//		//if (in == '\r') {
//		//	Serial.write("\\r");
//		//}
//		//else if (in == '\n') {
//		//	Serial.write("\\n");
//		//}
//		//else {
//		//	Serial.write(in);
//		//}
//
//
//
//		//Serial.write(btSerial.read());
//
//		// In the real world we would loop through all the characters and
//		// assemble the message. Then act on message
//
//		// Terminator is sequence of \r\n for data if sent from app connecting via Bluetooth
//		//char val = (char)btSerial.read();
//		//if (val == '\r') {
//		//	Serial.write("\\r");
//		//}
//		//else if (val == '\n') {
//		//	Serial.write("\\n");
//		//}
//		//Serial.write(val);
//	}
//	else {
//		if (hasInput) {
//			hasInput = false;
//			//Serial.println("");
//			// Bounce a message back to the BT terminal
//			//btSerial.write("Finished processing input\n");
//		}
//		if (i % 50 == 0) {
//			Serial.print("No BT msg # ");
//			Serial.print((i / 10));
//			Serial.println("");
//		}
//		i++;
//		delay(100);
//	}
//		*/
//}
