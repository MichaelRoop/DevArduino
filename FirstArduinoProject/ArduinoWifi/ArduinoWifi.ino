/*
 Name:		ArduinoWifi.ino
 Created:	10/16/2020 1:22:40 PM
 Author:	Michael
*/
#include <SPI.h>
// the setup function runs once when you press reset or power the board
#include <WiFiNINA.h>
#include "wifi_defines.h"

char ssid[] = MY_SSID;
char pwd[] = MY_PASS;
int keyIndex = 0;
int status = WL_IDLE_STATUS;
int led = LED_BUILTIN;
WiFiServer server(80); // Port 80 http

void setup() {
	Serial.begin(57600);
	while(!Serial){}
	Serial.println("Serial started");

	pinMode(led, OUTPUT);

	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE) {
		Serial.println("Communication with WiFi module failed!");
		// don't continue
		while (true);
	}

	String fv = WiFi.firmwareVersion();
	Serial.print("WIFI firmware version ");
	Serial.println(fv);

	
	if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
		Serial.print("Version below "); 
		Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
		Serial.println("Please upgrade the firmware");
	}

	/*
	// attempt to connect to Wifi network:
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		status = WiFi.begin(ssid, pwd);

		// wait 10 seconds for connection:
		delay(10000);
	}
	*/


	// by default the local IP address of will be 192.168.4.1
	 // you can override it with the following:
	 // WiFi.config(IPAddress(10, 0, 0, 1));

	 // print the network name (SSID);
	Serial.print("Creating access point named: ");
	Serial.println(ssid);

	// Create access point
	status = WiFi.beginAP(ssid, pwd);
	if (status != WL_AP_LISTENING) {
		Serial.print("Status "); Serial.println(status);
		Serial.println("Access point creation failed");
		while (true) { }
	}

	delay(10000);


	server.begin();
	// you're connected now, so print out the status:
	printWifiStatus();

}


// the loop function runs over and over again until power down or reset
void loop() {

	if (status != WiFi.status()) {
		status = WiFi.status();
		if (status == WL_AP_CONNECTED) {
			Serial.println("Device connected to AP");
		}
		else {
			// a device has disconnected from the AP, and we are back in listening mode
			Serial.println("Device disconnected from AP");
		}
	}
	ListenForClient();
}


void ListenForClient() {
	//https://www.arduino.cc/en/Reference/WiFiNINABeginAP
	WiFiClient client = server.available();
	if (client) {                     
		Serial.println("Got a client connected new client"); 
		String currentLine = ""; 

		// loop while the client's connected
		while (client.connected()) {
			if (client.available()) {
				// Read a byte then print it out to serial monitor
				char c = client.read();
				Serial.write(c);
				// This will bounce it back
			//	client.print(c);


				//// This example is for HTTP. I Just want to get bytes
				//if (c == '\n') {
				//	// if the current line is blank, you got two newline characters in a row.
				//	// that's the end of the client HTTP request, so send a response:
				//	if (currentLine.length() == 0) {
				//		// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
				//		// and a content-type so the client knows what's coming, then a blank line:
				//		client.println("HTTP/1.1 200 OK");
				//		client.println("Content-type:text/html");
				//		client.println();
				//		// the content of the HTTP response follows the header:
				//		client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
				//		client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");
				//		// The HTTP response ends with another blank line:
				//		client.println();
				//		// break out of the while loop:
				//		break;
				//	}
				//	else {      // if you got a newline, then clear currentLine:
				//		currentLine = "";
				//	}
				//}
				//else if (c != '\r') {    // if you got anything else but a carriage return character,
				//	currentLine += c;      // add it to the end of the currentLine
				//}
				//// Check to see if the client request was "GET /H" or "GET /L":
				//if (currentLine.endsWith("GET /H")) {
				//	digitalWrite(led, HIGH);               // GET /H turns the LED on
				//}
				//if (currentLine.endsWith("GET /L")) {
				//	digitalWrite(led, LOW);                // GET /L turns the LED off
				//}
			}
		} // While connected - socket?
		// close the connection:
		client.stop();
		Serial.println("client disconnected");
	}
}





void printWifiStatus() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

