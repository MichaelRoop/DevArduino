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
	SetupCommunications();
	//pinMode(13, OUTPUT); 
}


// the loop function runs over and over again until power down or reset
void loop() {
	//SimpleSerialOutput();
	ListenToBT();

	// To send a message through BT
	//btSerial.write("From board\n"); 
	//Serial.println("AT+NAME?/r/n");
}


void SetupCommunications(){
	Serial.begin(9600);
	btSerial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for Native USB only
	}
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
		Serial.write(btSerial.read());
		// In the real world we would loop through all the characters and
		// assemble the message. Then act on message
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