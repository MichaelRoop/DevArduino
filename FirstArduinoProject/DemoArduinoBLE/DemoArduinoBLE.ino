/*
  Name:    DemoArduinoBLE.ino
  Created: Jan 15, 2021

  Sets up the BLE to have characteristics that change value in real time.
	Battery Level	// 0x180F Defined in BLE Spec
	Temperature		// 0x2A6E
	Humidity		// 0x2A6F
	Pressure		// 0x2A6D
    Custom          // My Uuid 0xF001 - has format descriptor for parsing

	a list of Format data types are found in user manual

  For this to work you need the forked version of ArduinoBLE
  which supports connection to Windows and Android
  https://github.com/unknownconstant/ArduinoBLE


  Tested on Arduino Uno WIFI Rev 2

  Arduino code partialy based on:
  https://programmaticponderings.com/2020/08/04/getting-started-with-bluetooth-low-energy-ble-and-generic-attribute-profile-gatt-specification-for-iot/

  With some modifications. 
  Values are generated internally for demo instead or reading I/O
  
  NOTE: The Notify and Indicate are never set in the BLEArduino library. I have made changes to 
        it and submitted for review

 */
#include <ArduinoBLE.h>
#include "C:/Program Files (x86)/Arduino/hardware/tools/avr/avr/include/string.h"

#ifndef SECTION_MEMBERS

const int GENERIC_ACCESS_APPEARANCE_ID = 1792; // BLE Spec 0x1792

// Create battery service
BLEService batteryService("180F");
BLETypedCharacteristic<uint8_t> batteryLevelCharacteristic("2A19", BLERead | BLENotify);

// Environment sensing service
BLEService environmentService("181A");
BLETypedCharacteristic<uint32_t> pressureCharacteristic("2A6D", BLERead | BLENotify);
BLETypedCharacteristic<int16_t> tempCharacteristic("2A6E", BLERead | BLENotify);
BLETypedCharacteristic<uint16_t> humidCharacteristic("2A6F", BLERead | BLENotify);

// Custom 2 byte sensor service
BLEService ioService("FF01");
BLETypedCharacteristic<uint16_t> analogSensorCharacteristic("F001", BLERead | BLENotify | BLEWrite);
BLEDescriptor sensorDescriptorUserDescription("2901", "An Sensor 1"); // Defined in Spec

uint8_t format[7] = { 
	0x6,		// Data type. BLE spec 6 = uint16_t. Defines size of data in characteristic read 
	(byte)-2,	// Exponent -2 sbyte. stored as byte. parsed out as sbyte to maintain sign
	0x0, 0x0 ,	// Measurement uint16_t. 0 is unit-less. Adds abbreviation on display string
	0x0,		// Namespace byte. 0 is undefined, 1 is BLE Sig (only one defined in spec)
	0x0, 0x0	// Description uint
};
// BLE spec 0x2904 is the Format Descriptor
BLEDescriptor sensorDescriptorFormat("2904", format, sizeof(format));

// Custom bool onOff sensor
BLEDescriptor onOffDescriptorUserDescription("2901", "On Off Switch"); // Defined in Spec
BLEBooleanCharacteristic onOffCharacteristic("F002", BLERead | BLENotify | BLEWrite);
uint8_t onOffData[]{ 0x01,0x0, 0x0,0x0, 0x0,0x0, 0x0 };
BLEDescriptor onOffDataDesc("2904", onOffData, sizeof(onOffData));

// Track dummy values 
uint8_t batteryLevel = 93;
int16_t tempLevel = 3392;
uint16_t humidityLevl = 4400;
uint32_t pressureLevel = 28500;
uint16_t analogLevel = 16025;
byte isOn = 1; // On by default

unsigned long lasMsTimeUpdate = 0;
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
			WriteBatteryLevelOnMsInterval(2000);
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
	// Device
	BLE.setLocalName("Mikie BLE"); // visible in search
	BLE.setDeviceName("MR BLE Device");
	BLE.setAppearance(GENERIC_ACCESS_APPEARANCE_ID);
	BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
	BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);
	
	// Battery service
	batteryService.addCharacteristic(batteryLevelCharacteristic);
	BLE.addService(batteryService);
	BLE.setAdvertisedService(batteryService);

	// Environment sensing service with 3 characteristics
	environmentService.addCharacteristic(tempCharacteristic);
	environmentService.addCharacteristic(humidCharacteristic);
	environmentService.addCharacteristic(pressureCharacteristic);
	BLE.addService(environmentService);
	BLE.setAdvertisedService(environmentService);

	// My custom sensor service
	onOffCharacteristic.addDescriptor(onOffDataDesc);
	onOffCharacteristic.addDescriptor(onOffDescriptorUserDescription);
	onOffCharacteristic.setEventHandler(BLEWritten, onOffHandler);
	ioService.addCharacteristic(onOffCharacteristic);

	analogSensorCharacteristic.addDescriptor(sensorDescriptorUserDescription);
	analogSensorCharacteristic.addDescriptor(sensorDescriptorFormat);
	analogSensorCharacteristic.setEventHandler(BLEWritten, uint16CustomWriteHandler)
		;	ioService.addCharacteristic(analogSensorCharacteristic);

	BLE.addService(ioService);
	BLE.setAdvertisedService(ioService);

	BLE.advertise();

	//Serial.print("Temperature Properties: "); Serial.println(tempCharacteristic.properties());
	//Serial.print("    Presure Properties: "); Serial.println(pressureCharacteristic.properties());

	WriteValues();
	Serial.println("BLE started");
}

#endif // !SECTION_BLE_INIT


void WriteBatteryLevelOnMsInterval(unsigned long msInterval) {
	// Simulate reading of actual hardware sensors for demo
	unsigned long currentMs = millis();
	if ((currentMs - lasMsTimeUpdate) > msInterval) {
		lasMsTimeUpdate = currentMs;
		if (isOn) {
			if (upIncrement) {
				tempLevel += 111;
				humidityLevl += 212;
				pressureLevel += 18;
				analogLevel += 67;
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
				analogLevel -= 67;
				batteryLevel--;
				if (batteryLevel < 85) {
					upIncrement = true;
					batteryLevel = 60;
				}
			}
			WriteValues();
		}
	}
}


void ResetValues() {
	batteryLevel = 93;
	tempLevel = 3392;
	humidityLevl = 4400;
	pressureLevel = 285;
	analogLevel = 16025;
}


void WriteValues() {
	batteryLevelCharacteristic.writeValue(batteryLevel);
	tempCharacteristic.writeValue(tempLevel);
	humidCharacteristic.writeValue(humidityLevl);
	pressureCharacteristic.writeValue(pressureLevel);
	analogSensorCharacteristic.writeValue(analogLevel);

	Serial.print("    Battery:"); Serial.println(batteryLevel);
	Serial.print("Temperature:"); Serial.println(tempLevel);
	Serial.print("   Pressure:"); Serial.println(pressureLevel);
	Serial.print("   Humidity:"); Serial.println(humidityLevl);
	Serial.print("  An sensor:"); Serial.println(analogLevel);
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
	ResetValues();
}

void onOffHandler(BLEDevice central, BLECharacteristic characteristic) {
	isOn = characteristic.value()[0];
	Serial.print("OnOff:"); Serial.println(isOn);
}

void uint16CustomWriteHandler(BLEDevice central, BLECharacteristic characteristic) {
	memcpy(&analogLevel, characteristic.value(), sizeof(analogLevel));
	Serial.print("analogLevel:"); Serial.println(analogLevel);
}



#endif // !SECTION_EVENT_HANDLERS


