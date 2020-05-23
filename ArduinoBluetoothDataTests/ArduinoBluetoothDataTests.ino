
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
//		���|��  TX
//		��|���
//		������  RX
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
int i = 0;
// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX


bool hasInput = false;
#define IN_BUFF_SIZE 100
char buff[IN_BUFF_SIZE];
unsigned char inIndex = 0;
#endif // !SECTION_DATA


// the setup function runs once when you press reset or power the board
void setup() {
	// There is some strange behaviour when using different baud rates
	SetupCommunications(9600, 38400);
}

// the loop function runs over and over again until power down or reset
void loop() {
	ListenToBTData();
	delay(10);
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
	Serial.println("BT up and running");
}


void ListenToBTData() {
	if (btSerial.available() > 0) {
		if (inIndex >= IN_BUFF_SIZE) {
			inIndex = 0;
			Serial.println("Corrupt BT input. Purge buffer");
		}

		buff[inIndex] = (char)btSerial.read();
		if (buff[inIndex] == '\r') {
			Serial.write("CR");
		}
		else if (buff[inIndex] == '\n') {
			Serial.write("LN");
		}
		else {
			Serial.write(buff[inIndex]);
		}

		// Doing \n\r
		if (buff[inIndex] == '\r') {
			Serial.println("Printing msg in buffer");
			hasInput = true;
			Serial.write(buff, inIndex + 1);
			btSerial.write(buff, inIndex + 1);
			memset(buff, 0, IN_BUFF_SIZE);
			inIndex = 0;
		}
		else {
			inIndex++;
			// Wipe out buffer if too long
			if (inIndex >= IN_BUFF_SIZE) {
				inIndex = 0;
				Serial.println("Corrupt BT input. Purge buffer");
			}
			else {


			}

		}
	}
	else {
		if (hasInput) {
			hasInput = false;
			//Serial.println("");
			// Bounce a message back to the BT terminal
			//btSerial.write("Finished processing input\n");
		}

		if (i % 500 == 0) {
			Serial.print("No BT msg # ");
			Serial.print((i / 10));
			Serial.println("");
		}
		i++;
		delay(10);
	}

		/*

		hasInput = true;
		// Just bounce it to the serial output for debugging 
		int in = btSerial.read();

		// bounce back to both
		btSerial.write(in);
		Serial.write(in);

		//// debug incoming
		//if (in == '\r') {
		//	Serial.write("\\r");
		//}
		//else if (in == '\n') {
		//	Serial.write("\\n");
		//}
		//else {
		//	Serial.write(in);
		//}



		//Serial.write(btSerial.read());

		// In the real world we would loop through all the characters and
		// assemble the message. Then act on message

		// Terminator is sequence of \r\n for data if sent from app connecting via Bluetooth
		//char val = (char)btSerial.read();
		//if (val == '\r') {
		//	Serial.write("\\r");
		//}
		//else if (val == '\n') {
		//	Serial.write("\\n");
		//}
		//Serial.write(val);
	}
	else {
		if (hasInput) {
			hasInput = false;
			//Serial.println("");
			// Bounce a message back to the BT terminal
			//btSerial.write("Finished processing input\n");
		}
		if (i % 50 == 0) {
			Serial.print("No BT msg # ");
			Serial.print((i / 10));
			Serial.println("");
		}
		i++;
		delay(100);
	}
		*/
}
