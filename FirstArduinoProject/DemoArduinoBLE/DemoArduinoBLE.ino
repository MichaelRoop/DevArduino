/*
  Name:    DemoArduinoBLE.ino
  Created: Jan 15, 2021

  Sets up the BLE to have characteristics that change values 
  in real time.
	Battery Level
	Temperature
	Humidity
	Pressure
  For this to work you need the forked version of ArduinoBLE
  which supports connection to Windows and Android
  https://github.com/unknownconstant/ArduinoBLE/tree/pairing


  Tested on Arduino Uno WIFI Rev 2

  Arduino code based on:
  https://programmaticponderings.com/2020/08/04/getting-started-with-bluetooth-low-energy-ble-and-generic-attribute-profile-gatt-specification-for-iot/

  With some modifications. Values are generated internally as a demo
  
 */
#include <ArduinoBLE.h>

#define SERIAL_DEBUG

#ifndef SECTION_MEMBERS

const char* SERVICE_BATTERY_ID = "180F"; // 0x180F
const char* CHAR_BATTERY_LEVEL_ID = "2A19";
const int GENERIC_ACCESS_APPEARANCE_ID = 1792; // BLE Spec 0x1792

// Create battery service
BLEService batteryService(SERVICE_BATTERY_ID);
BLETypedCharacteristic<uint8_t> batteryLevelChar(CHAR_BATTERY_LEVEL_ID, BLERead | BLENotify);

BLEService environmentService("181A");
BLETypedCharacteristic<uint32_t> pressureCharacteristic("2A6D", BLERead | BLENotify);
BLETypedCharacteristic<int16_t> tempCharacteristic("2A6E", BLERead | BLENotify);
BLETypedCharacteristic<uint16_t> humidCharacteristic("2A6F", BLERead | BLENotify);

// Track between 0-100
uint8_t batteryLevel = 93;
int16_t tempLevel = 3392;
uint16_t humidityLevl = 4400;
uint32_t pressureLevel = 285;

unsigned long lasMsTimeBattery = 0;
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
			WriteBatteryLevelOnMsInterval(5000);
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

	environmentService.addCharacteristic(tempCharacteristic);
	environmentService.addCharacteristic(humidCharacteristic);
	environmentService.addCharacteristic(pressureCharacteristic);
	BLE.addService(environmentService);
	BLE.setAdvertisedService(environmentService);

	BLE.advertise();

	batteryLevelChar.writeValue(batteryLevel);
	tempCharacteristic.writeValue(tempLevel);
	pressureCharacteristic.writeValue(pressureLevel);
	humidCharacteristic.writeValue(humidityLevl);
	Serial.println("BLE started");

}

#endif // !SECTION_BLE_INIT


void WriteBatteryLevelOnMsInterval(unsigned long msInterval) {
	unsigned long currentMs = millis();
	if ((currentMs - lasMsTimeBattery) > msInterval) {
		lasMsTimeBattery = currentMs;
		if (upIncrement) {
			tempLevel += 111;
			humidityLevl += 212;
			pressureLevel += 18;
			batteryLevel++;
			if (batteryLevel > 99) {
				batteryLevel = 99;
				upIncrement = false;
				batteryLevel--;
			}
		}
		else {
			tempLevel -= 111;
			humidityLevl -= 212;
			pressureLevel -= 18;
			batteryLevel--;
			if (batteryLevel < 60) {
				upIncrement = true;
				batteryLevel = 60;
			}
		}
		Serial.print("Level:"); Serial.println(batteryLevel);
		if (batteryLevelChar.writeValue(batteryLevel) == 0) {
			Serial.println("Write fail");
		}

		tempCharacteristic.writeValue(tempLevel);
		humidCharacteristic.writeValue(humidityLevl);
		pressureCharacteristic.writeValue(pressureLevel);
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