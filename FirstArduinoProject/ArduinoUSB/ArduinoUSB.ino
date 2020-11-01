/*
 Name:		ArduinoUSB.ino
 Created:	11/1/2020 2:37:44 PM
 Author:	Michael
*/
#define IN_BUFF_SIZE 100

char inBuff[IN_BUFF_SIZE];

// the setup function runs once when you press reset or power the board
void setup() {
	//Serial.begin(57600, SERIAL_8N1);
	Serial.begin(115200, SERIAL_8N1);

	while (!Serial) {
	}
	delay(500);
	Serial.println("started");
	//DoBlink(10);
}



// the loop function runs over and over again until power down or reset
void loop() {
	int count = Serial.available();
	if (count > 0) {
		int c = (char)Serial.read();
		Serial.write(c);
		//DoBlink(1);
	}


}



void DoBlink(int count) {
	for (int i = 0; i < count; i++) {
		digitalWrite(LED_BUILTIN, HIGH);
		delay(250);
		digitalWrite(LED_BUILTIN, LOW);
	}
}