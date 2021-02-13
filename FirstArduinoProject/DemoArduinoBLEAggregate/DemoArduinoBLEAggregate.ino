/*
  Name:    DemoArduinoBLEAggregate.ino
  Created: Feb 12, 2021

  Sets up a BLE with one characteristic and an aggregate format descriptor

 */
#include <ArduinoBLE.h>
#include "C:/Program Files (x86)/Arduino/hardware/tools/avr/avr/include/string.h"
 


#ifndef SECTION_MEMBERS

const int GENERIC_ACCESS_APPEARANCE_ID = 1792; // BLE Spec 0x1792

BLEService ioService("FF01");

// Characteristic which transmits the composite data
// 1 byte var, 2 byte var, and 4 byte var
uint8_t dataBlock[]{ 0x0,0x0,0x0,0x0,0x0,0x0,0x00 };
BLECharacteristic multiDataCharacteristic ("F001", BLERead | BLENotify, sizeof(dataBlock), true);

// Format descriptor for the uint8_t
uint8_t formatUint8[7] = {
	0x4,		// BLE spec46 = uint8t. Defines size of data in characteristic read 
	0,	// Exponent display 0
	0x0, 0x0 ,	// Measurement unit-less
	0x0,		// Namespace byte. 0 undefined
	0x0, 0x0	// Description uint
};
// BLE spec 0x2904 is the Format Descriptor
BLEDescriptor descriptorFormat_Uint8("2904", formatUint8, sizeof(formatUint8));

//Descriptror format for uint 16, 2 place exponent display
uint8_t formatUint16[] = {	0x6, (byte)-2, 0x0, 0x0 , 0x0, 0x0, 0x0 };
BLEDescriptor descriptorFormat_Uint16("2904", formatUint16, sizeof(formatUint16));

//Descriptror format for uint 32, 3 place exponent display
uint8_t formatUint32[] = { 0x8, (byte)-3, 0x0, 0x0 , 0x0, 0x0, 0x0 };
BLEDescriptor descriptorFormat_Uint32("2904", formatUint32, sizeof(formatUint32));

// For aggregate format we need the uint16 handles of 3 format descriptors as data packet

// Aggregate Format Descriptor 0x2905. Data block enogh room for 3 uint16_t handles
uint8_t aggFormatHandles[] { 0x0, 0x0, 0x0, 0x0,0x0, 0x0 };


BLEDescriptor descriptorFormat_Aggregate("2905", aggFormatHandles, sizeof(aggFormatHandles));


uint8_t uint08Level = 128;
uint16_t uint16Level = 3110;
uint32_t uint32Level = 4400500;
unsigned long lasMsTimeUpdate = 0;
bool upIncrement = true;

#endif



// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	while (!Serial) {}
	Serial.println("");
	Serial.println(" *** debug started...");
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


