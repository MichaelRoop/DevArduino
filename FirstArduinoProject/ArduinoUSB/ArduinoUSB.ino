/*
   Name:		ArduinoUSB.ino
   Created:	11/1/2020 2:37:44 PM
   Author:	Michael

   Tested on an Arduino Uno using the default serial port
   You can apply the same code to boards with multiple serial
   ports by specifying the exact serial port

   The terminal must be set to 
   Baud 115,200, 8 Data bits, 1 Stop bit, no parity

*/
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


// the setup function runs once when you press reset or power the board
void setup() {
	// 115,200 Baud, 8 Data bits, no parity, 1 stop bit
	// You must also setup the terminal with same parameters
	Serial.begin(115200, SERIAL_8N1);
	while (!Serial) {
	}
}



// the loop function runs over and over again until power down or reset
void loop() {
	ListenForData();
}


void ListenForData() {
	int available = Serial.available();
	if (available > 0) {
		msgSize = 0;

		// Error check to avoid overrun of buffer
		if ((inIndex + available) > IN_BUFF_SIZE) {
			Blink();
			Serial.write("ERROR-PURGING INPUT\n\r");
			inIndex = 0;
			return;
		}

		size_t count = Serial.readBytes(&inBuff[inIndex], available);
		inIndex += count;

		for (int i = 0; i < inIndex; i++) {
			// Make assumption that \n\r comming in so look for \r for end
			if (i > 1) {
				if (inBuff[i-1] == '\n' && inBuff[i] == '\r') {
					msgSize = i - 1;
					memset(msgCmpBuff, 0, MSG_COMP_BUFF);
					memcpy(msgCmpBuff, inBuff, msgSize);
					memmove(inBuff , &inBuff[i + 1] , (inIndex + count) - (msgSize + 2));
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

	// Compare from start of buffer. Garbage at end of Command
	// and before terminator is ignored (OpenDoorlsdlfkdjdflj)
	if (strncmp(msgCmpBuff, OPEN_DOOR_CMD, OPEN_CMD_LEN) == 0) {
		Blink();
		Serial.write("OPENING\n\r");
		Serial.flush();
		OpenGarageDoor();
	}
	else if (strncmp(msgCmpBuff, CLOSE_DOOR_CMD, CLOSE_CMD_LEN) == 0) {
		Blink();
		Serial.write("CLOSING\n\r");
		Serial.flush();
		CloseGarageDoor();
	}
	else {
		Blink();
		Serial.write("NOT_HANDLED\n\r");
		Serial.flush();
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


