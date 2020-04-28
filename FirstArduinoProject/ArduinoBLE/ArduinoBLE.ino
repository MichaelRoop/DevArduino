/*
 Name:		ArduinoBLE.ino
 Created:	4/27/2020 1:44:30 PM
 Author:	Michael
*/

#include <WiFiNINA.h>
#include <ArduinoBLE.h>

//https://rootsaid.com/arduino-ble-example/

// 0x2A19 - 10777 decimal
BLEService batteryService("2A19");
//BLEService batteryService("1101");
// https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.battery_level.xml

//

BLEUnsignedCharCharacteristic batteryLevelChar("2101", BLERead | BLENotify);

int i = 0;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);
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

    // assign event handlers for connected, disconnected to peripheral
    BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
    BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);
    
    // This does the Device Name Characteristic
    BLE.setDeviceName("Mikies UNO Wifi Rev2");

    //https://rootsaid.com/arduino-ble-example/
    // This is the name that the 'Add Device' sees
    BLE.setLocalName("Mikies test Arduino");

    BLE.setAdvertisedService(batteryService);
    BLE.setAdvertisedServiceUuid("2A19"); // Not sure diff from previous

    batteryService.addCharacteristic(batteryLevelChar);
    BLE.addService(batteryService);
    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");
}

bool connectErrorPosted = false;


// the loop function runs over and over again until power down or reset
void loop() {

    // Central devices are those reading us
    // We are setup as a peripheral device which provides the services to read
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        digitalWrite(LED_BUILTIN, HIGH);

        //int tick = 0;
        while (central.connected()) {

            //int battery = analogRead(A0);
            //int batteryLevel = map(battery, 0, 1023, 0, 100);
            //Serial.print("Battery Level % is now: ");
            //Serial.println(batteryLevel);
            //batteryLevelChar.writeValue(batteryLevel);
            //batteryLevelChar.writeValue(32);
            delay(200);
            //tick++;
            //if (tick > 10) {
            //    break;
            //}
        }
        digitalWrite(LED_BUILTIN, LOW);
        //Serial.print("Disconnected from central");
    }
    else {
        if (!connectErrorPosted) {
            connectErrorPosted = true;
            Serial.println("BLE.central failed");
        }
    }

    //digitalWrite(LED_BUILTIN, LOW);
    //Serial.print("Disconnected from central: ");
    //Serial.println(central.address());


    if (i % 100 == 0) {
        Serial.print("Ping ");
        Serial.print((i / 10));
        Serial.println("");
    }
    i++;
    delay(50);
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


