// --------------------------------------------------------------
// Name:		TestArduinoWifi.ino
// Created:	10/16/2020 1:22:40 PM
// Author:	Michael
//
// Tested on the Arduino UNO WIFI Rev2
//
// Sets up the board as a WIFI access point with a socket
//
// Initial example code found in:
// https://www.arduino.cc/en/Reference/WiFiNINABeginAP
//
// --------------------------------------------------------------
#include <SPI.h>
#include <WiFiNINA.h>
#include "wifi_defines.h"
// --------------------------------------------------------------
// The wifi_defines.h has the strings for the SSID and password
// Here are the contents
//       #pragma once
//       
//       Must be 8 or more characters
//       #define MY_SSID "MikieArduinoWifi"
//
//       Must be 8 or more characters
//       #define MY_PASS "1234567890"
// 
//            Key: 1234567890 
// Address (HOST): 192.168.1.222
//            Port: 80
// 
// --------------------------------------------------------------

char ssid[] = MY_SSID;
char pwd[] = MY_PASS;
int keyIndex = 0;
int status = WL_IDLE_STATUS;
int led = LED_BUILTIN;

// Socket Port 80 is for HTTP - using it as demo entry point
// Set Multi Comm Terminal port number to the same as in Arduino
WiFiServer server(80);

void setup() {
	// Serial is just to push data for debugging the Arduino code. Can be removed
	Serial.begin(57600);
	while (!Serial) {}
	Serial.println("Serial started");

	pinMode(led, OUTPUT);

	// Check for the WiFi module:
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

	// by default the local IP address of will be 192.168.4.1
	// you can override it with the following:
	// Whatever you choose will be the IP that you enter in 
	// the Multi Comm Connection parameters
	 WiFi.config(IPAddress(192, 168, 1, 222));

	 // Print the network name (SSID);
	Serial.print("Creating access point named: ");
	Serial.println(ssid);

	// Create access point
	status = WiFi.beginAP(ssid, pwd);
	if (status != WL_AP_LISTENING) {
		Serial.print("Status "); Serial.println(status);
		Serial.println("Access point creation failed");
		while (true) {}
	}

	server.begin();
	// you're connected now, so print out the status to the serial debug:
	printWifiStatus();
}


// the loop function runs over and over again until power down or reset
void loop() {
	// Print a message to debug if a device has connected or disconnected
	if (status != WiFi.status()) {
		status = WiFi.status();
		if (status == WL_AP_CONNECTED) {
			Serial.println("Device connected to AP");
		}
		else {
			// Device has disconnected from AP. Back in listening mode
			Serial.println("Device disconnected from AP");
		}
	}
	ListenForClient();
}


// Determines if a client has connected a socket and sent a message
void ListenForClient() {
	//https://www.arduino.cc/en/Reference/WiFiNINABeginAP
	WiFiClient client = server.available();
	if (client) {
		Serial.println("Got a client connected new client");
		String currentLine = "";

		// Loop while the client's connected
		while (client.connected()) {
			if (client.available()) {
				// Read a byte
				char c = client.read();
				// Print character serial for debug
				Serial.write(c);

				// This will bounce each character through the socket
				// The MultiCommMonitor will pick up the terminators and
				// Display it as a return message
				//
				// In the real world, you would accumulate the bytes until
				// the expected termintor sequence is detected. 
				// You would then 
				//  - Look at the message
				// - Determine operation requested
				// - Do the operation
				// - Optionaly, send back a response with expected terminators
				//
				// See samples for BT Classic and BLE
				client.print(c);
			}
		}

		// close the connection:
		client.stop();
		// Send a debug message
		Serial.println("client disconnected");
	}
}





void printWifiStatus() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's socket IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

