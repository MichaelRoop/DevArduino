/*
 Name:		ArduinoUSB2Port.ino
 Created:	12/18/2020 10:52:45 AM
 Author:	Michael

    Tested on an Arduino DUE using the second SerialUSB port

   The connecting device must be set to:
   Baud 115,200, 8 Data bits, 1 Stop bit, no parity

*/
#include<HardwareSerial.h>
#include <string.h>

#define IN_BUFF_SIZE 50
#define MSG_COMP_BUFF 50
char inBuff[IN_BUFF_SIZE];
char msgCmpBuff[IN_BUFF_SIZE];
int msgSize = 0;
unsigned char inIndex = 0;

char OPEN_DOOR_CMD[] = "OpenDoor";
char CLOSE_DOOR_CMD[] = "CloseDoor";
int  OPEN_CMD_LEN = 8;
int CLOSE_CMD_LEN = 9;


// the setup function runs once when you press reset or power the board
void setup() {
	//Serial.begin(115200, SERIAL_8N1);
	//Serial.setTimeout(50);
	//while (!Serial) {}
	//Serial.setTimeout(50);
	//Serial.println("Program Serial 0 open");

	//// 115,200 baud, 8 bits, 1 stop bit, no parity
	SerialUSB.begin(115200, SERIAL_8N1);
	SerialUSB.setTimeout(50);

	// The bool is overriden. does not return true until someone connects
	//https://forum.arduino.cc/index.php?topic=157252.0
	//while (!SerialUSB) {}
}

// the loop function runs over and over again until power down or reset
void loop() {
	ListenForData();
}


void ListenForData() {
	int available = SerialUSB.available();
	if (available > 0) {
		msgSize = 0;

		Serial.print("Got "); Serial.print(available); Serial.println(" byte data");

		// Error check to avoid overrun of buffer
		if ((inIndex + available) > IN_BUFF_SIZE) {
			Blink();
			//SerialUSB.write("ERROR-PURGING INPUT\n\r");
			SendMsg("ERROR-PURGING INPUT\n\r");
			inIndex = 0;
			return;
		}

		size_t count = SerialUSB.readBytes(&inBuff[inIndex], available);
		inIndex += count;

		for (int i = 0; i < inIndex; i++) {
			Serial.print(inBuff[i]);
			// Make assumption that \n\r comming in so look for \r for end
			if (i > 1) {


				if (inBuff[i - 1] == '\n' && inBuff[i] == '\r') {
					msgSize = i - 1;
					memset(msgCmpBuff, 0, MSG_COMP_BUFF);
					memcpy(msgCmpBuff, inBuff, msgSize);
					//Serial.println(msgCmpBuff);

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

	//Serial.println("---------------------------------------------------");
	//Serial.println(msgCmpBuff);
	//Serial.println(OPEN_DOOR_CMD);
	//Serial.println(CLOSE_DOOR_CMD);
	//Serial.println("---------------------------------------------------");


	// Compare from start of buffer. Garbage at end of Command
	// and before terminator is ignored (OpenDoorlsdlfkdjdflj)
	if (strncmp(msgCmpBuff, OPEN_DOOR_CMD, OPEN_CMD_LEN) == 0) {
		Blink();
		SendMsg("OPENING\n\r");
		//SerialUSB.write("OPENING\n\r");
		//SerialUSB.flush();
		//Serial.println("Sending back OPENING");
		OpenGarageDoor();
	}
	else if (strncmp(msgCmpBuff, CLOSE_DOOR_CMD, CLOSE_CMD_LEN) == 0) {
		Blink();
		SendMsg("CLOSING\n\r");
		//SerialUSB.write("CLOSING\n\r");
		//SerialUSB.flush();
		//Serial.println("Sending back CLOSING");
		CloseGarageDoor();
	}
	else {
		Blink();
		SendMsg("NOT_HANDLED\n\r");
		//SerialUSB.write("NOT_HANDLED\n\r");
		//SerialUSB.flush();
		//Serial.println("Sending back NOT_HANDLED");


		//// TODO strrev not always found
		//// Reverse the incoming buffer. This will ignore garbage 
		//// at start of legit command (sdfsdfsOpenDoor)
		//strrev(msgCmpBuff);
		//if (strncmp(msgCmpBuff, OPEN_DOOR_CMD_REV, OPEN_CMD_LEN) == 0) {
		//	SerialUSB.write("OPENING\n\r");
		//	SerialUSB.flush();
		//	OpenGarageDoor();
		//}
		//else if (strncmp(msgCmpBuff, CLOSE_DOOR_CMD_REV, CLOSE_CMD_LEN) == 0) {
		//	Blink();
		//	SerialUSB.write("CLOSING\n\r");
		//	SerialUSB.flush();
		//	CloseGarageDoor();
		//}
		//else {
		//	Blink();
		//	SerialUSB.write("NOT_HANDLED\n\r");
		//	SerialUSB.flush();
		//}
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


void SendMsg(char* msg) {
	SerialUSB.write(msg);
	SerialUSB.flush();

	// For debug through programing port
	if (Serial) { Serial.write(msg); }
}


void SendDbgLn(char* msg) {
	if (Serial) { Serial.println(msg); }
}





