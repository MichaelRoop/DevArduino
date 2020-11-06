/*
 Name:		ArduinoEthernet.ino
 Created:	11/6/2020 12:17:15 PM
 Author:	Michael

 Tested against Arduino Uno with Arduino Ethernet Shield
 
 Ethernet Shield attached to pins 10,11,12,13



*/
#include <IPAddress.cpp>
#include <Ethernet.h>

#ifndef SECTION_DataMembers

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
// Make sure your IP is same network as other sides
IPAddress ip(192, 168, 1, 88);
//unsigned localPort = 9999;
unsigned localPort = 80;// TODO - remove. Only test with browser

// messages
// UDP_TX_PACKET_MAX_SIZE is 24 so leave enough space to accumulate 
// two full UDP packets. Your messages should only be short
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


EthernetUDP udp;

#endif

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("Debug serial active");
    StartEthernet(mac, ip);
}


// the loop function runs over and over again until power down or reset
void loop() {
	ListenForData();
}


#ifndef SECTION_EthernetFunctions


/// <summary>Fire up the Ethernet connection</summary>
/// <param name="macAddress">The MAC address</param>
/// <param name="ipAddress">The IP address</param>
void StartEthernet(uint8_t* macAddress, IPAddress ipAddress) {
    Ethernet.begin(mac, ip);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield not found.  Hardware required");
        HangProgram();
    }
    else if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
        HangProgram();
    }
    else {
        Serial.println("Ethernet successfuly setup");
		DebugPrintUDPInfo();
    }
}


/// <summary>Used to stop program from advancing</summary>
void HangProgram() {
    Serial.println("** Critical Error - The program is suspended **");
    while (true) {
        delay(1);
    }
}


void SendUdpResponse(char* msg) {
	Serial.print("Sending out:"); Serial.println(msg);

	if (udp.beginPacket(udp.remoteIP(), udp.remotePort())) {
		udp.write(msg);
		if (!udp.endPacket()) {
			DebugMsgOnFail("Error sending");
		}
	}
	else {
		DebugMsgOnFail("Failed to Connect. Bad address or port");
	}
}

void DebugMsgOnFail(char* msg) {
	// Id the IP and Port of remote
	Serial.println(msg);
}


void PrintOutIpAddress(IPAddress ipAddress, unsigned port) {
	for (int i = 0; i < 4; i++) {
		Serial.print(ipAddress[i], DEC);
		if (i < 3) {
			Serial.print(".");
		}
	}
	Serial.print(", port ");
	Serial.println(port);
}

void DebugPrintUDPInfo() {
	Serial.println("UDP Info");
	Serial.println("  Local");
	PrintOutIpAddress(ip, localPort);
	Serial.println("  Remote");
	PrintOutIpAddress(udp.remoteIP(), udp.remotePort());
}


#endif // !SECTION_EthernetFunction


#ifndef SECTION_InputOutputMsgFunctions

void ListenForData() {
	// TODO replace with udp

	int bytesAvailable = udp.parsePacket();
	if (bytesAvailable > 0) {
		Serial.print("Received "); Serial.print(bytesAvailable); Serial.println(" bytes");
		msgSize = 0;

		// Error check to avoid overrun of buffer
		if ((inIndex + bytesAvailable) > IN_BUFF_SIZE) {
			Blink();
			Serial.println("Overrun buffer. Purging all");
			SendUdpResponse("ERROR-PURGING INPUT\r\n");
			inIndex = 0;
			return;
		}

		size_t count = udp.readBytes(&inBuff[inIndex], bytesAvailable);
		inIndex += count;

		for (int i = 0; i < inIndex; i++) {
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

	// TODO - replace with udp.Send

	// Compare from start of buffer. Garbage at end of Command
	// and before terminator is ignored (OpenDoorlsdlfkdjdflj)
	if (strncmp(msgCmpBuff, OPEN_DOOR_CMD, OPEN_CMD_LEN) == 0) {
		Blink();
		SendUdpResponse("OPENING\r\n");
		OpenGarageDoor();
	}
	else if (strncmp(msgCmpBuff, CLOSE_DOOR_CMD, CLOSE_CMD_LEN) == 0) {
		Blink();
		SendUdpResponse("CLOSING\r\n");
		CloseGarageDoor();
	}
	else {
		// Reverse the incoming buffer. This will ignore garbage 
		// at start of legit command (sdfsdfsOpenDoor)
		strrev(msgCmpBuff);
		if (strncmp(msgCmpBuff, OPEN_DOOR_CMD_REV, OPEN_CMD_LEN) == 0) {
			SendUdpResponse("OPENING\r\n");
			OpenGarageDoor();
		}
		else if (strncmp(msgCmpBuff, CLOSE_DOOR_CMD_REV, CLOSE_CMD_LEN) == 0) {
			Blink();
			SendUdpResponse("CLOSING\r\n");
			CloseGarageDoor();
		}
		else {
			Blink();
			SendUdpResponse("NOT_HANDLED\r\n");
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

#endif // !SECTION_InputOutputMsgFunctions




