/*
 Name:		FirstArduinoProject.ino
 Created:	12/10/2019 5:12:56 PM
 Author:	Michael
*/

#include <SoftwareSerial.h>

int i = 0;
// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5,4); //RX,TX
bool hasInput = false;

// the setup function runs once when you press reset or power the board
void setup() {
	SetupDataComs();
	//SetupAtComs();

	//SetupCommunications();
	//pinMode(13, OUTPUT); 
}


// the loop function runs over and over again until power down or reset
void loop() {
	//SimpleSerialOutput();
	ListenToBT();
	//SendBT_AT();
}


void SetupDataComs() {
	//SetupCommunications(115200, 9600);
	SetupCommunications(9600, 38400);
}

void SetupAtComs() {
	// Need 38400 for BT and 9600 for serial to send AT commands
	// strange because BT AT cmd shows BT baud is 9600. 
	// If I set BT serial higher via an AT command, 
	// it still only works if I set the software serial for BT at 38400
	// Although we can set the debug serial higher 9600 is adequate
	SetupCommunications(9600, 38400);
}


void SetupCommunications(long dbg, long bt){
	btSerial.begin(bt);
	Serial.begin(dbg);

	while (!Serial) {
		; // wait for serial port to connect. Needed for Native USB only
	}
	Serial.println("Debug serial active");
	// example had pin 9 set low, then high but does not seem necessary
}


void SimpleSerialOutput() {
	Serial.print("Hi from VS # ");
	Serial.print(i);
	Serial.println("");
	delay(1000);
	i++;
}


void ListenToBT(){
	if(btSerial.available() > 0) {  
		hasInput = true;
		// Just bounce it to the serial output for debugging 
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
		if (hasInput){
			hasInput = false;
			Serial.println("");
			// Bounce a message back to the BT terminal
			btSerial.write("Finished processing input\n");
		}
		if (i % 50 == 0){
			Serial.print("No BT msg # ");
			Serial.print((i/10));
			Serial.println("");
		}
		i++;
		delay(100);
	}
}

