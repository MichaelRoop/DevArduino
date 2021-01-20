/*
  Name:    DemoArduinoBLE.ino
  Created: Jan 15, 2021

  Sets up the BLE to have characteristics that change values 
  in real time.
  First is a Battery Level (bogus)

  For this to work you need the forked version of ArduinoBLE
  which supports connection to Windows and Android

  Tested on Arduino Uno WIFI Rev 2
  
 */
#include <ArduinoBLE.h>

#define SERIAL_DEBUG

#ifndef SECTION_MEMBERS

const char* SERVICE_BATTERY_ID = "180F"; // 0x180F
const char* CHAR_BATTERY_LEVEL_ID = "2A19";
const int GENERIC_ACCESS_APPEARANCE_ID = 1792; // BLE Spec 0x1792

// Create battery service
BLEService batteryService(SERVICE_BATTERY_ID);
BLEUnsignedCharCharacteristic batteryLevelChar(CHAR_BATTERY_LEVEL_ID, BLERead | BLENotify);

// Track between 0-64 Hex 0-100%
unsigned char batteryLevel = 30;

unsigned long lasMsTime = 0;
bool upIncrement = true;

#endif // !SECTION_MEMBERS


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	while (!Serial) {}
	Serial.println("started...");
	SetupBLE();
}

// the loop function runs over and over again forever
void loop() {
	BLEDevice central = BLE.central();
	if (central) {
		while (central.connected()) {
			WriteBatteryLevelOnMsInterval(1000);
			delayMicroseconds(10);
		}
	}
}




#ifndef SECTION_BLE_INIT

void SetupBLE() {
	if (!BLE.begin()) {
		Serial.println("FAIL");
		while (1);
	}
	
	BLE.setLocalName("Mikie BLE"); // visible in search
	BLE.setDeviceName("MR BLE Device");
	BLE.setAppearance(GENERIC_ACCESS_APPEARANCE_ID);
	BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
	BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);

	batteryService.addCharacteristic(batteryLevelChar);
	BLE.addService(batteryService);
	BLE.setAdvertisedService(batteryService);
	BLE.advertise();

	batteryLevelChar.writeValue(batteryLevel);
	Serial.println("BLE started");

}

#endif // !SECTION_BLE_INIT


void WriteBatteryLevelOnMsInterval(unsigned long msInterval) {
	unsigned long currentMs = millis();
	if ((currentMs - lasMsTime) > msInterval) {
		lasMsTime = currentMs;
		if (upIncrement) {
			batteryLevel++;
			if (batteryLevel > 63) {
				batteryLevel = 62;
				upIncrement = false;
				batteryLevel--;
			}
		}
		else {
			batteryLevel--;
			if (batteryLevel < 30) {
				upIncrement = true;
				batteryLevel = 31;
			}
		}
		Serial.print("Level:"); Serial.println(batteryLevel);
		if (batteryLevelChar.writeValue(batteryLevel) == 0) {
			Serial.println("Write fail");
		}
	}
}


#ifndef SECTION_EVENT_HANDLERS

/// <summary>When a connection is established to device</summary>
/// <param name="central">The device connecting</param>
void bleOnConnectHandler(BLEDevice central) {
	Serial.println("CONNECTED");
}


/// <summary>When a connection to device is broken</summary>
/// <param name="central">The device disconnecting</param>
void bleOnDisconnectHandler(BLEDevice central) {
	Serial.println("DISCONNECTED");
	// any other reset stuff
}



#endif // !SECTION_EVENT_HANDLERS


#ifdef SERIAL_DEBUG

void SetupDbg(int baud) {
	Serial.begin(baud);
	while (!Serial) {}
	//DbgLine("started...");
	Serial.println("started...");
}

//void DbgLine(const char* msg) {
//	Serial.println(msg);
//}

#else
void SetupSerial(int baud) {}
//void DbgLine(char* msg) {}
#endif // SERIAL_DEBUG