void SetupBLE() {
	if (!BLE.begin()) {
		Serial.println("FAIL");
		while (1);
	}

	// Device
	BLE.setLocalName("MR Agg BLE"); // visible in search
	BLE.setDeviceName("MR Agg BLE Device");
	BLE.setAppearance(GENERIC_ACCESS_APPEARANCE_ID);
	BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
	BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);

	multiDataCharacteristic.addDescriptor(descriptorFormat_Aggregate);
	multiDataCharacteristic.addDescriptor(descriptorFormat_Uint8);
	multiDataCharacteristic.addDescriptor(descriptorFormat_Uint16);
	multiDataCharacteristic.addDescriptor(descriptorFormat_Uint32);

	ioService.addCharacteristic(multiDataCharacteristic);
	BLE.addService(ioService);

	// Handles are set when added to the service. They are now set
	Serial.println("5");
	Serial.println(descriptorFormat_Uint8.handle());
	Serial.println(descriptorFormat_Uint16.handle());
	Serial.println(descriptorFormat_Uint32.handle());

	Serial.print("0x"); Serial.print(aggFormatHandles[0]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[1]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[2]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[3]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[4]);
	Serial.print(",0x"); Serial.println(aggFormatHandles[5]);
	Serial.println("");

	// TODO problem. Handles only set after service added but the Characteristic needs it 
	// Copy the handles of the Format Descriptors as the Agg Format payload. ORDER IMPORTANT
	// TODO - the aggFormat was passed in as a const array so I do not think I can change it
	//memcpy(&aggFormatHandles[0], (void*)descriptorFormat_Uint8.handle(), sizeof(uint16_t));
	//memcpy(&aggFormatHandles[2], (void*)descriptorFormat_Uint16.handle(), sizeof(uint16_t));
	//memcpy(&aggFormatHandles[4], (void*)descriptorFormat_Uint32.handle(), sizeof(uint16_t));
	//// TODO - check. Think this will automatically become the data in the Agg descriptor

	uint16_t h1 = descriptorFormat_Uint8.handle();
	uint16_t h2 = descriptorFormat_Uint16.handle();
	uint16_t h3 = descriptorFormat_Uint32.handle();
	Serial.println(h1);
	Serial.println(h2);
	Serial.println(h3);
	Serial.println("");


	uint16_t* ptr = (uint16_t*)aggFormatHandles;
	memcpy(&ptr[0], &h1, sizeof(uint16_t));
	memcpy(&ptr[1], &h2, sizeof(uint16_t));
	memcpy(&ptr[2], &h3, sizeof(uint16_t));


	Serial.print("0x"); Serial.print(aggFormatHandles[0]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[1]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[2]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[3]);
	Serial.print(",0x"); Serial.print(aggFormatHandles[4]);
	Serial.print(",0x"); Serial.println(aggFormatHandles[5]);
	Serial.println("");
	Serial.print("0x"); Serial.print(descriptorFormat_Aggregate[0]);
	Serial.print(",0x"); Serial.print(descriptorFormat_Aggregate[1]);
	Serial.print(",0x"); Serial.print(descriptorFormat_Aggregate[2]);
	Serial.print(",0x"); Serial.print(descriptorFormat_Aggregate[3]);
	Serial.print(",0x"); Serial.print(descriptorFormat_Aggregate[4]);
	Serial.print(",0x"); Serial.println(descriptorFormat_Aggregate[5]);




	//descriptorFormat_Aggregate.value

	Serial.println("6");

	BLE.setAdvertisedService(ioService);
	BLE.advertise();
	//WriteValues();
	Serial.println("BLE started");
}


void WriteValues() {
	Serial.println(descriptorFormat_Uint8.handle());
	Serial.println(descriptorFormat_Uint16.handle());
	Serial.println(descriptorFormat_Uint32.handle());
	//return;

	// dest, source, size
	memcpy(&dataBlock[0], &uint08Level, 1);
	memcpy(&dataBlock[1], &uint16Level, 2);
	memcpy(&dataBlock[3], &uint32Level, 4);

	multiDataCharacteristic.writeValue(dataBlock, sizeof(dataBlock));

	Serial.print("Uint08:"); Serial.println(uint08Level);
	Serial.print("Uint:16"); Serial.println(uint16Level);
	Serial.print("Uint32:"); Serial.println(uint32Level);
}


void ResetValues() {
	uint08Level = 128;
	uint16Level = 3110;
	uint32Level = 4400500;
	long lasMsTimeUpdate = 0;
	upIncrement = true;
}


void WriteBatteryLevelOnMsInterval(unsigned long msInterval) {
	// Simulate reading of actual hardware sensors for demo
	unsigned long currentMs = millis();
	if ((currentMs - lasMsTimeUpdate) > msInterval) {
		lasMsTimeUpdate = currentMs;
		if (upIncrement) {
			uint16Level += 3113;
			uint32Level += 440057;
			uint08Level++;
			if (uint08Level > 199) {
				uint08Level = 198;
				upIncrement = false;
			}
		}
		else {
			uint16Level -= 3113;
			uint32Level -= 440057;
			uint08Level--;
			if (uint08Level < 128) {
				uint08Level = 127;
				upIncrement = true;
			}
		}
		WriteValues();
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
	ResetValues();
}

#endif // !SECTION_EVENT_HANDLERS


