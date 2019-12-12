/*
 Name:		FirstArduinoProject.ino
 Created:	12/10/2019 5:12:56 PM
 Author:	Michael
*/
int i = 0;

// the setup function runs once when you press reset or power the board
void setup() {
}


// the loop function runs over and over again until power down or reset
void loop() {
	SimpleSerialOutput();
}



void SimpleSerialOutput() {
	Serial.print("Hi from VS # ");
	Serial.print(i);
	Serial.println("");
	delay(1000);
	i++;
}

