/*
 Name:		ArduinoBLE.ino
 Created:	4/27/2020 1:44:30 PM
 Author:	Michael
*/

#include <WiFiNINA.h>
#include <ArduinoBLE.h>

#ifndef SECTION_DATA

// 0x180F SIG Battery service
BLEService batteryService("180F");
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.battery_level.xml
//https://rootsaid.com/arduino-ble-example/

// 0x2A19 SIG 16 bit value for battery level characteristic 
BLEUnsignedCharCharacteristic batteryLevelCharacteristic("2A19", BLERead | BLENotify);

BLEService genAttributeService1("1801");
BLEUnsignedCharCharacteristic serviceChangedCharacteristic("2A05", BLERead);

int oldBatteryLevel = 0;
long previousMsCount = 0;
bool connectErrorPosted = false;

#endif

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);

    // TODO - This needs to be removed to work if not connected to USB serial port
    while (!Serial) {
        ; // wait for serial port to connect. Needed for Native USB only
    }
    Serial.println("Debug serial port active");

    pinMode(LED_BUILTIN, OUTPUT);
    if (!BLE.begin()) {
        Serial.println("Failed to start BLE");
        while (1);
    }
    else {
        Serial.println("BLE begun");
    }

    // This is the name that the 'Add Device' sees. 
    // Visible to users to identify the device
    BLE.setLocalName("Mikies test Arduino");

    // assign event handlers for connected, disconnected to peripheral
    BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
    BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);
    
    // This does the Device Name Characteristic
    BLE.setDeviceName("Mikies UNO Wifi Rev2");

    //byte manu[7] = {9,21,22,45};
    //BLE.setManufacturerData(manu, 4);

    // Add the battery service and attach the batteryLevel Characteristic
    BLE.setAdvertisedService(batteryService);
    //BLE.setAdvertisedServiceUuid("2A19"); // Already done on init
    batteryService.addCharacteristic(batteryLevelCharacteristic);
    BLE.addService(batteryService);

    // This just made a duplicate. Original still failed
    // Service changed characteristic is already in by default. Set it so it can be read
    BLE.setAdvertisedService(genAttributeService1);
    genAttributeService1.addCharacteristic(serviceChangedCharacteristic);
    BLE.addService(genAttributeService1);

    //BLE.adv

    // Start advertising myself
    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");
}


// the loop function runs over and over again until power down or reset
void loop() {
    // Central devices are those reading us
    // We are setup as a peripheral device which provides the services to read
    BLEDevice central = BLE.central();
    if (central) {
        Serial.print("Connected to central: ");
        if (central.hasLocalName()) {
            Serial.print(central.localName());
            Serial.print(" : ");
        }
        Serial.print(central.deviceName());
        Serial.print(" : ");
        Serial.println(central.address());
        digitalWrite(LED_BUILTIN, HIGH);

        //int tick = 0;
        while (central.connected()) {
            long ms = millis();
            if (ms - previousMsCount >= 1000) {
                previousMsCount = ms;
                UpdateBatteryLevel();
            }
        }
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Disconnected from central");
    }
    else {
        if (!connectErrorPosted) {
            connectErrorPosted = true;
            //Serial.println("BLE.central failed");
        }
    }
}


// Simulate reading battery level
void UpdateBatteryLevel() {
    // Read current voltage level on A0 analog input pin to simulate battery charge level
    int battery = analogRead(A0);
    int batteryLevel = map(battery, 0, 1023, 0, 100);

    // If level changes update the battery level characteristic
    if (batteryLevel != oldBatteryLevel) { 
        Serial.print("Battery Level % is now: ");
        Serial.println(batteryLevel);
        batteryLevelCharacteristic.writeValue(batteryLevel);
        oldBatteryLevel = batteryLevel;
    }
}


void bleOnConnectHandler(BLEDevice central) {
    // central connected event handler
    Serial.print("Handling someone connected event, central: ");
    Serial.println(central.address());
}


void bleOnDisconnectHandler(BLEDevice central) {
    // central disconnected event handler
    Serial.print("Handling someone disconnected event, central: ");
    Serial.println(central.address());
}


